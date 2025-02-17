#include "stdafx.h"
#include "GameEventExecuter.h"
#include "Trigger_Manager.h"
#include "Section_Manager.h"

CGameEventExecuter::CGameEventExecuter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CGameObject(_pDevice, _pContext)

{
}

HRESULT CGameEventExecuter::Initialize_Prototype()
{
	return S_OK;
}




HRESULT CGameEventExecuter::Initialize(void* _pArg)
{
    EVENT_EXECUTER_DESC* pDesc = static_cast<EVENT_EXECUTER_DESC*>(_pArg);

    m_iEventExcuterAction =
        CTrigger_Manager::GetInstance()->Find_ExecuterAction(pDesc->strEventTag);

    if (m_iEventExcuterAction != -1 ||
        m_iEventExcuterAction == CTrigger_Manager::EVENT_EXECUTER_ACTION_TYPE_LAST)
        return E_FAIL;

    

    // 초기 설정 
    switch ((CTrigger_Manager::EVENT_EXECUTER_ACTION_TYPE)m_iEventExcuterAction)
    {
    case Client::CTrigger_Manager::C02P0708_LIGHTNING_BOLT_SPAWN:
        
        // 예시 코드
        // 이펙트를 생성하고 다음 이벤트 실행시간 맞추게 시간 타이머 설정.
        // C020708_Bolt_Spawn에서 시간이 지나면 다음 이벤트 실행(On_End 전달 & Executer 삭제)
        //Event_2DEffectCreate(EFFECT_LIGHTNINGBOLST, _float2(10.f,10.f), SECTION_MGR->Get_Cur_Section_Key())
        //m_fMaxTimer = 3.f;
        break;
    case Client::CTrigger_Manager::C02P0708_MONSTER_SPAWN:
        break;
    default:
        break;
    }


	return S_OK;
}

void CGameEventExecuter::Priority_Update(_float _fTimeDelta)
{
	_wstring strEventTag;
}

void CGameEventExecuter::Update(_float _fTimeDelta)
{
    switch ((CTrigger_Manager::EVENT_EXECUTER_ACTION_TYPE)m_iEventExcuterAction)
    {
    case Client::CTrigger_Manager::C02P0708_LIGHTNING_BOLT_SPAWN:
        C020708_Bolt_Spawn(_fTimeDelta);
        break;
    case Client::CTrigger_Manager::C02P0708_MONSTER_SPAWN:
        C020708_Monster_Spawn(_fTimeDelta);
        break;
    default:
        break;
    }

}

void CGameEventExecuter::Late_Update(_float _fTimeDelta)
{
}

void CGameEventExecuter::C020708_Bolt_Spawn(_float _fTimeDelta)
{
    m_fTimer += _fTimeDelta;

    if (m_fMaxTimer > m_fTimer)
        GameEvent_End();
}

void CGameEventExecuter::C020708_Monster_Spawn(_float _fTimeDelta)
{
}


void CGameEventExecuter::GameEvent_End()
{
    CTrigger_Manager::GetInstance()->On_End(m_strEventTag);
    Event_DeleteObject(this);
}



CGameEventExecuter* CGameEventExecuter::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CGameEventExecuter* pInstance = new CGameEventExecuter(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CGameEventExecuter");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CGameEventExecuter::Clone(void* _pArg)
{
    CGameEventExecuter* pInstance = new CGameEventExecuter(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CGameEventExecuter");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGameEventExecuter::Free()
{
    __super::Free();
}


HRESULT CGameEventExecuter::Cleanup_DeadReferences()
{
	return S_OK;
}
