#include "MeshEffect_Emitter.h"
#include "EffectModel.h"
#include "GameInstance.h"

CMeshEffect_Emitter::CMeshEffect_Emitter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CEmitter(_pDevice, _pContext)
{
	m_eEffectType = EFFECT;
}

CMeshEffect_Emitter::CMeshEffect_Emitter(const CMeshEffect_Emitter& _Prototype)
	: CEmitter(_Prototype)
	, m_pEffectModelCom(_Prototype.m_pEffectModelCom)
	, m_Textures(_Prototype.m_Textures)
	, m_vColor(_Prototype.m_vColor)
	, m_fAlpha(_Prototype.m_fAlpha)
#ifdef _DEBUG
	, m_strModelPath(_Prototype.m_strModelPath)
	, m_PreTransformMatrix(_Prototype.m_PreTransformMatrix)
#endif
{
	Safe_AddRef(m_pEffectModelCom);
	
	for (auto& pTexture : m_Textures)
		Safe_AddRef(pTexture);
}

HRESULT CMeshEffect_Emitter::Initialize_Prototype(const json& _jsonInfo)
{
	if (FAILED(__super::Initialize_Prototype(_jsonInfo)))
		return E_FAIL;

	m_Textures.resize(TEXTURE_END, nullptr);

	_float4x4 PreTransformMatrix;
	if (false == _jsonInfo.contains("PreTransform"))
	{
		XMStoreFloat4x4(&PreTransformMatrix, XMMatrixIdentity());
	}
	else
	{
		for (size_t i = 0; i < sizeof(_float4x4) / sizeof(_float); ++i)
		{
			*((_float*)(&PreTransformMatrix) + i) = _jsonInfo["PreTransform"][i];
		}
	}


	if (false == _jsonInfo.contains("Model"))
		return E_FAIL;
	_string strModelPath = _jsonInfo["Model"];
	m_pEffectModelCom = CEffectModel::Create(m_pDevice, m_pContext, strModelPath.c_str(), XMLoadFloat4x4(&PreTransformMatrix));

	if (_jsonInfo.contains("Color"))
	{
		for (_int i = 0; i < 3; ++i)
		{
			*((_float*)(&m_vColor) + i) = _jsonInfo["Color"][i];
		}
	}

	if (_jsonInfo.contains("Alpha"))
		m_fAlpha = _jsonInfo["Alpha"];

	if (FAILED(Load_TextureInfo(_jsonInfo)))
		return E_FAIL;

	



#ifdef _DEBUG
	m_strModelPath = strModelPath;
	m_PreTransformMatrix = PreTransformMatrix;
#endif
	return S_OK;
}

HRESULT CMeshEffect_Emitter::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;


	return S_OK;
}

void CMeshEffect_Emitter::Priority_Update(_float _fTimeDelta)
{
}

void CMeshEffect_Emitter::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CMeshEffect_Emitter::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);

	if (m_isActive)
		m_pGameInstance->Add_RenderObject_New(s_iRG_3D, s_iRGP_EFFECT, this);
}

HRESULT CMeshEffect_Emitter::Render()
{
	if (nullptr == m_pShaderCom || nullptr == m_pEffectModelCom)
		return S_OK;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pEffectModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		//m_pEffectModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)

		// TODO: PASS 설정
		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pEffectModelCom->Render(i);
	}


	return S_OK;
}

void CMeshEffect_Emitter::Reset()
{
	m_fAccTime = 0.f;

}

HRESULT CMeshEffect_Emitter::Bind_ShaderResources()
{
	if (m_isFollowParent)
	{
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrices[COORDINATE_3D])))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_IdentityMatrix)))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	// 누적 시간 전달 
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_fAccTime, sizeof(_float))))
		return E_FAIL;

	// Default Color
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float3))))
		return E_FAIL;

	// Default Alpha
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	// TODO : pass 등 구분..
	if (nullptr != m_Textures[MASK])
	{
		if (FAILED(m_Textures[MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
			return E_FAIL;
	}

	if (nullptr != m_Textures[NOISE])
	{
		if (FAILED(m_Textures[NOISE]->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture")))
			return E_FAIL;
	}



	return S_OK;
}



HRESULT CMeshEffect_Emitter::Ready_Components(const PARTICLE_EMITTER_DESC* _pDesc)
{
	if (FAILED(__super::Ready_Components(_pDesc)))
		return E_FAIL;

	m_Components.emplace(L"Com_Effect", m_pEffectModelCom);
	Safe_AddRef(m_pEffectModelCom);


	return S_OK;
}

HRESULT CMeshEffect_Emitter::Load_TextureInfo(const json& _jsonInfo)
{
	if (_jsonInfo.contains("Textures"))
	{
		for (_int i = 0; i < _jsonInfo["Textures"].size(); ++i)
		{
			TEXTURE_TYPE eType = _jsonInfo["Textures"][i]["Type"];
			_string str = _jsonInfo["Textures"][i]["Path"];

			CTexture* pTexture = CTexture::Create(m_pDevice, m_pContext, str.c_str());
			if (nullptr != pTexture)
				m_Textures[eType] = pTexture;

#ifdef _DEBUG
			m_Textures[eType]->Add_SRVName(STRINGTOWSTRING(str));
#endif
		}
	}

	return S_OK;
}



CMeshEffect_Emitter* CMeshEffect_Emitter::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const json& _jsonInfo)
{
	CMeshEffect_Emitter* pInstance = new CMeshEffect_Emitter(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_jsonInfo)))
	{
		MSG_BOX("Failed to Created : CMeshEffect_Emitter");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMeshEffect_Emitter::Clone(void* _pArg)
{
	CMeshEffect_Emitter* pInstance = new CMeshEffect_Emitter(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CMeshEffect_Emitter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMeshEffect_Emitter::Free()
{
	Safe_Release(m_pEffectModelCom);

	for (auto& pTexture : m_Textures)
		Safe_Release(pTexture);

	__super::Free();
}

HRESULT CMeshEffect_Emitter::Cleanup_DeadReferences()
{
	return S_OK;
}

void CMeshEffect_Emitter::Tool_Setting()
{
}

void CMeshEffect_Emitter::Tool_Update(_float _fTimeDelta)
{
	ImGui::Begin("Adjust_Effect_Emitter");

	if (ImGui::TreeNode("Set Emitter State"))
	{
		__super::Tool_Update(_fTimeDelta);

		ImGui::TreePop();
	}

	if (m_pEffectModelCom)
	{
		ImGui::InputFloat3("Default Color", (_float*)(&m_vColor), "%.3f");
		ImGui::InputFloat("Default Alpha", (_float*)(&m_fAlpha));
	}
	else
	{
		
	}


	ImGui::End();
}

HRESULT CMeshEffect_Emitter::Save(json& _jsonOut)
{
	if (nullptr == m_pEffectModelCom)
	{
		MSG_BOX("Texture 혹은 Buffer 없음");
		return E_FAIL;
	}

	if (FAILED(__super::Save(_jsonOut)))
		return E_FAIL;

	_jsonOut["Model"] = m_strModelPath.c_str();
	for (_int i = 0; i < 16; ++i)
	{
		_jsonOut["PreTransform"].push_back(*((_float*)(&m_PreTransformMatrix) + i));
	}

	for (_int i = 0; i < 3; ++i)
	{
		_jsonOut["Color"].push_back(*((_float*)(&m_vColor) + i));
	}
	_jsonOut["Alpha"] = m_fAlpha;
	
	if (nullptr != m_Textures[MASK])
	{
		json jsonInfo;
		jsonInfo["Path"] = WSTRINGTOSTRING(*m_Textures[MASK]->Get_SRVName(0)).c_str();
		jsonInfo["Type"] = MASK;

		_jsonOut["Textures"].push_back(jsonInfo);
	}

	if (nullptr != m_Textures[NOISE])
	{
		json jsonInfo;
		jsonInfo["Path"] = WSTRINGTOSTRING(*m_Textures[NOISE]->Get_SRVName(0)).c_str();
		jsonInfo["Type"] = NOISE;

		_jsonOut["Textures"].push_back(jsonInfo);
	}

	return S_OK;
}

#ifdef _DEBUG
void CMeshEffect_Emitter::Set_Texture(CTexture* _pTextureCom, _uint _iType)
{
	if (_iType >= m_Textures.size())
		return;

	if (nullptr != m_Textures[_iType])
	{
		Safe_Release(m_Textures[_iType]);
	}

	m_Textures[_iType] = _pTextureCom;
	Safe_AddRef(m_Textures[_iType]);

	
}
CMeshEffect_Emitter* CMeshEffect_Emitter::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg)
{
	return nullptr;
}
#endif