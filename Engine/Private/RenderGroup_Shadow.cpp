#include "RenderGroup_Shadow.h"
#include "GameInstance.h"
#include "Light.h"
#include "ModelObject.h"
//#include "RenderTarget.h"

CRenderGroup_Shadow::CRenderGroup_Shadow(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CRenderGroup(_pDevice, _pContext)
{
}

HRESULT CRenderGroup_Shadow::Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer)
{
    m_ShadowLights;
    /* 1. Shadow Light Loop  */
    static _int iBindMaxSizeRTs = 8;
    static vector<CRenderTarget*> DrawMRT;
    _pRTShader->Bind_RawValue("g_vCamWorld", m_pGameInstance->Get_CamPosition(), sizeof(_float4));
    for (_uint i = 0; i < m_ShadowLights.size(); ++i)
    {
        // 2. Light 가 가진 RenderTarget을 MRT로 바인딩. 
        CRenderTarget* pShadowRenderTarget = m_ShadowLights[i]->Get_ShadowRenderTarget();
        if (nullptr == pShadowRenderTarget)
            continue;
        DrawMRT.push_back(pShadowRenderTarget);

        if(FAILED(m_pGameInstance->Begin_MRT(DrawMRT, m_pGameInstance->Find_DSV(TEXT("DSV_Shadow")))))
            return E_FAIL;
        // 3. Viewport 설정.
        _float2 vSize = pShadowRenderTarget->Get_Size();
        if (FAILED(Setup_Viewport(vSize)))
            return E_FAIL;

        // 5. Render_Shadow()
        for (auto& pRenderObject : m_GroupObjects)
        {
            if (nullptr != pRenderObject && true == pRenderObject->Is_Render())
                static_cast<CModelObject*>(pRenderObject)->Render_Shadow(m_ShadowLights[i]->Get_ViewMatrix(), m_ShadowLights[i]->Get_ProjMatrix());

            if(i == m_ShadowLights.size() - 1)
            {
                /* 마지막조명을 렌더 한 뒤 Release */
                Safe_Release(pRenderObject);
            }
        }
        if (i == m_ShadowLights.size() - 1)
        {
            /* 마지막조명을 렌더 한 뒤 Release */
            m_GroupObjects.clear();
        }

        // 6. 뷰포트 원상복구 (사실 여기서 안해줘도되는데 추후 캐스케이트 고려해서 미리 걍 세팅해두겠음)
        if (FAILED(Setup_Viewport(_float2((_float)m_pGameInstance->Get_ViewportWidth(), (_float)m_pGameInstance->Get_ViewportHeight()))))
            return E_FAIL;

        if (FAILED(m_pGameInstance->End_MRT()))
            return E_FAIL;

        // 7. MRT 클리어
        DrawMRT.clear();
    }

    return S_OK;
}

HRESULT CRenderGroup_Shadow::Add_RenderObject(CGameObject* _pGameObject)
{
    if (true == m_ShadowLights.empty())
        return S_OK;

    __super::Add_RenderObject(_pGameObject);
    return S_OK;
}

HRESULT CRenderGroup_Shadow::Add_ShadowLight(CLight* _pShadowLight)
{
    if (nullptr == _pShadowLight)
        return E_FAIL;

    m_ShadowLights.push_back(_pShadowLight);
    Safe_AddRef(_pShadowLight);
    // 레벨전환시 >>> 이걸 지워주던지 remove 해야한다. 
    return S_OK;
}

HRESULT CRenderGroup_Shadow::Remove_ShadowLight(_int _iShadowLightID)
{
    if (true == m_ShadowLights.empty())
        return E_FAIL;

    auto iter = m_ShadowLights.begin();
    for (; iter != m_ShadowLights.end(); )
    {
        if (_iShadowLightID == (*iter)->Get_ShadowLightID())
        {
            Safe_Release(*iter);
            iter = m_ShadowLights.erase(iter);

            if (false == m_ShadowLights.empty())
            {
                for (auto& pGameObject : m_GroupObjects)
                {
                    Safe_Release(pGameObject);
                }
                m_GroupObjects.clear();
            }

            return S_OK;
        }
        else
        {
            ++iter;
        }
    }



    return E_FAIL;
}

HRESULT CRenderGroup_Shadow::Clear_ShadowLight()
{
    for (auto& pShadowLight : m_ShadowLights)
    {
        Safe_Release(pShadowLight);
    }
    m_ShadowLights.clear();

    return S_OK;
}

CRenderGroup_Shadow* CRenderGroup_Shadow::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pDesc)
{
    CRenderGroup_Shadow* pInstance = new CRenderGroup_Shadow(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(_pDesc)))
    {
        MSG_BOX("Create Failed : CRenderGroup_Shdow");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CRenderGroup_Shadow::Free()
{
    Clear_ShadowLight();

    __super::Free();
}
