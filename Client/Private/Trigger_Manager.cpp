#include "stdafx.h"
#include "Trigger_Manager.h"
#include "Section.h"

#include "GameInstance.h"

#include "Camera_Manager.h"
#include "Camera_Target.h"
#include "Section_2D_PlayMap.h"

#include "UI_Manager.h"
#include "PlayerData_Manager.h"
#include "GameEventExecuter.h"
#include "Effect_Manager.h"
#include "SampleBook.h"

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

	Resister_Trigger_Action();

    return S_OK;
}


void CTrigger_Manager::Update()
{
	Execute_Trigger_Event();
}

HRESULT CTrigger_Manager::Mapping_ExecuterTag()
{
	m_EventExecuterTags.resize(EVENT_EXECUTER_ACTION_TYPE_LAST);

	m_EventExecuterTags[C02P0708_LIGHTNING_BOLT_SPAWN] = L"C02P0708_Spawn_LightningBolt";
	m_EventExecuterTags[C02P0708_MONSTER_SPAWN] = L"C02P0708_Monster_Spawn";

	return S_OK;
}

CTrigger_Manager::EVENT_EXECUTER_ACTION_TYPE CTrigger_Manager::Find_ExecuterAction(const _wstring& _strTag)
{
	_uint iIndex = EVENT_EXECUTER_ACTION_TYPE_LAST;
	_uint iCount = 0;
	auto iter = find_if(m_EventExecuterTags.begin(), m_EventExecuterTags.end(), [&iCount, &_strTag](const _wstring& _strMappingTag) {
		
		if (_strMappingTag == _strTag)
			return true;
		else 
		{
			iCount++;
			return false;
		}
		});

	if (m_EventExecuterTags.end() != iter)
		iIndex = iCount;

	return (EVENT_EXECUTER_ACTION_TYPE)iIndex;
}

HRESULT CTrigger_Manager::Load_Trigger(LEVEL_ID _eProtoLevelId, LEVEL_ID _eObjectLevelId, _wstring _szFilePath, CSection* _pSection)
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
					if (FAILED(After_Initialize_Trigger_2D(Trigger_json, static_cast<CTriggerObject*>(pTrigger), Desc, _pSection)))
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

HRESULT CTrigger_Manager::Load_TriggerEvents(_wstring _szFilePath)
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

	for (auto& Trigger_Event : Result) {
		_wstring szTriggerEventTag = m_pGameInstance->StringToWString(Trigger_Event["Trigger_EventTag"]);

		if (Trigger_Event.contains("Actions") && Trigger_Event["Actions"].is_array()) {

			for (auto& Action : Trigger_Event["Actions"]) {
				ACTION tAction;

				_wstring szActionTag = m_pGameInstance->StringToWString(Action["ActionTag"]);
				tAction.Action = m_Actions[szActionTag];
				tAction.EventTag = m_pGameInstance->StringToWString(Action["EventTag"]);
				tAction.isSequence = Action["Is_Sequence"];
				tAction.isOn = false;

				m_TriggerEvents[szTriggerEventTag].push(tAction);
			}
		}
	}

	return S_OK;
}

void CTrigger_Manager::On_End(_wstring _szEventTag)
{
	if (m_CurTriggerEvent.size() <= 0)
		return;

	if(_szEventTag == m_CurTriggerEvent.front().EventTag)
		m_isEventEnd = true;
}

void CTrigger_Manager::Resister_TriggerEvent(_wstring _TriggerEventTag, _int _iTriggerID)
{
	auto iterator = m_TriggerEvents.find(_TriggerEventTag);

	if (iterator != m_TriggerEvents.end()) {
		m_CurTriggerEvent = iterator->second;
	}

	m_iTriggerID = _iTriggerID;
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
	_float3 vFreezeExitArm;

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
	case (_uint)TRIGGER_TYPE::FREEZE_X_TRIGGER:
	{

		if (_TriggerJson.contains("Freeze_X_Info")) {
			json FreezeExit = _TriggerJson["Freeze_X_Info"]["Freeze_Exit_Arm"];

			szKey = FreezeExit["CustomData_Tag"];
			vFreezeExitArm = { FreezeExit["CustomData"][0].get<_float>(), FreezeExit["CustomData"][1].get<_float>(),FreezeExit["CustomData"][2].get<_float>() };
		}

		dynamic_cast<CTriggerObject*>(_pTriggerObject)->Set_CustomData(m_pGameInstance->StringToWString(szKey), vFreezeExitArm);
	}
		break;
	case (_uint)TRIGGER_TYPE::FREEZE_Z_TRIGGER:
	{

		if (_TriggerJson.contains("Freeze_Z_Info")) {
			json FreezeExit = _TriggerJson["Freeze_Z_Info"]["Freeze_Exit_Arm"];

			szKey = FreezeExit["CustomData_Tag"];
			vFreezeExitArm = { FreezeExit["CustomData"][0].get<_float>(), FreezeExit["CustomData"][1].get<_float>(),FreezeExit["CustomData"][2].get<_float>() };
		}

		dynamic_cast<CTriggerObject*>(_pTriggerObject)->Set_CustomData(m_pGameInstance->StringToWString(szKey), vFreezeExitArm);
	}
		break;

	case (_uint)TRIGGER_TYPE::ENABLE_LOOKAT_TRIGGER:
	{
		if (_TriggerJson.contains("Enable_LookAt_Info")) {
			json isLook = _TriggerJson["Enable_LookAt_Info"]["Is_LookAt"];

			szKey = isLook["CustomData_Tag"];
			_bool iEnterLookAtMask = isLook["CustomData"];

			dynamic_cast<CTriggerObject*>(_pTriggerObject)->Set_CustomData(m_pGameInstance->StringToWString(szKey), iEnterLookAtMask);

			json ExitLookAt = _TriggerJson["Enable_LookAt_Info"]["Exit_LookAt_Mask"];
			_uint iExitLookAtMask;
			for (auto& [key, value] : ExitLookAt.items()) {
				szKey = key;
				iExitLookAtMask = value;
			}

			dynamic_cast<CTriggerObject*>(_pTriggerObject)->Set_CustomData(m_pGameInstance->StringToWString(szKey), iExitLookAtMask);
		}
	}
		break;
	}

	return S_OK;
}


HRESULT CTrigger_Manager::Fill_Trigger_2D_Desc(json _TriggerJson, CTriggerObject::TRIGGEROBJECT_DESC& _tDesc)
{
	json& ColliderInfoJson = _TriggerJson["Collider_Info"];

	_tDesc.tTransform2DDesc.vInitialPosition = { ColliderInfoJson["Position"][0].get<_float>(),  ColliderInfoJson["Position"][1].get<_float>() , 1.f };
	_tDesc.tTransform2DDesc.vInitialScaling = { ColliderInfoJson["Scale"][0].get<_float>(),  ColliderInfoJson["Scale"][1].get<_float>() ,1.f };

	return S_OK;
}


HRESULT CTrigger_Manager::After_Initialize_Trigger_2D(json _TriggerJson, CTriggerObject* _pTriggerObject, CTriggerObject::TRIGGEROBJECT_DESC& _tDesc, CSection* _pSection)
{
		// Custom Data
	_string szKey = "Next_Position";

	switch (_tDesc.iTriggerType) {
		case (_uint)TRIGGER_TYPE::SECTION_CHANGE_TRIGGER:
		{
			if (_TriggerJson.contains("MapTrigger_Info")) 
			{
				_float3 fNextPosition = { _TriggerJson["MapTrigger_Info"][szKey][0].get<_float>(),  _TriggerJson["MapTrigger_Info"][szKey][1].get<_float>(), 1.f};
				static_cast<CTriggerObject*>(_pTriggerObject)->Set_CustomData(m_pGameInstance->StringToWString(szKey), fNextPosition);
			}
		}
		break;
	}

	if (nullptr != _pSection)
		_pSection->Add_GameObject_ToSectionLayer(_pTriggerObject, SECTION_2D_PLAYMAP_TRIGGER);
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
	{
		_pTrigger->Resister_EnterHandler([_pTrigger](_uint _iTriggerType, _int _iTriggerID, _wstring& _szEventTag) {
			
			_float3 vFreezeExitArm = any_cast<_float3>(_pTrigger->Get_CustomData(TEXT("FreezeExitArm")));
			
			Event_Trigger_FreezeEnter(_iTriggerType, _iTriggerID, _szEventTag, vFreezeExitArm);
			});

		_pTrigger->Resister_ExitHandler([_pTrigger](_uint _iTriggerType, _int _iTriggerID, _wstring& _szEventTag) {
			Event_Trigger_Exit(_iTriggerType, _iTriggerID, _szEventTag);
		});
	}
		break;
	case (_uint)TRIGGER_TYPE::FREEZE_Z_TRIGGER:
	{
		_pTrigger->Resister_EnterHandler([_pTrigger](_uint _iTriggerType, _int _iTriggerID, _wstring& _szEventTag) {

			_float3 vFreezeExitArm = any_cast<_float3>(_pTrigger->Get_CustomData(TEXT("FreezeExitArm")));

			Event_Trigger_FreezeEnter(_iTriggerType, _iTriggerID, _szEventTag, vFreezeExitArm);
			});

		_pTrigger->Resister_ExitHandler([](_uint _iTriggerType, _int _iTriggerID, _wstring& _szEventTag) {
			Event_Trigger_Exit(_iTriggerType, _iTriggerID, _szEventTag);
			});
	}
		break;
	case (_uint)TRIGGER_TYPE::TELEPORT_TRIGGER:
		break;
	case (_uint)TRIGGER_TYPE::SECTION_CHANGE_TRIGGER:
		_pTrigger->Resister_EnterHandler([this, _pTrigger](_uint _iTriggerType, _int _iTriggerID, _wstring& _szEventTag) {
			_float3 fNextPosition = any_cast<_float3>(_pTrigger->Get_CustomData(TEXT("Next_Position")));
			if (_wstring::npos != _szEventTag.rfind(L"Next"))
			{
				Event_Book_Main_Section_Change_Start(1,&fNextPosition);
			}
			else
			{
				Event_Book_Main_Section_Change_Start(0, &fNextPosition);
			}
			});
		break;
	case (_uint)TRIGGER_TYPE::EVENT_TRIGGER:
	{
		_pTrigger->Resister_EnterHandler([](_uint _iTriggerType, _int _iTriggerID, _wstring& _szEventTag) {
			Event_Trigger_Enter(_iTriggerType, _iTriggerID, _szEventTag);
			});
	}
		break;
	case (_uint)TRIGGER_TYPE::ENABLE_LOOKAT_TRIGGER:
	{
		_pTrigger->Resister_EnterHandler([this, _pTrigger](_uint _iTriggerType, _int _iTriggerID, _wstring& _szEventTag) {

			_bool iReturnMask = any_cast<_bool>(_pTrigger->Get_CustomData(TEXT("IsLookAt")));
			Event_Trigger_LookAtEnter(_iTriggerType, _iTriggerID, _szEventTag, iReturnMask);

			});

		_pTrigger->Resister_ExitHandler_ByCollision([this, _pTrigger](_uint _iTriggerType, _int _iTriggerID, const COLL_INFO& _My, const COLL_INFO& _Other) {

			_vector vOtherPos = _Other.pActorUserData->pOwner->Get_ControllerTransform()->Get_Transform()->Get_State(CTransform::STATE_POSITION);
			_vector vPos = _My.pActorUserData->pOwner->Get_ControllerTransform()->Get_Transform()->Get_State(CTransform::STATE_POSITION);

			PxBoxGeometry Box;
			_My.pActorUserData->pOwner->Get_ActorCom()->Get_Shapes()[0]->getBoxGeometry(Box);

			_uint iExitDir = this->Calculate_ExitDir(vPos, vOtherPos, Box);
			_uint iExitLookAtMask = any_cast<_uint>(_pTrigger->Get_CustomData(TEXT("ExitLookAtMask")));

			_bool isEnableLookAt = iExitLookAtMask & iExitDir;

			Event_Trigger_Exit_ByCollision(_iTriggerType, _iTriggerID, isEnableLookAt);
			});
	}
		break;
	}
}

void CTrigger_Manager::Resister_Trigger_Action()
{
	m_Actions[TEXT("Camera_Arm_Move")] = [this](_wstring _wszEventTag) {
		if (true == CCamera_Manager::GetInstance()->Set_NextArmData(_wszEventTag, m_iTriggerID))
			CCamera_Manager::GetInstance()->Change_CameraMode(CCamera_Target::MOVE_TO_NEXTARM);
		};

	m_Actions[TEXT("Camera_Arm_Return")] = [this](_wstring _wszEventTag) {
		CCamera_Manager::GetInstance()->Set_PreArmDataState(m_iTriggerID, true);
		CCamera_Manager::GetInstance()->Change_CameraMode(CCamera_Target::RETURN_TO_PREARM);
		};

	m_Actions[TEXT("Dialogue")] = [this](_wstring _wszEventTag) {
		Uimgr->Set_DialogId(TEXT("Event_Dialogue_01"));
		};

	m_Actions[TEXT("Get_PlayerItem")] = [this](_wstring _wszEventTag) {
		CPlayerData_Manager::GetInstance()->Get_PlayerItem(_wszEventTag);
		};

	m_Actions[TEXT("MagicDust")] = [this](_wstring _wszEventTag) 
	{
		CEffect_Manager::GetInstance()->Active_EffectPosition(TEXT("Book_MagicDust2"), true, XMVectorSet(2.f, 0.4f, -17.3f, 1.f));
		};

	m_Actions[TEXT("MagicDust_Book")] = [this](_wstring _wszEventTag) {
		static_cast<CSampleBook*>(m_pGameInstance->Get_GameObject_Ptr(LEVEL_CHAPTER_2, TEXT("Layer_Book"), 0))->Execute_AnimEvent(5);
		};

	//m_Actions[TEXT("Create_EventExecuter")] = [this](_wstring _wszEventTag) 
	//{
	//	CGameEventExecuter::EVENT_EXECUTER_DESC Desc = {};
	//	Desc.strEventTag = _wszEventTag;
	//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_GameEventExecuter"),
	//		m_pGameInstance->Get_CurLevelID(), L"Layer_Event_Executer", &Desc)))
	//		return;
	//};


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

void CTrigger_Manager::Execute_Trigger_Event()
{
	//if (nullptr == m_pCurTriggerEvent)
	//	return;

	if (m_CurTriggerEvent.size() <= 0)
		return;

	// 바로 실행
	if (false == m_CurTriggerEvent.front().isSequence) {
		m_CurTriggerEvent.front().Action(m_CurTriggerEvent.front().EventTag);
		m_CurTriggerEvent.pop();

		Execute_Trigger_Event();
	}
	// 될 때까지 기다리기
	else {
		// 처음 시작
		if (false == m_CurTriggerEvent.front().isOn) {
			m_CurTriggerEvent.front().Action(m_CurTriggerEvent.front().EventTag);
			m_CurTriggerEvent.front().isOn = true;
		}
		// 실행 중, 끝나서 m_isEventEnd가 true가 될 때까지 대기
		else {
			// 해당 Event가 끝남
			if (true == m_isEventEnd) {
				m_CurTriggerEvent.pop();
				m_isEventEnd = false;
			}
			// 해당 Event가 아직 안 끝남
			else { 

			}

		}
	}
}

void CTrigger_Manager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	__super::Free();
}
