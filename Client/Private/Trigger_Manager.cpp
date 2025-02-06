#include "stdafx.h"
#include "Trigger_Manager.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CTrigger_Manager)

CTrigger_Manager::CTrigger_Manager()
    : m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CTrigger_Manager::Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	if (nullptr == _pDevice)
		return E_FAIL;
	if (nullptr == _pContext)
		return E_FAIL;

	m_pDevice = _pDevice;
	m_pContext = _pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

    return S_OK;
}

HRESULT CTrigger_Manager::Load_Trigger(LEVEL_ID _eProtoLevelId, LEVEL_ID _eObjectLevelId, _wstring _szFilePath)
{
	ifstream file(_szFilePath);

	if (!file.is_open())
	{
		MSG_BOX("파일을 열 수 없습니다.");
		file.close();
		return E_FAIL;
	}

	json Result;
	file >> Result;
	file.close();

	for (auto& Trigger_json : Result) {

		CTriggerObject::TRIGGEROBJECT_DESC Desc;

		#pragma region 1. Trigger 공통 정보 로드

			Desc.eStartCoord = (COORDINATE)Trigger_json["Trigger_Coordinate"];
			Desc.iTriggerType = Trigger_json["Trigger_Type"];
			Desc.szEventTag = m_pGameInstance->StringToWString(Trigger_json["Trigger_EventTag"]);
			Desc.eConditionType = (CTriggerObject::TRIGGER_CONDITION)Trigger_json["Trigger_ConditionType"];

			Desc.iFillterMyGroup = Trigger_json["Fillter_MyGroup"];
			Desc.iFillterOtherGroupMask = Trigger_json["Fillter_OtherGroupMask"];

		#pragma endregion

		#pragma region 2. Coord별 Desc 편집
			if (COORDINATE_3D == Desc.eStartCoord)
			{
				if (FAILED(Fill_Trigger_3D_Desc(Trigger_json, Desc)))
					return E_FAIL;
			}
			else if (COORDINATE_2D == Desc.eStartCoord)
			{
				if (FAILED(Fill_Trigger_2D_Desc(Trigger_json, Desc)))
					return E_FAIL;
			}
		#pragma endregion

		#pragma region 3. 트리거 객체 생성
			CGameObject* pTrigger = nullptr;

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(_eProtoLevelId, TEXT("Prototype_GameObject_TriggerObject"), _eObjectLevelId, TEXT("Layer_Trigger"), &pTrigger, &Desc))) {
				MSG_BOX("Failed To Load TriggerObject");
				return E_FAIL;
			}
		#pragma endregion

		#pragma region 4. 객체 외부초기화
			if (nullptr != pTrigger)
			{
				if (COORDINATE_3D == Desc.eStartCoord)
				{														// 무조건 CTriggerObject가 부모라고 판단. 
					if (FAILED(After_Initialize_Trigger_3D(Trigger_json, static_cast<CTriggerObject*>(pTrigger), Desc)))
						return E_FAIL;
				}
				else if (COORDINATE_2D == Desc.eStartCoord)
				{
					if (FAILED(After_Initialize_Trigger_2D(Trigger_json, static_cast<CTriggerObject*>(pTrigger), Desc)))
						return E_FAIL;
				}
			}
		#pragma endregion

		#pragma region 5. 핸들러 등록
			Resister_Event_Handler(Desc.iTriggerType, dynamic_cast<CTriggerObject*>(pTrigger));
		#pragma endregion

	}

	return S_OK;
}

HRESULT CTrigger_Manager::Fill_Trigger_3D_Desc(json _TriggerJson, CTriggerObject::TRIGGEROBJECT_DESC& _tDesc)
{
	json& ColliderInfoJson = _TriggerJson["Collider_Info"];

	_tDesc.eShapeType = (SHAPE_TYPE)ColliderInfoJson["ShapeType"];
	_tDesc.tTransform3DDesc.vInitialPosition = { ColliderInfoJson["Position"][0].get<_float>(),  ColliderInfoJson["Position"][1].get<_float>() , ColliderInfoJson["Position"][2].get<_float>() };
	_tDesc.vRotation = { ColliderInfoJson["Rotation"][0].get<_float>(),  ColliderInfoJson["Rotation"][1].get<_float>() , ColliderInfoJson["Rotation"][2].get<_float>() };
	_tDesc.vHalfExtents = { ColliderInfoJson["HalfExtents"][0].get<_float>(),  ColliderInfoJson["HalfExtents"][1].get<_float>() , ColliderInfoJson["HalfExtents"][2].get<_float>() };
	_tDesc.fRadius = ColliderInfoJson["Radius"];

	return S_OK;
}

HRESULT CTrigger_Manager::After_Initialize_Trigger_3D(json _TriggerJson, CTriggerObject* _pTriggerObject, CTriggerObject::TRIGGEROBJECT_DESC& _tDesc)
{

	// Rotation
	_matrix RotationMat = XMMatrixRotationX(XMConvertToRadians(_tDesc.vRotation.x)) * XMMatrixRotationY(XMConvertToRadians(_tDesc.vRotation.y)) * XMMatrixRotationZ(XMConvertToRadians(_tDesc.vRotation.z));
	dynamic_cast<CTriggerObject*>(_pTriggerObject)->Get_ActorCom()->Set_ShapeLocalOffsetMatrix(0, RotationMat);


	// Custom Data
	_string szKey;
	_uint iReturnMask;

	switch (_tDesc.iTriggerType) {
	case (_uint)TRIGGER_TYPE::ARM_TRIGGER:
	{
		if (_TriggerJson.contains("Arm_Info")) {
			json exitReturnMask = _TriggerJson["Arm_Info"]["Exit_Return_Mask"];

			for (auto& [key, value] : exitReturnMask.items()) {
				szKey = key;
				iReturnMask = value;
			}
		}

		dynamic_cast<CTriggerObject*>(_pTriggerObject)->Set_CustomData(m_pGameInstance->StringToWString(szKey), iReturnMask);
	}
	break;
	}

	return E_NOTIMPL;
}


HRESULT CTrigger_Manager::Fill_Trigger_2D_Desc(json _TriggerJson, CTriggerObject::TRIGGEROBJECT_DESC& _tDesc)
{
	return S_OK;
}


HRESULT CTrigger_Manager::After_Initialize_Trigger_2D(json _TriggerJson, CTriggerObject* _pTriggerObject, CTriggerObject::TRIGGEROBJECT_DESC& _tDesc)
{
	return E_NOTIMPL;
}

void CTrigger_Manager::Resister_Event_Handler(_uint _iTriggerType, CTriggerObject* _pTrigger)
{
	// Handler 추가
	//pTrigger->Resister_EnterHandler([](_uint _iTriggerType, _int _iTriggerID, _wstring& _szEventTag) {
	//	Event_Trigger_Enter(_iTriggerType, _iTriggerID, _szEventTag);
	//	});

	//pTrigger->Resister_StayHandler([](_uint _iTriggerType, _int _iTriggerID, _wstring& _szEventTag) {
	//	Event_Trigger_Stay(_iTriggerType, _iTriggerID, _szEventTag);
	//	});

	//pTrigger->Resister_ExitHandler([](_uint _iTriggerType, _int _iTriggerID, _wstring& _szEventTag) {
	//	Event_Trigger_Exit(_iTriggerType, _iTriggerID, _szEventTag);
	//	});

	switch (_iTriggerType) {
	case (_uint)TRIGGER_TYPE::ARM_TRIGGER:
	{
		_pTrigger->Resister_EnterHandler([](_uint _iTriggerType, _int _iTriggerID, _wstring& _szEventTag) {
			Event_Trigger_Enter(_iTriggerType, _iTriggerID, _szEventTag);
			});

		_pTrigger->Resister_ExitHandler_ByCollision([this, _pTrigger](_uint _iTriggerType, _int _iTriggerID, const COLL_INFO& _My, const COLL_INFO& _Other) {
			
			_vector vOtherPos = _Other.pActorUserData->pOwner->Get_ControllerTransform()->Get_Transform()->Get_State(CTransform::STATE_POSITION);
			_vector vPos = _My.pActorUserData->pOwner->Get_ControllerTransform()->Get_Transform()->Get_State(CTransform::STATE_POSITION);

			PxBoxGeometry Box;
			_My.pActorUserData->pOwner->Get_ActorCom()->Get_Shapes()[0]->getBoxGeometry(Box);

			_uint iExitDir = this->Calculate_ExitDir(vPos, vOtherPos, Box);
			_uint iReturnMask = any_cast<_uint>(_pTrigger->Get_CustomData(TEXT("ReturnMask")));

			_bool isReturn = iReturnMask & iExitDir;
			
			Event_Trigger_Exit_ByCollision(_iTriggerType, _iTriggerID, isReturn);
			});

	}
		break;
	case (_uint)TRIGGER_TYPE::CUTSCENE_TRIGGER:
		break;
	case (_uint)TRIGGER_TYPE::FREEZE_X_TRIGGER:
		break;
	case (_uint)TRIGGER_TYPE::FREEZE_Y_TRIGGER:
		break;
	case (_uint)TRIGGER_TYPE::TELEPORT_TRIGGER:
		break;
	case (_uint)TRIGGER_TYPE::EVENT_TRIGGER:
		break;
	}
}

_uint CTrigger_Manager::Calculate_ExitDir(_fvector _vPos, _fvector _vOtherPos, PxBoxGeometry& _Box)
{
	// Right
	if (XMVectorGetX(_vPos) + _Box.halfExtents.x <= XMVectorGetX(_vOtherPos)
		&& XMVectorGetZ(_vPos) + _Box.halfExtents.z >= XMVectorGetZ(_vOtherPos)
		&& XMVectorGetZ(_vPos) - _Box.halfExtents.z <= XMVectorGetZ(_vOtherPos)) {
		return EXIT_RETURN_MASK::RIGHT;
	}

	// LEFT
	if (XMVectorGetX(_vPos) - _Box.halfExtents.x >= XMVectorGetX(_vOtherPos)
		&& XMVectorGetZ(_vPos) + _Box.halfExtents.z >= XMVectorGetZ(_vOtherPos)
		&& XMVectorGetZ(_vPos) - _Box.halfExtents.z <= XMVectorGetZ(_vOtherPos)) {
		return EXIT_RETURN_MASK::LEFT;
	}

	// UP
	if (XMVectorGetZ(_vPos) + _Box.halfExtents.z <= XMVectorGetZ(_vOtherPos)
		&& XMVectorGetX(_vPos) + _Box.halfExtents.x >= XMVectorGetX(_vOtherPos)
		&& XMVectorGetX(_vPos) - _Box.halfExtents.x <= XMVectorGetX(_vOtherPos)) {
		return EXIT_RETURN_MASK::UP;
	}

	// DOWN
	if (XMVectorGetZ(_vPos) - _Box.halfExtents.z >= XMVectorGetZ(_vOtherPos)
		&& XMVectorGetX(_vPos) + _Box.halfExtents.x >= XMVectorGetX(_vOtherPos)
		&& XMVectorGetX(_vPos) - _Box.halfExtents.x <= XMVectorGetX(_vOtherPos)) {
		return EXIT_RETURN_MASK::DOWN;
	}

	return EXIT_RETURN_MASK::NONE;
}

void CTrigger_Manager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	__super::Free();
}
