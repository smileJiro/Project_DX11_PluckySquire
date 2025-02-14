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

    Set_Active(false);
    return S_OK;
}

HRESULT CRenderGroup_PostProcessing::Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer)
{
    _pRTShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_WorldMatrix_Renderer());
    _pRTShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_ViewMatrix_Renderer());
    _pRTShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_ProjMatrix_Renderer());


    /* 렌더 콜 한번에 여러번 그리게할거야 for 문 돌면서 renderTarget을 등록하고, srv를 다른 렌더타겟의 srv로 */
    static _int iBindMaxSizeRTs = 8;
    static vector<CRenderTarget*> DrawMRT;
    for (int i = 0; i < m_BlurRenderTargets.size(); ++i)
    {
        // 1. Blur RenderTarget을 MRT로 만들고 바인딩.
        DrawMRT.push_back(m_BlurRenderTargets[i]);

        m_pGameInstance->Begin_MRT(DrawMRT, m_DSVs[i]);
        // 2. Change ViewportSize
        _float2 vSize = m_BlurRenderTargets[i]->Get_Size();
        Setup_Viewport(vSize);
        // 3. Bind TexelSize 
        _float2 vTexelSize = {};
        vTexelSize.x = 1.0f / vSize.x;
        vTexelSize.y = 1.0f / vSize.y;
        _pRTShader->Bind_RawValue("g_TexelSize", &vTexelSize, sizeof(_float2));

        // 4. 다운 샘플링 할 SRV Bind 
        if (0 == i)
        {
            // i 가 0인 경우 
            if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_SrcBlurTargetTexture", TEXT("Target_Lighting"))))
                return E_FAIL;
        }
        else
        {
            ID3D11ShaderResourceView* pSrcBlurSRV = m_BlurRenderTargets[i - 1]->Get_SRV();
            if (FAILED(_pRTShader->Bind_SRV("g_SrcBlurTargetTexture", pSrcBlurSRV)))
                return E_FAIL;
        }

        // 5. 패스 설정
        if (i < m_BlurRenderTargets.size() / 3)
        {
            _pRTShader->Begin((_uint)PASS_DEFERRED::PBR_BLUR_DOWN);
        }
        else
        {
            _pRTShader->Begin((_uint)PASS_DEFERRED::PBR_BLUR_UP);
        }
        _pRTBuffer->Bind_BufferDesc(); /* 이거 한번만해줘도 됨*/
        _pRTBuffer->Render();

        if (FAILED(m_pGameInstance->End_MRT()))
            return E_FAIL;

        DrawMRT.clear();
    }


    //Setup_Viewport(_float2((_float)m_pGameInstance->Get_ViewportWidth(), (_float)m_pGameInstance->Get_ViewportHeight()));



    
    return S_OK;
}

HRESULT CRenderGroup_PostProcessing::Ready_BlurRenderTarget()
{
    /* Blur Level에 따라 사이즈에 맞는 RenderTarget을 생성 */ /* 블러레벨이 4라면, 총 6장 생성 */
    _uint iNumRenderTargets = (m_iBlurLevel - 1) * 2; // Down샘플용 절반, Up샘플용 절반. 마지막 인덱스의 RenderTarget이 최종 화면
    m_BlurRenderTargets.resize(iNumRenderTargets);
    m_DSVs.resize(iNumRenderTargets);
    /* DownSample용 RenderTarget 생성 */
    for (_uint i = 0; i < iNumRenderTargets / 2; ++i)
    {
        /* blur level 4 기준 : 2, 4, 8*/
        _wstring strRTName = TEXT("Post_Blur_");
        strRTName += to_wstring(i);
        _int iDiv = (_int)pow(2, i + 1); /* 2 제곱수부터 늘어나는 */
        CRenderTarget* pDownTarget = CRenderTarget::Create(m_pDevice, m_pContext, strRTName, g_iWinSizeX / iDiv, g_iWinSizeY / iDiv, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f));
        assert(pDownTarget);

        m_BlurRenderTargets[i] = pDownTarget;
        m_pGameInstance->Create_DSV(g_iWinSizeX / iDiv, g_iWinSizeY / iDiv, &(m_DSVs[i]));
    }

    /* UpSample용 RenderTarget 생성 */
    for (_uint i = 0; i < iNumRenderTargets / 2; ++i)
    {
        /* blur level 4 기준 : 4, 2, 1 */
        _wstring strRTName = TEXT("Post_Blur_");
        strRTName += to_wstring((iNumRenderTargets /2) + i);
        _int iLevel = m_iBlurLevel - 2 - i;
        _int iDiv = (_int)pow(2, iLevel);
        CRenderTarget* pUpTarget = CRenderTarget::Create(m_pDevice, m_pContext, strRTName, g_iWinSizeX / iDiv, g_iWinSizeY / iDiv, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f));

        m_BlurRenderTargets[iNumRenderTargets / 2 + i] = pUpTarget;
        m_pGameInstance->Create_DSV(g_iWinSizeX / iDiv, g_iWinSizeY / iDiv, &(m_DSVs[iNumRenderTargets / 2 + i]));
    }

    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_PBRBlurFinal"), m_BlurRenderTargets[iNumRenderTargets - 1])))
        return E_FAIL;

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
