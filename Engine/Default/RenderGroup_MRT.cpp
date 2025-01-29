#include "CRenderGroup_MRT.h"
#include "GameInstance.h"

CRenderGroup_MRT::CRenderGroup_MRT(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CRenderGroup(_pDevice, _pContext)
{
}

HRESULT CRenderGroup_MRT::Initialize(void* _pArg)
{
    RG_MRT_DESC* pDesc = static_cast<RG_MRT_DESC*>(_pArg);

    // Save Desc
    m_strMRTTag = pDesc->strMRTTag;
    m_isClear = pDesc->isClear;
    m_pDSV = pDesc->pDSV;
    Safe_AddRef(m_pDSV);
    m_isViewportSizeChange = pDesc->isViewportSizeChange;
    m_vViewportSize = pDesc->vViewportSize;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderGroup_MRT::Render(CShader* _pRTShader, CVIBuffer* _pRTBuffer)
{
    /* Diffuse, Normal 등등에 대한 정보를 각각의 RTV에 저장하는 단계. */
    if (FAILED(m_pGameInstance->Begin_MRT(m_strMRTTag, m_pDSV, m_isClear))) // MRT_GameObjects 라는 타겟에 각각 diffuse, normal을 저장하기 위해 바인딩하는 과정. 
        return E_FAIL;

    if (true == m_isViewportSizeChange)
        Setup_Viewport();

    for (auto& pGroupObject : m_GroupObjects)
    {
        if (nullptr != pGroupObject && true == pGroupObject->Is_Render())
            pGroupObject->Render();

        // Render 수행 후 해당 객체는 RefCount--
        Safe_Release(pGroupObject);
    }
    // 해당 그룹에 속한 모든 Object들에 대해 Render 수행 후, 해당 리스트는 Clear
    m_GroupObjects.clear();

    if (FAILED(m_pGameInstance->End_MRT())) // MRT에 정보들을 모두 저장했다면, 기존의 BackBuffer로 RTV를 변경. 
        return E_FAIL;

    return S_OK;
}

CRenderGroup_MRT* CRenderGroup_MRT::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pDesc)
{
    return nullptr;
}

void CRenderGroup_MRT::Free()
{
}
