#include "Compute_Shader.h"

CCompute_Shader::CCompute_Shader(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CComponent(_pDevice, _pContext)
{
}

CCompute_Shader::CCompute_Shader(const CCompute_Shader& _Prototype)
    : CComponent(_Prototype)
    , m_pComputeShader(_Prototype.m_pComputeShader)
{
    Safe_AddRef(m_pComputeShader);
}

HRESULT CCompute_Shader::Initialize_Prototype(const _tchar* _pShaderFilePath, const _char* _szEntry)
{
    _uint iHlslFlag = 0;

#if defined(DEBUG) || defined(_DEBUG)
    iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ID3DBlob* pShaderBlob = nullptr;
    ID3DBlob* pErrorBlob = nullptr;

    HRESULT hr = D3DCompileFromFile(
        _pShaderFilePath, 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, _szEntry,
        "cs_5_0", iHlslFlag, 0, &pShaderBlob, &pErrorBlob);

    if (FAILED(hr))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateComputeShader(pShaderBlob->GetBufferPointer(),
        pShaderBlob->GetBufferSize(), NULL, &m_pComputeShader)))
        return E_FAIL;



    return S_OK;
}

HRESULT CCompute_Shader::Initialize(void* _pArg)
{
    return S_OK;
}

HRESULT CCompute_Shader::Compute(ID3D11UnorderedAccessView** _pUAV, _uint _iThreadGroupCountX, _uint _iThreadGroupCountY, _uint _iThreadGroupCountZ)
{
    m_pContext->CSSetUnorderedAccessViews(0, 1, _pUAV, nullptr);

    m_pContext->CSSetShader(m_pComputeShader, 0, 0);
    m_pContext->Dispatch(_iThreadGroupCountX, _iThreadGroupCountY, _iThreadGroupCountZ);


    ID3D11ShaderResourceView* nullSRV[6] = {
        0,
    };
    m_pContext->CSSetShaderResources(0, 6, nullSRV);
    ID3D11UnorderedAccessView* nullUAV[6] = {
        0,
    };
    m_pContext->CSSetUnorderedAccessViews(0, 6, nullUAV, NULL);
    m_pContext->CSSetShader(nullptr, 0, 0);

    return S_OK;
}

CCompute_Shader* CCompute_Shader::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pShaderFilePath, const _char* _szEntry)
{
    CCompute_Shader* pInstance = new CCompute_Shader(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype(_pShaderFilePath, _szEntry)))
    {
        MSG_BOX("Failed to Created : CCompute_Shader");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CComponent* CCompute_Shader::Clone(void* _pArg)
{
    CCompute_Shader* pInstance = new CCompute_Shader(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CCompute_Shader");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCompute_Shader::Free()
{
    Safe_Release(m_pComputeShader);

    __super::Free();
}
