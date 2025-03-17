#include "stdafx.h"
#include "GameEventExecuter.h"
#include "Trigger_Manager.h"
#include "Section_Manager.h"
#include "Dialog_Manager.h"
#include "Pooling_Manager.h"
#include "Camera_Manager.h"
#include "Effect2D_Manager.h"
#include "PlayerData_Manager.h"
#include "Friend_Controller.h"

#include "GameInstance.h"
#include "Book.h"
#include "Camera_2D.h"
#include "UI_Manager.h"
#include "Narration_Manager.h"
#include "Effect_Manager.h"
#include "Section_2D.h"
#include "MapObject.h"
#include "Camera_Target.h"
#include "2DMapActionObject.h"
#include "Postit_Page.h"

CGameEventExecuter::CGameEventExecuter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CGameObject(_pDevice, _pContext)
{
}

HRESULT CGameEventExecuter::Initialize(void* _pArg)
{
	CGameEventExecuter::EVENT_EXECUTER_DESC* pDesc =
		static_cast<CGameEventExecuter::EVENT_EXECUTER_DESC*>(_pArg);

	m_isNextChapter = pDesc->isChapterChangeEvent;
	if (m_isNextChapter)
	{
		m_strNextChapter = pDesc->strEventTag;
	}
	else 
	{
		m_iEventExcuterAction =
			CTrigger_Manager::GetInstance()->Find_ExecuterAction(pDesc->strEventTag);

		m_strEventTag = pDesc->strEventTag;

		if (m_iEventExcuterAction == -1 ||
			m_iEventExcuterAction == CTrigger_Manager::EVENT_EXECUTER_ACTION_TYPE_LAST)
			return E_FAIL;
	}


	return S_OK;
}

void CGameEventExecuter::Priority_Update(_float _fTimeDelta)
{
	if (m_isNextChapter)
	{
		Next_Event_Process(_fTimeDelta);
	}
}

void CGameEventExecuter::GameEvent_End()
{
	if (!Is_Dead())
	{
		CTrigger_Manager::GetInstance()->On_End(m_strEventTag);
		CTrigger_Manager::GetInstance()->On_End(m_iEventExcuterAction);
		Event_DeleteObject(this);
	}
}

void CGameEventExecuter::Start_Train(_float _fTimeDelta)
{
	if(false == CFriend_Controller::GetInstance()->Is_Train())
		CFriend_Controller::GetInstance()->Start_Train();

	GameEvent_End();
	int a = 0;
}


_bool CGameEventExecuter::Postit_Process(const _wstring& _strPostItSectionTag, const _wstring& _strPostItDialogTag, _float _fStartChaseCameraTime, CPostit_Page::POSTIT_PAGE_POSTION_TYPE _ePostionType, _bool _isResetStart, function<void()> _FirstCamFunc)
{
	if (Step_Check(STEP_0))
	{
		Postit_Process_Start(_strPostItSectionTag, _fStartChaseCameraTime, _isResetStart, _FirstCamFunc);
		return false;
	}
	else if (Step_Check(STEP_1))
	{
		Postit_Process_PageAppear(_ePostionType);
		return false;
	}
	else if (Step_Check(STEP_2))
	{
		Postit_Process_PageTalk(_strPostItDialogTag, _ePostionType);
		return false;
	}
	else if (Step_Check(STEP_3))
	{
		Postit_Process_End(_fStartChaseCameraTime);
		return false;
	}

	return true;
}

_bool CGameEventExecuter::Postit_Process_Start(const _wstring& _strPostItSectionTag, _float _fStartChaseCameraTime, _bool _isResetStart, function<void()> _FirstCamFunc)
{
	if (Is_Start())
	{
		// 기존이 플레이 암데이터라면 저장
		if (_isResetStart)
			CCamera_Manager::GetInstance()->Set_ResetData(CCamera_Manager::TARGET);
		// 컷씬 시작이기 때문에, 플레이어를 잠근다.
		CPlayer* pPlayer = Get_Player();
		pPlayer->Set_BlockPlayerInput(true);

		// executer에 책벌레 세팅
		if (false == Setting_Postit_Page(_strPostItSectionTag))
			assert(nullptr);

		// 가까운 포스트잇을 찾아 카메라를 땡긴다
		CGameObject* pTargetPostit = Find_Postit();
		if (nullptr == pTargetPostit)
			assert(nullptr);

		// 카메라를 _fStartChaseCameraTime 만큼 포스트잇으로 땡긴다.
		CCamera_Manager::GetInstance()->Change_CameraTarget(pTargetPostit, _fStartChaseCameraTime);

		// 땡긴 후 추가액션이 세팅되어 있다면 실행한다
		if (nullptr != _FirstCamFunc)
			_FirstCamFunc();
		//CCamera_Manager::GetInstance()->Start_Turn_AxisY(CCamera_Manager::TARGET, 1.f, XMConvertToRadians(-40.f), XMConvertToRadians(-25.f));
		//// 암 3.f 줄이기, 1초동안
		//CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Decrease(CCamera_Manager::TARGET, 1.f,
		//	3.f, EASE_IN_OUT);
		//// 타겟 오프셋 y -2.f
		//CCamera_Manager::GetInstance()->Start_Changing_AtOffset(CCamera_Manager::TARGET,
		//	1.f,
		//	XMVectorSet(0.f, -2.f, 0.f, 0.f),
		//	EASE_IN_OUT);
	}

	return Next_Step_Over(_fStartChaseCameraTime);
}

_bool CGameEventExecuter::Postit_Process_PageAppear(CPostit_Page::POSTIT_PAGE_POSTION_TYPE _ePostionType)
{
	if (Is_Start())
	{
		m_pPostitPage->Anim_Action(CPostit_Page::POSTIT_PAGE_APPEAR, false, _ePostionType);
		return false;
	}
	else
	{
		if (nullptr == m_pPostitPage)
			assert(nullptr);
		return Next_Step(!m_pPostitPage->Is_DuringAnimation());
	}
}

_bool CGameEventExecuter::Postit_Process_PageTalk(const _wstring& _strPostItDialogTag, CPostit_Page::POSTIT_PAGE_POSTION_TYPE _ePostionType)
{
	if (Is_Start())
	{
		// 말함
		if (nullptr == m_pPostitPage)
		{
			assert(nullptr);
		}
		m_pPostitPage->Anim_Action(CPostit_Page::POSTIT_PAGE_TALK_1, true, _ePostionType);

		CDialog_Manager::GetInstance()->Set_DialogId(_strPostItDialogTag.c_str());
		return false;
	}
	else
		Next_Step(!CDialog_Manager::GetInstance()->Get_DisPlayDialogue());
	return true;
}

_bool CGameEventExecuter::Postit_Process_End(_float _fStartChaseCameraTime)
{
	if (Is_Start())
	{
		if (nullptr == m_pPostitPage)
			assert(nullptr);
		m_pPostitPage->Anim_Action(CPostit_Page::POSTIT_PAGE_DISAPPEAR, false);

		return false;
	}
	else if(Next_Step_Over(1.f))
	{
		CPlayer* pPlayer = Get_Player();
		pPlayer->Set_BlockPlayerInput(false);
		CCamera_Manager::GetInstance()->Change_CameraTarget(pPlayer, _fStartChaseCameraTime);
		CCamera_Manager::GetInstance()->Start_ResetArm_To_SettingPoint(CCamera_Manager::TARGET, 0.5f);
	}
	return true;
}

CGameObject* CGameEventExecuter::Find_Postit()
{
	_float fLength = -1.f;
	CGameObject* pReturnObject = nullptr;
	_vector vCamPos = XMLoadFloat4(m_pGameInstance->Get_CamPosition());
	
	auto pLayer = m_pGameInstance->Find_Layer((_uint)m_iCurLevelID, L"Layer_Postit");
	if (nullptr != pLayer) 
	{
		auto& GameObjects = pLayer->Get_GameObjects();
		for_each(GameObjects.begin(), GameObjects.end(), [&pReturnObject , &fLength , &vCamPos](CGameObject* pGameObject ) {
			
			_vector vPostifPos = pGameObject->Get_FinalPosition((COORDINATE_3D));
			
			_float fThisLength = XMVectorGetX(XMVector3Length(vCamPos - vPostifPos));
			if (fLength < 0.f || fLength > fabs(fThisLength))
			{
				fLength = fThisLength;
				pReturnObject = pGameObject;
			}
			});
	}
	
	return pReturnObject;
}

CPlayer* CGameEventExecuter::Get_Player()
{
	return CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr();
	/*CGameObject* pGameObject = m_pGameInstance->Get_GameObject_Ptr(m_pGameInstance->Get_CurLevelID(), TEXT("Layer_Player"), 0);
	if (nullptr == pGameObject)
		return nullptr;
	return static_cast<CPlayer*>(pGameObject);*/
}

CBook* CGameEventExecuter::Get_Book()
{
	CGameObject* pGameObject = m_pGameInstance->Get_GameObject_Ptr(m_pGameInstance->Get_CurLevelID(), TEXT("Layer_Book"), 0);
	if (nullptr == pGameObject)
		return nullptr;
	return static_cast<CBook*>(pGameObject);
}

_bool CGameEventExecuter::Setting_Postit_Page(const _wstring& _strPostItSectionTag)
{
	CSection_2D* pSection = static_cast<CSection_2D*>(SECTION_MGR->Find_Section(_strPostItSectionTag));
	auto pLayer = pSection->Get_Section_Layer(SECTION_PLAYMAP_2D_RENDERGROUP::SECTION_2D_PLAYMAP_BACKGROUND);

	const auto& Objects = pLayer->Get_GameObjects();
	if (Objects.size() != 1)
		assert(nullptr);
	m_TargetObjects.push_back(Objects.front());
	if (nullptr == m_TargetObjects[0])
		return false;
	else
		m_pPostitPage = dynamic_cast<CPostit_Page*>(m_TargetObjects[0]);
	m_TargetObjects[0] = nullptr;
	return true;
}

_bool CGameEventExecuter::Next_Event_Process(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;
	if (Is_Start())
	{
		auto pPlayer = Get_Player();
		if (nullptr != pPlayer)
		{
			pPlayer->Set_BlockPlayerInput(false);
		}
		CCamera_Manager::GetInstance()->Start_FadeOut(2.f);
	}

	if (Next_Step_Over(2.05f))
	{
		LEVEL_ID eNextLevelID = LEVEL_END;


		if (L"Chapter2" == m_strNextChapter)
		{
			eNextLevelID = LEVEL_CHAPTER_2;
		}
		else if (L"Chapter4" == m_strNextChapter)
		{
			eNextLevelID = LEVEL_CHAPTER_4;
		}
		else if (L"Chapter6" == m_strNextChapter)
		{
			eNextLevelID = LEVEL_CHAPTER_6;
		}
		else if (L"Chapter8" == m_strNextChapter)
		{
			eNextLevelID = LEVEL_CHAPTER_8;
		}

		if (eNextLevelID != LEVEL_END)
		{
			Event_LevelChange(LEVEL_LOADING, eNextLevelID);
			//Event_LevelChange(m_pGameInstance->Get_CurLevelID(), eNextLevelID);
			GameEvent_End();
		}
	}
	
	return true;
}


void CGameEventExecuter::Free()
{
	__super::Free();
}


HRESULT CGameEventExecuter::Cleanup_DeadReferences()
{
	return S_OK;
}
