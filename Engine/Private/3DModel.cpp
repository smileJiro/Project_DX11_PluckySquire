#include "3DModel.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
#include "Bone.h"
#include "Animation.h"
#include "iostream"

C3DModel::C3DModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
{
}

C3DModel::C3DModel(const C3DModel& _Prototype)
	: CComponent(_Prototype)
	, m_eModelType{ _Prototype.m_eModelType }
	, m_iNumMeshes(_Prototype.m_iNumMeshes)
	, m_Meshes(_Prototype.m_Meshes)
	, m_iNumMaterials{ _Prototype.m_iNumMaterials }
	, m_Materials{ _Prototype.m_Materials }
	, m_PreTransformMatrix{ _Prototype.m_PreTransformMatrix }
	//, m_Bones{ _Prototype.m_Bones }
	, m_iNumBones(_Prototype.m_iNumBones)
	, m_iNumAnimations(_Prototype.m_iNumAnimations)
	//, m_Animations{ _Prototype.m_Animations }
{
	for (auto& PrototypeAnim : _Prototype.m_Animations)
	{
		m_Animations.push_back(PrototypeAnim->Clone());
	}
	for (auto& pPrototypeBone : _Prototype.m_Bones)
	{
		m_Bones.push_back(pPrototypeBone->Clone());
	}

	for (auto& pMaterial : m_Materials)
		Safe_AddRef(pMaterial);

	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);

}

HRESULT C3DModel::Initialize_Prototype(const _char* pModelFilePath, _fmatrix PreTransformMatrix)
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
	m_eModelType = bAnim ? ANIM : NONANIM;


	if (FAILED(Ready_Bones(inFile, -1)))
		return E_FAIL;

	if (FAILED(Ready_Meshes(inFile)))
		return E_FAIL;

	if (FAILED(Ready_Materials(inFile, pModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Animations(inFile)))
		return E_FAIL;
	inFile.close();
	//std::cout << pModelFilePath << endl;
	return S_OK;
}


HRESULT C3DModel::Initialize(void* _pArg)
{
	return S_OK;
}


HRESULT C3DModel::Render(_uint _iMeshIndex)
{
	if (_iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	/* 순회하며 그리지 않고, 외부에서 Mesh Index를 받아와서 그리고있다. */
	/* 각각의 메쉬를 그리기 전, 각각의 메쉬가 사용하는 Material Texture를 세팅해주어야한다. 그래서 그러한 작업을 클라에서 수행하고,
	Engine에서는 매개변수로 들어온 인덱스에 해당하는 메쉬를 그리는 작업만 수행한다. */
	m_Meshes[_iMeshIndex]->Bind_BufferDesc();
	m_Meshes[_iMeshIndex]->Render();

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

_uint C3DModel::Find_BoneIndex(const _char* _pBoneName) const
{
	_uint iBoneIndex = 0;
	/* 1. 매개변수로 들어온 BoneName과 같은 이름을 가진 Bone을 찾고, 해당 본의 인덱스를 리턴함. */
	/* 람다식에 대한 이해. */
	/* https://www.notion.so/43-120b1e26c8a880d0a730d21cf62d76f2 */
	auto iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)-> bool 
		{
			/* 캡쳐절을 사용하여 외부에 있는 데이터도 받을 수 있게 했다. */
			/* 이름이 같다면, 0를 반환하는 strcmp 함수. 이름이 같다면 true 로 find_if 탈출.*/
			if (false == strcmp(pBone->Get_Name(), _pBoneName))
				return true;

			/* 해당 람다식 탈출조건을 만족했을때의 BoneIndex를 리턴할 것임.*/
			++iBoneIndex;

			return false;
		});

	/* 만약 같은 이름이 없다면,*/
	if (iter == m_Bones.end())
		MSG_BOX("없다.");


	return iBoneIndex;
}

const _float4x4* C3DModel::Find_BoneMatrix(const _char* _pBoneName) const
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
		{
			if (false == strcmp(pBone->Get_Name(), _pBoneName))
				return true;

			return false;
		});

	if (iter == m_Bones.end())
		return nullptr;

	return (*iter)->Get_CombinedTransformationFloat4x4();
}

_bool C3DModel::Play_Animation(_float fTimeDelta)
{

	//뼈들의 변환행렬을 갱신
	_bool bReturn = false;
	if (m_iCurrentAnimIndex == m_iPrevAnimIndex)
	{
		bReturn = m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_Bones, fTimeDelta);
	}
	else
	{
		if (m_Animations[m_iCurrentAnimIndex]->Update_AnimTransition(m_Bones, fTimeDelta, m_mapAnimTransLeftFrame))
			m_iPrevAnimIndex = m_iCurrentAnimIndex;
	}


	//뼈들의 합성변환행렬을 갱신
	for (auto& pBone : m_Bones)
		pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));

	return bReturn;
}

HRESULT C3DModel::Copy_BoneMatrices(_int iNumMeshIndex, array<_float4x4, 256>* _pOutBoneMatrices)
{
	if (m_Meshes.size() <= iNumMeshIndex)
		return E_FAIL;

	if(FAILED(m_Meshes[iNumMeshIndex]->Copy_BoneMatrices(_pOutBoneMatrices)))
		return E_FAIL;

	return S_OK;
}

_string C3DModel::Get_MeshName(_uint _iMeshIndex)
{
	if (m_Meshes.size() <= _iMeshIndex)
		return _string();

	return m_Meshes[_iMeshIndex]->Get_Name();
}

_float C3DModel::Get_RootBonePositionY()
{
	for (auto& pBones : m_Bones)
	{
		if (!strcmp(pBones->Get_Name(), "Bip001"))
		{
			return pBones->Get_CombinedPosition().m128_f32[1];
		}
	}
	
	return 0.0f;
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

void C3DModel::Set_AnimationLoop(_uint iIdx, _bool bIsLoop)
{
	m_Animations[iIdx]->Set_Loop(bIsLoop);
}

void C3DModel::Set_Animation(_uint iIdx)
{
	m_iCurrentAnimIndex = iIdx;
	m_iPrevAnimIndex = iIdx;
	m_Animations[m_iCurrentAnimIndex]->Reset_CurrentTrackPosition();

}


void C3DModel::Switch_Animation(_uint iIdx)
{
	m_iPrevAnimIndex = m_iCurrentAnimIndex;
	m_iCurrentAnimIndex = iIdx;
	m_mapAnimTransLeftFrame.clear();
	m_Animations[m_iCurrentAnimIndex]->Reset_CurrentTrackPosition();
	m_Animations[m_iPrevAnimIndex]->Get_CurrentFrame(&m_mapAnimTransLeftFrame);

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
	//cout << iNumChildren << endl;
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
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, this, inFile, XMLoadFloat4x4(&m_PreTransformMatrix));
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
		CAnimation* pAnimation = CAnimation::Create(inFile, this);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

C3DModel* C3DModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
	C3DModel* pInstance = new C3DModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath, PreTransformMatrix)))
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* C3DModel::Clone(void* _pArg)
{
	C3DModel* pInstance = new C3DModel(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel");
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

	__super::Free();
}

