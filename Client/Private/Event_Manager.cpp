#include "stdafx.h"
#include "Event_Manager.h"
#include "GameInstance.h"

#include "Level_Static.h"
#include "Level_Loading.h"
#include "Level_Logo.h"
#include "Level_Chapter_02.h"
#include "Level_Chapter_04.h"
#include "Level_Chapter_06.h"
#include "Level_Chapter_08.h"
#include "Level_Camera_Tool_Client.h"
#include "Section_Manager.h"
#include "Layer.h"

#include "Pooling_Manager.h"
#include "UI_Manager.h"
#include "NPC_Manager.h"
#include "Dialog_Manager.h"
#include "PlayerData_Manager.h"
#include "Player.h"

#include "FSM.h"
#include "FSM_Boss.h"
#include "ActorObject.h"
#include "Actor_Dynamic.h"

#include "Camera_Manager.h"
#include "Camera_Target.h"
#include "Camera_2D.h"

#include "Book.h"

#include "Trigger_Manager.h"
#include "Effect2D_Manager.h"

#include "Effect_Manager.h"
#include "3DMapObject.h"
#include "MapObjectFactory.h"

IMPLEMENT_SINGLETON(CEvent_Manager)

CEvent_Manager::CEvent_Manager()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CEvent_Manager::Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
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

void CEvent_Manager::Update(_float _fTimeDelta)
{
	if (true == m_isCreateFinished)
	{
		m_isCreateFinished = false;
		/* 쓰레드에서 모든 맵오브젝트 로드가 끝이났다면 이때 AddActorTOScene*/
		for (auto& pActorObject : m_ThreadCreateMapObjects)
		{
			pActorObject->Add_ActorToScene();
			Safe_Release(pActorObject);
		}
		m_ThreadCreateMapObjects.clear();
	}
	for (auto& Pair : m_DelayActiveList)
	{
		Pair.first->Set_Active(Pair.second);
		Safe_Release(Pair.first);
	}
	m_DelayActiveList.clear();
	
	for (auto& pDeadObject : m_DeadObjectsList)
	{
		SECTION_MGR->Remove_GameObject_FromSectionLayer(pDeadObject->Get_Include_Section_Name(), pDeadObject); /* Delete Event 호출 시, 섹션에서 제거 */
		Safe_Release(pDeadObject);
	}
	m_DeadObjectsList.clear();

	for (size_t i = 0; i < m_Events.size(); ++i)
	{
		//if(EVENT_TYPE::LEVEL_CHANGE != (EVENT_TYPE)m_Events[i].eType)
			Execute(m_Events[i]);
	}
	m_Events.clear();

	Level_Change();
	//for (size_t i = 0; i < m_Events.size(); ++i)
	//{
	//	if (EVENT_TYPE::LEVEL_CHANGE == (EVENT_TYPE)m_Events[i].eType)
	//		Execute(m_Events[i]);
	//}


}

HRESULT CEvent_Manager::Execute(const EVENT& _tEvent)
{
	switch (_tEvent.eType)
	{
	case Client::EVENT_TYPE::CREATE_OBJECT:
	{
		Execute_CreateObject(_tEvent);
	}
		break;
	case Client::EVENT_TYPE::DELETE_OBJECT:
	{
		Execute_DeleteObject(_tEvent);
	}
		break;
	case Client::EVENT_TYPE::LEVEL_CHANGE:
	{
		Execute_LevelChange(_tEvent);
	}
		break;
	case Client::EVENT_TYPE::SET_ACTIVE:
	{
		Execute_SetActive(_tEvent);
	}
		break;
	case Client::EVENT_TYPE::CHANGE_MONSTERSTATE:
	{
		Execute_ChangeMonsterState(_tEvent);
	}
		break;
	case Client::EVENT_TYPE::CHANGE_BOSSSTATE:
	{
		Execute_ChangeBossState(_tEvent);
	}
	break;
	case Client::EVENT_TYPE::SETUP_SIMULATION_FILTER:
	{
		Execute_Setup_SimulationFilter(_tEvent);
	}
	break;
	case Client::EVENT_TYPE::CHANGE_COORDINATE:
	{
		Execute_Change_Coordinate(_tEvent);
	}
	break;
	case Client::EVENT_TYPE::SET_KINEMATIC:
	{
		Execute_Set_Kinematic(_tEvent);
	}
		break;
	case Client::EVENT_TYPE::TRIGGER_ENTER_EVENT:
	{
		Execute_Trigger_Enter(_tEvent);
	}
		break;
	case Client::EVENT_TYPE::TRIGGER_STAY_EVENT:
	{
		Execute_Trigger_Stay(_tEvent);
	}
		break;
	case Client::EVENT_TYPE::TRIGGER_EXIT_EVENT:
	{
		Execute_Trigger_Exit(_tEvent);
	}
	break;
	case Client::EVENT_TYPE::TRIGGER_FREEZE_ENTER_EVENT:
	{
		Execute_Trigger_FreezeEnter(_tEvent);
	}
	break;
	case Client::EVENT_TYPE::TRIGGER_EXIT_BYCOLLISION_EVENT:
	{
		Execute_Trigger_Exit_ByCollision(_tEvent);
	}
	break;
	case Client::EVENT_TYPE::TRIGGER_LOOKAT_ENTER_EVENT:
	{
		Execute_Trigger_LookAtEnter(_tEvent);
	}
	break;
	case Client::EVENT_TYPE::BOOK_MAIN_SECTION_CHANGE_ACTION_START:
	{
		Execute_Book_Main_Section_Change_Start(_tEvent);
	}
	break;	
	case Client::EVENT_TYPE::BOOK_MAIN_SECTION_CHANGE_ACTION_END:
	{
		Execute_Book_Main_Section_Change_End(_tEvent);
	}
	break;
	case Client::EVENT_TYPE::SET_SCENEQUERYFLAG:
	{
		Execute_SetSceneQueryFlag(_tEvent);
	}
	break;
	case Client::EVENT_TYPE::BOOK_MAIN_CHANGE:
	{
		Execute_Book_Main_Change(_tEvent);
	}
	break;
	case Client::EVENT_TYPE::HIT:
	{
		Execute_Hit(_tEvent);
	}
	break;
	case Client::EVENT_TYPE::GET_BULB:
	{
		Execute_Get_Bulb(_tEvent);
	}
	break;
	case Client::EVENT_TYPE::KNOCKBACK :
	{
		Execute_KnockBack(_tEvent);
	}
	break;
	case Client::EVENT_TYPE::SNEAK_BEETLECAUGHT:
	{
		Execute_Sneak_BeetleCaught(_tEvent);
	}
	break;
	case Client::EVENT_TYPE::CHANGE_MAPOBJECT:
	{
		Execute_ChangeMapObject(_tEvent);
	}
	break;
	case Client::EVENT_TYPE::SETPLAYERSTATE:
	{
		Execute_SetPlayerState(_tEvent);
		break;
	}
	default:
		break;
	}
	
	//이벤트 매니저에서 Excute 함수 안에 Swichcase 추가
	return S_OK;
}

HRESULT CEvent_Manager::Execute_CreateObject(const EVENT& _tEvent)
{
	/* Parameter_0 : LevelD; Parameter_1 : LayerTag; Parameter_2 : GameObject Adress */
	_uint iCurLevelID = (_uint)_tEvent.Parameters[0];

	if (iCurLevelID >= LEVEL_ID::LEVEL_END)
		return E_FAIL;

	_wstring strLayerTag = reinterpret_cast<const _tchar*>(_tEvent.Parameters[1]);
	CGameObject* pCreateObject = reinterpret_cast<CGameObject*>(_tEvent.Parameters[2]);

	Safe_AddRef(pCreateObject);
	if (nullptr == pCreateObject)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(iCurLevelID, strLayerTag, pCreateObject)))
	{
		Safe_Release(pCreateObject);
		return E_FAIL;
	}

	Safe_Release(pCreateObject);

	return S_OK;
}

HRESULT CEvent_Manager::Execute_DeleteObject(const EVENT& _tEvent)
{
	/* Parameter_0 : Object Address */
	CGameObject* pDeleteObject = reinterpret_cast<CGameObject*>(_tEvent.Parameters[0]);
	if (nullptr == pDeleteObject)
		return E_FAIL;

	pDeleteObject->Set_Dead();
	m_DeadObjectsList.push_back(pDeleteObject);// ClientFunction에서 AddRef하고있다.

	return S_OK;
}

HRESULT CEvent_Manager::Execute_LevelChange(const EVENT& _tEvent)
{
	// par
	if (true == m_isLevelChange)
	{
		MSG_BOX("레벨전환 중복 호출");
		return E_FAIL; // 중복 호출
	}

	m_iChangeLevelID	= (_int)(_tEvent.Parameters[0]);
	m_iNextChangeLevelID	= (_int)(_tEvent.Parameters[1]);

	if (m_iChangeLevelID >= (_int)LEVEL_ID::LEVEL_END)
		return E_FAIL;

	m_isLevelChange = true;

	//_int iChangeLevelID = (_int)(_tEvent.Parameters[0]);
	//_int iNextChangeLevelID = (_int)(_tEvent.Parameters[1]);
	//
	//if (iChangeLevelID >= (_int)LEVEL_ID::LEVEL_END)
	//	return E_FAIL;

	///* Client Exit */
	//if (FAILED(Client_Level_Exit(iChangeLevelID, iNextChangeLevelID)))
	//	return E_FAIL;

	///* Engine Exit */
	//if (FAILED(m_pGameInstance->Engine_Level_Exit(iChangeLevelID, iNextChangeLevelID)))
	//	return E_FAIL;



	///* Level_Manager Exit */
	//if (FAILED(m_pGameInstance->Level_Manager_Exit(iChangeLevelID, iNextChangeLevelID)))
	//	return E_FAIL;

	//CLevel* pChangeLevel = nullptr;
	//switch ((LEVEL_ID)iChangeLevelID)
	//{
	//case Client::LEVEL_STATIC:
	//	pChangeLevel = CLevel_Static::Create(m_pDevice, m_pContext);
	//	break;
	//case Client::LEVEL_LOADING:
	//	pChangeLevel = CLevel_Loading::Create(m_pDevice, m_pContext, (LEVEL_ID)iNextChangeLevelID);
	//	break;
	//case Client::LEVEL_LOGO:
	//	pChangeLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
	//	break;
	//case Client::LEVEL_CHAPTER_2:
	//	pChangeLevel = CLevel_Chapter_02::Create(m_pDevice, m_pContext, (LEVEL_ID)iChangeLevelID);
	//	break;
	//case Client::LEVEL_CHAPTER_4:
	//	pChangeLevel = CLevel_Chapter_04::Create(m_pDevice, m_pContext, (LEVEL_ID)iChangeLevelID);
	//	break;
	//case Client::LEVEL_CHAPTER_6:
	//	pChangeLevel = CLevel_Chapter_06::Create(m_pDevice, m_pContext, (LEVEL_ID)iChangeLevelID);
	//	break;
	//case Client::LEVEL_CHAPTER_TEST:
	//{
	//	//switch (iChangeLevelID)
	//	//{
	//	//default:
	//	//	break;
	//	//}
	//
	//}
	//	break;
	//case Client::LEVEL_CAMERA_TOOL:
	//	pChangeLevel = CLevel_Camera_Tool_Client::Create(m_pDevice, m_pContext, (LEVEL_ID)iChangeLevelID);
	//	break;
	//default:
	//	break;
	//}

	//if (nullptr == pChangeLevel)
	//	return E_FAIL;

	///* Level_Manager Enter */
	//if (FAILED(m_pGameInstance->Level_Manager_Enter(iChangeLevelID, pChangeLevel)))
	//{
	//	Safe_Release(pChangeLevel);
	//	return E_FAIL;
	//}

	///* Engine Level Enter */
	//if (FAILED(m_pGameInstance->Engine_Level_Enter(iChangeLevelID)))
	//	return E_FAIL;

	///* Client Enter */
	//if (FAILED(Client_Level_Enter(iChangeLevelID)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CEvent_Manager::Level_Change()
{
	if (false == m_isLevelChange)
		return S_OK;

	_int iChangeLevelID = m_iChangeLevelID;
	_int iNextChangeLevelID = m_iNextChangeLevelID;
	m_isLevelChange = false;
	/* Client Exit */
	if (FAILED(Client_Level_Exit(iChangeLevelID, iNextChangeLevelID)))
		return E_FAIL;

	/* Engine Exit */
	if (FAILED(m_pGameInstance->Engine_Level_Exit(iChangeLevelID, iNextChangeLevelID)))
		return E_FAIL;



	/* Level_Manager Exit */
	if (FAILED(m_pGameInstance->Level_Manager_Exit(iChangeLevelID, iNextChangeLevelID)))
		return E_FAIL;

	CLevel* pChangeLevel = nullptr;
	switch ((LEVEL_ID)iChangeLevelID)
	{
	case Client::LEVEL_STATIC:
		pChangeLevel = CLevel_Static::Create(m_pDevice, m_pContext);
		break;
	case Client::LEVEL_LOADING:
		pChangeLevel = CLevel_Loading::Create(m_pDevice, m_pContext, (LEVEL_ID)iNextChangeLevelID);
		break;
	case Client::LEVEL_LOGO:
		pChangeLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
		break;
	case Client::LEVEL_CHAPTER_2:
		pChangeLevel = CLevel_Chapter_02::Create(m_pDevice, m_pContext, (LEVEL_ID)iChangeLevelID);
		break;
	case Client::LEVEL_CHAPTER_4:
		pChangeLevel = CLevel_Chapter_04::Create(m_pDevice, m_pContext, (LEVEL_ID)iChangeLevelID);
		break;
	case Client::LEVEL_CHAPTER_6:
		pChangeLevel = CLevel_Chapter_06::Create(m_pDevice, m_pContext, (LEVEL_ID)iChangeLevelID);
		break;
		break;
	case Client::LEVEL_CHAPTER_8:
		pChangeLevel = CLevel_Chapter_08::Create(m_pDevice, m_pContext, (LEVEL_ID)iChangeLevelID);
		break;
	case Client::LEVEL_CHAPTER_TEST:
	{
		//switch (iChangeLevelID)
		//{
		//default:
		//	break;
		//}
	
	}
		break;
	case Client::LEVEL_CAMERA_TOOL:
		pChangeLevel = CLevel_Camera_Tool_Client::Create(m_pDevice, m_pContext, (LEVEL_ID)iChangeLevelID);
		break;
	default:
		break;
	}

	if (nullptr == pChangeLevel)
		return E_FAIL;

	/* Level_Manager Enter */
	if (FAILED(m_pGameInstance->Level_Manager_Enter(iChangeLevelID, pChangeLevel)))
	{
		Safe_Release(pChangeLevel);
		return E_FAIL;
	}

	/* Engine Level Enter */
	if (FAILED(m_pGameInstance->Engine_Level_Enter(iChangeLevelID)))
		return E_FAIL;

	/* Client Enter */
	if (FAILED(Client_Level_Enter(iChangeLevelID)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEvent_Manager::Execute_SetActive(const EVENT& _tEvent)
{
	if (_tEvent.Parameters[0] == 0)
		return E_FAIL;

	CBase* pBase = (CBase*)_tEvent.Parameters[0];
	if (nullptr == pBase)
		return E_FAIL;

	_bool isActive = (_bool)(_tEvent.Parameters[1]);
	_bool isDelay = _tEvent.Parameters[2];

	if (true == isDelay)
	{
		m_DelayActiveList.push_back(make_pair(pBase, isActive));
	}
	else
	{
		pBase->Set_Active(isActive);
		Safe_Release(pBase);
	}

	return S_OK;
}

HRESULT CEvent_Manager::Execute_ChangeMapObject(const EVENT& _tEvent)
{
	_uint iCurLevelID = (_uint)(_tEvent.Parameters[0]);
	_wstring* pMapObjectFilePath = (_wstring*)(_tEvent.Parameters[1]);
	_wstring* pMapObjectLayerTag = (_wstring*)(_tEvent.Parameters[2]);
	_bool useThread = (_bool)(_tEvent.Parameters[2]);

	/* 현재 맵오브젝트를 전부 삭제 */
	CLayer* pLayer = m_pGameInstance->Find_Layer(iCurLevelID, (*pMapObjectLayerTag));
	if (nullptr == pLayer)
	{
		Safe_Delete(pMapObjectFilePath);
		Safe_Delete(pMapObjectLayerTag);
		return E_FAIL;
	}

	m_isCreateFinished = false;
	for (auto& pGameObject : pLayer->Get_GameObjects())
	{
		pGameObject->Set_Dead();
		m_DeadObjectsList.push_back(pGameObject);
		Safe_AddRef(pGameObject);
	}
	
	m_pMapObjectFilePath = pMapObjectFilePath;
	m_pMapObjectLayerTag = pMapObjectLayerTag;
	/* 쓰레드 풀을 통해 새로운 오브젝트를 로드 */
	m_pGameInstance->Get_ThreadPool()->EnqueueJob([this, iCurLevelID, pMapObjectFilePath, pMapObjectLayerTag]()
		{
			HRESULT iResult = E_FAIL;
			iResult = Map_Object_Create(*pMapObjectFilePath, iCurLevelID);

			if (S_OK == iResult)
			{
				MapObjectCreate_End();
				Safe_Delete(m_pMapObjectFilePath);
				Safe_Delete(m_pMapObjectLayerTag);
			}
			/* 모든 맵오브젝트 로드가 끝이났다면, */
			/* 오브젝트 레이어를 순회하며 */
		});



	return S_OK;
}

HRESULT CEvent_Manager::Execute_Setup_SimulationFilter(const EVENT& _tEvent)
{
	/* Parameter_0 : CActor Address*/
	/* Parameter_1 : MyGroup */
	/* Parameter_2 : OtherGroupMask */
	CActor* pActor = (CActor*)(_tEvent.Parameters[0]);
	if (nullptr == pActor)
		return E_FAIL;

	_uint iMyGroup = (_uint)_tEvent.Parameters[1];
	_uint iOtherGroupMask = (_uint)_tEvent.Parameters[2];
	pActor->Setup_SimulationFiltering(iMyGroup, iOtherGroupMask, true);

	Safe_Release(pActor);
	return S_OK;
}

HRESULT CEvent_Manager::Execute_Change_Coordinate(const EVENT& _tEvent)
{
	CActorObject* pActorObject = reinterpret_cast<CActorObject*>(_tEvent.Parameters[0]);
	COORDINATE eChangeCoord = (COORDINATE)(_tEvent.Parameters[1]);
	_float3* pPosition = (_float3*)(_tEvent.Parameters[2]);

	if (nullptr == pActorObject)
	{
		if (nullptr != pPosition)
		{
			delete pPosition;
			pPosition = nullptr;
		}
		return E_FAIL;
	}

	pActorObject->Change_Coordinate(eChangeCoord, pPosition);

	delete pPosition;
	pPosition = nullptr;
	Safe_Release(pActorObject);
	return S_OK;
}

HRESULT CEvent_Manager::Execute_Set_Kinematic(const EVENT& _tEvent)
{
	CActor_Dynamic* pActor = (CActor_Dynamic*)_tEvent.Parameters[0];
	_bool bValue = (_bool)_tEvent.Parameters[1];
	if (bValue)
		pActor->Set_Kinematic();
	else
		pActor->Set_Dynamic();

	Safe_Release(pActor);

	return S_OK;
}

HRESULT CEvent_Manager::Execute_ChangeMonsterState(const EVENT& _tEvent)
{
	/* Parameter_0 : Changing State */
	MONSTER_STATE eState = (MONSTER_STATE)_tEvent.Parameters[0];
	
	if (MONSTER_STATE::LAST == eState)
		return E_FAIL;

	/* Parameter_1 : FSM Address */
	CFSM* pFSM=(CFSM*)_tEvent.Parameters[1];

	if (nullptr == pFSM)
		return E_FAIL;

	pFSM->Change_State((_uint)eState);
	Safe_Release(pFSM);
	return S_OK;
}

HRESULT CEvent_Manager::Execute_ChangeBossState(const EVENT& _tEvent)
{
	/* Parameter_0 : Changing State */
	BOSS_STATE eState = (BOSS_STATE)_tEvent.Parameters[0];

	if (BOSS_STATE::LAST == eState)
		return E_FAIL;

	/* Parameter_1 : FSM Address */
	CFSM_Boss* pFSM = (CFSM_Boss*)_tEvent.Parameters[1];

	if (nullptr == pFSM)
		return E_FAIL;

	pFSM->Change_State((_uint)eState);
	Safe_Release(pFSM);
	return S_OK;
}

HRESULT CEvent_Manager::Execute_Trigger_Enter(const EVENT& _tEvent)
{
	_uint iCameraTriggerType = (_uint)_tEvent.Parameters[0];
	_int iTriggerID = (_int)_tEvent.Parameters[1];
	_wstring* pStr = (_wstring*)_tEvent.Parameters[2];
	
	if (nullptr == pStr)
		return E_FAIL;

	switch (iCameraTriggerType) {
	case (_uint)TRIGGER_TYPE::ARM_TRIGGER:
	{
		if (true == CCamera_Manager::GetInstance()->Set_NextArmData(*pStr, iTriggerID)) {

			if(CCamera_Manager::TARGET == CCamera_Manager::GetInstance()->Get_CurCameraMode())
				CCamera_Manager::GetInstance()->Change_CameraMode(CCamera_Target::MOVE_TO_NEXTARM);
			else if(CCamera_Manager::TARGET_2D == CCamera_Manager::GetInstance()->Get_CurCameraMode())
				CCamera_Manager::GetInstance()->Change_CameraMode(CCamera_2D::MOVE_TO_NEXTARM);
		}
	}
		break;
	case (_uint)TRIGGER_TYPE::CUTSCENE_TRIGGER:
	{
		if (true == CCamera_Manager::GetInstance()->Set_NextCutSceneData(*pStr))
			CCamera_Manager::GetInstance()->Change_CameraType(CCamera_Manager::CUTSCENE);
	}
		break;
	case (_uint)TRIGGER_TYPE::FREEZE_X_TRIGGER:
	{
	}
		break;
	case (_uint)TRIGGER_TYPE::FREEZE_Z_TRIGGER:
	{
	}
		break;
	case (_uint)TRIGGER_TYPE::TELEPORT_TRIGGER:
		break;
	case (_uint)TRIGGER_TYPE::EVENT_TRIGGER:
	{
		CTrigger_Manager::GetInstance()->Register_TriggerEvent(*pStr, iTriggerID);
	}
		break;
	}

	Safe_Delete(pStr);

	return S_OK;
}

HRESULT CEvent_Manager::Execute_Trigger_Stay(const EVENT& _tEvent)
{
	_uint iCameraTriggerType = (_uint)_tEvent.Parameters[0];
	_int iTriggerID = (_int)_tEvent.Parameters[1];
	_wstring* pStr = (_wstring*)_tEvent.Parameters[2];

	if (nullptr == pStr)
		return E_FAIL;

	switch (iCameraTriggerType) {
	case (_uint)TRIGGER_TYPE::ARM_TRIGGER:
		break;
	case (_uint)TRIGGER_TYPE::CUTSCENE_TRIGGER:
		break;
	case (_uint)TRIGGER_TYPE::FREEZE_X_TRIGGER:
		break;
	case (_uint)TRIGGER_TYPE::FREEZE_Z_TRIGGER:
		break;
	case (_uint)TRIGGER_TYPE::TELEPORT_TRIGGER:
		break;
	case (_uint)TRIGGER_TYPE::EVENT_TRIGGER:
		break;
	}

	Safe_Delete(pStr);

	return S_OK;
}

HRESULT CEvent_Manager::Execute_Trigger_Exit(const EVENT& _tEvent)
{
	_uint iCameraTriggerType = (_uint)_tEvent.Parameters[0];
	_int iTriggerID = (_int)_tEvent.Parameters[1];
	_wstring* pStr = (_wstring*)_tEvent.Parameters[2];

	if (nullptr == pStr)
		return E_FAIL;

	switch (iCameraTriggerType) {
	case (_uint)TRIGGER_TYPE::ARM_TRIGGER:
		break;
	case (_uint)TRIGGER_TYPE::CUTSCENE_TRIGGER:
		break;
	case (_uint)TRIGGER_TYPE::FREEZE_X_TRIGGER:
	{
		CCamera_Manager::GetInstance()->Set_FreezeExit(CCamera_Target::FREEZE_X, iTriggerID);
	}
		break;
	case (_uint)TRIGGER_TYPE::FREEZE_Z_TRIGGER:
	{
		CCamera_Manager::GetInstance()->Set_FreezeExit(CCamera_Target::FREEZE_Z, iTriggerID);
	}
		break;
	case (_uint)TRIGGER_TYPE::TELEPORT_TRIGGER:
		break;
	case (_uint)TRIGGER_TYPE::EVENT_TRIGGER:
		break;
	}

	Safe_Delete(pStr);

	return S_OK;
}

HRESULT CEvent_Manager::Execute_Trigger_FreezeEnter(const EVENT& _tEvent)
{
	_uint iCameraTriggerType = (_uint)_tEvent.Parameters[0];
	_int iTriggerID = (_int)_tEvent.Parameters[1];
	_wstring* pStr = (_wstring*)_tEvent.Parameters[2];
	_float3* pArm = (_float3*)_tEvent.Parameters[3];
	_vector vArm = XMLoadFloat3(pArm);
	if (nullptr == pStr)
		return E_FAIL;

	switch (iCameraTriggerType) {
	case (_uint)TRIGGER_TYPE::FREEZE_X_TRIGGER:
	{
		CCamera_Manager::GetInstance()->Set_FreezeEnter(CCamera_Target::FREEZE_X, vArm, iTriggerID);
	}
	break;
	case (_uint)TRIGGER_TYPE::FREEZE_Z_TRIGGER:
	{
		CCamera_Manager::GetInstance()->Set_FreezeEnter(CCamera_Target::FREEZE_Z, vArm, iTriggerID);
	}
	break;
	}

	Safe_Delete(pStr);
	Safe_Delete(pArm);

	return S_OK;
}

HRESULT CEvent_Manager::Execute_Trigger_LookAtEnter(const EVENT& _tEvent)
{
	_uint iTriggerType = (_uint)_tEvent.Parameters[0];;
	_int iTriggerID = (_int)_tEvent.Parameters[1];
	_wstring* pStr = (_wstring*)_tEvent.Parameters[2];
	_bool isEnableLookAt = (_bool)_tEvent.Parameters[3];

	switch (iTriggerType) {
	case (_uint)TRIGGER_TYPE::ENABLE_LOOKAT_TRIGGER:
	{
		CCamera* pCamera = CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET);
		static_cast<CCamera_Target*>(pCamera)->Set_EnableLookAt(isEnableLookAt);
	}

	break;
	}

	return S_OK;
}

HRESULT CEvent_Manager::Execute_Trigger_Exit_ByCollision(const EVENT& _tEvent)
{
	_uint iTriggerType = (_uint)_tEvent.Parameters[0];;
	_int iTriggerID = (_int)_tEvent.Parameters[1];
	_bool isReturn = (_bool)_tEvent.Parameters[2];

	switch (iTriggerType) {
	case (_uint)TRIGGER_TYPE::ARM_TRIGGER:
	{
		CCamera_Manager::GetInstance()->Set_PreArmDataState(iTriggerID, isReturn);

		if (true == isReturn)
			CCamera_Manager::GetInstance()->Change_CameraMode(CCamera_Target::RETURN_TO_PREARM);
	}

		break;
	case (_uint)TRIGGER_TYPE::ENABLE_LOOKAT_TRIGGER:
	{
		CCamera* pCamera = CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET);
		static_cast<CCamera_Target*>(pCamera)->Set_EnableLookAt(isReturn);
	}

	break;
	}

	return S_OK;
}

HRESULT CEvent_Manager::Execute_SetSceneQueryFlag(const EVENT& _tEvent)
{
	PxShape* pShape = (PxShape*)_tEvent.Parameters[0];
	_bool bEnable = (_bool)_tEvent.Parameters[1];
	pShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, bEnable);

	pShape->release();
	return S_OK;
}

HRESULT CEvent_Manager::Execute_Book_Main_Section_Change_Start(const EVENT& _tEvent)
{
	
	CBook::BOOK_PAGE_ACTION eAction = (CBook::BOOK_PAGE_ACTION)(_tEvent.Parameters[0]);
	_float3* pPosition = (_float3*)(_tEvent.Parameters[1]);
	
	static_cast<CBook*>(m_pGameInstance->Get_GameObject_Ptr(m_pGameInstance->Get_CurLevelID(),L"Layer_Book",0))
		->Execute_Action(eAction, *pPosition);
	Safe_Delete(pPosition);
	pPosition = nullptr;

	return S_OK;
}

HRESULT CEvent_Manager::Execute_Book_Main_Section_Change_End(const EVENT& _tEvent)
{
	_wstring* strSectionTag = (_wstring*)_tEvent.Parameters[0];
	HRESULT hr = SECTION_MGR->Change_CurSection(*strSectionTag);
	
	Safe_Delete(strSectionTag);
	return hr;
}

HRESULT CEvent_Manager::Execute_Book_Main_Change(const EVENT& _tEvent)
{
	_uint iCameraType = (_uint)_tEvent.Parameters[0];

	switch (iCameraType) {
	case CCamera_Manager::TARGET:
		break;
	case CCamera_Manager::TARGET_2D:
		CCamera_Manager::GetInstance()->Change_CameraMode(CCamera_2D::FLIPPING_DOWN);
		break;
	}

	return S_OK;
}

HRESULT CEvent_Manager::Execute_Hit(const EVENT& _tEvent)
{
	CGameObject* pHitter = (CGameObject*)_tEvent.Parameters[0];
	CCharacter* pVictim = (CCharacter*)_tEvent.Parameters[1];
	_int iDamg = (_int)_tEvent.Parameters[2];
	_vector vForce = XMLoadFloat3((_float3*)_tEvent.Parameters[3]);

	if (nullptr == pHitter || nullptr == pVictim)
		return E_FAIL;

	pVictim->On_Hit(pHitter, iDamg, vForce);

	delete ((_float3*)_tEvent.Parameters[3]);
	Safe_Release(pHitter);
	Safe_Release(pVictim);
	return S_OK;
}

HRESULT CEvent_Manager::Execute_Get_Bulb(const EVENT& _tEvent)
{
	_uint iCoordinate = (_uint)_tEvent.Parameters[0];

	switch (iCoordinate) {
	case (_uint)COORDINATE::COORDINATE_2D:
		CPlayerData_Manager::GetInstance()->Increase_BulbCount();
		break;
	case (_uint)COORDINATE::COORDINATE_3D:
		CPlayerData_Manager::GetInstance()->Increase_BulbCount();
		break;
	}

	return S_OK;
}

HRESULT CEvent_Manager::Execute_KnockBack(const EVENT& _tEvent)
{
	CCharacter* pCharacter = (CCharacter*)_tEvent.Parameters[0];
	_vector vForce = XMLoadFloat3( (_float3*)_tEvent.Parameters[1]);
	pCharacter->KnockBack(vForce);
	delete (_float3*)_tEvent.Parameters[1];
	Safe_Release(pCharacter);
	return S_OK;
}

HRESULT CEvent_Manager::Execute_SetPlayerState(const EVENT& _tEvent)
{
	CPlayer* pPlayer = (CPlayer*)_tEvent.Parameters[0];
	_uint iStateId =(_uint)_tEvent.Parameters[1];
	pPlayer->Set_State((CPlayer::STATE)iStateId);
	Safe_Release(pPlayer);
	return S_OK;
}

HRESULT CEvent_Manager::Execute_Sneak_BeetleCaught(const EVENT& _tEvent)
{
	CActorObject* pPlayer = (CActorObject*)(_tEvent.Parameters[0]);
	CActorObject* pMonster = (CActorObject*)(_tEvent.Parameters[1]);
	_float3* vPlayerPos= (_float3*)(_tEvent.Parameters[2]);
	_float3* vMonsterPos= (_float3*)(_tEvent.Parameters[3]);

	pPlayer->Get_ActorCom()->Set_GlobalPose(*vPlayerPos);
	pMonster->Get_ActorCom()->Set_GlobalPose(*vMonsterPos);

	Safe_Delete(vPlayerPos);
	Safe_Delete(vMonsterPos);

	Safe_Release(pPlayer);
	Safe_Release(pMonster);
	return S_OK;
}

HRESULT CEvent_Manager::Client_Level_Enter(_int _iChangeLevelID)
{
	CPooling_Manager::GetInstance()->Level_Enter(_iChangeLevelID);
	CEffect2D_Manager::GetInstance()->Level_Enter(_iChangeLevelID);
	CDialog_Manager::GetInstance()->Level_Enter(_iChangeLevelID);
	CNPC_Manager::GetInstance()->Level_Enter(_iChangeLevelID);
	return S_OK;
}

HRESULT CEvent_Manager::Client_Level_Exit(_int _iChangeLevelID, _int _iNextChangeLevelID)
{
	_int iCurLevelID = m_pGameInstance->Get_CurLevelID();

	CSection_Manager::GetInstance()->Level_Exit(_iChangeLevelID, _iNextChangeLevelID);
	CPooling_Manager::GetInstance()->Level_Exit(_iChangeLevelID, _iNextChangeLevelID);
	CCamera_Manager::GetInstance()->Level_Exit(_iChangeLevelID, _iNextChangeLevelID);
	CPlayerData_Manager::GetInstance()->Level_Exit(_iChangeLevelID, _iNextChangeLevelID);
	CEffect_Manager::GetInstance()->Level_Exit();

	CEffect2D_Manager::GetInstance()->Level_Exit(_iChangeLevelID, _iNextChangeLevelID);
	Uimgr->Level_Exit(iCurLevelID, _iChangeLevelID, _iNextChangeLevelID);
	CDialog_Manager::GetInstance()->Level_Exit(iCurLevelID, _iChangeLevelID, _iNextChangeLevelID);
	CNPC_Manager::GetInstance()->Level_Exit(iCurLevelID, _iChangeLevelID, _iNextChangeLevelID);

	return S_OK;
}

void CEvent_Manager::MapObjectCreate_End()
{
	m_isCreateFinished = true;

}

HRESULT CEvent_Manager::Map_Object_Create(const _wstring& _strFileName, _uint _iCurLevelID)
{
	wstring strFileName = _strFileName;

	_wstring strFullFilePath = MAP_3D_DEFAULT_PATH + strFileName;

	HANDLE	hFile = CreateFile(strFullFilePath.c_str(),
		GENERIC_READ,
		NULL,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		return E_FAIL;
	}
	_uint iCount = 0;

	DWORD	dwByte(0);
	_uint iLayerCount = 0;
	_int isTempReturn = 0;
	isTempReturn = ReadFile(hFile, &iLayerCount, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iLayerCount; i++)
	{
		_uint		iObjectCnt = 0;
		_char		szLayerTag[MAX_PATH];
		_string		strLayerTag;
		_wstring		wstrLayerTag;



		isTempReturn = ReadFile(hFile, &szLayerTag, (DWORD)(sizeof(_char) * MAX_PATH), &dwByte, nullptr);
		isTempReturn = ReadFile(hFile, &iObjectCnt, sizeof(_uint), &dwByte, nullptr);
		strLayerTag = szLayerTag;
		wstrLayerTag = m_pGameInstance->StringToWString(strLayerTag);

		m_ThreadCreateMapObjects.reserve(iObjectCnt);
		for (size_t i = 0; i < iObjectCnt; i++)
		{
			if (i == 694)
			{
				int a = 1;

			}

			C3DMapObject* pGameObject =
				CMapObjectFactory::Bulid_3DObject<C3DMapObject>(
					(LEVEL_ID)_iCurLevelID,
					m_pGameInstance,
					hFile, true, false);

			if (nullptr != pGameObject)
			{
				m_pGameInstance->Add_GameObject_ToLayer(_iCurLevelID, wstrLayerTag.c_str(), pGameObject);
				// 쓰레드를 통해 생성된 맵오브젝트들을 별도 저장. 
				m_ThreadCreateMapObjects.push_back(static_cast<CActorObject*>(pGameObject));
				Safe_AddRef(m_ThreadCreateMapObjects[i]);
			}


		}
	}
	CloseHandle(hFile);
	return S_OK;
}

void CEvent_Manager::Free()
{
	for (auto& pDeadObject : m_DeadObjectsList)
		Safe_Release(pDeadObject);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	__super::Free();
}
