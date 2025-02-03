#include "MipMapTexture.h"
#include "Shader.h"

CMipMapTexture::CMipMapTexture(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CTexture(_pDevice, _pContext)
	, m_Textures{}
{
}

CMipMapTexture::CMipMapTexture(const CMipMapTexture& _Prototype)
	: CTexture(_Prototype)
	, m_Textures(_Prototype.m_Textures)
{
	for (auto& pSRV : m_SRVs)
		Safe_AddRef(pSRV);
}

HRESULT CMipMapTexture::Initialize(void* _pArg)
{
	return S_OK;
}

HRESULT CMipMapTexture::Bind_ShaderResource(CShader* _pShader, const _char* _pConstantName, _uint _iSRVIndex)
{
	if (_iSRVIndex >= m_iNumSRVs)
		return E_FAIL;


	_pShader->Bind_SRV(_pConstantName, m_SRVs[_iSRVIndex]);

	return S_OK;
}

ID3D11ShaderResourceView* CMipMapTexture::Get_SRV(const _wstring _strTextureName, _uint* _pIndex)
{
	ID3D11ShaderResourceView* pReturnTexture = nullptr;

	_uint iLoopCnt = 0;
	for (auto& strTextureName : m_SRVNames)
	{
		if (_strTextureName == strTextureName &&
			iLoopCnt < m_iNumSRVs &&
			iLoopCnt < (_uint)m_SRVs.size()
			)
			pReturnTexture = m_SRVs[iLoopCnt];
		if (nullptr != pReturnTexture)
			break;
		iLoopCnt++;
	}

	if (nullptr != _pIndex)
		*_pIndex = iLoopCnt;

	return pReturnTexture;
}

ID3D11ShaderResourceView* CMipMapTexture::Get_SRV(_uint _iSRVIndex)
{
	if (_iSRVIndex >= m_SRVs.size())
		return nullptr;

	//if (nullptr == m_SRVs[_iSRVIndex])
	//return m_SRVs[_iSRVIndex];

	return nullptr;
}

const _float2 CMipMapTexture::Get_Size(_uint _iSRVIndex)
{
	_float2 fReturn = _float2(-1.f, -1.f);
	if (_iSRVIndex >= m_iNumSRVs)
		return fReturn;
	ID3D11Resource* pResource = nullptr;

	m_SRVs[_iSRVIndex]->GetResource(&pResource);
	if (pResource)
	{
		ID3D11Texture2D* pTexture2D = nullptr;
		HRESULT hr = pResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pTexture2D));
		pResource->Release();

		if (SUCCEEDED(hr) && pTexture2D)
		{
			D3D11_TEXTURE2D_DESC desc;
			pTexture2D->GetDesc(&desc);
			fReturn.x = (_float)desc.Width;
			fReturn.y = (_float)desc.Height;
		}
	}
	return fReturn;
}

void CMipMapTexture::Add_Texture(ID3D11ShaderResourceView* _pSRV, const _wstring strSRVName)
{

	m_SRVNames.push_back(strSRVName);
	ID3D11Resource* pResource = nullptr;

	_pSRV->GetResource(&pResource);

	if (nullptr != pResource)
	{

		ID3D11Texture2D* pTexture = nullptr;
		pResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&pTexture);

		if (nullptr != pResource)
		{
			Safe_Release(_pSRV);
			Safe_Release(pResource);
			m_Textures.push_back(pTexture);
		}
	}
	m_iNumSRVs++;
	m_SRVNames.resize(m_iNumSRVs);
}


CMipMapTexture* CMipMapTexture::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	return new CMipMapTexture(_pDevice, _pContext);

}
CComponent* CMipMapTexture::Clone(void* pArg)
{
	CMipMapTexture* pInstance = new CMipMapTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMipMapTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMipMapTexture::Free()
{
	for (auto& pSRV : m_SRVs)
		Safe_Release(pSRV);

	m_SRVs.clear();

	__super::Free();
}
