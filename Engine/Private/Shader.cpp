#include "Shader.h"
CShader::CShader(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CComponent(_pDevice, _pContext)
    , m_pEffect(nullptr)
    , m_iNumLayouts(0)
    , m_InputLayouts{}
{
}

CShader::CShader(const CShader& _Prototype)
    : CComponent(_Prototype)
    , m_pEffect(_Prototype.m_pEffect)
    , m_iNumLayouts(_Prototype.m_iNumLayouts)
    , m_InputLayouts{ _Prototype .m_InputLayouts }
{
    Safe_AddRef(m_pEffect);
    for (auto& InputLayout : m_InputLayouts)
    {
        Safe_AddRef(InputLayout);
    }
}

HRESULT CShader::Initialize_Prototype(const _tchar* _pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* _pElementsDesc, const _uint _iNumElements)
{
    /* 셰이더 코드를 컴파일 하고, 해당 셰이더 객체의 정보를 기반으로 InputLayout을 생성 후 저장. */
    _uint iHlslFlag = 0;

#ifdef _DEBUG
    iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif // _DEBUG

    /* 셰이더 파일 컴파일 도중 오류 발생 시 해당하는 정보를 문자열로 기록하여 리턴함. */
    ID3DBlob* pBlobBuffer = nullptr;
    /* 셰이더 컴파일 */
    if (FAILED(D3DX11CompileEffectFromFile(_pShaderFilePath, 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0, m_pDevice, &m_pEffect, &pBlobBuffer)))
        return E_FAIL;

    /* technique desc >> pass desc >> shader desc 진입 하여 InputSignatue, InputSignaturesize 를 가져온다. */
    
    // 1. technique desc 에 접근하여 pass의 개수를 가져와 멤버에 채운다.
    ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
    if (nullptr == pTechnique)
        return E_FAIL;

    D3DX11_TECHNIQUE_DESC TechniqueDesc = {};
    pTechnique->GetDesc(&TechniqueDesc);
    m_iNumLayouts = TechniqueDesc.Passes;

    // 2. 패스의 개수만큼 반복문을 수행하며 InputLayout을 생성한다.
    for (_uint i = 0; i < m_iNumLayouts; ++i)
    {
        ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(i);
        if (nullptr == pPass)
            return E_FAIL;

        D3DX11_PASS_DESC PassDesc = {};
        pPass->GetDesc(&PassDesc);

        ID3D11InputLayout* pInputLayout = nullptr;
        
        if (FAILED(m_pDevice->CreateInputLayout(_pElementsDesc, _iNumElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pInputLayout)))
            return E_FAIL;

        // 3. InputLayout 벡터에 담는다. >>> 추후 해당 InputLayout은 지정한 pass와 연동하여 CShader::Begin() 함수에서 사용 된다.
        m_InputLayouts.push_back(pInputLayout);
    }


    return S_OK;
}

HRESULT CShader::Initialize(void* _pArg)
{
    return S_OK;
}

HRESULT CShader::Begin(_uint _iPassIndex)
{
    /* Object를 렌더링하기 전 PassIndex에 해당하는 InputLayout을 IA에 세팅한다. */
    if (_iPassIndex >= m_iNumLayouts)
        return E_FAIL;

    ID3DX11EffectPass* pPass = m_pEffect->GetTechniqueByIndex(0)->GetPassByIndex(_iPassIndex);
    if (pPass == nullptr)
        return E_FAIL;

    
    // pPass에 담겨있는 데이터가 어떠한 타입의 데이터인지 모른다. 
    // 그래서 해당 pass 를 사용해서 그리기 연산을 수행하겠다! 라는 의미로 Apply() 함수를 호출.
    pPass->Apply(0, m_pContext);

    // 해당 패스에 맞는 InputLayout도 
    m_pContext->IASetInputLayout(m_InputLayouts[_iPassIndex]);
   
    return S_OK;
}

HRESULT CShader::Bind_RawValue(const _char* _pConstantName, const void* _pData, _uint _iLength)
{
    ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(_pConstantName);
    if (nullptr == pVariable)
        return E_FAIL;


    return pVariable->SetRawValue(_pData, 0, _iLength);
}

HRESULT CShader::Bind_Matrix(const _char* _pConstantName, const _float4x4* _pMatrix)
{
    // 상수 데이터 변수의 이름과 전송하고자하는 데이터를 매개변수로 받는다. 

    // 해당 이름의 변수가 있다면 그 변수에 값을 채워줄 수 있는 컴객체를 리턴한다.
    ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(_pConstantName);
    if (nullptr == pVariable)
        return E_FAIL;
    // 해당 자료형이 무슨 자료형인지 나만안다고. >>> 그래서 캐스팅 해줘야함 (AsMatrix() 가 캐스팅 함수)
    ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
    if (nullptr == pMatrixVariable)
        return E_FAIL;
    
    return pMatrixVariable->SetMatrix(reinterpret_cast<const _float*>(_pMatrix));
}

HRESULT CShader::Bind_Matrices(const _char* _pConstantName, const _float4x4* _pMatrix, _uint _iNumMatrices)
{
    ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(_pConstantName);
    if (nullptr == pVariable)
        return E_FAIL;

    ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
    if (nullptr == pMatrixVariable)
        return E_FAIL;

    /* 매트릭스 Array를 바인딩하는 함수가 있다. >>> 배열의 개수를 함께 전달해야한다.*/
    return pMatrixVariable->SetMatrixArray(reinterpret_cast<const _float*>(_pMatrix), 0, _iNumMatrices);



    return S_OK;
}

HRESULT CShader::Bind_SRV(const _char* _pConstantName, ID3D11ShaderResourceView* _pSRV)
{
    ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(_pConstantName);
    if (nullptr == pVariable)
        return E_FAIL;

    ID3DX11EffectShaderResourceVariable* pSRVVariable = pVariable->AsShaderResource();
    if (nullptr == pSRVVariable)
        return E_FAIL;

    return pSRVVariable->SetResource(_pSRV);
}

HRESULT CShader::Bind_SRVs(const _char* _pConstantName, vector<ID3D11ShaderResourceView*>& _SRVs)
{
    // Effect 변수 가져오기
    ID3DX11EffectVariable* pVariables = m_pEffect->GetVariableByName(_pConstantName);
    if (nullptr == pVariables)
        return E_FAIL;

    ID3DX11EffectShaderResourceVariable* pSRVVariables = pVariables->AsShaderResource();
    if (nullptr == pSRVVariables)
        return E_FAIL;

    // 여러 텍스처를 배열로 바인딩
    if (pSRVVariables != nullptr) 
    {
        pSRVVariables->SetResourceArray(_SRVs.data(), 0, static_cast<UINT>(_SRVs.size()));
    }
    return S_OK;
}

CShader* CShader::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* _pElementsDesc, const _uint _iNumElements)
{
    CShader* pInstance = new CShader(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype(_pShaderFilePath, _pElementsDesc, _iNumElements)))
    {
        MSG_BOX("Failed to Created : CShader");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CComponent* CShader::Clone(void* _pArg)
{
    CShader* pInstance = new CShader(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CShader");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CShader::Free()
{
    for (auto& InputLayout : m_InputLayouts)
    {
        Safe_Release(InputLayout);
    }
    m_InputLayouts.clear();

    Safe_Release(m_pEffect);

    __super::Free();
}
