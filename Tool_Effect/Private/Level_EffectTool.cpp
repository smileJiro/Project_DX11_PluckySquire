#include "pch.h"
#include "GameInstance.h"
#include "Level_EffectTool.h"


CLevel_EffectTool::CLevel_EffectTool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_EffectTool::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_TestTerrain(TEXT("Layer_Terrain"))))
		return E_FAIL;
	if (FAILED(Ready_SkyBox(TEXT("Layer_SkyBox"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_EffectTool::Update(_float _fTimeDelta)
{
	Update_Particle_Tool(_fTimeDelta);
}

HRESULT CLevel_EffectTool::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("이펙트 툴입니다."));
#endif

	return S_OK;
}

HRESULT CLevel_EffectTool::Ready_Lights()
{
	LIGHT_DESC LightDesc{};

	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTOINAL;
	LightDesc.vDirection = _float4(-1.f, -1.f, 0.5f, 0.f);
	LightDesc.vDiffuse = _float4(1.0f, 1.0f, 1.0f, 1.f);
	LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_EffectTool::Ready_Layer_Camera(const _wstring& _strLayerTag)
{
	CCamera* pOut;
	CCamera_Free::CAMERA_FREE_DESC Desc{};

	Desc.fMouseSensor = 0.1f;

	Desc.fFovy = XMConvertToRadians(30.f);
	Desc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	Desc.vEye = _float3(0.f, 0.f, -5.f);
	Desc.vAt = _float3(0.f, 0.f, 1.f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Free"),
		LEVEL_TOOL, _strLayerTag, reinterpret_cast<CGameObject**>(&pOut), &Desc)))
		return E_FAIL;

	m_pGameInstance->Add_Camera(0, pOut);

	return S_OK;
}

HRESULT CLevel_EffectTool::Ready_Layer_Effect(const _wstring& _strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_TOOL, TEXT("Prototype_GameObject_EffectReference"),
		LEVEL_TOOL, _strLayerTag)))
		return E_FAIL;
	

	//CParticle_Sprite_Emitter::PARTICLE_EMITTER_DESC TempDesc = {};
	//TempDesc.eStartCoord = COORDINATE_3D;
	//TempDesc.iCurLevelID = LEVEL_TOOL;
	//TempDesc.isCoordChangeEnable = false;
	//TempDesc.iProtoShaderLevel = LEVEL_STATIC;
	//TempDesc.szShaderTag = L"Prototype_Component_Shader_VtxMeshInstance";
	//
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_Component_MeshParticle_Temp"),
	//	LEVEL_TOOL, _strLayerTag, &TempDesc)))
	//	return E_FAIL;

	/*CParticle_System* pOut;

	CParticle_System::PARTICLE_SYSTEM_DESC Desc = {};

	Desc.eStartCoord = COORDINATE_3D;
	Desc.iCurLevelID = LEVEL_TOOL;
	Desc.isCoordChangeEnable = false;
	Desc.EmitterShaderLevels.push_back(LEVEL_STATIC);
	Desc.ShaderTags.push_back(TEXT("Prototype_Component_Shader_VtxPointInstance"));

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_TOOL, TEXT("Prototype_Effect_Temp"),
		LEVEL_TOOL, _strLayerTag, reinterpret_cast<CGameObject**>(&pOut), &Desc)))
		return E_FAIL;

	if (nullptr == pOut)
		return E_FAIL;

	m_pNowItem = pOut;
	Safe_AddRef(pOut);
	m_ParticleSystems.push_back(pOut);*/


	//ZeroMemory(&Desc, sizeof(Desc));

	//Desc.eStartCoord = COORDINATE_3D;
	//Desc.iCurLevelID = LEVEL_TOOL;
	//Desc.isCoordChangeEnable = false;
	//Desc.EmitterShaderLevels.push_back(LEVEL_STATIC);
	//Desc.ShaderTags.push_back(TEXT("Prototype_Component_Shader_VtxMeshInstance"));

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_TOOL, TEXT("Prototype_MeshEffect_Temp"),
	//	LEVEL_TOOL, _strLayerTag, reinterpret_cast<CGameObject**>(&pOut), &Desc)))
	//	return E_FAIL;

	//if (nullptr == pOut)
	//	return E_FAIL;
	//m_pNowItem = pOut;
	//Safe_AddRef(pOut);
	//m_ParticleSystems.push_back(pOut);


	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_TOOL, TEXT("Prototype_Effect_Temp"),
	//	LEVEL_TOOL, _strLayerTag, reinterpret_cast<CGameObject**>(&pOut), &Desc)))
	//	return E_FAIL;

	//if (nullptr == pOut)
	//	return E_FAIL;
	//pOut->Set_Name(TEXT("Testing2"));
	//pOut->Set_Active(false);
	//Safe_AddRef(pOut);
	//m_ParticleSystems.push_back(pOut);

	return S_OK;
}

HRESULT CLevel_EffectTool::Ready_Layer_TestTerrain(const _wstring& _strLayerTag)
{
	CGameObject::GAMEOBJECT_DESC Desc = {};
	Desc.iCurLevelID = LEVEL_TOOL;
	Desc.isCoordChangeEnable = false;
	Desc.eStartCoord = COORDINATE_3D;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Object_Background"),
		LEVEL_TOOL, _strLayerTag, &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_EffectTool::Ready_SkyBox(const _wstring& _strLayerTag)
{
	CGameObject::GAMEOBJECT_DESC Desc = {};
	Desc.iCurLevelID = LEVEL_TOOL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Object_SkyBox"),
		LEVEL_TOOL, _strLayerTag, &Desc)))
		return E_FAIL;

	return S_OK;
}

void CLevel_EffectTool::Update_Particle_Tool(_float _fTimeDelta)
{
	ImGui::Begin("Particle System");
	Tool_System_List();
	ImGui::End();

	ImGui::Begin("Emitter");
	Tool_Adjust_System(_fTimeDelta);
	ImGui::End();

	ImGui::Begin("Textures");
	Tool_Texture();
	ImGui::End();
}

void CLevel_EffectTool::Tool_System_List()
{
	if (ImGui::TreeNode("Particle System List"))
	{
		ImGui::PushItemFlag(ImGuiItemFlags_NoNav, true);

		if (ImGui::BeginListBox("List"))
		{
			_int n = 0;
			static _int iNowIndex = 0;
			for (auto& Item : m_ParticleSystems)
			{
				const _bool is_selected = (iNowIndex == n);
				if (ImGui::Selectable(WSTRINGTOSTRING(Item->Get_Name()).c_str(), is_selected))
				{
					iNowIndex = n;
					if (m_pNowItem)
					{
						m_pNowItem->Set_Active(false);
					}
					m_pNowItem = m_ParticleSystems[iNowIndex];
					m_pNowItem->Set_Active(true);
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
				++n;
			}
			ImGui::EndListBox();
		}
		ImGui::PopItemFlag();

		if (ImGui::Button("New System"))
		{
			CParticle_System* pOut;
			CPartObject::PARTOBJECT_DESC Desc = {};

			Desc.eStartCoord = COORDINATE_3D;
			Desc.iCurLevelID = LEVEL_TOOL;
			Desc.isCoordChangeEnable = false;
			Desc.pParentMatrices[COORDINATE_3D] = nullptr;

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_TOOL, TEXT("Prototype_Effect_ForNew"),
				LEVEL_TOOL, TEXT("Layer_Effect"), reinterpret_cast<CGameObject**>(&pOut), &Desc)))
			{
				MSG_BOX("파티클 시스템 추가 불가능");
				return;
			}

			if (nullptr == pOut)
			{
				MSG_BOX("파티클 시스템 추가 불가능");
				return;
			}

			static _int iNumberName = 0;
			_wstring strName = L"New";
			_wstring strNum = to_wstring(iNumberName);
			strName += strNum;

			++iNumberName;

			pOut->Set_Name(strName);
			if (m_pNowItem)
			{
				m_pNowItem->Set_Active(false);
			}
			pOut->Set_Active(true);
			Safe_AddRef(pOut);
			m_ParticleSystems.push_back(pOut);
		}

		ImGui::Dummy(ImVec2(0.f, 5.f));

		if (ImGui::Button("Save All"))
		{
			Save_All();
		}
		ImGui::SameLine();
		if (ImGui::Button("Load All"))
		{
			Load_All("../Bin/DataFiles/Effects/");
		}

		ImGui::TreePop();
	}

}

void CLevel_EffectTool::Tool_Adjust_System(_float _fTimeDelta)
{
	if (m_pNowItem)
	{		
		m_pNowItem->Tool_Update(_fTimeDelta);

		if (ImGui::Button("New_Sprite_Emitter"))
		{
			CParticle_Emitter::PARTICLE_EMITTER_DESC Desc = {};

			Desc.eStartCoord = COORDINATE_3D;
			Desc.iCurLevelID = LEVEL_TOOL;
			Desc.isCoordChangeEnable = false;
			Desc.iProtoShaderLevel = LEVEL_STATIC;
			Desc.szShaderTag = L"Prototype_Component_Shader_VtxPointInstance";
			if (FAILED(m_pNowItem->Add_New_Emitter(CParticle_Emitter::SPRITE, &Desc)))
			{
				MSG_BOX("Sprite Emitter 만들기 실패");
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("New_Mesh_Emitter"))
		{
			CParticle_Emitter::PARTICLE_EMITTER_DESC Desc = {};

			Desc.eStartCoord = COORDINATE_3D; 
			Desc.iCurLevelID = LEVEL_TOOL;
			Desc.isCoordChangeEnable = false;
			Desc.iProtoShaderLevel = LEVEL_STATIC;
			Desc.szShaderTag = L"Prototype_Component_Shader_VtxModelInstance";
			if (FAILED(m_pNowItem->Add_New_Emitter(CParticle_Emitter::MESH, &Desc)))
			{
				MSG_BOX("Mesh Emitter 만들기 실패");
			}

		}



		if (ImGui::Button("Save_this_system"))
		{
			m_pNowItem->Save_File();
		}


	}

}



void CLevel_EffectTool::Tool_Texture()
{
	if (ImGui::TreeNode("Preview Texture"))
	{
		if (m_pNowTexture)
		{
			ImVec2 imageSize(100, 100); // 이미지 크기 설정
			ID3D11ShaderResourceView* pSelectImage = m_pNowTexture->Get_SRV(0);
			if (nullptr != pSelectImage)
			{
				ImGui::Image((ImTextureID)pSelectImage, imageSize);
			}

			if (m_pNowItem)
			{
				if (ImGui::Button("Set_Texture"))
				{
					m_pNowItem->Set_Texture(m_pNowTexture);
				}
			}

		}

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Texture"))
	{
		static _wstring wstrSelectedKey = L"";
		static int iSelectedIndex = -1;

		if (ImGui::BeginListBox("Textures List"))
		{
			int iIndex = 0;
			for (const auto& [key, pTexture] : m_Textures)
			{
				std::string strKey = WSTRINGTOSTRING(key);

				const bool is_selected = (iSelectedIndex == iIndex);

				if (ImGui::Selectable(strKey.c_str(), is_selected))
				{
					if (is_selected)
					{
						m_pNowTexture = nullptr;
						iSelectedIndex = -1; // 선택 해제
						wstrSelectedKey = L"";
					}
					else
					{
						m_pNowTexture = pTexture;
						iSelectedIndex = iIndex;
						wstrSelectedKey = key;
					}
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();

				++iIndex;
			}
			ImGui::EndListBox();
		}

		ImGui::InputText("Texture Directory", m_szTexturePath, MAX_PATH);

		if (ImGui::Button("Load_DDSTextures"))
		{
			Load_Textures(".dds");
		}

		if (ImGui::Button("Load_PNGTextures"))
		{
			Load_Textures(".png");
		}


		ImGui::TreePop();
	}
}

HRESULT CLevel_EffectTool::Load_Textures(const _char* _szExtension)
{
	std::filesystem::path path;
	path = m_szTexturePath;
	for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
		if (entry.path().extension() == _szExtension) {
			//cout << entry.path().string() << endl;

			CTexture* pTexture = CTexture::Create(m_pDevice, m_pContext, entry.path().string().c_str());
			
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, entry.path().filename(),
				pTexture)))
			{
				string str = "Failed to Create CTexture";
				str += entry.path().filename().string();
				MessageBoxA(NULL, str.c_str(), "에러", MB_OK);
				
				Safe_Release(pTexture);

				return E_FAIL;
			}

			pTexture->Add_SRVName(entry.path().c_str());
			m_Textures.emplace(entry.path().c_str(), pTexture);
			

			Safe_AddRef(pTexture);
		}
	}

	return S_OK;
}

HRESULT CLevel_EffectTool::Save_All()
{
	for (auto& pSystem : m_ParticleSystems)
	{
		if (FAILED(pSystem->Save_File()))
		{
			continue;
		}
	}

	return S_OK;
}

HRESULT CLevel_EffectTool::Load_All(const _char* _szPath)
{
	CParticle_System* pOut;
	CParticle_System::PARTICLE_SYSTEM_DESC Desc = {};

	Desc.eStartCoord = COORDINATE_3D;
	Desc.iCurLevelID = LEVEL_TOOL;
	Desc.isCoordChangeEnable = false;
	Desc.iSpriteShaderLevel = LEVEL_STATIC;
	Desc.szSpriteShaderTags = L"Prototype_Component_Shader_VtxPointInstance";
	Desc.iModelShaderLevel = LEVEL_STATIC;
	Desc.szModelShaderTags = L"Prototype_Component_Shader_VtxModelInstance";
	
	std::filesystem::path path;
	path = _szPath;

	for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
		if (entry.path().extension() == ".json") {
			//cout << entry.path().string() << endl;

			CParticle_System* pParticleSystem = CParticle_System::Create(m_pDevice, m_pContext, entry.path().c_str());

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, entry.path().filename(), pParticleSystem)))
			{
				MSG_BOX("불러오기 실패");
				return E_FAIL;
			}

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_TOOL, entry.path().filename(),
				LEVEL_TOOL, TEXT("Layer_Effect"), reinterpret_cast<CGameObject**>(&pOut), &Desc)))
			{
				MSG_BOX("Layer 추가 실패");
				return E_FAIL;
			}

			if (nullptr == pOut)
				return E_FAIL;

			//m_pNowItem = pOut;
			Safe_AddRef(pOut);
			m_ParticleSystems.push_back(pOut);
			pOut->Set_Active(false);

		}
	}

	return S_OK;

}

CLevel_EffectTool* CLevel_EffectTool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLevel_EffectTool* pInstance = new CLevel_EffectTool(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_EffectTool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_EffectTool::Free()
{
	for (auto& pParticleSystem : m_ParticleSystems)
		Safe_Release(pParticleSystem);
	m_ParticleSystems.clear();

	for (auto& Pair : m_Textures)
	{
		Safe_Release(Pair.second);
	}
	m_Textures.clear();

	__super::Free();


}
