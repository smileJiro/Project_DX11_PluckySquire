#include "stdafx.h"
#include "RenderGroup_PlayerDepth.h"
#include "GameInstance.h"
#include "ModelObject.h"

CRenderGroup_PlayerDepth::CRenderGroup_PlayerDepth(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CRenderGroup_MRT(_pDevice, _pContext)
{
}

HRESULT CRenderGroup_PlayerDepth::Initialize(void* _pArg)
{
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderGroup_PlayerDepth::Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer)
{
    /* Diffuse, Normal 등등에 대한 정보를 각각의 RTV에 저장하는 단계. */
    if (FAILED(m_pGameInstance->Begin_MRT(m_strMRTTag, nullptr, true))) // MRT_GameObjects 라는 타겟에 각각 diffuse, normal을 저장하기 위해 바인딩하는 과정. 
        return E_FAIL;

    for (auto& pGroupObject : m_GroupObjects)
    {
        if (nullptr != pGroupObject && true == pGroupObject->Is_Render())
            static_cast<CModelObject*>(pGroupObject)->Render_PlayerDepth();

        // Render 수행 후 해당 객체는 RefCount--
        Safe_Release(pGroupObject);
    }
    // 해당 그룹에 속한 모든 Object들에 대해 Render 수행 후, 해당 리스트는 Clear
    m_GroupObjects.clear();

    if (FAILED(m_pGameInstance->End_MRT())) // MRT에 정보들을 모두 저장했다면, 기존의 BackBuffer로 RTV를 변경. 
        return E_FAIL;

    return S_OK;
}

CRenderGroup_PlayerDepth* CRenderGroup_PlayerDepth::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pDesc)
{
    CRenderGroup_PlayerDepth* pInstance = new CRenderGroup_PlayerDepth(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize(_pDesc)))
    {
        MSG_BOX("Failed Create CRenderGroup_PlayerDepth");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRenderGroup_PlayerDepth::Free()
{
    Safe_Release(m_pDSV);

    __super::Free();
}
