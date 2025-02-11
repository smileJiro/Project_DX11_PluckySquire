#include "Material.h"
#include "GameInstance.h"

CMaterial::CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComponent(pDevice, pContext)
{
	ZeroMemory(m_MaterialTextures, sizeof(m_MaterialTextures));
}

CMaterial::CMaterial(const CMaterial& _Rhs)
	:CComponent(_Rhs)
{
	for (_uint i = 0; i < aiTextureType_UNKNOWN; ++i)
	{
		m_MaterialTextures[i] = _Rhs.m_MaterialTextures[i];
		Safe_AddRef(m_MaterialTextures[i]);
	}


	// 이 두 데이터는 복사하지 않는다. 추후 CloneInitialize
	// CONST_PS m_tPixelConstData = {};
	//ID3D11Buffer* m_pPixeConstBuffer = nullptr;
}

HRESULT CMaterial::Initialize(const _char* szDirPath, ifstream& inFile)
{
	//cout << m_iNumSRVs << endl;
	for (_uint texIdx = 1; texIdx < aiTextureType_UNKNOWN; texIdx++)
	{
		_uint iNumSRVs = 0;
		inFile.read(reinterpret_cast<char*>(&iNumSRVs), sizeof(_uint));
		for (_uint iSRVIndex = 0; iSRVIndex < iNumSRVs; iSRVIndex++)
		{

			_uint strLen;
 			inFile.read(reinterpret_cast<char*>(&strLen), sizeof(_uint));
			//cout << strLen << endl;
			_char* strTexturePath = new _char[strLen + 1];
			inFile.read(strTexturePath, sizeof(_char) * strLen);
			strTexturePath[strLen] = '\0';
			//cout << strTexturePath << endl;
			string strTexturePathStr = strTexturePath;

			_char		szFileName[MAX_PATH] = "";
			_char		szExt[MAX_PATH] = "";

 			_char		szFullPath[MAX_PATH] = "";

			_splitpath_s(strTexturePath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			strcat_s(szFullPath, szDirPath);
			strcat_s(szFullPath, szFileName);
			if (false == strcmp(szExt, ".tga"))
			{
				strcpy_s(szExt, ".dds");
			}
			strcat_s(szFullPath, szExt);

			_tchar		szWideFullPath[MAX_PATH] = TEXT("");
			_tchar		szWideName[MAX_PATH] = TEXT("");
			_tchar		szWideExt[MAX_PATH] = TEXT("");
			MultiByteToWideChar(CP_ACP, 0, szFullPath, (int)strlen(szFullPath), szWideFullPath, MAX_PATH);
			MultiByteToWideChar(CP_ACP, 0, szFileName, (int)strlen(szFileName), szWideName, MAX_PATH);
			MultiByteToWideChar(CP_ACP, 0, szExt, (int)strlen(szExt), szWideExt, MAX_PATH);

			ID3D11ShaderResourceView* pSRV = { nullptr };
			HRESULT		hr = {};
			if (false == strcmp(szExt, ".dds") || false == strcmp(szExt, ".DDS"))
			{
				if (aiTextureType_DIFFUSE == texIdx || aiTextureType_EMISSIVE == texIdx || aiTextureType_EMISSION_COLOR == texIdx)
				{
					hr = DirectX::CreateDDSTextureFromFileEx(
						m_pDevice,               // Direct3D 장치
						szWideFullPath,             // DDS 파일 경로
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
					hr = DirectX::CreateDDSTextureFromFile(m_pDevice, szWideFullPath, nullptr, &pSRV);
				}
			}
			else if (false == strcmp(szExt, ".tga"))
				return E_FAIL;
			else
				hr = CreateWICTextureFromFile(m_pDevice, szWideFullPath, nullptr, &pSRV);

			delete[] strTexturePath;
			if (FAILED(hr))
				continue;

			if (nullptr == m_MaterialTextures[texIdx])
				m_MaterialTextures[texIdx] = CTexture::Create(m_pDevice,m_pContext);
			_wstring strNameAndExt = szWideName;
			strNameAndExt += szWideExt;
			m_MaterialTextures[texIdx]->Add_Texture(pSRV, strNameAndExt);

		}
	}

	if (FAILED(Ready_PixelConstBuffer()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMaterial::Bind_PixelConstBuffer(CShader* _pShader)
{
	if (nullptr == _pShader)
		return E_FAIL;

	return _pShader->Bind_ConstBuffer("BasicPixelConstData", m_pPixeConstBuffer);
}

HRESULT CMaterial::Ready_PixelConstBuffer()
{
	for (_uint i = 0; i < aiTextureType_UNKNOWN; ++i)
	{
		_uint iNumSRVs = 0;
		if (nullptr == m_MaterialTextures[i])
			iNumSRVs = 0;
		else
			iNumSRVs = m_MaterialTextures[i]->Get_NumSRVs();

		switch ((aiTextureType)i)
		{
		case aiTextureType_NONE:
			break;
		case aiTextureType_DIFFUSE:
			m_tPixelConstData.useAlbedoMap = iNumSRVs; // 1이상이면 true고 0이면 false니까.
			break;
		case aiTextureType_EMISSIVE:
			m_tPixelConstData.useEmissiveMap = iNumSRVs;
			break;
		case aiTextureType_NORMALS:
			m_tPixelConstData.useNormalMap = iNumSRVs;
			break;
		case aiTextureType_METALNESS:
			m_tPixelConstData.useMetallicMap = iNumSRVs;
			break;
		case aiTextureType_DIFFUSE_ROUGHNESS:
			m_tPixelConstData.useRoughnessMap = iNumSRVs;
			break;
		case aiTextureType_AMBIENT_OCCLUSION:
			m_tPixelConstData.useAOMap = iNumSRVs;
			break;
		case aiTextureType_BASE_COLOR: // ORMH
			m_tPixelConstData.useORMHMap = iNumSRVs;
			break;
		default:
			break;
		}

	}

	D3D11_USAGE eUsage = D3D11_USAGE_DEFAULT;

#ifdef _DEBUG
	eUsage = D3D11_USAGE_DYNAMIC;
#endif // _DEBUG


	// Create ConstantBuffer
	if (FAILED(m_pGameInstance->CreateConstBuffer(m_tPixelConstData, eUsage, &m_pPixeConstBuffer)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMaterial::Update_PixelConstBuffer()
{
	if (FAILED(Ready_PixelConstBuffer()))
			return E_FAIL;

	return m_pGameInstance->UpdateConstBuffer(m_tPixelConstData, m_pPixeConstBuffer);
}

CMaterial* CMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* szDirPath, ifstream& inFile)
{
	CMaterial* pInstance = new CMaterial(pDevice, pContext);

	if (FAILED(pInstance->Initialize(szDirPath, inFile)))
	{
		MSG_BOX("Failed to Created : CMaterial");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMaterial::Free()
{
	Safe_Release(m_pPixeConstBuffer);

	for (_uint i = 0; i < aiTextureType_UNKNOWN; ++i)
		Safe_Release(m_MaterialTextures[i]);

	__super::Free();
}

CComponent* CMaterial::Clone(void* pArg)
{
	return this;
}

