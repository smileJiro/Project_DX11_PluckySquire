#include "stdafx.h"
#include "ToolMaterial.h"


CToolMaterial::CToolMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMaterial(pDevice, pContext)
{
}

HRESULT CToolMaterial::Initialize(const _char* szDirPath, ifstream& inFile)
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
				hr = CreateDDSTextureFromFile(m_pDevice, szWideFullPath, nullptr, &pSRV);
			else if (false == strcmp(szExt, ".tga"))
				return E_FAIL;
			else
				hr = CreateWICTextureFromFile(m_pDevice, szWideFullPath, nullptr, &pSRV);

			m_MaterialTexturePaths[texIdx] = szWideFullPath;
			delete[] strTexturePath;
			if (FAILED(hr))
				continue;

			if (nullptr == m_MaterialTextures[texIdx])
				m_MaterialTextures[texIdx] = CTexture::Create(m_pDevice, m_pContext);
			_wstring strNameAndExt = szWideName;
			strNameAndExt += szWideExt;
			m_MaterialTextures[texIdx]->Add_Texture(pSRV, strNameAndExt);

		}
	}

	if (FAILED(Ready_PixelConstBuffer()))
		return E_FAIL;

	return S_OK;
}

void CToolMaterial::Export(ofstream& _outfile)
{
	for (_uint texIdx = 1; texIdx < aiTextureType_UNKNOWN; texIdx++)
	{
		_uint iCount = Get_TextureCount(texIdx);
		_outfile.write(reinterpret_cast<char*>(&iCount), sizeof(_uint));

		for (_uint iSRVIndex = 0; iSRVIndex < iCount; iSRVIndex++)
		{
			_uint iNameSize = 0;
			string strTextureName = WstringToString(*m_MaterialTextures[texIdx]->Get_SRVName(iSRVIndex));
			iNameSize = (_uint)strTextureName.length();
			_outfile.write(reinterpret_cast<char*>(&iNameSize), sizeof(_uint));
			_outfile.write(strTextureName.c_str(), iNameSize);
		}
	}
}

HRESULT CToolMaterial::Copy_Texture(const filesystem::path& _szDestPath)
{
	for (auto& pathSrc : m_MaterialTexturePaths)
	{
		filesystem::copy_file(pathSrc, _szDestPath / pathSrc.filename(), filesystem::copy_options::overwrite_existing);
	}
	return S_OK;
}

void CToolMaterial::Get_TextureNames(set<wstring>& _outTextureNames)
{
	for (auto& pTexture : m_MaterialTextures)
	{
		_outTextureNames.insert(*pTexture->Get_SRVName(0));
	}
}

_uint CToolMaterial::Get_TextureCount(_uint iTexType)
{
	if (m_MaterialTextures[iTexType])
		return m_MaterialTextures[iTexType]->Get_NumSRVs();
	else
		return 0;
}

CToolMaterial* CToolMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* szDirPath, ifstream& inFile)
{
	CToolMaterial* pInstance = new CToolMaterial(pDevice, pContext);

	if (FAILED(pInstance->Initialize(szDirPath, inFile)))
	{
		MSG_BOX("Failed to Created : ToolMaterial");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CToolMaterial::Clone(void* pArg)
{
	return this;
}

void CToolMaterial::Free()
{
	__super::Free();
}
