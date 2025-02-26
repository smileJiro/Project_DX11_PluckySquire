#include "Target_Manager.h"
#include "RenderTarget.h"
#include "RenderTarget_MSAA.h"

CTarget_Manager::CTarget_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : m_pDevice(_pDevice)
    , m_pContext(_pContext)
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CTarget_Manager::Initialize()
{
    return S_OK;
}

HRESULT CTarget_Manager::Add_RenderTarget(const _wstring& _strTargetTag, _uint _iWidth, _uint _iHeight, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor, CRenderTarget** _pReturnRT)
{
    if (nullptr != Find_RenderTarget(_strTargetTag))
        return E_FAIL;

    CRenderTarget* pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, _strTargetTag, _iWidth, _iHeight, _ePixelFormat, _vClearColor);
    if (nullptr == pRenderTarget)
        return E_FAIL;



    m_RenderTargets.emplace(_strTargetTag, pRenderTarget);

    if (_pReturnRT != nullptr)
        *_pReturnRT = pRenderTarget;

    return S_OK;
}

HRESULT CTarget_Manager::Add_RenderTarget(const _wstring& _strTargetTag, CRenderTarget* _pRenderTarget)
{
    if (nullptr != Find_RenderTarget(_strTargetTag))
        return E_FAIL;

    m_RenderTargets.emplace(_strTargetTag, _pRenderTarget);
    Safe_AddRef(_pRenderTarget);
    return S_OK;
}

HRESULT CTarget_Manager::Add_RenderTarget_MSAA(const _wstring& _strTargetTag, _uint _iWidth, _uint _iHeight, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor, CRenderTarget** _pReturnRT)
{
    if (nullptr != Find_RenderTarget(_strTargetTag))
        return E_FAIL;

    CRenderTarget_MSAA* pRenderTarget = CRenderTarget_MSAA::Create(m_pDevice, m_pContext, _strTargetTag, _iWidth, _iHeight, _ePixelFormat, _vClearColor);
    if (nullptr == pRenderTarget)
        return E_FAIL;

    m_RenderTargets.emplace(_strTargetTag, pRenderTarget);

    if (_pReturnRT != nullptr)
        *_pReturnRT = pRenderTarget;

    return S_OK;
}

HRESULT CTarget_Manager::Add_MRT(const _wstring& _strMRTTag, const _wstring& _strTargetTag)
{
    /* 1. 렌더타겟을 찾는다. */
    CRenderTarget* pRenderTarget = Find_RenderTarget(_strTargetTag);
    if (nullptr == pRenderTarget)
        return E_FAIL;

    /* 2. 넣고자하는 MRTs를 찾는다. */
    list<CRenderTarget*>* pMRTList = Find_MRT(_strMRTTag);
    if (nullptr == pMRTList)
    {
        /* if MRTs가 존재하지 않는다면, 새롭게 생성 후 insert */
        list<CRenderTarget*> MRTList = { pRenderTarget };

        m_MRTs.emplace(_strMRTTag, MRTList);
    }
    else
    {
        pMRTList->push_back(pRenderTarget);
    }

    Safe_AddRef(pRenderTarget);


    return S_OK;
}

HRESULT CTarget_Manager::Begin_MRT(const _wstring& _strMRTTag, ID3D11DepthStencilView* _pDSV, _bool isClear)
{
    /* 지정한 MRT를 셰이더에 바인딩하는 함수 */
    list<CRenderTarget*>* pMRTList = Find_MRT(_strMRTTag);
    if (nullptr == pMRTList)
        return E_FAIL;

    ID3D11ShaderResourceView* pSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {
        nullptr
    };

    m_pContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRV);

    /* 1. 현재 사용중인 BackBuffer RTV와 DSV 정보를 저장한다.  >>> 추후 다시 원상복구 하기 위해서. */
    m_pContext->OMGetRenderTargets(1, &m_pBackRTV, &m_pOriginalDSV);

    /* 2. MultiRenderTarget을 바인딩하기 위해서는 배열 형태의 데이터로 셰이더에 바인딩해야한다. 미리 배열 형태를 잡아주고 값을 채워 바인딩 할 것이다. */
    ID3D11RenderTargetView* pRenderTargets[8] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

    _uint iNumRenderTargets = 0;
    for (auto& pRenderTarget : *pMRTList)
    {
        if (true == isClear)
            pRenderTarget->Clear();

        pRenderTargets[iNumRenderTargets++] = pRenderTarget->Get_RTV();
    }

    /* 3. MRT 를 바인딩한다. 이때 OriginDSV를 함께 바인딩한다. */
    /* 4. 인자로 들어온 DSV가 nullptr이 아니라면, 인자로 들어온 DSV로 세팅 */
    if (nullptr != _pDSV)
        m_pContext->ClearDepthStencilView(_pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

    m_pContext->OMSetRenderTargets(iNumRenderTargets, pRenderTargets, nullptr == _pDSV ? m_pOriginalDSV : _pDSV);
    
    return S_OK;
}

HRESULT CTarget_Manager::Begin_MRT(const vector<CRenderTarget*>& _MRT, ID3D11DepthStencilView* _pDSV, _bool _isClear)
{
    /* 외부에서 등록한 MRT를 그려주는 BeginMRT */
    /* 지정한 MRT를 셰이더에 바인딩하는 함수 */
    ID3D11ShaderResourceView* pSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {
        nullptr
    };

    m_pContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRV);

    /* 1. 현재 사용중인 BackBuffer RTV와 DSV 정보를 저장한다.  >>> 추후 다시 원상복구 하기 위해서. */
    m_pContext->OMGetRenderTargets(1, &m_pBackRTV, &m_pOriginalDSV);

    /* 2. MultiRenderTarget을 바인딩하기 위해서는 배열 형태의 데이터로 셰이더에 바인딩해야한다. 미리 배열 형태를 잡아주고 값을 채워 바인딩 할 것이다. */
    ID3D11RenderTargetView* pRenderTargets[8] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

    _uint iNumRenderTargets = 0;
    for (auto& pRenderTarget : _MRT)
    {
        if (true == _isClear)
            pRenderTarget->Clear();

        pRenderTargets[iNumRenderTargets++] = pRenderTarget->Get_RTV();
    }

    /* 3. MRT 를 바인딩한다. 이때 OriginDSV를 함께 바인딩한다. */
    /* 4. 인자로 들어온 DSV가 nullptr이 아니라면, 인자로 들어온 DSV로 세팅 */
    if (nullptr != _pDSV)
        m_pContext->ClearDepthStencilView(_pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

    m_pContext->OMSetRenderTargets(iNumRenderTargets, pRenderTargets, nullptr == _pDSV ? m_pOriginalDSV : _pDSV);

    return S_OK;
}

HRESULT CTarget_Manager::End_MRT()
{
    m_pContext->OMSetRenderTargets(1, &m_pBackRTV, m_pOriginalDSV);

    Safe_Release(m_pOriginalDSV);
    Safe_Release(m_pBackRTV);

    return S_OK;
}

HRESULT CTarget_Manager::Clear_MRT(const _wstring& _strMRTTag, ID3D11DepthStencilView* _pDSV, _bool isClear)
{
    /* 지정한 MRT를 셰이더에 바인딩하는 함수 */
    list<CRenderTarget*>* pMRTList = Find_MRT(_strMRTTag);
    if (nullptr == pMRTList)
        return E_FAIL;

    _uint iNumRenderTargets = 0;
    for (auto& pRenderTarget : *pMRTList)
    {
        pRenderTarget->Clear();
    }

    /* 3. MRT 를 바인딩한다. 이때 OriginDSV를 함께 바인딩한다. */
    /* 4. 인자로 들어온 DSV가 nullptr이 아니라면, 인자로 들어온 DSV로 세팅 */
    if (nullptr != _pDSV)
        m_pContext->ClearDepthStencilView(_pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
    else
        m_pContext->ClearDepthStencilView(m_pOriginalDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

    return S_OK;
}

HRESULT CTarget_Manager::Bind_ShaderResource(CShader* _pShader, const _char* _pConstantName, const _wstring& _strTargetTag)
{
    CRenderTarget* pRenderTarget = Find_RenderTarget(_strTargetTag);
    if (nullptr == pRenderTarget)
        return E_FAIL;


    return pRenderTarget->Bind_ShaderResource(_pShader, _pConstantName);
}

HRESULT CTarget_Manager::Copy_RT_Resource(const _wstring& _strTargetTag, ID3D11Texture2D* _pDest)
{
    CRenderTarget* pRenderTarget = Find_RenderTarget(_strTargetTag);
    if (nullptr == pRenderTarget)
        return E_FAIL;


    return pRenderTarget->Copy_Resource(_pDest);
}

HRESULT CTarget_Manager::Copy_BackBuffer_RT_Resource(const _wstring& _strTargetTag)
{
    CRenderTarget* pRenderTarget = Find_RenderTarget(_strTargetTag);
    if (nullptr == pRenderTarget)
        return E_FAIL;

    if (nullptr == m_pBackRTV)
        return E_FAIL;

    ID3D11Resource* pBackBufferResource = nullptr;
    m_pBackRTV->GetResource(&pBackBufferResource);

    m_pContext->CopyResource(pRenderTarget->Get_Resource(), pBackBufferResource);
    return S_OK;
}

HRESULT CTarget_Manager::Resolve_RT_MSAA(const _wstring& _strTargetTag)
{
    CRenderTarget_MSAA* pRenderTarget_MSAA = dynamic_cast<CRenderTarget_MSAA*>(Find_RenderTarget(_strTargetTag));
    if (nullptr == pRenderTarget_MSAA)
        return E_FAIL;

    return pRenderTarget_MSAA->Resolve_MSAA();
}

HRESULT CTarget_Manager::Resolve_MRT_MSAA(const _wstring& _strMRTTag)
{
    list<CRenderTarget*>* pMRTs_MSAA = Find_MRT(_strMRTTag);
    if (nullptr == pMRTs_MSAA)
        return E_FAIL;

    for (auto& pRenderTarget : *pMRTs_MSAA)
    {
        static_cast<CRenderTarget_MSAA*>(pRenderTarget)->Resolve_MSAA();
    }

    return S_OK;
}

HRESULT CTarget_Manager::Erase_RenderTarget(const _wstring& _strTargetTag)
{
    auto iter = m_RenderTargets.find(_strTargetTag);
    if (m_RenderTargets.end() == iter)
        return E_FAIL;

    Safe_Release(iter->second);
    m_RenderTargets.erase(iter);

    return S_OK;
}

HRESULT CTarget_Manager::Erase_MRT(const _wstring& _strMRTTag)
{
    auto iter = m_MRTs.find(_strMRTTag);
    if (m_MRTs.end() == iter)
        return E_FAIL;

    for (auto& pRenderTarget : iter->second)
    {
        Safe_Release(pRenderTarget);
    }
    iter->second.clear();
    m_MRTs.erase(iter);

    return S_OK;
}

ID3D11ShaderResourceView* CTarget_Manager::Get_SRV(const _wstring& _strTargetTag)
{
    CRenderTarget* pRenderTarget = Find_RenderTarget(_strTargetTag);
    if (nullptr == pRenderTarget)
        return nullptr;


    return pRenderTarget->Get_SRV();
}

#ifdef _DEBUG
HRESULT CTarget_Manager::Ready_Debug(const _wstring& _strTargetTag, _float _fX, _float _fY, _float _fSizeX, _float _fSizeY)
{
    CRenderTarget* pRenderTarget = Find_RenderTarget(_strTargetTag);
    if (nullptr == pRenderTarget)
        return E_FAIL;

    return pRenderTarget->Ready_Debug(_fX, _fY, _fSizeX, _fSizeY);
}

HRESULT CTarget_Manager::Render_Debug(const _wstring& _strMRTTag, CShader* _pShader, CVIBuffer_Rect* _pVIBufferRect)
{
    /* 디버그용으로 세팅해둔 RTVs을 순회하며 화면에 렌더한다. */
    list<CRenderTarget*>* pMRTList = Find_MRT(_strMRTTag);
    if (nullptr == pMRTList)
        return E_FAIL;

    for (auto& pRenderTarget : *pMRTList)
        pRenderTarget->Render_Debug(_pShader, _pVIBufferRect);

    return S_OK;
}

#endif //_DEBUG
_float2 CTarget_Manager::Get_RT_Size(const _wstring& _strTargetTag)
{
    CRenderTarget* pRenderTarget = Find_RenderTarget(_strTargetTag);
    if (nullptr == pRenderTarget)
    {
        MSG_BOX("Failed Get_RT_Size (Target_Manager)");
        return _float2();
    }
    return pRenderTarget->Get_Size();
}
CRenderTarget* CTarget_Manager::Find_RenderTarget(const _wstring& _strTargetTag)
{
    auto	iter = m_RenderTargets.find(_strTargetTag);
    if (iter == m_RenderTargets.end())
        return nullptr;
    
    return iter->second;
}

list<CRenderTarget*>* CTarget_Manager::Find_MRT(const _wstring _strMRTTag)
{
    auto iter = m_MRTs.find(_strMRTTag);
    if (iter == m_MRTs.end())
        return nullptr;

    /* list* 타입으로 리턴 nullptr 체크 등의 편리함을 위해 */
    return &iter->second;
}

CTarget_Manager* CTarget_Manager::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CTarget_Manager* pInstance = new CTarget_Manager(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CTarget_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTarget_Manager::Free()
{
    for (auto& Pair : m_MRTs)
    {
        for (auto& pRenderTarget : Pair.second)
            Safe_Release(pRenderTarget);

        Pair.second.clear();
    }
    m_MRTs.clear();

    for (auto& Pair : m_RenderTargets)
        Safe_Release(Pair.second);

    m_RenderTargets.clear();

    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
    __super::Free();

}
