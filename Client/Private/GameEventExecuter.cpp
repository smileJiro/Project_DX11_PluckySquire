#include "stdafx.h"
#include "GameEventExecuter.h"
#include "Trigger_Manager.h"
#include "Section_Manager.h"
#include "Dialog_Manager.h"
#include "Pooling_Manager.h"
#include "Camera_Manager.h"
#include "Effect2D_Manager.h"

#include "GameInstance.h"
#include "SampleBook.h"
#include "Camera_2D.h"
#include "UI_Manager.h"
#include "Effect_Manager.h"
#include "Section_2D.h"
#include "MapObject.h"
#include "Camera_Target.h"
#include "2DMapActionObject.h"

CGameEventExecuter::CGameEventExecuter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CGameObject(_pDevice, _pContext)
{
}

HRESULT CGameEventExecuter::Initialize(void* _pArg)
{
	CGameEventExecuter::EVENT_EXECUTER_DESC* pDesc =
		static_cast<CGameEventExecuter::EVENT_EXECUTER_DESC*>(_pArg);

	m_iEventExcuterAction =
		CTrigger_Manager::GetInstance()->Find_ExecuterAction(pDesc->strEventTag);

	m_strEventTag = pDesc->strEventTag;

	if (m_iEventExcuterAction == -1 ||
		m_iEventExcuterAction == CTrigger_Manager::EVENT_EXECUTER_ACTION_TYPE_LAST)
		return E_FAIL;


	return S_OK;
}

void CGameEventExecuter::GameEvent_End()
{
	if (!Is_Dead())
	{
		CTrigger_Manager::GetInstance()->On_End(m_strEventTag);
		Event_DeleteObject(this);
	}
}


CPlayer* CGameEventExecuter::Get_Player()
{
	CGameObject* pGameObject = m_pGameInstance->Get_GameObject_Ptr(m_pGameInstance->Get_CurLevelID(), TEXT("Layer_Player"), 0);
	if (nullptr == pGameObject)
		return nullptr;
	return static_cast<CPlayer*>(pGameObject);
}

CSampleBook* CGameEventExecuter::Get_Book()
{
	CGameObject* pGameObject = m_pGameInstance->Get_GameObject_Ptr(m_pGameInstance->Get_CurLevelID(), TEXT("Layer_Book"), 0);
	if (nullptr == pGameObject)
		return nullptr;
	return static_cast<CSampleBook*>(pGameObject);
}


void CGameEventExecuter::Free()
{
	__super::Free();
}


HRESULT CGameEventExecuter::Cleanup_DeadReferences()
{
	return S_OK;
}
