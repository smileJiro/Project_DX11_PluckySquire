#include "Material.h"
#include "GameInstance.h"

CMaterial::CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComponent(pDevice, pContext)
{
	ZeroMemory(m_MaterialTextures, sizeof(m_MaterialTextures));
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
			cout << strTexturePath << endl;
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
				hr = CreateDDSTextureFromFile(m_pDevice, szWideFullPath, nullptr, &pSRV);
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
	for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
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
	// Create ConstantBuffer
	if (FAILED(m_pGameInstance->CreateConstBuffer(m_tPixelConstData, D3D11_USAGE_DEFAULT, &m_pPixeConstBuffer)))
		return E_FAIL;

	return S_OK;
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

	for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
		Safe_Release(m_MaterialTextures[i]);

	__super::Free();
}

CComponent* CMaterial::Clone(void* pArg)
{
	return this;
}

