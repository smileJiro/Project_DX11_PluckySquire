#include "stdafx.h"
#include "ToolBone.h"

CToolBone::CToolBone()
	:CBone()
{
}

HRESULT CToolBone::Initialize(ifstream& _inFile, _int iParentBoneIndex)
{
	_uint iNameLength = 0;
	_inFile.read((char*)&iNameLength, sizeof(_uint));
	_inFile.read((char*)&m_szName, iNameLength);
	m_szName[iNameLength] = '\0';
	//cout << m_szName << endl;

	_inFile.read(reinterpret_cast<char*>(&m_TransformationMatrix), sizeof(_float4x4));
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	m_iParentBoneIndex = iParentBoneIndex;
	
	_inFile.read(reinterpret_cast<char*>(&m_iNumChildBones), sizeof(_uint));

	return S_OK;
}

void CToolBone::Export(ofstream& _outfile)
{
	string strName = m_szName;
	_uint iCount = 0;
	//Name
	iCount = (_uint)strName.length();
	_outfile.write(reinterpret_cast<const char*>(&iCount), sizeof(_uint));
	_outfile.write(strName.c_str(), iCount);

	_float4x4 matTransposed;
	XMStoreFloat4x4(&matTransposed, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
	_outfile.write(reinterpret_cast<const char*>(&matTransposed), sizeof(_float4x4));
_outfile.write(reinterpret_cast<const char*>(&m_iNumChildBones), sizeof(_uint));

}

CToolBone* CToolBone::Create(ifstream& _inFile, _int _iParentBoneIndex)
{
	CToolBone* pInstance = new CToolBone();

	if (FAILED(pInstance->Initialize(_inFile, _iParentBoneIndex)))
	{
		MSG_BOX("Failed to Created : ToolBone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CBone* CToolBone::Clone()
{
	return new CToolBone(*this);
}

void CToolBone::Free()
{
	__super::Free();

}
