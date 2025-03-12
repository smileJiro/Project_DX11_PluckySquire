#include "stdafx.h"
#include "PortalLocker_LayerCount.h"
#include "GameInstance.h"
#include "Layer.h"
#include "Portal.h"

CPortalLocker_LayerCount::CPortalLocker_LayerCount(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CPortalLocker(_pDevice, _pContext)
{
}

CPortalLocker_LayerCount::CPortalLocker_LayerCount(const CPortalLocker_LayerCount& _Prototype)
    :CPortalLocker(_Prototype)
{
}

HRESULT CPortalLocker_LayerCount::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPortalLocker_LayerCount::Initialize(void* _pArg)
{
    PORTALLOCKER_LAYER_DESC* pDesc = static_cast<CPortalLocker_LayerCount::PORTALLOCKER_LAYER_DESC*>(_pArg);
    // Save
    m_strCountingLayerTag = pDesc->strCountingLayerTag;

    // Add
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    return S_OK;
}

void CPortalLocker_LayerCount::Action_State_Lock(_float _fTimeDelta)
{
    if (nullptr == m_pTargetPortal)
        return;

    if (true == m_pTargetPortal->Is_Active())
    {
        Event_SetActive(m_pTargetPortal, false);
    }

    if (STATE_OPEN == m_eCurState)
        return;

    /* 1. 특정 레이어에 대한 검사를 수행한다. */
    CLayer* pLayer = m_pGameInstance->Find_Layer(m_iCurLevelID, m_strCountingLayerTag);
    if (nullptr == pLayer)
    {
        Open_Locker();
        return;
    }

    const list<CGameObject*>& pObjects = pLayer->Get_GameObjects();
    /* 2. 레이어를 확인하며, 현재 Active가 True 이며, Dead 상태가 아닌 오브젝트를 검사한다. */

    if (true == pObjects.empty())                       // 레이어가 비어있다면, 이건 완료 카운트로 본다.
        ++m_iCheckCount;
    else
    {
        m_iAliveObjectCount = 0;
        for (auto& pGameObject : pObjects)
        {
            if (true == pGameObject->Is_ValidGameObject())
                ++m_iAliveObjectCount;
        }
        
        if (0 == m_iAliveObjectCount)                   // 살아 있는 유효한 객체가 0이라면, 완료 카운트로 본다.
            ++m_iCheckCount;
        else
            m_iCheckCount = 0;                          // 살아 있는 유효한 객체가 0이 아니라면, 검사카운트도 0으로 리셋한다.
    }

    /* 3. 해당 카운트가 0이 되면, PortalLocker는 검사 카운트가 1 오르고 총 3번의 프레임 동안 검사 수행 후 같은 결과가 나왔다면, 포탈을 개방한다. */
    if (m_iMaxCheckCount <= m_iCheckCount)
        Open_Locker();
}

CPortalLocker_LayerCount* CPortalLocker_LayerCount::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CPortalLocker_LayerCount* pInstance = new CPortalLocker_LayerCount(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Created CPortalLocker_LayerCount Failed");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

CGameObject* CPortalLocker_LayerCount::Clone(void* _pArg)
{
    CPortalLocker_LayerCount* pInstance = new CPortalLocker_LayerCount(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Clone CPortalLocker_LayerCount Failed");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

void CPortalLocker_LayerCount::Free()
{
    __super::Free();
}
