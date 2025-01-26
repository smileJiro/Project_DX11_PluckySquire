#include "Particle_Mesh_Emitter.h"
#include "GameInstance.h"
#include "VIBuffer_Mesh_Particle.h"
#include "Material.h"
#include "Bone.h"

CParticle_Mesh_Emitter::CParticle_Mesh_Emitter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CParticle_Emitter(_pDevice, _pContext)
{
	m_eType = MESH;
}

CParticle_Mesh_Emitter::CParticle_Mesh_Emitter(const CParticle_Mesh_Emitter& _Prototype)
	: CParticle_Emitter(_Prototype)
	, m_PreTransformMatrix(_Prototype.m_PreTransformMatrix)
	, m_iNumMeshes(_Prototype.m_iNumMeshes)
	, m_iNumMaterials(_Prototype.m_iNumMaterials)
	, m_Materials(_Prototype.m_Materials)
{
	if (_Prototype.m_Meshes.size())
	{
		for (auto& pPrototypeMesh : _Prototype.m_Meshes)
		{
			CVIBuffer_Mesh_Particle* pMesh = static_cast<CVIBuffer_Mesh_Particle*>(pPrototypeMesh->Clone(nullptr));
			if (nullptr != pMesh)
				m_Meshes.push_back(pMesh);
		}
	}

	if (_Prototype.m_Bones.size())
	{
		for (auto& pPrototypeBone : _Prototype.m_Bones)
		{
			m_Bones.push_back(pPrototypeBone->Clone());
		}
	}

	for (auto& pMaterial : m_Materials)
		Safe_AddRef(pMaterial);

}

HRESULT CParticle_Mesh_Emitter::Initialize_Prototype(const _char* _szModelPath, const _tchar* _szInfoPath)
{
	//XMMATRIX matPreTransform = XMMatrixScaling(1 / 150.0f, 1 / 150.0f, 1 / 150.0f);
	//matPreTransform *= XMMatrixRotationAxis(_vector{ 0,1,0,0 }, XMConvertToRadians(180));
	//XMStoreFloat4x4(&m_PreTransformMatrix, matPreTransform);

	json jsonEmitterInfo;
	if (FAILED(m_pGameInstance->Load_Json(_szInfoPath, &jsonEmitterInfo)))
		return E_FAIL;

	if (false == jsonEmitterInfo.contains("Emitters"))
		return E_FAIL;

	if (false == jsonEmitterInfo["Emitters"].contains("PreTransform"))
		return E_FAIL;

	for (size_t i = 0; i < sizeof(_float4x4) / sizeof(_float); ++i)
	{
		*((_float*)(&m_PreTransformMatrix) + i) = jsonEmitterInfo["Emitters"]["PreTransform"][i];
	}


	std::ifstream inFile(_szModelPath, std::ios::binary);
	if (!inFile) {
		string str = "파일을 열 수 없습니다.";
		str += _szModelPath;
		MessageBoxA(NULL, str.c_str(), "에러", MB_OK);
		return E_FAIL;
	}

	_bool bAnim;
	inFile.read(reinterpret_cast<char*>(&bAnim), 1);
	if (FAILED(Ready_Bones(inFile, -1)))
		return E_FAIL;


	// TODO: 바꿔!!
	if (FAILED(Ready_Meshes(inFile, jsonEmitterInfo["Emitters"][0]["Buffer"])))
		return E_FAIL;

	if (FAILED(Ready_Materials(inFile, _szModelPath)))
		return E_FAIL;

	inFile.close();

	return S_OK;
}

HRESULT CParticle_Mesh_Emitter::Initialize_Prototype(const json& _jsonInfo)
{
	if (false == _jsonInfo.contains("PreTransform"))
	{
		XMStoreFloat4x4(&m_PreTransformMatrix, XMMatrixIdentity());
	}
	else
	{
		for (size_t i = 0; i < sizeof(_float4x4) / sizeof(_float); ++i)
		{
			*((_float*)(&m_PreTransformMatrix) + i) = _jsonInfo["PreTransform"][i];
		}
	}


	if (false == _jsonInfo.contains("Model"))
		return E_FAIL;

	_string strModelPath = _jsonInfo["Model"];

	std::ifstream inFile(strModelPath.c_str(), std::ios::binary);
	if (!inFile) {
		string str = "파일을 열 수 없습니다.";
		str += strModelPath;
		MessageBoxA(NULL, str.c_str(), "에러", MB_OK);
		return E_FAIL;
	}

	_bool bAnim;
	inFile.read(reinterpret_cast<char*>(&bAnim), 1);
	if (FAILED(Ready_Bones(inFile, -1)))
		return E_FAIL;


	if (FAILED(Ready_Meshes(inFile, _jsonInfo["Buffer"])))
		return E_FAIL;

	if (FAILED(Ready_Materials(inFile, strModelPath.c_str())))
		return E_FAIL;

	inFile.close();



	return S_OK;
}


HRESULT CParticle_Mesh_Emitter::Initialize(void* _pArg)
{
	PARTICLE_EMITTER_DESC* pDesc = static_cast<PARTICLE_EMITTER_DESC*>(_pArg);

	if (nullptr == pDesc)
		return E_FAIL;

	pDesc->eStartCoord = COORDINATE_3D;
	pDesc->isCoordChangeEnable = false;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CParticle_Mesh_Emitter::Priority_Update(_float _fTimeDelta)
{
}

void CParticle_Mesh_Emitter::Update(_float _fTimeDelta)
{
	Update_Component(_fTimeDelta);
}

void CParticle_Mesh_Emitter::Late_Update(_float _fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CParticle_Mesh_Emitter::Render()
{
	if (nullptr == m_pShaderCom || 0 == m_Meshes.size())
		return S_OK;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		// TODO : 어떻게 하지
		if (FAILED(Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		// PASS 설정
		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_Meshes[i]->Bind_BufferDesc();
		m_Meshes[i]->Render();
	}


	return S_OK;
}

HRESULT CParticle_Mesh_Emitter::Bind_ShaderResources()
{
	// TODO
	if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;



	return S_OK;
}

HRESULT CParticle_Mesh_Emitter::Bind_Material(CShader* _pShader, const _char* _pConstantName, _uint _iMeshIndex, aiTextureType _eTextureType, _uint _iTextureIndex)
{
	_uint iMaterialIndex = m_Meshes[_iMeshIndex]->Get_MaterialIndex();

	ID3D11ShaderResourceView* pSRV = m_Materials[iMaterialIndex]->Find_Texture(_eTextureType, _iTextureIndex);
	if (nullptr == pSRV)
		return E_FAIL;

	return _pShader->Bind_SRV(_pConstantName, pSRV);

}

HRESULT CParticle_Mesh_Emitter::Ready_Components(const PARTICLE_EMITTER_DESC* _pDesc)
{
	if (FAILED(__super::Ready_Components(_pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_Mesh_Emitter::Ready_Bones(ifstream& _inFile, _uint _iParentBoneIndex)
{
	CBone* pBone = CBone::Create(_inFile, _iParentBoneIndex);

	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);

	_iParentBoneIndex = (_uint)m_Bones.size() - 1;
	_uint iNumChildren = 0;
	_inFile.read(reinterpret_cast<char*>(&iNumChildren), sizeof(_uint));
	//cout << iNumChildren << endl;
	//for (_uint i = 0; i < iNumChildren; ++i)
	//{
	//	Ready_Bones(_inFile, _iParentBoneIndex);
	//}

	return S_OK;
}

HRESULT CParticle_Mesh_Emitter::Ready_Meshes(ifstream& _inFile, const json& _jsonBufferInfo)
{
	_inFile.read(reinterpret_cast<char*>(&m_iNumMeshes), sizeof(_uint));
	for (_uint i = 0; i < m_iNumMeshes; i++)
	{
		CVIBuffer_Mesh_Particle* pMesh = CVIBuffer_Mesh_Particle::Create(m_pDevice, m_pContext, _inFile, _jsonBufferInfo, XMLoadFloat4x4(&m_PreTransformMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CParticle_Mesh_Emitter::Ready_Materials(ifstream& _inFile, const _char* _szModelFilePath)
{
	_inFile.read(reinterpret_cast<char*>(&m_iNumMaterials), sizeof(_uint));
	//cout << m_iNumMaterials << endl;
	m_Materials.resize(m_iNumMaterials);
	_char		szDrive[MAX_PATH] = "";
	_char		szDirectory[MAX_PATH] = "";
	_splitpath_s(_szModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);
	strcat_s(szDrive, szDirectory);
	for (_uint i = 0; i < m_iNumMaterials; i++)
	{
		CMaterial* pMaterial = CMaterial::Create(m_pDevice, m_pContext, szDrive, _inFile);
		m_Materials[i] = pMaterial;
	}
	return S_OK;
}

CParticle_Mesh_Emitter* CParticle_Mesh_Emitter::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _char* _szModelPath, const _tchar* _szInfoPath)
{
	CParticle_Mesh_Emitter* pInstance = new CParticle_Mesh_Emitter(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_szModelPath, _szInfoPath)))
	{
		MSG_BOX("Failed to Created : CParticle_Mesh_Emitter");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CParticle_Mesh_Emitter* CParticle_Mesh_Emitter::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const json& _jsonInfo)
{
	CParticle_Mesh_Emitter* pInstance = new CParticle_Mesh_Emitter(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_jsonInfo)))
	{
		MSG_BOX("Failed to Created : CParticle_Mesh_Emitter");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CGameObject* CParticle_Mesh_Emitter::Clone(void* _pArg)
{
	CParticle_Mesh_Emitter* pInstance = new CParticle_Mesh_Emitter(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CParticle_Mesh_Emitter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_Mesh_Emitter::Free()
{
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

HRESULT CParticle_Mesh_Emitter::Cleanup_DeadReferences()
{
	return S_OK;
}
