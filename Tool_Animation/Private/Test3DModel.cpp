#include "stdafx.h"
#include "Test3DModel.h"
#include "Animation.h"
#include "ToolAnimation3D.h"
#include "ToolMaterial.h"
#include "ToolBone.h"
#include "ToolMesh.h"

CTest3DModel::CTest3DModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: C3DModel(_pDevice, _pContext)
{
}

CTest3DModel::CTest3DModel(const CTest3DModel& _Prototype)
	: C3DModel(_Prototype)
{
}

HRESULT CTest3DModel::Export_Model(ofstream& _outfile)
{
	_bool bAnim = Is_AnimModel();
	_outfile.write(reinterpret_cast<char*>(&bAnim), 1);

	if (FAILED(Export_Bone(_outfile)))
		return E_FAIL;

	if (FAILED(Export_Meshes(_outfile, bAnim)))
		return E_FAIL;

	if (FAILED(Export_Materials(_outfile)))
		return E_FAIL;

	if (FAILED(Export_Animations(_outfile)))
		return E_FAIL;


	_outfile.close();

	return S_OK;
}

void CTest3DModel::Set_Progress(_float _fTrackPos)
{
	if (m_Animations.empty())
		return;
	static_cast<CToolAnimation3D*> (m_Animations[m_iCurrentAnimIndex])->Set_Progress(_fTrackPos);
}

void CTest3DModel::Get_TextureNames(set<wstring>& _outTextureNames)
{
	for (auto& pMaterial : m_Materials)
	{
		static_cast<CToolMaterial*>(pMaterial)->Get_TextureNames(_outTextureNames);
	}

}

_float CTest3DModel::Get_Progress()
{
	if (m_Animations.empty())
		return 0;
	return static_cast<CToolAnimation3D*> (m_Animations[m_iCurrentAnimIndex])->Get_Progress();
}


HRESULT CTest3DModel::Ready_Bones(ifstream& inFile, _uint iParentBoneIndex)
{
	CToolBone* pBone = CToolBone::Create(inFile, iParentBoneIndex);;

	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);

	iParentBoneIndex = (_uint)m_Bones.size() - 1;
	_uint iCount = pBone->Get_NumChildBones();
	for (_uint i = 0; i < iCount; ++i)
	{
		Ready_Bones(inFile, iParentBoneIndex);
	}

	return S_OK;
}

HRESULT CTest3DModel::Ready_Meshes(ifstream& inFile)
{
	inFile.read(reinterpret_cast<char*>(&m_iNumMeshes), sizeof(_uint));
	for (_uint i = 0; i < m_iNumMeshes; i++)
	{
		CToolMesh* pMesh = CToolMesh::Create(m_pDevice, m_pContext, m_eAnimType, this, inFile, XMLoadFloat4x4(&m_PreTransformMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CTest3DModel::Ready_Materials(ifstream& inFile, const _char* pModelFilePath)
{
	inFile.read(reinterpret_cast<char*>(&m_iNumMaterials), sizeof(_uint));
	//cout << m_iNumMaterials << endl;
	m_Materials.resize(m_iNumMaterials);
	_char		szDrive[MAX_PATH] = "";
	_char		szDirectory[MAX_PATH] = "";
	_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);
	strcat_s(szDrive, szDirectory);
	for (_uint i = 0; i < m_iNumMaterials; i++)
	{
		CToolMaterial* pMaterial = CToolMaterial::Create(m_pDevice, m_pContext, szDrive, inFile);
		m_Materials[i] = pMaterial;
	}

	return S_OK;
}

HRESULT CTest3DModel::Ready_Animations(ifstream& inFile)
{
	inFile.read(reinterpret_cast<char*>(&m_iNumAnimations), sizeof(_uint));
	m_Animations.reserve(m_iNumAnimations);
	for (_uint i = 0; i < m_iNumAnimations; i++)
	{
		CToolAnimation3D* pAnimation = CToolAnimation3D::Create(inFile, this);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

HRESULT CTest3DModel::Export_Bone(ofstream& _outFile)
{
	_uint iNumBones = (_uint)m_Bones.size();
	for (_uint i = 0; i < iNumBones; ++i)
		static_cast<CToolBone*>(m_Bones[i])->Export(_outFile);

	return S_OK;
}

HRESULT CTest3DModel::Export_Meshes(ofstream& _outFile, _bool _bAnim)
{

	_outFile.write(reinterpret_cast<char*>(&m_iNumMeshes), sizeof(_uint));
	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		static_cast<CToolMesh*>(m_Meshes[i])->Export(_outFile, _bAnim);
	}
	return S_OK;
}

HRESULT CTest3DModel::Export_Materials(ofstream& _outFile)
{
	_outFile.write(reinterpret_cast<char*>(&m_iNumMaterials), sizeof(_uint));
	//cout << m_iNumMaterials << endl;
	for (_uint i = 0; i < m_iNumMaterials; i++)
	{
		CToolMaterial* pToolMat =static_cast<CToolMaterial*>(m_Materials[i]);
		pToolMat->Export(_outFile);

	}

	return S_OK;
}

HRESULT CTest3DModel::Export_Animations(ofstream& _outFile)
{

	_outFile.write(reinterpret_cast<char*>(&m_iNumAnimations), sizeof(_uint));
	for (_uint i = 0; i < m_iNumAnimations; i++)
	{
		static_cast<CToolAnimation3D*>(m_Animations[i])->Export(_outFile);
	}

	return S_OK;
}

CTest3DModel* CTest3DModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
	CTest3DModel* pInstance = new CTest3DModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath, PreTransformMatrix)))
	{
		MSG_BOX("Failed to Created : Test3DModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CTest3DModel::Clone(void* _pArg)
{
	CTest3DModel* pInstance = new CTest3DModel(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : Test3DModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTest3DModel::Free()
{
	__super::Free();
}
