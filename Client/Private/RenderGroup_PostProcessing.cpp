#include "stdafx.h"
#include "RenderGroup_PostProcessing.h"
#include "GameInstance.h"



CRenderGroup_PostProcessing::CRenderGroup_PostProcessing(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CRenderGroup(_pDevice, _pContext)
{
}

HRESULT CRenderGroup_PostProcessing::Initialize(void* _pArg)
{
    RG_POST_DESC* pDesc = static_cast<RG_POST_DESC*>(_pArg);

    // Save
    m_iBlurLevel = pDesc->iBlurLevel;

    // Add
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    /* 블러 레벨에 따라 미리 렌더 타겟을 생성한다. */
    Ready_BlurRenderTarget();


    return S_OK;
}

HRESULT CRenderGroup_PostProcessing::Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer)
{


    return S_OK;
}

HRESULT CRenderGroup_PostProcessing::Ready_BlurRenderTarget()
{
    /* Blur Level에 따라 사이즈에 맞는 RenderTarget을 생성 */ /* 블러레벨이 4라면, 총 6장 생성 */
    _uint iNumRenderTargets = (m_iBlurLevel - 1) * 2; // Down샘플용 절반, Up샘플용 절반. 마지막 인덱스의 RenderTarget이 최종 화면
    m_BlurRenderTargets.resize(iNumRenderTargets);

    /* DownSample용 RenderTarget 생성 */
    for (_uint i = 0; i < iNumRenderTargets / 2; ++i)
    {
        /* blur level 4 기준 : 2, 4, 8*/
        _wstring strRTName = TEXT("Post_Blur_");
        strRTName += to_wstring(i);
        _int iDiv = pow(2, i + 1); /* 2 제곱수부터 늘어나는 */
        CRenderTarget* pDownTarget = CRenderTarget::Create(m_pDevice, m_pContext, strRTName, g_iWinSizeX / iDiv, g_iWinSizeY / iDiv, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f));
        assert(pDownTarget);

        m_BlurRenderTargets[i] = pDownTarget;
    }

    /* UpSample용 RenderTarget 생성 */

    for (_uint i = 0; i < iNumRenderTargets / 2; ++i)
    {
        /* blur level 4 기준 : 4, 2, 1 */
        _wstring strRTName = TEXT("Post_Blur_");
        strRTName += to_wstring((iNumRenderTargets /2) + i);
        _int iLevel = m_iBlurLevel - 2 - i;
        _int iDiv = pow(2, iLevel);
        CRenderTarget* pUpTarget = CRenderTarget::Create(m_pDevice, m_pContext, strRTName, g_iWinSizeX / iDiv, g_iWinSizeY / iDiv, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f));

        m_BlurRenderTargets[iNumRenderTargets / 2 + i] = pUpTarget;
    }


    return S_OK;
}

CRenderGroup_PostProcessing* CRenderGroup_PostProcessing::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg)
{
    CRenderGroup_PostProcessing* pInstance = new CRenderGroup_PostProcessing(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Create Failed : CRenderGroup_PostProcessing");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRenderGroup_PostProcessing::Free()
{
    for (auto& pRenderTarget : m_BlurRenderTargets)
    {
        Safe_Release(pRenderTarget);
    }
    m_BlurRenderTargets.clear();

    __super::Free();
}
