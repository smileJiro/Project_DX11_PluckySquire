#include "stdafx.h"
#include "RenderGroup_WorldPos.h"
#include "GameInstance.h"
#include "ModelObject.h"

CRenderGroup_WorldPos::CRenderGroup_WorldPos(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CRenderGroup_MRT(_pDevice, _pContext)
{
}

HRESULT CRenderGroup_WorldPos::Initialize(void* _pArg)
{
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderGroup_WorldPos::Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer)
{
  /* Diffuse, Normal 등등에 대한 정보를 각각의 RTV에 저장하는 단계. */
    //if (FAILED(m_pGameInstance->Begin_MRT(m_strMRTTag, m_pDSV, m_isClear))) // MRT_GameObjects 라는 타겟에 각각 diffuse, normal을 저장하기 위해 바인딩하는 과정. 
    //    return E_FAIL;

    //if (true == m_isViewportSizeChange)
    //    Setup_Viewport(m_vViewportSize);

    //for (auto& pGroupObject : m_GroupObjects)
    //{
    //    if (nullptr != pGroupObject && true == pGroupObject->Is_Render())
    //        static_cast<CModelObject*>(pGroupObject)->Render_WorldPosMap();

    //    // Render 수행 후 해당 객체는 RefCount--
    //    Safe_Release(pGroupObject);
    //}
    //// 해당 그룹에 속한 모든 Object들에 대해 Render 수행 후, 해당 리스트는 Clear
    //m_GroupObjects.clear();

    //// Origin Viewport 로 변경해야해.////// 
    //if (true == m_isViewportSizeChange)
    //    Setup_Viewport(_float2((_float)m_pGameInstance->Get_ViewportWidth(), (_float)m_pGameInstance->Get_ViewportHeight()));

    //if (FAILED(m_pGameInstance->End_MRT())) // MRT에 정보들을 모두 저장했다면, 기존의 BackBuffer로 RTV를 변경. 
    //    return E_FAIL;

    return S_OK;
}

CRenderGroup_WorldPos* CRenderGroup_WorldPos::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg)
{
    CRenderGroup_WorldPos* pInstance = new CRenderGroup_WorldPos(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Create Failed CRenderGroup_WorldPos");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CRenderGroup_WorldPos::Free()
{

    __super::Free();
}
