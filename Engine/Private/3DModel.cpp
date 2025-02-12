#include "3DModel.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
#include "Bone.h"
#include "Animation3D.h"
#include "iostream"
#include "AnimEventGenerator.h"

C3DModel::C3DModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CModel(_pDevice, _pContext)
{
	ZeroMemory(m_arrTextureBindingIndex, sizeof m_arrTextureBindingIndex);
}

C3DModel::C3DModel(const C3DModel& _Prototype)
	: CModel(_Prototype)
	, m_iNumMeshes(_Prototype.m_iNumMeshes)
	, m_Meshes(_Prototype.m_Meshes)
	, m_iNumMaterials{ _Prototype.m_iNumMaterials }
	, m_Materials{ _Prototype.m_Materials }
	, m_PreTransformMatrix{ _Prototype.m_PreTransformMatrix }
	, m_iNumAnimations(_Prototype.m_iNumAnimations)
	, m_isCookingCollider(_Prototype.m_isCookingCollider)
	, m_iCookingColliderDataLength(_Prototype.m_iCookingColliderDataLength)
	, m_arrCookingColliderData(_Prototype.m_arrCookingColliderData)
{

	for (_uint i = 0; i < aiTextureType_UNKNOWN; i++)
	{
		for (_uint j= 0; j< aiTextureType_UNKNOWN; j++)
		{
			m_arrTextureBindingIndex[i][j] = _Prototype.m_arrTextureBindingIndex[i][j];
		}
	}
	for (auto& pPrototypeBone : _Prototype.m_Bones)
	{
		m_Bones.push_back(pPrototypeBone->Clone());
	}
	for (auto& PrototypeAnim : _Prototype.m_Animations)
	{
		m_Animations.push_back(PrototypeAnim->Clone());
	}

	for (auto& pMaterial : m_Materials)
		Safe_AddRef(pMaterial);

	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);


}

HRESULT C3DModel::Initialize_Prototype(const _char* pModelFilePath, _fmatrix PreTransformMatrix, _bool _isCookingCollider)
{
	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);


	std::ifstream inFile(pModelFilePath, std::ios::binary);
	if (!inFile) {
		string str = "파일을 열 수 없습니다.";
		str += pModelFilePath;
		MessageBoxA(NULL, str.c_str(), "에러", MB_OK);
		return E_FAIL;
	}
	bool bAnim;
	inFile.read(reinterpret_cast<char*>(&bAnim), 1);
	m_eAnimType = bAnim ? ANIM : NONANIM;

	if (FAILED(Ready_Bones(inFile, -1)))
	{
		inFile.close();
		return E_FAIL;
	}

	if (FAILED(Ready_Meshes(inFile)))
	{
		inFile.close();
		return E_FAIL;
	}

	if (FAILED(Ready_Materials(inFile, pModelFilePath)))
	{
		inFile.close();
		return E_FAIL;
	}

	if (FAILED(Ready_Animations(inFile)))
	{
		inFile.close();
		return E_FAIL;
	}
	inFile.close();
	//std::cout << pModelFilePath << endl;


	if (_isCookingCollider)
	{
		_string strColliderPath = pModelFilePath;
		strColliderPath += "Coll";
		if (filesystem::exists(strColliderPath))
		{
			ifstream  file;
			file.open(strColliderPath, ios::binary);
			if (!file)
				return E_FAIL;
			file.read(reinterpret_cast<_char*>(&m_iCookingColliderDataLength), sizeof(_uint));
			m_arrCookingColliderData = new _char[m_iCookingColliderDataLength];
			file.read(m_arrCookingColliderData, m_iCookingColliderDataLength);

			m_isCookingCollider = _isCookingCollider;

		}
	}
	return S_OK;
}


HRESULT C3DModel::Initialize(void* _pArg)
{

	return S_OK;
}

HRESULT C3DModel::Render(CShader* _pShader, _uint _iShaderPass)
{
	/* Mesh 단위 렌더. */
	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		_uint iMaterialIndex = m_Meshes[i]->Get_MaterialIndex();

		if(FAILED(Bind_Material_PixelConstBuffer(iMaterialIndex, _pShader)))
			return E_FAIL;

		if (FAILED(Bind_Material(_pShader, "g_AlbedoTexture", i, aiTextureType_DIFFUSE, m_arrTextureBindingIndex[iMaterialIndex][aiTextureType_DIFFUSE])))
		{
			//continue;
		}

		if (FAILED(Bind_Material(_pShader, "g_NormalTexture", i, aiTextureType_NORMALS, m_arrTextureBindingIndex[iMaterialIndex][aiTextureType_NORMALS])))
		{
			int a = 0;
		}
		if (FAILED(Bind_Material(_pShader, "g_ORMHTexture", i, aiTextureType_BASE_COLOR, m_arrTextureBindingIndex[iMaterialIndex][aiTextureType_BASE_COLOR])))
		{
			int a = 0;
		}
		if (FAILED(Bind_Material(_pShader, "g_MetallicTexture", i, aiTextureType_METALNESS, m_arrTextureBindingIndex[iMaterialIndex][aiTextureType_METALNESS])))
		{
			int a = 0;
		}
		if (FAILED(Bind_Material(_pShader, "g_RoughnessTexture", i, aiTextureType_DIFFUSE_ROUGHNESS, m_arrTextureBindingIndex[iMaterialIndex][aiTextureType_DIFFUSE_ROUGHNESS])))
		{
			int a = 0;
		}
		if (FAILED(Bind_Material(_pShader, "g_AOTexture", i, aiTextureType_AMBIENT_OCCLUSION, m_arrTextureBindingIndex[iMaterialIndex][aiTextureType_AMBIENT_OCCLUSION])))
		{
			int a = 0;
		}
		
		/* Bind Bone Matrices */
		if (Is_AnimModel())
		{
			if (FAILED(Bind_Matrices(_pShader, "g_BoneMatrices", i)))
				return E_FAIL;
		}


		/* Shader Pass */
		_pShader->Begin(_iShaderPass);

		/* Bind Mesh Vertex Buffer */
		m_Meshes[i]->Bind_BufferDesc();
		m_Meshes[i]->Render();
	}

	return S_OK;
}



HRESULT C3DModel::Bind_Matrices(CShader* _pShader, const _char* _pConstantName, _uint _iMeshIndex)
{
	/* 모델의 m_Bones 벡터를 넘겨 여기서 자기들이 필요한 메쉬에 접근해서 행렬을 가져오고 연산할 것임. */
 	return m_Meshes[_iMeshIndex]->Bind_BoneMatrices(_pShader, _pConstantName, m_Bones);
}

HRESULT C3DModel::Bind_Material(CShader* _pShader, const _char* _pConstantName, _uint _iMeshIndex, aiTextureType _eTextureType, _uint _iTextureIndex)
{
	if (nullptr == _pShader)
		return E_FAIL;

	_uint iMaterialIndex = m_Meshes[_iMeshIndex]->Get_MaterialIndex();
	 
	ID3D11ShaderResourceView* pSRV = m_Materials[iMaterialIndex]->Find_Texture(_eTextureType, _iTextureIndex);
	if (nullptr == pSRV)
		return E_FAIL;

	return _pShader->Bind_SRV(_pConstantName, pSRV);
}

_bool C3DModel::Play_Animation(_float fTimeDelta, _bool bReverse)
{
	m_bReverseAnimation = bReverse;
	_bool bReturn = false;
	//뼈들의 변환행렬을 갱신
	if (m_iCurrentAnimIndex == m_iPrevAnimIndex)
	{
		bReturn = m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_Bones, fTimeDelta,bReverse);
	}
	else
	{
		if (m_Animations[m_iCurrentAnimIndex]->Update_AnimTransition(m_Bones, fTimeDelta, m_mapAnimTransLeftFrame, bReverse))
			m_iPrevAnimIndex = m_iCurrentAnimIndex;
	}


	//뼈들의 합성변환행렬을 갱신
	for (auto& pBone : m_Bones)
		pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));

 	return bReturn;
}

HRESULT C3DModel::Bind_Material_PixelConstBuffer(_uint _iMaterialIndex, CShader* _pShader)
{
	if (m_Materials.size() <= _iMaterialIndex)
		return E_FAIL;

	return m_Materials[_iMaterialIndex]->Bind_PixelConstBuffer(_pShader);
}

_uint C3DModel::Get_MeshIndex(const _char* _szName) const
{

	_uint	 iMeshIndex = { 0 };

	auto	iter = find_if(m_Meshes.begin(), m_Meshes.end(), [&](CMesh* pMesh)->_bool
		{
			if (false == strcmp(pMesh->Get_Name(), _szName))
				return true;

			++iMeshIndex;

			return false;
		});

	if (iter == m_Meshes.end())
		MSG_BOX("그런 메쉬가 없어");


	return iMeshIndex;
}

_uint C3DModel::Get_BoneIndex(const _char* pBoneName) const
{

	_uint	iBoneIndex = { 0 };

	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
		{
			if (false == strcmp(pBone->Get_Name(), pBoneName))
				return true;

			++iBoneIndex;

			return false;
		});

	if (iter == m_Bones.end())
		MSG_BOX("그런 뼈가 없어");

	return iBoneIndex;
}

float C3DModel::Get_AnimTime()
{
	return m_Animations[m_iCurrentAnimIndex]->Get_AnimTime();
}

_uint C3DModel::Get_AnimIndex()
{
	return m_iCurrentAnimIndex;
}

CAnimation* C3DModel::Get_Animation(_uint iAnimIndex)
{
	if (iAnimIndex >= m_Animations.size())
	{
		cout << "애니메이션 인덱스가 범위를 벗어났습니다." << endl;
		return nullptr;
	}
	return m_Animations[iAnimIndex];
}

_float C3DModel::Get_AnimationProgress(_uint iAnimIdx)
{
	if (m_iCurrentAnimIndex == m_iPrevAnimIndex)
		return m_Animations[m_iCurrentAnimIndex]->Get_Progress();
	else
		return 0;
}

const _float4x4* C3DModel::Get_BoneMatrix(const _char* pBoneName) const
{

	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
		{
			if (false == strcmp(pBone->Get_Name(), pBoneName))
				return true;

			return false;
		});

	if (iter == m_Bones.end())
		return nullptr;

	return (*iter)->Get_CombinedTransformationFloat4x4();
}

CBone* C3DModel::Get_Bone(const _char* pBoneName) const
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
		{
			if (false == strcmp(pBone->Get_Name(), pBoneName))
				return true;

			return false;
		});

	if (iter == m_Bones.end())
		return nullptr;
	return *iter;
}


void C3DModel::Set_AnimationLoop(_uint iIdx, _bool bIsLoop)
{
	_int iTemp = (_int)m_Animations.size() - 1;
	if (iTemp < (_int)iIdx)
	{
		cout << "애니메이션 인덱스가 범위를 벗어났습니다." << endl;
		return;
	}
	m_Animations[iIdx]->Set_Loop(bIsLoop);
}

void C3DModel::Set_Animation(_uint iIdx, _bool _bReverse)
{
	_int iTemp = (_int)m_Animations.size() - 1;
	if (iTemp < (_int)iIdx)
	{
		cout << "애니메이션 인덱스가 범위를 벗어났습니다." << endl;
		return;
	}
	m_iCurrentAnimIndex = iIdx;
	m_iPrevAnimIndex = iIdx;
	m_Animations[m_iCurrentAnimIndex]->Reset(_bReverse);
}

void C3DModel::Set_AnimationTransitionTime(_uint iIdx, _float _fTime)
{
	_int iTemp = (_int)m_Animations.size() - 1;
	if (iTemp < (_int)iIdx)
	{
		cout << "애니메이션 인덱스가 범위를 벗어났습니다." << endl;
		return;
	}
	m_Animations[iIdx]->Set_AnimTransitionTime(_fTime);
}


void C3DModel::Switch_Animation(_uint iIdx, _bool _bReverse)
{
	_int iTemp = (_int)m_Animations.size() - 1;
	if(iTemp < (_int)iIdx)
	{
		cout << "애니메이션 인덱스가 범위를 벗어났습니다." << endl;
		return;
	}
	m_iPrevAnimIndex = m_iCurrentAnimIndex;
	m_iCurrentAnimIndex = iIdx;
	m_mapAnimTransLeftFrame.clear();
	m_Animations[m_iCurrentAnimIndex]->Reset(_bReverse);
	m_Animations[m_iPrevAnimIndex]->Get_CurrentFrame(&m_mapAnimTransLeftFrame);
}

void C3DModel::To_NextAnimation()
{
	Switch_Animation((m_iCurrentAnimIndex + 1) % m_Animations.size());
}

void C3DModel::Set_AnimSpeedMagnifier(_uint iAnimIndex, _float _fMag)
{
	if (iAnimIndex >= m_Animations.size())
	{
		cout << "애니메이션 인덱스가 범위를 벗어났습니다." << endl;
		return;
	}
	m_Animations[iAnimIndex]->Set_SpeedMagnifier(_fMag);
}

_uint C3DModel::Get_AnimCount()
{
	return (_uint)m_Animations.size();
}


HRESULT C3DModel::Ready_Bones(ifstream& inFile, _uint iParentBoneIndex)
{

	CBone* pBone = CBone::Create(inFile, iParentBoneIndex);;


	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);

	iParentBoneIndex = (_uint)m_Bones.size() - 1;
	_uint iNumChildren = 0;
	inFile.read(reinterpret_cast<char*>(&iNumChildren), sizeof(_uint));
	for (_uint i = 0; i < iNumChildren; ++i)
	{
		Ready_Bones(inFile, iParentBoneIndex);
	}

	return S_OK;
}

HRESULT C3DModel::Ready_Meshes(ifstream& inFile)
{

	//inFile.read(reinterpret_cast<char*>(&m_iNumMeshes), sizeof(_uint));
	inFile.read(reinterpret_cast<char*>(&m_iNumMeshes), sizeof(_uint));
	//cout  << m_iNumMeshes << endl;
	for (_uint i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eAnimType, this, inFile, XMLoadFloat4x4(&m_PreTransformMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT C3DModel::Ready_Materials(ifstream& inFile, const _char* pModelFilePath)
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
		CMaterial* pMaterial = CMaterial::Create(m_pDevice, m_pContext, szDrive, inFile);
		m_Materials[i] = pMaterial;
	}

	return S_OK;
}

HRESULT C3DModel::Ready_Animations(ifstream& inFile)
{
	inFile.read(reinterpret_cast<char*>(&m_iNumAnimations), sizeof(_uint));
	m_Animations.reserve(m_iNumAnimations);
	for (_uint i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation3D* pAnimation = CAnimation3D::Create(inFile, this);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}
						   
C3DModel* C3DModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, _fmatrix PreTransformMatrix, _bool _isCookingCollider)
{
	C3DModel* pInstance = new C3DModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath, PreTransformMatrix, _isCookingCollider)))
	{
		MSG_BOX("Failed to Created : 3DModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* C3DModel::Clone(void* _pArg)
{
	C3DModel* pInstance = new C3DModel(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : 3DModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void C3DModel::Free()
{
	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);
	m_Animations.clear();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);
	m_Bones.clear();

	for (auto& pMaterial : m_Materials)
		Safe_Release(pMaterial);
	m_Materials.clear();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);
	m_Meshes.clear();

	if (!m_isCloned)
		Safe_Delete_Array(m_arrCookingColliderData);

	__super::Free();
}

_float C3DModel::Get_CurrentAnimProgeress()
{
	return m_Animations[m_iCurrentAnimIndex]->Get_Progress();
}

