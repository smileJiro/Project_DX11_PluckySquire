#include "stdafx.h"
#include "Trigger_Manager.h"
#include "Section.h"

#include "GameInstance.h"

#include "Camera_Manager.h"
#include "Camera_Target.h"
#include "Section_2D_PlayMap.h"

#include "UI_Manager.h"
#include "Dialog_Manager.h"

#include "PlayerData_Manager.h"
#include "GameEventExecuter.h"
#include "Effect_Manager.h"
#include "Book.h"
#include "PlayerItem.h"
#include "Magic_Hand.h"

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

	Register_Trigger_Action();
	Mapping_ExecuterTag();
    return S_OK;

}


void CTrigger_Manager::Update()
{
	Execute_Trigger_Event();
}

void CTrigger_Manager::Level_Exit(_int _iChangeLevelID, _int _iNextChangeLevelID)
{
	m_TriggerEvents.clear();
}

HRESULT CTrigger_Manager::Mapping_ExecuterTag()
{
	m_EventExecuterTags.resize(EVENT_EXECUTER_ACTION_TYPE_LAST);

	m_EventExecuterTags[C02P0910_LIGHTNING_BOLT_SPAWN] = L"C02P0910_Spawn_LightningBolt";
	m_EventExecuterTags[C02P0910_MONSTER_SPAWN] = L"C02P0910_Monster_Spawn";
	m_EventExecuterTags[CHAPTER2_BOOKMAGIC] = L"Chapter2_BookMagic";
	m_EventExecuterTags[CHAPTER2_INTRO] = L"Chapter2_Intro";
	m_EventExecuterTags[CHAPTER2_HUMGRUMP] = L"Chapter2_Humgrump";
	m_EventExecuterTags[CHAPTER2_LUNCHBOX_APPEAR] = L"Chapter2_LunchBox_Appear";
	m_EventExecuterTags[CHAPTER2_LUNCHBOX_OPEN] = L"Chapter2_LunchBox_Open";
	m_EventExecuterTags[CHAPTER2_BETTLE_PAGE] = L"Chapter2_Bettle_Page";
	m_EventExecuterTags[CHAPTER2_OPENBOOKEVENT] = L"Chapter2_OpenBookEvent";
	m_EventExecuterTags[CHAPTER2_STORYSEQUENCE] = L"Chapter2_StorySequence";
	m_EventExecuterTags[CHAPTER2_AFTER_OPENING_BOOK] = L"Chapter2_After_Opening_Book";
	m_EventExecuterTags[CHAPTER2_GOING_TO_ARTIA] = L"Chapter2_Going_To_Artia";
	m_EventExecuterTags[CHAPTER2_FRIENDEVENT_0] = L"Chapter2_FriendEvent_0";
	m_EventExecuterTags[CHAPTER2_FRIENDEVENT_1] = L"Chapter2_FriendEvent_1";
	m_EventExecuterTags[CHAPTER2_PIP_0] = L"Chapter2_Pip_0";

	m_EventExecuterTags[CHAPTER4_3D_OUT_01] = L"Chapter4_3D_Out_01";
	m_EventExecuterTags[CHAPTER4_3D_OUT_02] = L"Chapter4_3D_Out_02";
	m_EventExecuterTags[CHAPTER4_INTRO] = L"Chapter4_Intro";
	m_EventExecuterTags[CHAPTER4_INTRO_POSTIT_SEQUENCE] = L"Chapter4_Intro_Postit_Sequence";
	m_EventExecuterTags[CHAPTER4_RIDE_ZIPLINE] = L"Chapter4_Ride_Zipline";
	m_EventExecuterTags[CHAPTER4_EVENT_FLAG] = L"Chapter4_Event_Flag";
	m_EventExecuterTags[CHAPTER4_GATEEVENT] = L"Chapter4_GateEvent";
	m_EventExecuterTags[CHAPTER4_STORYSEQUENCE] = L"Chapter4_StorySequence";

	m_EventExecuterTags[CHAPTER6_FATHERGAME_PROGRESS_START_CLEAR] = L"Chapter6_FatherGame_Progress_Start_Clear";
	m_EventExecuterTags[CHAPTER6_FATHERGAME_PROGRESS_ZETPACK_CLEAR] = L"Chapter6_FatherGame_Progress_ZetPack_Clear";
	m_EventExecuterTags[CHAPTER6_FATHERGAME_PROGRESS_FATHERPART_1] = L"Chapter6_FatherGame_Progress_Fatherpart_1";
	m_EventExecuterTags[CHAPTER6_FATHERGAME_PROGRESS_FATHERPART_2] = L"Chapter6_FatherGame_Progress_Fatherpart_2";
	m_EventExecuterTags[CHAPTER6_FATHERGAME_PROGRESS_FATHERPART_3] = L"Chapter6_FatherGame_Progress_Fatherpart_3";
	m_EventExecuterTags[CHAPTER6_FRIENDEVENT_0] = L"Chapter6_FriendEvent_0";


	m_EventExecuterTags[CHAPTER8_INTRO_POSTIT_SEQUENCE] = L"Chapter8_Intro_Postit_Sequence";
	m_EventExecuterTags[CHAPTER8_SWORD] = 	L"Chapter8_Sword";
	m_EventExecuterTags[CHAPTER8_STOP_STAMP] = 	L"Chapter8_Stop_Stamp";
	m_EventExecuterTags[CHAPTER8_BOMB_STAMP] = 	L"Chapter8_Bomb_Stamp";
	m_EventExecuterTags[CHAPTER8_TILTING_GLOVE] = 	L"Chapter8_Tilting_Glove";
	m_EventExecuterTags[CHAPTER8_OUTRO_POSTIT_SEQUENCE] = 	L"Chapter8_Outro_Postit_Sequence";

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

			if(Trigger_json.contains("Trigger_Reusable"))
				Desc.isReusable = Trigger_json["Trigger_Reusable"];

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
			Register_Event_Handler(Desc.iTriggerType, dynamic_cast<CTriggerObject*>(pTrigger), Desc.eStartCoord);
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

HRESULT CTrigger_Manager::Create_TriggerObject(LEVEL_ID _eProtoLevelId, LEVEL_ID _eObjectLevelId, void* _pArg, CSection* _pSection, any _any)
{
	CTriggerObject::TRIGGEROBJECT_DESC* pDesc = static_cast<CTriggerObject::TRIGGEROBJECT_DESC*>(_pArg);

#pragma region 예시

	//CTriggerObject::TRIGGEROBJECT_DESC DescA = {};
	//DescA.vHalfExtents = {};
	//DescA.iTriggerType = (_uint)TRIGGER_TYPE::EVENT_TRIGGER;
	//DescA.szEventTag = TEXT("Chapter2_Intro");
	//DescA.eConditionType = CTriggerObject::TRIGGER_ENTER;
	//DescA.isReusable = false; // 한 번 하고 삭제할 때
	
	//// 기본적인 EventTrigger 3D 생성할 때
	//DescA.tTransform3DDesc.vInitialPosition = {};
	//DescA.eStartCoord = COORDINATE_3D; 
	//CTrigger_Manager::GetInstance()->Create_TriggerObject(LEVEL_STATIC, LEVEL_CHAPTER_2, &DescA);

	//// 기본적인 EventTrigger 2D 생성할 때
	//DescA.tTransform2DDesc.vInitialPosition = {};
	// DescA.eStartCoord = COORDINATE_2D;
	//_wstring wsSectionKey = CSection_Manager::GetInstance()->Get_Cur_Section_Key();
	//CSection* pSection = CSection_Manager::GetInstance()->Find_Section(wsSectionKey);
	//CTrigger_Manager::GetInstance()->Create_TriggerObject(LEVEL_STATIC, LEVEL_CHAPTER_2, &DescA, pSection);

	// Custom 데이터가 필요한 경우 any에 알아서 값 넣는다
#pragma endregion

#pragma region 1. Trigger 공통 정보 로드
	pDesc->eShapeType = SHAPE_TYPE::BOX;
	pDesc->iFillterMyGroup = OBJECT_GROUP::TRIGGER_OBJECT;
	pDesc->iFillterOtherGroupMask = OBJECT_GROUP::PLAYER;
#pragma endregion

#pragma region 2. Coord별 Desc 편집
	if (COORDINATE_3D == pDesc->eStartCoord)
	{
		
	}
	else if (COORDINATE_2D == pDesc->eStartCoord)
	{
		pDesc->tTransform2DDesc.vInitialScaling = { pDesc->vHalfExtents.x * 2.f, pDesc->vHalfExtents.y * 2.f, 0.f};

	}
#pragma endregion

#pragma region 3. 트리거 객체 생성
	CGameObject* pTrigger = nullptr;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(_eProtoLevelId, TEXT("Prototype_GameObject_TriggerObject"), _eObjectLevelId, TEXT("Layer_Trigger"), &pTrigger, pDesc))) {
		MSG_BOX("Failed To Load TriggerObject");
		return E_FAIL;
	}
#pragma endregion

#pragma region 4. 객체 외부초기화
	if (nullptr != pTrigger)
	{
		if (COORDINATE_3D == pDesc->eStartCoord)
		{														// 무조건 CTriggerObject가 부모라고 판단. 
			if (FAILED(After_Initialize_Trigger_3D(static_cast<CTriggerObject*>(pTrigger), pDesc, _any)))
				return E_FAIL;
		}
		else if (COORDINATE_2D == pDesc->eStartCoord)
		{
			if (FAILED(After_Initialize_Trigger_2D(static_cast<CTriggerObject*>(pTrigger), pDesc, _pSection, _any)))
				return E_FAIL;
		}
	}
#pragma endregion

#pragma region 5. 핸들러 등록
	Register_Event_Handler(pDesc->iTriggerType, dynamic_cast<CTriggerObject*>(pTrigger), pDesc->eStartCoord);
#pragma endregion

	return S_OK;
}

void CTrigger_Manager::On_End(_wstring _szEventTag)
{
	if (m_CurTriggerEvent.size() <= 0)
		return;

	if(_szEventTag == m_CurTriggerEvent.front().EventTag)
		m_isEventEnd = true;
}

_int CTrigger_Manager::Get_Running_EventExecuterAction()
{
	_uint iCount = 0;

	for (auto& EventExecuterTag : m_EventExecuterTags) {
		if (true == m_isRunningEvents[iCount]) {
			return iCount;
		}
		++iCount;
	}
	
	return -1;
}

void CTrigger_Manager::Register_TriggerEvent(_wstring _TriggerEventTag, _int _iTriggerID)
{
	auto iterator = m_TriggerEvents.find(_TriggerEventTag);

	if (iterator != m_TriggerEvents.end()) {
		m_CurTriggerEvent = iterator->second;
	}

	m_iTriggerID = _iTriggerID;

	// 진행할 이벤트를 True로 만들어 준다
	_uint iCount = 0;

	for (auto& EventExecuterTag : m_EventExecuterTags) {
		if (EventExecuterTag == _TriggerEventTag) {
			m_isRunningEvents[iCount] = true;
			return;
		}

		++iCount;
	}
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

HRESULT CTrigger_Manager::After_Initialize_Trigger_3D(CTriggerObject* _pTriggerObject, CTriggerObject::TRIGGEROBJECT_DESC* _pDesc, any _any)
{
	// Custom Data
	_string szKey;
	//_uint iReturnMask;
	_float3 vFreezeExitArm;

	switch (_pDesc->iTriggerType) {
	case (_uint)TRIGGER_TYPE::ARM_TRIGGER:
	{
		dynamic_cast<CTriggerObject*>(_pTriggerObject)->Set_CustomData(TEXT("Exit_Return_Mask"), _any);
	}
	break;
	case (_uint)TRIGGER_TYPE::FREEZE_X_TRIGGER:
	{
		dynamic_cast<CTriggerObject*>(_pTriggerObject)->Set_CustomData(TEXT("Freeze_Exit_Arm"), _any);
	}
	break;
	case (_uint)TRIGGER_TYPE::FREEZE_Z_TRIGGER:
	{
		dynamic_cast<CTriggerObject*>(_pTriggerObject)->Set_CustomData(TEXT("Freeze_Exit_Arm"), vFreezeExitArm);
	}
	break;

	case (_uint)TRIGGER_TYPE::ENABLE_LOOKAT_TRIGGER:
	{
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
		case (_uint)TRIGGER_TYPE::ARM_TRIGGER:
		{
			if (_TriggerJson.contains("Arm_Info")) {
				json exitReturnMask = _TriggerJson["Arm_Info"]["Exit_Return_Mask"];
				_uint iReturnMask = {};
				for (auto& [key, value] : exitReturnMask.items()) {
					szKey = key;
					iReturnMask = value;
				}

				dynamic_cast<CTriggerObject*>(_pTriggerObject)->Set_CustomData(m_pGameInstance->StringToWString(szKey), iReturnMask);
			}
		}
		break;
	}

	if (nullptr != _pSection)
		_pSection->Add_GameObject_ToSectionLayer(_pTriggerObject, SECTION_2D_PLAYMAP_TRIGGER);
	return S_OK;
}

HRESULT CTrigger_Manager::After_Initialize_Trigger_2D(CTriggerObject* _pTriggerObject, CTriggerObject::TRIGGEROBJECT_DESC* _pDesc, CSection* _pSection, any _any)
{
	_string szKey = "Next_Position";

	switch (_pDesc->iTriggerType) {

	case (_uint)TRIGGER_TYPE::SECTION_CHANGE_TRIGGER:
	{
		static_cast<CTriggerObject*>(_pTriggerObject)->Set_CustomData(TEXT("Next_Position"), _any);
	}
	break;
	case (_uint)TRIGGER_TYPE::ARM_TRIGGER:
	{
		static_cast<CTriggerObject*>(_pTriggerObject)->Set_CustomData(TEXT("Exit_Return_Mask"), _any);
	}
	break;
	}

	if (nullptr != _pSection)
		_pSection->Add_GameObject_ToSectionLayer(_pTriggerObject, SECTION_2D_PLAYMAP_TRIGGER);

	return S_OK;
}

void CTrigger_Manager::Register_Event_Handler(_uint _iTriggerType, CTriggerObject* _pTrigger, _uint _iCoordinateType)
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
		_pTrigger->Register_EnterHandler([](_uint _iTriggerType, _int _iTriggerID, _wstring& _szEventTag) {
			Event_Trigger_Enter(_iTriggerType, _iTriggerID, _szEventTag);
			});

		// Trigger 3D일 때
		if (COORDINATE_3D == _iCoordinateType) {
			_pTrigger->Register_ExitHandler_ByCollision([this, _pTrigger](_uint _iTriggerType, _int _iTriggerID, const COLL_INFO& _My, const COLL_INFO& _Other) {

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
		// Trigger 2D일 때
		else if (COORDINATE_2D == _iCoordinateType) {
			_pTrigger->Register_ExitHandler_ByCollision2D([this, _pTrigger](_uint _iTriggerType, _int _iTriggerID, CCollider* _pMyCollider, CCollider* _pOtherCollider) {

				_uint iExitDir = this->Calculate_ExitDir(_pMyCollider->Get_Position(), _pOtherCollider->Get_Position(), static_cast<CCollider_AABB*>(_pMyCollider)->Get_FinalExtents());
				_uint iReturnMask = any_cast<_uint>(_pTrigger->Get_CustomData(TEXT("ReturnMask")));

				_bool isReturn = iReturnMask & iExitDir;

				Event_Trigger_Exit_ByCollision(_iTriggerType, _iTriggerID, isReturn);
				});
		}
	}
		break;
	case (_uint)TRIGGER_TYPE::CUTSCENE_TRIGGER:
		break;
	case (_uint)TRIGGER_TYPE::FREEZE_X_TRIGGER:
	{
		_pTrigger->Register_EnterHandler([_pTrigger](_uint _iTriggerType, _int _iTriggerID, _wstring& _szEventTag) {
			
			_float3 vFreezeExitArm = any_cast<_float3>(_pTrigger->Get_CustomData(TEXT("FreezeExitArm")));
			
			Event_Trigger_FreezeEnter(_iTriggerType, _iTriggerID, _szEventTag, vFreezeExitArm);
			});

		_pTrigger->Register_ExitHandler([_pTrigger](_uint _iTriggerType, _int _iTriggerID, _wstring& _szEventTag) {
			Event_Trigger_Exit(_iTriggerType, _iTriggerID, _szEventTag);
		});
	}
		break;
	case (_uint)TRIGGER_TYPE::FREEZE_Z_TRIGGER:
	{
		_pTrigger->Register_EnterHandler([_pTrigger](_uint _iTriggerType, _int _iTriggerID, _wstring& _szEventTag) {

			_float3 vFreezeExitArm = any_cast<_float3>(_pTrigger->Get_CustomData(TEXT("FreezeExitArm")));

			Event_Trigger_FreezeEnter(_iTriggerType, _iTriggerID, _szEventTag, vFreezeExitArm);
			});

		_pTrigger->Register_ExitHandler([](_uint _iTriggerType, _int _iTriggerID, _wstring& _szEventTag) {
			Event_Trigger_Exit(_iTriggerType, _iTriggerID, _szEventTag);
			});
	}
		break;
	case (_uint)TRIGGER_TYPE::TELEPORT_TRIGGER:
		break;
	case (_uint)TRIGGER_TYPE::SECTION_CHANGE_TRIGGER:
		_pTrigger->Register_EnterHandler([this, _pTrigger](_uint _iTriggerType, _int _iTriggerID, _wstring& _szEventTag) {
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
		_pTrigger->Register_EnterHandler([](_uint _iTriggerType, _int _iTriggerID, _wstring& _szEventTag) {
			Event_Trigger_Enter(_iTriggerType, _iTriggerID, _szEventTag);
			});
	}
		break;
	case (_uint)TRIGGER_TYPE::ENABLE_LOOKAT_TRIGGER:
	{
		_pTrigger->Register_EnterHandler([this, _pTrigger](_uint _iTriggerType, _int _iTriggerID, _wstring& _szEventTag) {

			_bool iReturnMask = any_cast<_bool>(_pTrigger->Get_CustomData(TEXT("IsLookAt")));
			Event_Trigger_LookAtEnter(_iTriggerType, _iTriggerID, _szEventTag, iReturnMask);

			});

		_pTrigger->Register_ExitHandler_ByCollision([this, _pTrigger](_uint _iTriggerType, _int _iTriggerID, const COLL_INFO& _My, const COLL_INFO& _Other) {

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
	case (_uint)TRIGGER_TYPE::DIALOG_TRIGGER:
	{
		_pTrigger->Register_EnterHandler([this, _pTrigger](_uint _iTriggerType, _int _iTriggerID, _wstring& _szEventTag) {
			CDialog_Manager::GetInstance()->Set_DialogId(_szEventTag.c_str());
			});

		//_pTrigger->Register_ExitHandler_ByCollision([this, _pTrigger](_uint _iTriggerType, _int _iTriggerID, const COLL_INFO& _My, const COLL_INFO& _Other) {
		//	CDialog_Manager::GetInstance()->Set_DialogId(_szEventTag.c_str());
		//	});
	}
	break;

	}
}

void CTrigger_Manager::Register_Trigger_Action()
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
		CDialog_Manager::GetInstance()->Set_DialogId(_wszEventTag.c_str());
		};
	m_Actions[TEXT("Use_Portal")] = [this](_wstring _wszEventTag) {

		CSection* pSection = SECTION_MGR->Find_Section(_wszEventTag);

		if (nullptr != pSection)
			static_cast<CSection_2D_PlayMap*>(pSection)->Set_PortalActive(true);
		};

	m_Actions[TEXT("Get_PlayerItem_Before")] = [this](_wstring _wszEventTag) {
		// 1. 플레이어 잠그기.
		// 2. 애니메이션 재생하기컷씬 카메라 위치로 고정하기
		
		CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr()->Set_BlockPlayerInput(true);
			
		CCamera_Manager::GetInstance()->Set_ResetData(CCamera_Manager::TARGET);

		CCamera_Manager::GetInstance()->Start_Changing_ArmLength(CCamera_Manager::TARGET, 0.f, 6.f, EASE_IN_OUT);
		
		CCamera_Manager::GetInstance()->Start_Turn_AxisY(CCamera_Manager::TARGET, 0.f, XMConvertToRadians(45.f));
		CCamera_Manager::GetInstance()->Start_Turn_AxisRight(CCamera_Manager::TARGET, 0.f, XMConvertToRadians(20.f));
		//auto Arm = CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET)->Get_Arm();
		//Arm->Turn_ArmY(XMConvertToRadians(45.f));
		//Arm->Turn_ArmX(XMConvertToRadians(20.f));
		};
	
	m_Actions[TEXT("Get_PlayerItem")] = [this](_wstring _wszEventTag) 
		{
			CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr()->Set_State(CPlayer::STATE::GET_ITEM);
			CPlayerData_Manager::GetInstance()->Get_PlayerItem(_wszEventTag);
		};
	;
	// 습득후 이벤트 추가하고싶을때 쓰고있음. 아이템 습득 카메라로부터 원복 안시키게
	m_Actions[TEXT("Get_PlayerItem_After")] = [this](_wstring _wszEventTag) 
{
		CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr()->Set_BlockPlayerInput(false);
		CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr()->Set_State(CPlayer::STATE::IDLE);
		CPlayerData_Manager::GetInstance()->Change_PlayerItemMode(_wszEventTag, CPlayerItem::DISAPPEAR);
		};
	// 습득후 이벤트 가 없을때 카메라 원복,
	m_Actions[TEXT("Get_PlayerItem_After_End")] = [this](_wstring _wszEventTag) 
	{
		CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr()->Set_BlockPlayerInput(false);
		CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr()->Set_State(CPlayer::STATE::IDLE);
		CPlayerData_Manager::GetInstance()->Change_PlayerItemMode(_wszEventTag, CPlayerItem::DISAPPEAR);
		CCamera_Manager::GetInstance()->Start_ResetArm_To_SettingPoint(CCamera_Manager::TARGET, 1.f);
	};

	m_Actions[TEXT("Active_MagicDust")] = [this](_wstring _wszEventTag) 
	{
		CEffect_Manager::GetInstance()->Active_EffectPosition(TEXT("Book_MagicDust"), true, XMVectorSet(2.f, 0.4f, -17.3f, 1.f));
		};

	m_Actions[TEXT("Active_BookMagicDust")] = [this](_wstring _wszEventTag) 
{
		static_cast<CBook*>(m_pGameInstance->Get_GameObject_Ptr(LEVEL_CHAPTER_2, TEXT("Layer_Book"), 0))->Execute_AnimEvent(5);
		};
	m_Actions[TEXT("Active_MagicHand")] = [this](_wstring _wszEventTag) 
{
		static_cast<CMagic_Hand*>(m_pGameInstance->Get_GameObject_Ptr(LEVEL_CHAPTER_2, TEXT("Layer_MagicHand"), 0))->Set_Start(true);
		};
	m_Actions[TEXT("Create_EventExecuter")] = [this](_wstring _wszEventTag) 
	{
		CGameEventExecuter::EVENT_EXECUTER_DESC Desc = {};
		Desc.strEventTag = _wszEventTag;
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_pGameInstance->Get_CurLevelID(), TEXT("Prototype_GameObject_GameEventExecuter"),
			m_pGameInstance->Get_CurLevelID(), L"Layer_Event_Executer", &Desc)))
			return;
	};
	m_Actions[TEXT("Next_Chapter_Event")] = [this](_wstring _wszEventTag)
		{
			CGameEventExecuter::EVENT_EXECUTER_DESC Desc = {};
			Desc.strEventTag = _wszEventTag;
			Desc.isChapterChangeEvent = true;
			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_pGameInstance->Get_CurLevelID(), TEXT("Prototype_GameObject_GameEventExecuter"),
				m_pGameInstance->Get_CurLevelID(), L"Layer_Event_Executer", &Desc)))
				return;
		};

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

_uint CTrigger_Manager::Calculate_ExitDir(_float2 _vPos, _float2 _vOtherPos, _float2 _myExtents)
{

	// Right
	if (_vPos.x + _myExtents.x <= _vOtherPos.x
		&& _vPos.y + _myExtents.y >= _vOtherPos.y
		&& _vPos.y - _myExtents.y <= _vOtherPos.y) {
		return EXIT_RETURN_MASK::RIGHT;
	}

	// LEFT
	if (_vPos.x - _myExtents.x >= _vOtherPos.x
		&& _vPos.y + _myExtents.y >= _vOtherPos.y
		&& _vPos.y - _myExtents.y <= _vOtherPos.y) {
		return EXIT_RETURN_MASK::LEFT;
	}

	// UP
	if (_vPos.y + _myExtents.y <= _vOtherPos.y
		&& _vPos.x + _myExtents.x >= _vOtherPos.x
		&& _vPos.x - _myExtents.x <= _vOtherPos.x) {
		return EXIT_RETURN_MASK::UP;
	}

	// DOWN
	if (_vPos.y - _myExtents.y >= _vOtherPos.y
		&& _vPos.x + _myExtents.x >= _vOtherPos.x
		&& _vPos.x - _myExtents.x <= _vOtherPos.x) {
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
