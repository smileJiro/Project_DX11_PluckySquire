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

		Desc.iTriggerType = Trigger_json["Trigger_Type"];
		Desc.szEventTag = m_pGameInstance->StringToWString(Trigger_json["Trigger_EventTag"]);
		Desc.eStartCoord = (COORDINATE)Trigger_json["Trigger_Coordinate"];
		Desc.eConditionType = (CTriggerObject::TRIGGER_CONDITION)Trigger_json["Trigger_ConditionType"];

		Desc.tTransform3DDesc.vInitialPosition = { Trigger_json["Position"][0].get<_float>(),  Trigger_json["Position"][1].get<_float>() , Trigger_json["Position"][2].get<_float>() };
		_float3 vRotation = { Trigger_json["Rotation"][0].get<_float>(),  Trigger_json["Rotation"][1].get<_float>() , Trigger_json["Rotation"][2].get<_float>() };

		Desc.eShapeType = (SHAPE_TYPE)Trigger_json["ShapeType"];
		Desc.vHalfExtents = { Trigger_json["HalfExtents"][0].get<_float>(),  Trigger_json["HalfExtents"][1].get<_float>() , Trigger_json["HalfExtents"][2].get<_float>() };
		Desc.fRadius = Trigger_json["Radius"];

		Desc.iFillterMyGroup = Trigger_json["Fillter_MyGroup"];
		Desc.iFillterOtherGroupMask = Trigger_json["Fillter_OtherGroupMask"];

		CGameObject* pTrigger = nullptr;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_TriggerObject"), LEVEL_GAMEPLAY, TEXT("Layer_Trigger"), &pTrigger, &Desc))) {
			MSG_BOX("Failed To Load TriggerObject");
			return E_FAIL;
		}

		// Rotation
		_matrix RotationMat = XMMatrixRotationX(XMConvertToRadians(vRotation.x)) * XMMatrixRotationY(XMConvertToRadians(vRotation.y)) * XMMatrixRotationZ(XMConvertToRadians(vRotation.z));
		dynamic_cast<CTriggerObject*>(pTrigger)->Get_ActorCom()->Set_ShapeLocalOffsetMatrix(0, RotationMat);


		// Custom Data
		_string szKey;
		_uint iReturnMask;

		switch (Desc.iTriggerType) {
		case (_uint)TRIGGER_TYPE::ARM_TRIGGER:
		{
			if (Trigger_json.contains("Arm_Info")) {
				json exitReturnMask = Trigger_json["Arm_Info"]["Exit_Return_Mask"];

				for (auto& [key, value] : exitReturnMask.items()) {
					szKey = key;
					iReturnMask = value;
				}
			}

			dynamic_cast<CTriggerObject*>(pTrigger)->Set_CustomData(m_pGameInstance->StringToWString(szKey), iReturnMask);
		}
			break;
		}

		Resister_Event_Handler(Desc.iTriggerType, dynamic_cast<CTriggerObject*>(pTrigger));
	}

	return S_OK;
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
