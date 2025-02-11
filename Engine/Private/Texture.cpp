#include "Texture.h"
#include "Shader.h"


CTexture::CTexture(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
	, m_iNumSRVs(0)
	, m_SRVs{}
{
}

CTexture::CTexture(const CTexture& _Prototype)
	: CComponent(_Prototype)
	, m_iNumSRVs(_Prototype.m_iNumSRVs)
	, m_SRVs{ _Prototype.m_SRVs }
	, m_SRVNames(_Prototype.m_SRVNames)
{
	for (auto& pSRV : m_SRVs)
		Safe_AddRef(pSRV);

}

HRESULT CTexture::Initialize_Prototype(const _tchar* _pTextureFilePath, _uint _iNumTextures, _bool _isSRGB, _bool _isCubeMap)
{
	/* TextureFilePath 에서 확장자의 이름, %d >>> 숫자로 바꾸기, 등을 수행 후 Load 하고 m_SRVs 에 Pushback */
	m_iNumSRVs = _iNumTextures;

	_tchar szTextureFilePath[MAX_PATH] = TEXT("");

	for (_uint i = 0; i < m_iNumSRVs; ++i)
	{
		// %d 에 i를 대입한 후, szTextureFilePath 구성.
		// 만약 Single_Texture 라면, i 대입하는 구문이 생략됌.
		wsprintf(szTextureFilePath, _pTextureFilePath, i);

		/* wsplitpath_s : 드라이브 경로, 디렉토리경로, 파일이름, 파일확장자 중 원하는 데이터들을 분리하여 추출해주는 함수 */
		_tchar szEXT[MAX_PATH] = TEXT("");
		_wsplitpath_s(szTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);

		HRESULT hr = {};

		ID3D11ShaderResourceView* pSRV = { nullptr };

		if (false == lstrcmpW(szEXT, TEXT(".dds")))
		{
			if (_isCubeMap) 
			{
				UINT miscFlags = 0;
				// CubeMap인 경우 해당 Flag를 추가하여 TextureFile Create
				miscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;

				CreateDDSTextureFromFileEx(m_pDevice, szTextureFilePath, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, miscFlags,
										   DDS_LOADER_FLAGS(false), nullptr, &pSRV, NULL);

#ifdef _DEBUG
				D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
				pSRV->GetDesc(&srvDesc);
#endif // _DEBUG
			}
			else
			{
				if (true == _isSRGB)
				{
					hr = DirectX::CreateDDSTextureFromFileEx(
						m_pDevice,               // Direct3D 장치
						szTextureFilePath,             // DDS 파일 경로
						0,                       // 최대 텍스처 크기 (0은 제한 없음)
						D3D11_USAGE_DEFAULT,     // 리소스 사용 방법
						D3D11_BIND_SHADER_RESOURCE, // 바인딩 플래그
						0,                       // CPU 접근 플래그
						0,                       // 기타 플래그
						DirectX::DDS_LOADER_FORCE_SRGB, // sRGB 강제 적용
						nullptr,                 // 생성된 텍스처 리소스 (필요 시 포인터 제공)
						&pSRV                    // 생성된 셰이더 리소스 뷰
					);
				}
				else
				{
					hr = DirectX::CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);
				}
				//hr = DirectX::CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);
			}

		}
		else if (false == lstrcmpW(szEXT, TEXT(".tga")))
		{

		}
		else
		{
			hr = DirectX::CreateWICTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);
		}

		if (FAILED(hr))
			return E_FAIL;

		// 제대로 Load 되었다면 push_back
		m_SRVs.push_back(pSRV);
	}

	return S_OK;
}

HRESULT CTexture::Initialize_Prototype(const _char* _szTextureFilePath, _uint _iNumTextures, _bool _isSRGB, _bool _isCubeMap)
{
	m_iNumSRVs = _iNumTextures;

	_char szTextureFullPath[MAX_PATH] = "";

	for (size_t i = 0; i < m_iNumSRVs; ++i)
	{
		sprintf_s(szTextureFullPath, _szTextureFilePath, i);

		_char szEXT[MAX_PATH] = "";

		_splitpath_s(szTextureFullPath, nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);

		_tchar wszFullPath[MAX_PATH] = L"";

		MultiByteToWideChar(CP_ACP, 0, szTextureFullPath, (_uint)strlen(szTextureFullPath), wszFullPath, MAX_PATH);

		HRESULT hr = {};

		ID3D11ShaderResourceView* pSRV = { nullptr };

		if (false == strcmp(szEXT, ".dds"))
		{
			if (_isCubeMap)
			{
				UINT miscFlags = 0;
				// CubeMap인 경우 해당 Flag를 추가하여 TextureFile Create
				miscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
				CreateDDSTextureFromFileEx(m_pDevice, wszFullPath, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, miscFlags,
					DDS_LOADER_FLAGS(false), nullptr, &pSRV, NULL);


#ifdef _DEBUG
				D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
				pSRV->GetDesc(&srvDesc);
#endif // _DEBUG
			}
			else
			{
				if (true == _isSRGB)
				{
					hr = DirectX::CreateDDSTextureFromFileEx(
						m_pDevice,               // Direct3D 장치
						wszFullPath,             // DDS 파일 경로
						0,                       // 최대 텍스처 크기 (0은 제한 없음)
						D3D11_USAGE_DEFAULT,     // 리소스 사용 방법
						D3D11_BIND_SHADER_RESOURCE, // 바인딩 플래그
						0,                       // CPU 접근 플래그
						0,                       // 기타 플래그
						DirectX::DDS_LOADER_FORCE_SRGB, // sRGB 강제 적용
						nullptr,                 // 생성된 텍스처 리소스 (필요 시 포인터 제공)
						&pSRV                    // 생성된 셰이더 리소스 뷰
					);
				}
				else
				{
					hr = DirectX::CreateDDSTextureFromFile(m_pDevice, wszFullPath, nullptr, &pSRV);
				}


			}
		}
		else if (false == strcmp(szEXT, ".tga"))
		{
			hr = E_FAIL;
		}
		else
		{
			hr = DirectX::CreateWICTextureFromFile(m_pDevice, wszFullPath, nullptr, &pSRV);
		}

		if (FAILED(hr))
		{
			return E_FAIL;
		}

		m_SRVs.push_back(pSRV);
	}

	return S_OK;
}

HRESULT CTexture::Initialize(void* _pArg)
{
	return S_OK;
}

HRESULT CTexture::Bind_ShaderResource(CShader* _pShader, const _char* _pConstantName, _uint _iSRVIndex)
{
	if (_iSRVIndex >= m_iNumSRVs)
		return E_FAIL;


	_pShader->Bind_SRV(_pConstantName, m_SRVs[_iSRVIndex]);
	return S_OK;
}

ID3D11ShaderResourceView* CTexture::Get_SRV(const _wstring _strTextureName, _uint* _pIndex)
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

const _float2 CTexture::Get_Size(_uint _iSRVIndex)
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

CTexture* CTexture::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _char* _szTextureFilePath, _uint _iNumTextures, _bool _isCubeMap)
{
	CTexture* pInstance = new CTexture(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_szTextureFilePath, _iNumTextures, _isCubeMap)))
	{
		MSG_BOX("Failed to Created : CTexture");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CTexture* CTexture::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	return new CTexture(_pDevice, _pContext);

}
CTexture* CTexture::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pTextureFilePath, _uint _iNumTextures, _bool _isCubeMap)
{
	CTexture* pInstance = new CTexture(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_pTextureFilePath, _iNumTextures, _isCubeMap)))
	{
		MSG_BOX("Failed to Created : CTexture");
		Safe_Release(pInstance);
	}
	return pInstance;
}
CComponent* CTexture::Clone(void* pArg)
{
	CTexture* pInstance = new CTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
	for (auto& pSRV : m_SRVs)
		Safe_Release(pSRV);

	m_SRVs.clear();

	__super::Free();
}
