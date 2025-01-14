#include "Material.h"

CMaterial::CMaterial()
	: m_MaterialTexture{}
{
}

HRESULT CMaterial::Initialize()
{
    return S_OK;
}

CMaterial* CMaterial::Create()
{
	CMaterial* pInstance = new CMaterial();

	if (FAILED(pInstance->Initialize()))
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

HRESULT CMaterial::ConvertToBinary(HANDLE _hFile, DWORD* _dwByte)
{
	for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
	{
		_uint iNumTextures = (_uint)m_TexturePaths[i].size();
		if (!WriteFile(_hFile, &iNumTextures, sizeof(_uint), _dwByte, nullptr))
			return E_FAIL;
		for (auto& TexturePath : m_TexturePaths[i])
		{
			_tchar szTexturePath[MAX_PATH] = L"";
			wcscpy_s(szTexturePath, TexturePath.c_str());

			if (!WriteFile(_hFile, szTexturePath, sizeof(szTexturePath), _dwByte, nullptr))
				return E_FAIL;
		}
	}
	return S_OK;
}
