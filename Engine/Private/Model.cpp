#include "Model.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
#include "Bone.h"
#include "Animation.h"
CModel::CModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
{
}

CModel::CModel(const CModel& _Prototype)
	: CComponent(_Prototype)
	, m_eModelType{ _Prototype.m_eModelType }
	, m_pAIScene{ _Prototype.m_pAIScene }
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

// TODO :: 모델 컴포넌트 생성 시 _matrix 받게 바꾸기. >>> 반복 사용성 및 표현가능 옵션 다양함. 
HRESULT CModel::Initialize_Prototype(TYPE _eModelType, const _char* _pModelFilePath, _fmatrix _PreTransformMatrix)
{
	/* 1. 플래그를 설정한다. { 왼손좌표계 기준으로 변환 | 로드 속도 최적화 } */
	_uint iFlag = { aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };

	/* 추가! : Animation Model인지, NonAnimation Model인지에 따라서 aiProcess_PreTransformVertices 플래그 추가 여부를 결정. */
	if (_eModelType == TYPE::NONANIM)
		iFlag |= aiProcess_PreTransformVertices;

	/* 2. Importer를 통해 Model을 Load 후 읽어들인 데이터를 m_pAIScene에 담아준다. */
	m_pAIScene = m_Importer.ReadFile(_pModelFilePath, iFlag);

	if (0 == m_pAIScene)
		return E_FAIL;

	m_eModelType = _eModelType;

	XMStoreFloat4x4(&m_PreTransformMatrix, _PreTransformMatrix);

	/* ps. 루트 노드와 부모가 없음을 알리는 -1을 던짐으로서 재귀 루틴 시작. */
	if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
		return E_FAIL;

	/* 3. 읽어들인 m_pAIScene을 통해 Mesh에 데이터를 정리해준다. */
	if (FAILED(Ready_Meshes()))
		return E_FAIL;

	/* 4. 각각의 Mesh가 사용 할 Material texture 정리. */
	if (FAILED(Ready_Materials(_pModelFilePath)))
		return E_FAIL;

	/* 5. 모델이 사용하는 애니메이션 >>> 채널 >>> 키프레임 정리 */
	if (FAILED(Ready_Animations()))
		return E_FAIL;


	return S_OK;
}

HRESULT CModel::Initialize_Prototype(TYPE _eModelType, void* _pModelDesc, _fmatrix _PreTransformMatrix)
{
	/* 추가! : Animation Model인지, NonAnimation Model인지에 따라서 aiProcess_PreTransformVertices 플래그 추가 여부를 결정. */
	m_eModelType = _eModelType;
	XMStoreFloat4x4(&m_PreTransformMatrix, _PreTransformMatrix);


	if (m_eModelType == TYPE::NONANIM)
	{
		NONANIM_FBX* pNonAnimDesc = static_cast<NONANIM_FBX*>(_pModelDesc);
		m_iNumMeshes = pNonAnimDesc->iNumMeshes;
		m_iNumMaterials = pNonAnimDesc->iNumMaterials;
		/* 3. 읽어들인 m_pAIScene을 통해 Mesh에 데이터를 정리해준다. */
		if (FAILED(Ready_Meshes(pNonAnimDesc->vecMeshes)))
			return E_FAIL;

		/* 4. 각각의 Mesh가 사용 할 Material texture 정리. */
		if (FAILED(Ready_Materials(_pModelDesc)))
			return E_FAIL;
	}
	else if (m_eModelType == TYPE::ANIM)
	{
		ANIM_FBX* pAnimDesc = static_cast<ANIM_FBX*>(_pModelDesc);
		m_iNumMeshes = pAnimDesc->iNumMeshes;
		m_iNumMaterials = pAnimDesc->iNumMaterials;
		m_iNumBones = pAnimDesc->iNumDebugBones;
		m_iNumAnimations = pAnimDesc->iNumAnimations;

		if (FAILED(Ready_Bones(pAnimDesc->vecBones)))
			return E_FAIL;

		/* 3. 읽어들인 m_pAIScene을 통해 Mesh에 데이터를 정리해준다. */
		if (FAILED(Ready_Meshes(pAnimDesc->vecMeshes)))
			return E_FAIL;
		
		if (FAILED(Ready_Animations(pAnimDesc->vecAnimations)))
			return E_FAIL;


		/* 5. 각각의 Mesh가 사용 할 Material texture 정리. */
		if (FAILED(Ready_Materials(_pModelDesc)))
			return E_FAIL;


	}
	
	

	

	return S_OK;
}

HRESULT CModel::Initialize(void* _pArg)
{
	return S_OK;
}

HRESULT CModel::Ready_Meshes()
{
	/* 0. 읽어들인 데이터를 기반으로 잘 정리하여, Mesh 안에 데이터를 넣어준다. */
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	/* 1. Mesh의 개수 : 구분되어있는 각 파츠 Mesh의 개수를 의미함. (Assimp 특징 : Num 변수가 있으면 해당 객체에 대한 배열을 저장하는 포인터 타입 변수가 있다. )*/
	/* 2. Mesh의 개수만큼 반복문을 돌며, Mesh 객체를 생성하고, 벡터 컨테이너에 넣어준다. */
	/* 3. 크기, 회전에 대한 변환 행렬을 만들어서 메쉬를 생성시 초기 vertex의 로컬스페이스에서의 변환을 수행해준다. */
	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		// m_pAIScene->mMeshes[i] >>> aiMesh** Mesh의 정점 개수, 면의 개수 등등 VB, IB를 구성하기 위한 데이터들이 모두 들어있다.
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, this, m_pAIScene->mMeshes[i], XMLoadFloat4x4(&m_PreTransformMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const _char* pModelFilePath)
{
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	/* resize() : 설정 한개수만큼 size 자체가 늘어남. 기존에 데이터가 들어있었다면 그 데이터는 보존하고 추가공간에 대해서는 0으로 채움.
	만약, 축소가 발생했다면, resize() 한 사이즈만큼은 데이터의 값이 남아있고, 범위보다 큰 데이터는 날라감. 여기서 push_back하게되면 마지막 원소 다음 
	공간에 데이터가 추가됌.*/
	m_Materials.resize(m_iNumMaterials);
	for (_uint i = 0; i < m_iNumMaterials; i++)
	{
		m_Materials[i] = CMaterial::Create();
		// 해당되는 텍스쳐 정보를 받아옴 (diffuse, )
		aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];
		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			// 해당 되는 텍스쳐 장수를 받아옴.
			_uint		iNumTextures = pAIMaterial->GetTextureCount(aiTextureType(j));
			for (_uint k = 0; k < iNumTextures; k++)
			{
				// Texture에 경로 정보를 만들어서 저장한다. >> 우리는 fbx 폴더와 같은 공간에 텍스쳐를 두는 것을 기준으로 함.
				// fbx 파일 경로 + 텍스쳐이름을 조합해서 만들 것.
				aiString strTexturePath;

				// 해당 텍스쳐의 경로를 가져오는 함수. 하지만 여기서 받아오는 경로는 fbx파일 저장 당시의 경로일 것임.
				// 여기선 파일의 이름과 확장자를 받아온다. 
				if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), k, &strTexturePath)))
					break;

				_char szDrive[MAX_PATH] = "";
				_char szDirectory[MAX_PATH] = "";
				_char szFileName[MAX_PATH] = "";
				_char szExt[MAX_PATH] = "";

				_char szFullPath[MAX_PATH] = "";

				// _splitpath_s() : 파일의 경로를 분리하는데에 사용되는 함수.
				// fbx 경로에선 Drive 경로와 Directory 경로만을 가져온다. 
				_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

				// Texture Path에선 파일명과 확장자만을 가져온다.
				_splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

				strcpy_s(szFullPath, szDrive); // 복사
				strcat_s(szFullPath, szDirectory); // 문자열 이어붙이기.
				strcat_s(szFullPath, szFileName);
				strcat_s(szFullPath, szExt);

				_tchar szFinalPath[MAX_PATH] = TEXT("");

				// 1바이트 문자열을 멀티바이트 문자열로 변환. (szFinalPath는 현재는 Length가 0이니까, MAX_PATH 넣어줘야함.)
				MultiByteToWideChar(CP_ACP, 0, szFullPath, (_int)strlen(szFullPath), szFinalPath, MAX_PATH);

				ID3D11ShaderResourceView* pSRV = nullptr;

				if (false == strcmp(szExt, ".dds"))
				{
					if (SUCCEEDED(CreateDDSTextureFromFile(m_pDevice, szFinalPath, nullptr, &pSRV)))
					{
						m_Materials[i]->Add_Texture((aiTextureType)j, pSRV);
						m_Materials[i]->Add_TexturePath((aiTextureType)j, szFinalPath);
					}
				}
				else if (false == strcmp(szExt, ".tga"))
				{
				}
				else
				{
					if (SUCCEEDED(CreateWICTextureFromFile(m_pDevice, szFinalPath, nullptr, &pSRV)))
					{
						m_Materials[i]->Add_Texture((aiTextureType)j, pSRV);
						m_Materials[i]->Add_TexturePath((aiTextureType)j, szFinalPath);
					}
				}
			}
		}
	}

	return S_OK;
}

HRESULT CModel::Ready_Meshes(vector<FBX_MESH>& _vecMesh)
{
	/* 0. 읽어들인 데이터를 기반으로 잘 정리하여, Mesh 안에 데이터를 넣어준다. */
	
	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		// m_pAIScene->mMeshes[i] >>> aiMesh** Mesh의 정점 개수, 면의 개수 등등 VB, IB를 구성하기 위한 데이터들이 모두 들어있다.
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, _vecMesh[i], XMLoadFloat4x4(&m_PreTransformMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);


	}

	
	/* 1. Mesh의 개수 : 구분되어있는 각 파츠 Mesh의 개수를 의미함. (Assimp 특징 : Num 변수가 있으면 해당 객체에 대한 배열을 저장하는 포인터 타입 변수가 있다. )*/
	/* 2. Mesh의 개수만큼 반복문을 돌며, Mesh 객체를 생성하고, 벡터 컨테이너에 넣어준다. */
	/* 3. 크기, 회전에 대한 변환 행렬을 만들어서 메쉬를 생성시 초기 vertex의 로컬스페이스에서의 변환을 수행해준다. */


	return S_OK;
}

HRESULT CModel::Ready_Materials(void* _pModelDesc)
{
	if (m_eModelType == NONANIM)
	{
		NONANIM_FBX* pNonAnimDesc = static_cast<NONANIM_FBX*>(_pModelDesc);
		if (nullptr == pNonAnimDesc)
			return E_FAIL;

		const vector<FBX_MATERIAL>& vecMaterialData = pNonAnimDesc->vecMaterials;
		m_iNumMaterials = pNonAnimDesc->iNumMaterials;
		m_Materials.resize(m_iNumMaterials);
		for (_uint i = 0; i < m_iNumMaterials; ++i)
		{
			m_Materials[i] = CMaterial::Create();

			for (_uint j = 0; j < MATERIAL_TEXTURE_MAX; ++j)
			{
				for (size_t k = 0; k < vecMaterialData[i].vecTextureFullPath[j].size(); ++k)
				{

					TCHAR szExt[MAX_PATH] = L"";
					_wsplitpath_s(vecMaterialData[i].vecTextureFullPath[j][k].c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

					ID3D11ShaderResourceView* pSRV = nullptr;

					if (false == wcscmp(szExt, L".dds"))
					{
						if (SUCCEEDED(CreateDDSTextureFromFile(m_pDevice, vecMaterialData[i].vecTextureFullPath[j][k].c_str(), nullptr, &pSRV)))
						{
							m_Materials[i]->Add_Texture((aiTextureType)j, pSRV);
							m_Materials[i]->Add_TexturePath((aiTextureType)j, vecMaterialData[i].vecTextureFullPath[j][k].c_str());
						}
					}
					else if (false == wcscmp(szExt, L".tga"))
					{
					}
					else
					{
						if (SUCCEEDED(CreateWICTextureFromFile(m_pDevice, vecMaterialData[i].vecTextureFullPath[j][k].c_str(), nullptr, &pSRV)))
						{
							m_Materials[i]->Add_Texture((aiTextureType)j, pSRV);
							m_Materials[i]->Add_TexturePath((aiTextureType)j, vecMaterialData[i].vecTextureFullPath[j][k].c_str());
						}
					}
				}
			}
		}
		
	}
	else
	{
		ANIM_FBX* pAnimDesc = static_cast<ANIM_FBX*>(_pModelDesc);
		if (nullptr == pAnimDesc)
			return E_FAIL;

		const vector<FBX_MATERIAL>& vecMaterialData = pAnimDesc->vecMaterials;
		m_iNumMaterials = pAnimDesc->iNumMaterials;
		m_Materials.resize(m_iNumMaterials);
		for (_uint i = 0; i < m_iNumMaterials; ++i)
		{
			m_Materials[i] = CMaterial::Create();

			for (_uint j = 0; j < MATERIAL_TEXTURE_MAX; ++j)
			{
				for (size_t k = 0; k < vecMaterialData[i].vecTextureFullPath[j].size(); ++k)
				{

					TCHAR szExt[MAX_PATH] = L"";
					_wsplitpath_s(vecMaterialData[i].vecTextureFullPath[j][k].c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

					ID3D11ShaderResourceView* pSRV = nullptr;

					if (false == wcscmp(szExt, L".dds"))
					{
						if (SUCCEEDED(CreateDDSTextureFromFile(m_pDevice, vecMaterialData[i].vecTextureFullPath[j][k].c_str(), nullptr, &pSRV)))
						{
							m_Materials[i]->Add_Texture((aiTextureType)j, pSRV);
							m_Materials[i]->Add_TexturePath((aiTextureType)j, vecMaterialData[i].vecTextureFullPath[j][k].c_str());

						}
					}
					else if (false == wcscmp(szExt, L".tga"))
					{
					}
					else
					{
						if (SUCCEEDED(CreateWICTextureFromFile(m_pDevice, vecMaterialData[i].vecTextureFullPath[j][k].c_str(), nullptr, &pSRV)))
						{
							m_Materials[i]->Add_Texture((aiTextureType)j, pSRV);
							m_Materials[i]->Add_TexturePath((aiTextureType)j, vecMaterialData[i].vecTextureFullPath[j][k].c_str());

						}
					}
				}
			}
		}

	}
	return S_OK;
}

HRESULT CModel::Ready_Bones(const aiNode* _pNode, _int _iParentBoneIndex)
{
	CBone* pBone = CBone::Create(_pNode, _iParentBoneIndex);

	if (nullptr == pBone)
		return E_FAIL;

	/* 1. 벡터에 생성한 Bone을 담는다. */
	m_Bones.push_back(pBone);
	/* 2. 벡터의 현재 사이즈 - 1을 부모노드 인덱스로 설정한다. */
	_iParentBoneIndex = ((_int)m_Bones.size()) - 1;

	/* 3. 자식 뼈의 개수만큼 루프 */
	for (_uint i = 0; i < _pNode->mNumChildren; ++i)
	{
		/* 4. 재귀 호출 (자식 정보를 들고있는 aiNode*, 위에서 구해둔 부모 인덱스.)*/
		Ready_Bones(_pNode->mChildren[i], _iParentBoneIndex);
	}
	/* 정리 : 재귀호출을 하며 모든 Bone들은 자기 자신의 부모 Bone의 인덱스를 저장한다. */
	return S_OK;
}

HRESULT CModel::Ready_Bones(const vector<FBX_BONE>& _vecBones)
{
	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		CBone* pBone = CBone::Create(_vecBones[i]);

		m_Bones.push_back(pBone);
	}
	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	/* 해당 모델의 총 애니메이션 개수 저장. (idle, attack etc...)*/
	m_iNumAnimations = m_pAIScene->mNumAnimations;
	
	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		CAnimation* pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], this);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations(const vector<FBX_ANIMATION>& _vecAnimation)
{
	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		CAnimation* pAnimation = CAnimation::Create(_vecAnimation[i]);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}


HRESULT CModel::Render(_uint _iMeshIndex)
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

//void CModel::Play_Animation(_float _fTimeDelta)
//{
//	if (NONANIM == m_eModelType)
//		return;
//	/* 1. 표현하고자 하는 애니메이션에 따른 뼈의 상태를 갱신해준다. (TransformationMatrix 값을 조작하며..)*/
//	/* 현재 재생중인 애니메이션들의 Channel들을 순회하며, 각각의 키프레임의 값을 확인 후, 흐른 시간에 맞게 보간하여 Transformation Matrix를 만들고,
//	그 데이터를 각각의 Channel(모델이 가진 뼈)들에게 적용시켜주는 함수.*/
// 	m_isFinished = m_Animations[m_iCurAnimIndex]->Update_TransformationMatrices(m_Bones, m_isLoop ,_fTimeDelta);
//
//
//
//	/* 2. Transformation에 대한 조작이 모두 끝났다면, 벡터를 순회하며, CombinedMatrix를 구성해준다.*/
//	for (auto& pBone : m_Bones)
//	{
//		pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
//	}
//
//}

HRESULT CModel::Bind_Matrices(CShader* _pShader, const _char* _pConstantName, _uint _iMeshIndex)
{
	/* 모델의 m_Bones 벡터를 넘겨 여기서 자기들이 필요한 메쉬에 접근해서 행렬을 가져오고 연산할 것임. */
	return m_Meshes[_iMeshIndex]->Bind_BoneMatrices(_pShader, _pConstantName, m_Bones);
}

HRESULT CModel::Bind_Material(CShader* _pShader, const _char* _pConstantName, _uint _iMeshIndex, aiTextureType _eTextureType, _uint _iTextureIndex)
{
	if (nullptr == _pShader)
		return E_FAIL;

	_uint iMaterialIndex = m_Meshes[_iMeshIndex]->Get_MaterialIndex();

	ID3D11ShaderResourceView* pSRV = m_Materials[iMaterialIndex]->Find_Texture(_eTextureType, _iTextureIndex);
	if (nullptr == pSRV)
		return E_FAIL;

	return _pShader->Bind_SRV(_pConstantName, pSRV);
}

_uint CModel::Find_BoneIndex(const _char* _pBoneName) const
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

const _float4x4* CModel::Find_BoneMatrix(const _char* _pBoneName) const
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


_bool CModel::Play_Animation_Tool(_float _fTimeDelta)
{
	/* 1. 표현하고자 하는 애니메이션에 따른 뼈의 상태를 갱신해준다. (TransformationMatrix 값을 조작하며..)*/
	/* 현재 재생중인 애니메이션들의 Channel들을 순회하며, 각각의 키프레임의 값을 확인 후, 흐른 시간에 맞게 보간하여 Transformation Matrix를 만들고,
	그 데이터를 각각의 Channel(모델이 가진 뼈)들에게 적용시켜주는 함수.*/
	if (true == m_Animations[m_iCurAnimIndex]->Update_TransformationMatrices(m_Bones, m_isLoop, _fTimeDelta))
	{

	}

	

	/* 2. Transformation에 대한 조작이 모두 끝났다면, 벡터를 순회하며, CombinedMatrix를 구성해준다.*/
	for (auto& pBone : m_Bones)
	{
		pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
	}
	return true;
}

HRESULT CModel::Copy_BoneMatrices(_int iNumMeshIndex, array<_float4x4, 256>* _pOutBoneMatrices)
{
	if (m_Meshes.size() <= iNumMeshIndex)
		return E_FAIL;

	if(FAILED(m_Meshes[iNumMeshIndex]->Copy_BoneMatrices(_pOutBoneMatrices)))
		return E_FAIL;

	return S_OK;
}

_string CModel::Get_MeshName(_uint _iMeshIndex)
{
	if (m_Meshes.size() <= _iMeshIndex)
		return _string();

	return m_Meshes[_iMeshIndex]->Get_Name();
}

_float CModel::Get_RootBonePositionY()
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


CModel* CModel::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, TYPE _eModelType,const _char* _pModelFilePath, _fmatrix _PreTransformMatrix)
{
	CModel* pInstance = new CModel(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_eModelType, _pModelFilePath, _PreTransformMatrix)))
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CModel* CModel::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, TYPE _eModelType, void* _pModelDesc, _fmatrix _PreTransformMatrix)
{
	CModel* pInstance = new CModel(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_eModelType, _pModelDesc, _PreTransformMatrix)))
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CModel::Clone(void* _pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
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


	/* 이걸 해도 Assimp 고질 병인 내부적인 누수는 발생한다. 하지만 해주자. 우리눈에 안보이는 누수임. */
	m_Importer.FreeScene();

	__super::Free();
}

HRESULT CModel::ConvertToBinary(const _char* _pModelFilePath, const _tchar* _szSavePath)
{
	/* 1. 플래그를 설정한다. { 왼손좌표계 기준으로 변환 | 로드 속도 최적화 } */
	_uint iFlag = { aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };

	/* 추가! : Animation Model인지, NonAnimation Model인지에 따라서 aiProcess_PreTransformVertices 플래그 추가 여부를 결정. */
	if (m_eModelType == TYPE::NONANIM)
		iFlag |= aiProcess_PreTransformVertices;

	/* 2. Importer를 통해 Model을 Load 후 읽어들인 데이터를 m_pAIScene에 담아준다. */
	m_pAIScene = m_Importer.ReadFile(_pModelFilePath, iFlag);

	if (0 == m_pAIScene)
	{
		MSG_BOX("The FBX file does not exist at the specified path.");
		return E_FAIL;
	}




	_ulong dwByte = {};
	HANDLE hFile = CreateFile(_szSavePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	/* Model 기본데이터 */
	_uint iModelType = (_uint)m_eModelType;
	WriteFile(hFile, &iModelType, sizeof(_uint), &dwByte, NULL);
	_uint iNumBones = (_uint)m_Bones.size();
	WriteFile(hFile, &iNumBones, sizeof(_uint), &dwByte, NULL);
	WriteFile(hFile, &m_iNumMeshes, sizeof(_uint), &dwByte, NULL);
	WriteFile(hFile, &m_iNumAnimations, sizeof(_uint), &dwByte, NULL);
	WriteFile(hFile, &m_iNumMaterials, sizeof(_uint), &dwByte, NULL);
	

	/* Bone 벡터 순회 */
	for (auto& pBone : m_Bones)
	{
		pBone->ConvertToBinary(hFile, &dwByte);
	}

	/* Mesh 벡터 순회 */
	int iMeshIndex = 0;
	for (auto& pMesh : m_Meshes)
	{
		pMesh->ConvertToBinary(hFile, &dwByte, m_pAIScene->mMeshes[iMeshIndex++], this);
	}

	/* Animation 벡터 순회 */
	for (auto& pAnimation : m_Animations)
	{
		pAnimation->ConvertToBinary(hFile, &dwByte);
	}

	/* Material 순회 */
	for (auto& pMaterial : m_Materials)
	{
		pMaterial->ConvertToBinary(hFile, &dwByte);
	}

	CloseHandle(hFile);
	return S_OK;
}
