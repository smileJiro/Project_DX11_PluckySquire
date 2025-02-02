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
	, m_eShaderPass(_Prototype.m_eShaderPass)
	, m_FloatDatas(_Prototype.m_FloatDatas)
	, m_Float2Datas(_Prototype.m_Float2Datas)
	, m_Float4Datas(_Prototype.m_Float4Datas)
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
		for (_int i = 0; i < 4; ++i)
		{
			*((_float*)(&m_vColor) + i) = _jsonInfo["Color"][i];
		}
	}

	if (_jsonInfo.contains("Float"))
	{
		for (_int i = 0; i < _jsonInfo["Float"].size(); ++i)
		{
			_string str = _jsonInfo["Float"][i]["Name"];
			_float fValue = _jsonInfo["Float"][i]["Value"];
			m_FloatDatas.emplace(str, fValue);
		}
	}

	if (_jsonInfo.contains("Float2"))
	{
		for (_int i = 0; i < _jsonInfo["Float2"].size(); ++i)
		{
			_string str = _jsonInfo["Float2"][i]["Name"];
			_float2 vValue;
			vValue.x = _jsonInfo["Float2"][i]["Value"][0];
			vValue.y = _jsonInfo["Float2"][i]["Value"][1];

			m_Float2Datas.emplace(str, vValue);
		}
	}

	if (_jsonInfo.contains("Float4"))
	{
		for (_int i = 0; i < _jsonInfo["Float4"].size(); ++i)
		{
			_string str = _jsonInfo["Float4"][i]["Name"];
			_float4 vValue;
			vValue.x = _jsonInfo["Float4"][i]["Value"][0];
			vValue.y = _jsonInfo["Float4"][i]["Value"][1];
			vValue.z = _jsonInfo["Float4"][i]["Value"][2];
			vValue.w = _jsonInfo["Float4"][i]["Value"][3];

			m_Float4Datas.emplace(str, vValue);
		}
	}


	if (_jsonInfo.contains("ShaderPass"))
		m_eShaderPass = _jsonInfo["ShaderPass"];

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
	// World Matrix 설정을 어떻게 해야할까 ? 
	__super::Late_Update(_fTimeDelta);

	if (m_isActive && m_iAccLoop)
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
		if (FAILED(m_pShaderCom->Begin(m_eShaderPass)))
			return E_FAIL;

		m_pEffectModelCom->Render(i);
	}


	return S_OK;
}

void CMeshEffect_Emitter::Reset()
{
	m_fAccTime = 0.f;

}

void CMeshEffect_Emitter::On_Event()
{
}

void CMeshEffect_Emitter::Off_Event()
{
}

HRESULT CMeshEffect_Emitter::Bind_ShaderResources()
{
	if (m_isWorld)
	{
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrices[COORDINATE_3D])))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	// Default Color
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(Bind_ShaderValue_ByPass()))
		return E_FAIL;


	return S_OK;
}

HRESULT CMeshEffect_Emitter::Bind_ShaderValue_ByPass()
{
	switch (m_eShaderPass)
	{
	case DEFAULT:
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
			return E_FAIL;
		break;
	case DISSOLVE:
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
			return E_FAIL;


		if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_fAccTime, sizeof(_float))))
			return E_FAIL;

		if (FAILED(Bind_Texture(ALPHA, "g_AlphaTexture")))
			return E_FAIL;
		if (FAILED(Bind_Texture(MASK, "g_MaskTexture")))
			return E_FAIL;
		if (FAILED(Bind_Texture(NOISE, "g_NoiseTexture")))
			return E_FAIL;

		if (FAILED(Bind_Float("DissolveFactor", "g_fDissolveFactor")))
			return E_FAIL;
		if (FAILED(Bind_Float("AlphaTest", "g_fAlphaTest")))
			return E_FAIL;
		if (FAILED(Bind_Float("ColorTest", "g_fColorTest")))
			return E_FAIL;
		if (FAILED(Bind_Float4("AlphaUVScale", "g_AlphaUVScale")))
			return E_FAIL;
		if (FAILED(Bind_Float4("MaskUVScale", "g_MaskUVScale")))
			return E_FAIL;
		if (FAILED(Bind_Float4("NoiseUVScale", "g_NoiseUVScale")))
			return E_FAIL;

		//if (FAILED(Bind_Float2("AlphaUVScale", "g_AlphaUVScale")))
		//	return E_FAIL;
		//if (FAILED(Bind_Float2("MaskUVScale", "g_MaskUVScale")))
		//	return E_FAIL;
		//if (FAILED(Bind_Float2("NoiseUVScale", "g_NoiseUVScale")))
		//	return E_FAIL;

		break;
	}

	}


	return S_OK;
}

HRESULT CMeshEffect_Emitter::Bind_Texture(TEXTURE_TYPE _eType, const _char* _szConstantName)
{
	if (nullptr != m_Textures[_eType] && _eType < TEXTURE_END)
	{
		return m_Textures[_eType]->Bind_ShaderResource(m_pShaderCom, _szConstantName);
	}
	else
		return E_FAIL;

	return S_OK;
}

HRESULT CMeshEffect_Emitter::Bind_Float(const _char* _szDataName, const _char* _szConstantName)
{
	return m_pShaderCom->Bind_RawValue(_szConstantName, &m_FloatDatas[_szDataName], sizeof(_float));
}

HRESULT CMeshEffect_Emitter::Bind_Float2(const _char* _szDataName, const _char* _szConstantName)
{
	return m_pShaderCom->Bind_RawValue(_szConstantName, &m_Float2Datas[_szDataName], sizeof(_float2));
}

HRESULT CMeshEffect_Emitter::Bind_Float4(const _char* _szDataName, const _char* _szConstantName)
{
	return m_pShaderCom->Bind_RawValue(_szConstantName, &m_Float4Datas[_szDataName], sizeof(_float4));
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

#ifdef _DEBUG

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

	Tool_SetEffect();


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

	for (_int i = 0; i < 4; ++i)
	{
		_jsonOut["Color"].push_back(*((_float*)(&m_vColor) + i));
	}

	_jsonOut["ShaderPass"] = m_eShaderPass;

	for (_int i = 0; i < TEXTURE_END; ++i)
	{
		if (nullptr != m_Textures[i])
		{
			json jsonInfo;
			jsonInfo["Path"] = WSTRINGTOSTRING(*m_Textures[i]->Get_SRVName(0)).c_str();
			jsonInfo["Type"] = (TEXTURE_TYPE)i;

			_jsonOut["Textures"].push_back(jsonInfo);
		}
	}

	for (auto& Pair : m_FloatDatas)
	{
		json jsonInfo;
		jsonInfo["Name"] = Pair.first.c_str();
		jsonInfo["Value"] = Pair.second;
		_jsonOut["Float"].push_back(jsonInfo);
	}

	for (auto& Pair : m_Float2Datas)
	{
		json jsonInfo;
		jsonInfo["Name"] = Pair.first.c_str();
		jsonInfo["Value"].push_back(Pair.second.x);
		jsonInfo["Value"].push_back(Pair.second.y);
		_jsonOut["Float2"].push_back(jsonInfo);
	}

	for (auto& Pair : m_Float4Datas)
	{
		json jsonInfo;
		jsonInfo["Name"] = Pair.first.c_str();
		jsonInfo["Value"].push_back(Pair.second.x);
		jsonInfo["Value"].push_back(Pair.second.y);
		jsonInfo["Value"].push_back(Pair.second.z);
		jsonInfo["Value"].push_back(Pair.second.w);
		_jsonOut["Float4"].push_back(jsonInfo);
	}



	return S_OK;
}

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
void CMeshEffect_Emitter::Tool_SetEffect()
{
	if (m_pEffectModelCom)
	{
		if (ImGui::TreeNode("Color"))
		{
			ImGui::DragFloat4("Default Color", (_float*)(&m_vColor), 0.01f);


			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Pass"))
		{
			const _char* items[] = { "Default", "Dissolve", "Dissolve With UV" };
			static _int item_selected_idx = 0;
			const char* combo_preview_value = items[item_selected_idx];

			if (ImGui::BeginCombo("Effect Type", combo_preview_value))
			{
				item_selected_idx = m_eShaderPass;
				ImGui::SetItemDefaultFocus();

				for (int n = 0; n < IM_ARRAYSIZE(items); n++)
				{
					const bool is_selected = (item_selected_idx == n);

					if (ImGui::Selectable(items[n], is_selected))
					{
						item_selected_idx = n;
						if (m_eShaderPass != n)
						{
							m_eShaderPass = (EFFECT_SHADERPASS)n;
							m_FloatDatas.clear();
							m_Float2Datas.clear();
						}
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Float"))
		{
			static _char szInput[MAX_PATH] = "";
			 

			for (auto& Pair : m_FloatDatas)
			{
				ImGui::DragFloat(Pair.first.c_str(), (_float*)&Pair.second, 0.01f);
			}

			ImGui::TreePop();
		}
	
		if (ImGui::TreeNode("Float2"))
		{
			for (auto& Pair : m_Float2Datas)
			{
				ImGui::DragFloat2(Pair.first.c_str(), (_float*)&Pair.second, 0.01f);
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Float4"))
		{
			for (auto& Pair : m_Float4Datas)
			{
				ImGui::DragFloat4(Pair.first.c_str(), (_float*)&Pair.second, 0.01f);
			}

			ImGui::TreePop();
		}
	
		if (ImGui::TreeNode("Show Textures"))
		{
			if (ImGui::TreeNode("Alpha"))
			{
				if (nullptr != m_Textures[ALPHA])
				{
					ImVec2 imageSize(100, 100); // 이미지 크기 설정
					ID3D11ShaderResourceView* pSelectImage = m_Textures[ALPHA]->Get_SRV(0);
					if (nullptr != pSelectImage)
					{
						ImGui::Image((ImTextureID)pSelectImage, imageSize);
					}
					
					ImGui::Text(WSTRINGTOSTRING(*m_Textures[ALPHA]->Get_SRVName(0)).c_str());

					if (ImGui::Button("Delete"))
					{
						Safe_Release(m_Textures[ALPHA]);
					}
				}
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Mask"))
			{
				if (nullptr != m_Textures[MASK])
				{
					ImVec2 imageSize(100, 100); // 이미지 크기 설정
					ID3D11ShaderResourceView* pSelectImage = m_Textures[MASK]->Get_SRV(0);
					if (nullptr != pSelectImage)
					{
						ImGui::Image((ImTextureID)pSelectImage, imageSize);
					}

					ImGui::Text(WSTRINGTOSTRING(*m_Textures[MASK]->Get_SRVName(0)).c_str());

					if (ImGui::Button("Delete"))
					{
						Safe_Release(m_Textures[MASK]);
					}
				}
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Noise"))
			{
				if (nullptr != m_Textures[NOISE])
				{
					ImVec2 imageSize(100, 100); // 이미지 크기 설정
					ID3D11ShaderResourceView* pSelectImage = m_Textures[NOISE]->Get_SRV(0);
					if (nullptr != pSelectImage)
					{
						ImGui::Image((ImTextureID)pSelectImage, imageSize);
					}

					ImGui::Text(WSTRINGTOSTRING(*m_Textures[NOISE]->Get_SRVName(0)).c_str());


					if (ImGui::Button("Delete"))
					{
						Safe_Release(m_Textures[NOISE]);
					}
				}
				ImGui::TreePop();
			}
			ImGui::TreePop();

		}
	}
	else
	{

	}
}
CMeshEffect_Emitter* CMeshEffect_Emitter::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg)
{
	return nullptr;
}
#endif