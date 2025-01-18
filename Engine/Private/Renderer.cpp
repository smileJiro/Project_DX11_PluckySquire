#include "Renderer.h"
#include "ModelObject.h"
#include "GameInstance.h"

CRenderer::CRenderer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : m_pDevice(_pDevice)
    , m_pContext(_pContext)
    , m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CRenderer::Initialize()
{
    _uint iNumViewport = 1;
    D3D11_VIEWPORT ViewportDesc = {};

    m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);
    m_iOriginViewportWidth = (_uint)ViewportDesc.Width;
    m_iOriginViewportHeight = (_uint)ViewportDesc.Height;

    /* Target Book2D */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Book_2D"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
        return E_FAIL;

    /* Target Diffuse */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
        return E_FAIL;

    /* Target_Normal */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.0f, 0.0f, 0.0f, 1.0f))))
        return E_FAIL;

    /* Target_Depth */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.0f, 0.0f, 0.0f, 1.0f))))
        return E_FAIL;  

    /* Target_Shade */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.0f, 0.0f, 0.0f, 1.0f))))
        return E_FAIL;

    /* Target_Specular */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.0f, 0.0f, 0.0f, 1.0f))))
        return E_FAIL;

    /* Target_LightDepth */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_LightDepth"), g_iShadowWidth, g_iShadowHeight, DXGI_FORMAT_R32_FLOAT, _float4(1.0f, 1.0f, 1.0f, 1.0f))))
        return E_FAIL;

    /* Target_Final */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Final"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* RTV를 모아두는 MRT를 세팅 */

    /* MRT_Book_2D*/
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Book_2D"), TEXT("Target_Book_2D"))))
        return E_FAIL;

    /* MRT_GameObjects */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
        return E_FAIL;

    /* MRT_LightAcc */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
        return E_FAIL;

    /* MRT_Shadow */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Shadow"), TEXT("Target_LightDepth"))))
        return E_FAIL;

    /* MRT_Final */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Final"), TEXT("Target_Final"))))
        return E_FAIL;

    /* 직교 투영으로 그리기 위한 Shader, VIBuffer, Matrix Init */
    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../../EngineSDK/Hlsl/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;

    m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
    if (nullptr == m_pVIBuffer)
        return E_FAIL;

    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 1.f));
    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

    /* DepthStencilView Init */
    if (FAILED(Ready_DepthStencilView(DSV_SHADOW, g_iShadowWidth, g_iShadowHeight)))
        return E_FAIL;

#ifdef _DEBUG
    /* 위치 설정. */
    _float fSizeX = ViewportDesc.Width * 0.2f;
    _float fSizeY = ViewportDesc.Height * 0.2f;
    _float fX = ViewportDesc.Width * 0.2f * 0.5f;
    _float fY = ViewportDesc.Height * 0.2f * 0.5f;
    m_pGameInstance->Ready_RT_Debug(TEXT("Target_Diffuse"),     fX, fY, fSizeX, fSizeY);
    m_pGameInstance->Ready_RT_Debug(TEXT("Target_Normal"),      fX, fY + fSizeY * 1.0f, ViewportDesc.Width * 0.2f, ViewportDesc.Height * 0.2f);
    m_pGameInstance->Ready_RT_Debug(TEXT("Target_Shade"),       fX, fY + fSizeY * 2.0f, ViewportDesc.Width * 0.2f, ViewportDesc.Height * 0.2f);
    m_pGameInstance->Ready_RT_Debug(TEXT("Target_Book_2D"),      fX, fY + fSizeY * 3.0f, ViewportDesc.Width * 0.2f, ViewportDesc.Height * 0.2f);
    m_pGameInstance->Ready_RT_Debug(TEXT("Target_LightDepth"),  fX, fY + fSizeY * 4.0f, ViewportDesc.Width * 0.2f, ViewportDesc.Height * 0.2f);
#endif // _DEBUG

    return S_OK;
}

HRESULT CRenderer::Add_RenderObject(RENDERGROUP _eRenderGroup, CGameObject* _pRenderObject)
{
    if (_eRenderGroup >= RG_END || nullptr == _pRenderObject)
        return E_FAIL;

    m_RenderObjects[_eRenderGroup].push_back(_pRenderObject);

    // ++RefCount
    Safe_AddRef(_pRenderObject);

    return S_OK;
}

HRESULT CRenderer::Draw_RenderObject()
{
    if (FAILED(Render_Book2D()))
    {
        MSG_BOX("Render Failed Render_Book2D");
        return E_FAIL;
    }
    
    if (FAILED(Render_Priority()))
    {
        MSG_BOX("Render Failed Render_Priority");
        return E_FAIL;
    }

    if (FAILED(Render_Shadow()))
    {
        MSG_BOX("Render Failed Render_Shadow");
        return E_FAIL;
    }

    if (FAILED(Render_NonBlend()))
    {
        MSG_BOX("Render Failed Render_NonBlend");
        return E_FAIL;
    }

    if (FAILED(Render_Lights()))
    {
        MSG_BOX("Render Failed Render_Lights");
        return E_FAIL;
    }

    if (FAILED(Render_Final()))
    {
        MSG_BOX("Render Failed Render_Final");
        return E_FAIL;
    }

    if (FAILED(Render_Blend()))
    {
        MSG_BOX("Render Failed Render_Blend");
        return E_FAIL;
    }

    if (FAILED(Render_UI()))
    {
        MSG_BOX("Render Failed Render_UI");
        return E_FAIL;
    }

#ifdef _DEBUG
    if(true == m_isDebugRender)
        if (FAILED(Render_Debug()))
        {
            MSG_BOX("Render Failed Render_Debug");
            return E_FAIL;
        }

    if (KEY_DOWN(KEY::F6))
        m_isDebugRender ^= 1;
#endif


    return S_OK;
}

HRESULT CRenderer::Render_Book2D()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Book_2D"))))
        return E_FAIL;

    for (auto& pRenderObject : m_RenderObjects[RG_BOOK_2D])
    {
        if (nullptr != pRenderObject && true == pRenderObject->Is_Render())
        {
            pRenderObject->Render();
        }
        // Render 수행 후 해당 객체는 RefCount--
        Safe_Release(pRenderObject);
    }
    // 해당 그룹에 속한 모든 Object들에 대해 Render 수행 후, 해당 리스트는 Clear
    m_RenderObjects[RG_BOOK_2D].clear();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
    for (auto& pRenderObject : m_RenderObjects[RG_PRIORITY])
    {
        if (nullptr != pRenderObject && true == pRenderObject->Is_Render())
        {
            pRenderObject->Render();
        }
        // Render 수행 후 해당 객체는 RefCount--
        Safe_Release(pRenderObject);
    }
    // 해당 그룹에 속한 모든 Object들에 대해 Render 수행 후, 해당 리스트는 Clear
    m_RenderObjects[RG_PRIORITY].clear();

    return S_OK;
}

HRESULT CRenderer::Render_Shadow()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Shadow"), m_pShadowDepthStencilView)))
        return E_FAIL;

    if (FAILED(SetUp_Viewport(g_iShadowWidth, g_iShadowHeight)))
        return E_FAIL;

    /* Render */
    for (auto& pRenderObject : m_RenderObjects[RG_SHADOW])
    {
        if (nullptr != pRenderObject && true == pRenderObject->Is_Render())
            static_cast<CModelObject*>(pRenderObject)->Render_Shadow();

        Safe_Release(pRenderObject);
    }
    m_RenderObjects[RG_SHADOW].clear();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    /* Origin Viewport Setup */
    if (FAILED(SetUp_Viewport(m_iOriginViewportWidth, m_iOriginViewportHeight)))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
    /* Diffuse, Normal 등등에 대한 정보를 각각의 RTV에 저장하는 단계. */
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects")))) // MRT_GameObjects 라는 타겟에 각각 diffuse, normal을 저장하기 위해 바인딩하는 과정. 
        return E_FAIL;

    for (auto& pRenderObject : m_RenderObjects[RG_NONBLEND])
    {
        if (nullptr != pRenderObject && true == pRenderObject->Is_Render())
            pRenderObject->Render();
       
        // Render 수행 후 해당 객체는 RefCount--
        Safe_Release(pRenderObject);
    }
    // 해당 그룹에 속한 모든 Object들에 대해 Render 수행 후, 해당 리스트는 Clear
    m_RenderObjects[RG_NONBLEND].clear();

    if (FAILED(m_pGameInstance->End_MRT())) // MRT에 정보들을 모두 저장했다면, 기존의 BackBuffer로 RTV를 변경. 
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Lights()
{
    /* Diffuse, Normal 등의 정보를 기반으로 Shade RTV를 구성하는 단계 */

    /* 1. shade MRT 세팅 */
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_LightAcc")))) // 조명별 연산의 결과를 블렌드하며 누적시키는 RTV
        return E_FAIL;

    /* 2. 직교투영 Matrix 정보 전달. */
    m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
    m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
    m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

    /* 3. 조명연산을 위해 픽셀의 좌표를 월드까지 내리는 과정에서 필요한 데이터 및 카메라의 위치 벡터 */
    m_pShader->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_TransformInverseFloat4x4(CPipeLine::D3DTS_VIEW));
    m_pShader->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_TransformInverseFloat4x4(CPipeLine::D3DTS_PROJ));
    m_pShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4));
    m_pShader->Bind_RawValue("g_fFarZ", m_pGameInstance->Get_FarZ(), sizeof(_float));

    /* 4. Normal Texture Bind */
    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, "g_NormalTexture", TEXT("Target_Normal"))))
        return E_FAIL;
    /* 5. Depth Texture Bind */
    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, "g_DepthTexture", TEXT("Target_Depth"))))
        return E_FAIL;

    /* 4. 사각형 정점 정보 바인딩 */
    m_pVIBuffer->Bind_BufferDesc();

    if (FAILED(m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer)))
        return E_FAIL;

    /* 5. 렌더 종료 시, 기존 Back Buffer RTV로 RTV 변경. */
    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Final()
{
    /* 최종 화면을 그려내는 단계 */

    // Render_Light 단계에서 직교투영 사각형을 그리기위한 데이터는 전달되어서 그대로 남아있는 상태라 안해줘도 되긴 하지만, 추후 사이에 별도의 작업이 생길수도있으니 안전성을 위해 바인드.
    m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
    m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
    m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

    /* 픽셀을 월드위치까지 변환하기 위한 역행렬 바인딩 */
    m_pShader->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_TransformInverseFloat4x4(CPipeLine::D3DTS_VIEW));
    m_pShader->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_TransformInverseFloat4x4(CPipeLine::D3DTS_PROJ));

    m_pShader->Bind_Matrix("g_LightViewMatrix", m_pGameInstance->Get_Shadow_Transform_Ptr(CShadow::D3DTS_VIEW));
    m_pShader->Bind_Matrix("g_LightProjMatrix", m_pGameInstance->Get_Shadow_Transform_Ptr(CShadow::D3DTS_PROJ));

    /* Diffuse 와 Shade RTV를 바인드하고 이제 이 두 데이터를 가지고 최종 화면을 그려낼 것 이다. */
    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, "g_DiffuseTexture", TEXT("Target_Diffuse"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, "g_ShadeTexture", TEXT("Target_Shade"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, "g_SpecularTexture", TEXT("Target_Specular"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, "g_LightDepthTexture", TEXT("Target_LightDepth"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, "g_DepthTexture", TEXT("Target_Depth"))))
        return E_FAIL;

    /* Final Pass */
    m_pShader->Begin((_uint)PASS_DEFERRED::FINAL);

    m_pVIBuffer->Bind_BufferDesc();

    m_pVIBuffer->Render();

    return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
    for (auto& pRenderObject : m_RenderObjects[RG_BLEND])
    {
        if (nullptr != pRenderObject && true == pRenderObject->Is_Render())
        {
            pRenderObject->Render();
        }
        // Render 수행 후 해당 객체는 RefCount-- 
        Safe_Release(pRenderObject);
    }
    // 해당 그룹에 속한 모든 Object들에 대해 Render 수행 후, 해당 리스트는 Clear
    m_RenderObjects[RG_BLEND].clear();


    return S_OK;
}

#ifdef _DEBUG

HRESULT CRenderer::Render_Debug()
{
    for (auto& pDebugCom : m_DebugComponents)
    {
        if (nullptr != pDebugCom)
            pDebugCom->Render();

        Safe_Release(pDebugCom);
    }

    m_DebugComponents.clear();

    //if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
    //    return E_FAIL;
    //if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
    //    return E_FAIL;

    //if (FAILED(m_pVIBuffer->Bind_BufferDesc()))
    //    return E_FAIL;

    //if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_Book_2D"), m_pShader, m_pVIBuffer)))
    //    return E_FAIL;
    //if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_GameObjects"), m_pShader, m_pVIBuffer)))
    //    return E_FAIL;
    //if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_LightAcc"), m_pShader, m_pVIBuffer)))
    //    return E_FAIL;
    //if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_Shadow"), m_pShader, m_pVIBuffer)))
    //    return E_FAIL;
    //if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_Final"), m_pShader, m_pVIBuffer)))
    //    return E_FAIL;

    return S_OK;
}


#endif // _DEBUG

HRESULT CRenderer::Render_UI()
{
    for (auto& pRenderObject : m_RenderObjects[RG_UI])
    {
        if (nullptr != pRenderObject)
        {
            pRenderObject->Render();
        }
        // Render 수행 후 해당 객체는 RefCount-- 
        Safe_Release(pRenderObject);
    }
    // 해당 그룹에 속한 모든 Object들에 대해 Render 수행 후, 해당 리스트는 Clear
    m_RenderObjects[RG_UI].clear();

    return S_OK;
}

HRESULT CRenderer::Ready_DepthStencilView(DSVTYPE _eType, _uint _iWidth, _uint _iHeight)
{
    if (nullptr == m_pDevice)
        return E_FAIL;

    ID3D11Texture2D* pDepthStencilTexture = nullptr;

    D3D11_TEXTURE2D_DESC TextureDesc;
    ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
    
    /* RTV의 사이즈와 DSV의 사이즈가 일치 되어야한다. */
    TextureDesc.Width = _iWidth;
    TextureDesc.Height = _iHeight;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.SampleDesc.Count = 1;

    TextureDesc.Usage = D3D11_USAGE_DEFAULT;
    TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    TextureDesc.CPUAccessFlags = 0;
    TextureDesc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
        return E_FAIL;

    switch (_eType)
    {
    case Engine::CRenderer::DSV_SHADOW:
        if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pShadowDepthStencilView)))
            return E_FAIL;
        break;
    default:
        break;
    }

    Safe_Release(pDepthStencilTexture);

    return S_OK;
}

HRESULT CRenderer::SetUp_Viewport(_uint _iWidth, _uint _iHeight)
{
    D3D11_VIEWPORT			ViewPortDesc;
    ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
    ViewPortDesc.TopLeftX = 0;
    ViewPortDesc.TopLeftY = 0;
    ViewPortDesc.Width = (_float)_iWidth;
    ViewPortDesc.Height = (_float)_iHeight;
    ViewPortDesc.MinDepth = 0.f;
    ViewPortDesc.MaxDepth = 1.f;

    m_pContext->RSSetViewports(1, &ViewPortDesc);
    return S_OK;
}

CRenderer* CRenderer::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CRenderer* pInstance = new CRenderer(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CRenderer");
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CRenderer::Free()
{
    __super::Free();

#ifdef _DEBUG

    for (auto& pDebugCom : m_DebugComponents)
        Safe_Release(pDebugCom);
    m_DebugComponents.clear();

#endif // _DEBUG Components

    for (_uint i = 0; i < RG_END; ++i)
    {
        for (auto& pRenderObject : m_RenderObjects[i])
        {
            Safe_Release(pRenderObject);
        }
        m_RenderObjects[i].clear();
    }


    Safe_Release(m_pShader);
    Safe_Release(m_pVIBuffer);

    Safe_Release(m_pShadowDepthStencilView);

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
}
