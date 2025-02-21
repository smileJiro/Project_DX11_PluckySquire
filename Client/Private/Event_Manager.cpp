#include "stdafx.h"
#include "Event_Manager.h"
#include "GameInstance.h"

#include "Level_Static.h"
#include "Level_Loading.h"
#include "Level_Logo.h"
#include "Level_Chapter_02.h"
#include "Level_Chapter_04.h"
#include "Level_Chapter_Test.h"
#include "Level_Camera_Tool_Client.h"
#include "Section_Manager.h"
#include "Layer.h"

#include "Pooling_Manager.h"
#include "Section_Manager.h"
#include "UI_Manager.h"

#include "FSM.h"
#include "FSM_Boss.h"
#include "ActorObject.h"
#include "Actor_Dynamic.h"

#include "Camera_Manager.h"
#include "Camera_Target.h"
#include "Camera_2D.h"

#include "SampleBook.h"

#include "Trigger_Manager.h"
#include "PlayerData_Manager.h"
#include "Effect2D_Manager.h"

#include "Effect_Manager.h"

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
		Execute(m_Events[i]);
	}
	m_Events.clear();

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
	m_DeadObjectsList.push_back(pDeleteObject);
	Safe_AddRef(pDeleteObject);

	return S_OK;
}

HRESULT CEvent_Manager::Execute_LevelChange(const EVENT& _tEvent)
{
	// par
	_int iChangeLevelID = (_int)(_tEvent.Parameters[0]);
	_int iNextChangeLevelID = (_int)(_tEvent.Parameters[1]);
	
	if (iChangeLevelID >= (_int)LEVEL_ID::LEVEL_END)
		return E_FAIL;

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
	case Client::LEVEL_CHAPTER_TEST:
		pChangeLevel = CLevel_Chapter_Test::Create(m_pDevice, m_pContext, (LEVEL_ID)iChangeLevelID);
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
		Safe_AddRef(pBase);
	}
	else
		pBase->Set_Active(isActive);

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
		if (true == CCamera_Manager::GetInstance()->Set_NextArmData(*pStr, iTriggerID))
			CCamera_Manager::GetInstance()->Change_CameraMode(CCamera_Target::MOVE_TO_NEXTARM);
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
		CTrigger_Manager::GetInstance()->Resister_TriggerEvent(*pStr, iTriggerID);
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
	CActorObject* pActor = (CActorObject*)_tEvent.Parameters[0];
	_uint iShapeID = (_uint)_tEvent.Parameters[1];
	_bool bEnable = (_bool)_tEvent.Parameters[2];
	pActor->Get_ActorCom()->Get_Shapes()[iShapeID]->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, bEnable);
	return S_OK;
}

HRESULT CEvent_Manager::Execute_Book_Main_Section_Change_Start(const EVENT& _tEvent)
{
	
	CSampleBook::BOOK_PAGE_ACTION eAction = (CSampleBook::BOOK_PAGE_ACTION)(_tEvent.Parameters[0]);
	_float3* pPosition = (_float3*)(_tEvent.Parameters[1]);
	
	static_cast<CSampleBook*>(m_pGameInstance->Get_GameObject_Ptr(m_pGameInstance->Get_CurLevelID(),L"Layer_Book",0))
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
	CGameObject* pVIctim = (CGameObject*)_tEvent.Parameters[1];
	_int iDamg = _tEvent.Parameters[2];

	if (nullptr == pHitter || nullptr == pVIctim)
		return E_FAIL;

	pVIctim->On_Hit(pHitter, iDamg);
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

	return S_OK;
}

HRESULT CEvent_Manager::Client_Level_Enter(_int _iChangeLevelID)
{
	CSection_Manager::GetInstance()->Level_Enter(_iChangeLevelID);
	CPooling_Manager::GetInstance()->Level_Enter(_iChangeLevelID);
	CEffect2D_Manager::GetInstance()->Level_Enter(_iChangeLevelID);
	return S_OK;
}

HRESULT CEvent_Manager::Client_Level_Exit(_int _iChangeLevelID, _int _iNextChangeLevelID)
{
	_int iCurLevelID = m_pGameInstance->Get_CurLevelID();

	CSection_Manager::GetInstance()->Level_Exit(_iChangeLevelID, _iNextChangeLevelID);
	CPooling_Manager::GetInstance()->Level_Exit(_iChangeLevelID, _iNextChangeLevelID);
	CCamera_Manager::GetInstance()->Level_Exit(_iChangeLevelID, _iNextChangeLevelID);
	CEffect_Manager::GetInstance()->Level_Exit();

	CEffect2D_Manager::GetInstance()->Level_Exit(_iChangeLevelID, _iNextChangeLevelID);
	Uimgr->Level_Exit(iCurLevelID, _iChangeLevelID, _iNextChangeLevelID);
	

	return S_OK;
}

void CEvent_Manager::Free()
{
	for (auto& pDeadObject : m_DeadObjectsList)
		Safe_Release(pDeadObject);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	__super::Free();
}
