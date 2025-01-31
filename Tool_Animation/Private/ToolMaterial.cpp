#include "stdafx.h"
#include "ToolMaterial.h"


CToolMaterial::CToolMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMaterial(pDevice, pContext)
{
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
