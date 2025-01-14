#include "Material.h"

CMaterial::CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComponent(pDevice, pContext)
{
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
			//cout << strTexturePath << endl;z
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
			MultiByteToWideChar(CP_ACP, 0, szFullPath, (int)strlen(szFullPath), szWideFullPath, MAX_PATH);

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

			m_MaterialTexture[texIdx].push_back(pSRV);

		}
	}
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
	for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
	{
		for (size_t j = 0; j < m_MaterialTexture[i].size(); ++j)
		{
			Safe_Release(m_MaterialTexture[i][j]);
		}
		m_MaterialTexture[i].clear();
	}

	__super::Free();
}

CComponent* CMaterial::Clone(void* pArg)
{
	return nullptr;
}

