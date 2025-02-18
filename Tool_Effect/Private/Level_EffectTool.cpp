#include "pch.h"
#include "GameInstance.h"
#include "Level_EffectTool.h"
#include "CubeMap.h"
#include "SpriteEffect_Emitter.h"
#include "ModelObject.h"
#include "ContainerObject.h"
#include "3DModel.h"

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
	if (FAILED(Ready_Layer_Model(TEXT("Layer_Model"))))
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
	CONST_LIGHT LightDesc{};

	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.vPosition = _float3(0.0f, 20.0f, 0.0f);
	LightDesc.fFallOutStart = 20.0f;
	LightDesc.fFallOutEnd = 1000.0f;
	LightDesc.vRadiance = _float3(1.0f, 1.0f, 1.0f);
	LightDesc.vDiffuse = _float4(1.0f, 0.0f, 0.0f, 1.0f);
	LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 1.0f);
	LightDesc.vSpecular = _float4(1.0f, 0.0f, 0.0f, 1.0f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc, LIGHT_TYPE::POINT)))
		return E_FAIL;


	return S_OK;


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

	/*CEffect_System* pOut;

	CEffect_System::EFFECT_SYSTEM_DESC Desc = {};

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

	CGameObject* pCubeMap = nullptr;
	CCubeMap::CUBEMAP_DESC CubeMapDesc;
	CubeMapDesc.iCurLevelID = LEVEL_TOOL;
	CubeMapDesc.iRenderGroupID = RG_3D;
	CubeMapDesc.iPriorityID = PR3D_PRIORITY;
	CubeMapDesc.strBRDFPrototypeTag = TEXT("Prototype_Component_Texture_BRDF_Shilick");
	CubeMapDesc.strCubeMapPrototypeTag = TEXT("Prototype_Component_Texture_TestEnv");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_CubeMap"),
		LEVEL_TOOL, _strLayerTag, &pCubeMap, &CubeMapDesc)))
		return E_FAIL;

	m_pGameInstance->Set_CubeMap(static_cast<CCubeMap*>(pCubeMap));
	return S_OK;

	return S_OK;
}

HRESULT CLevel_EffectTool::Ready_Layer_Model(const _wstring& _strLayerTag)
{
	CModelObject* pOut = { nullptr };
	CModelObject::MODELOBJECT_DESC Desc = {};

	Desc.eStartCoord = COORDINATE_3D;
	Desc.iCurLevelID = LEVEL_TOOL;
	Desc.iModelPrototypeLevelID_3D = LEVEL_TOOL;
	Desc.strModelPrototypeTag_3D = L"Prototype_Model_Player";
	Desc.strShaderPrototypeTag_3D = L"Prototype_Component_Shader_VtxAnimMesh";
	Desc.iShaderPass_3D = 0;
	Desc.iPriorityID_3D = PR3D_GEOMETRY;
	Desc.iRenderGroupID_3D = RG_3D;
	Desc.tTransform3DDesc.fSpeedPerSec = 1.f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_ModelObject"),
		LEVEL_TOOL, _strLayerTag, reinterpret_cast<CGameObject**>(&pOut), &Desc)))
		return E_FAIL;

	pOut->Set_Active(false);
	m_ModelObjects.push_back(pOut);


	Desc.eStartCoord = COORDINATE_3D;
	Desc.iCurLevelID = LEVEL_TOOL;
	Desc.iModelPrototypeLevelID_3D = LEVEL_TOOL;
	Desc.strModelPrototypeTag_3D = L"Prototype_Model_Book";
	Desc.strShaderPrototypeTag_3D = L"Prototype_Component_Shader_VtxAnimMesh";
	Desc.iShaderPass_3D = 0;
	Desc.iPriorityID_3D = PR3D_GEOMETRY;
	Desc.iRenderGroupID_3D = RG_3D;
	Desc.tTransform3DDesc.fSpeedPerSec = 1.f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_ModelObject"),
		LEVEL_TOOL, _strLayerTag, reinterpret_cast<CGameObject**>(&pOut), &Desc)))
		return E_FAIL;

	pOut->Set_Active(false);
	pOut->Set_Position(XMVectorSet(2.f, 0.4f, -17.3f, 1.f));
	m_ModelObjects.push_back(pOut);


	Desc.eStartCoord = COORDINATE_3D;
	Desc.iCurLevelID = LEVEL_TOOL;
	Desc.iModelPrototypeLevelID_3D = LEVEL_TOOL;
	Desc.strModelPrototypeTag_3D = L"Prototype_Model_MagicHand";
	Desc.strShaderPrototypeTag_3D = L"Prototype_Component_Shader_VtxAnimMesh";
	Desc.iShaderPass_3D = 6;
	Desc.iPriorityID_3D = PR3D_BLEND;
	Desc.iRenderGroupID_3D = RG_3D;
	Desc.tTransform3DDesc.fSpeedPerSec = 1.f;


	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_TOOL, TEXT("Prototype_GameObject_MagicHandBody"),
		LEVEL_TOOL, _strLayerTag, reinterpret_cast<CGameObject**>(&pOut), &Desc)))
		return E_FAIL;

	pOut->Set_Active(false);
	pOut->Set_Position(XMVectorSet(2.f, 0.4f, -17.3f, 1.f));
	m_ModelObjects.push_back(pOut);

	//

	CContainerObject::CONTAINEROBJ_DESC ContainerDesc = {};
	ContainerDesc.eStartCoord = COORDINATE_3D;
	ContainerDesc.iCurLevelID = LEVEL_TOOL;
	ContainerDesc.iNumPartObjects = 2;
	ContainerDesc.isCoordChangeEnable = false;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_TOOL, TEXT("Prototype_GameObject_MagicHand"),
		LEVEL_TOOL, _strLayerTag, &ContainerDesc)))
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

	Tool_Texture();
	Tool_ControlModel(_fTimeDelta);
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
					m_pNowItem->Active_Effect(true, 0);
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
			CEffect_System* pOut;
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
			CEmitter::PARTICLE_EMITTER_DESC Desc = {};

			Desc.eStartCoord = COORDINATE_3D;
			Desc.iCurLevelID = LEVEL_TOOL;
			Desc.isCoordChangeEnable = false;
			Desc.iProtoShaderLevel = LEVEL_STATIC;
			Desc.szShaderTag = L"Prototype_Component_Shader_VtxPointInstance";
			Desc.szComputeShaderTag = L"Prototype_ComputeShader";
			if (FAILED(m_pNowItem->Add_New_Emitter(CEmitter::SPRITE, &Desc)))
			{
				MSG_BOX("Sprite Emitter 만들기 실패");
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("New_Mesh_Emitter"))
		{
			CEmitter::PARTICLE_EMITTER_DESC Desc = {};

			Desc.eStartCoord = COORDINATE_3D; 
			Desc.iCurLevelID = LEVEL_TOOL;
			Desc.isCoordChangeEnable = false;
			Desc.iProtoShaderLevel = LEVEL_STATIC;
			Desc.szShaderTag = L"Prototype_Component_Shader_VtxModelInstance";
			Desc.szComputeShaderTag = L"Prototype_ComputeShader_Mesh";

			if (FAILED(m_pNowItem->Add_New_Emitter(CEmitter::MESH, &Desc)))
			{
				MSG_BOX("Mesh Emitter 만들기 실패");
			}

		}
		ImGui::SameLine();

		if (ImGui::Button("New_Sprite_Effect"))
		{
			CSpriteEffect_Emitter::SPRITE_EMITTER_DESC Desc = {};

			Desc.eStartCoord = COORDINATE_3D;
			Desc.iCurLevelID = LEVEL_TOOL;
			Desc.isCoordChangeEnable = false;
			Desc.iProtoShaderLevel = LEVEL_STATIC;
			Desc.szShaderTag = L"Prototype_Component_Shader_VtxPoint";
			Desc.iProtoRectLevel = LEVEL_STATIC;
			Desc.szRectTag = L"Prototype_Component_VIBuffer_Point";
			Desc.szComputeShaderTag = nullptr;

			if (FAILED(m_pNowItem->Add_New_Emitter(CEmitter::SINGLE_SPRITE, &Desc)))
			{
				MSG_BOX("Effect 만들기 실패");
			}

		}

		ImGui::SameLine();

		if (ImGui::Button("New_Effect"))
		{
			CEmitter::PARTICLE_EMITTER_DESC Desc = {};

			Desc.eStartCoord = COORDINATE_3D;
			Desc.iCurLevelID = LEVEL_TOOL;
			Desc.isCoordChangeEnable = false;
			Desc.iProtoShaderLevel = LEVEL_STATIC;
			Desc.szShaderTag = L"Prototype_Component_Shader_Effect";
			Desc.szComputeShaderTag = nullptr;

			if (FAILED(m_pNowItem->Add_New_Emitter(CEmitter::EFFECT, &Desc)))
			{
				MSG_BOX("Effect 만들기 실패");
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
	ImGui::Begin("Particle Textures");

	if (ImGui::TreeNode("Preview Particle Texture"))
	{
		if (m_pParticleTexture)
		{
			ImVec2 imageSize(300, 300); // 이미지 크기 설정
			ID3D11ShaderResourceView* pSelectImage = m_pParticleTexture->Get_SRV(0);
			if (nullptr != pSelectImage)
			{
				ImGui::Image((ImTextureID)pSelectImage, imageSize);
			}

			if (m_pNowItem)
			{
				if (ImGui::Button("Set_Texture"))
				{
					m_pNowItem->Set_Texture(m_pParticleTexture);
				}
			}

		}

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Particle Texture"))
	{
		static _wstring wstrSelectedKey = L"";
		static int iSelectedIndex = -1;

		if (ImGui::BeginListBox("Textures List"))
		{
			int iIndex = 0;
			for (const auto& [key, pTexture] : m_ParticleTextures)
			{
				std::string strKey = WSTRINGTOSTRING(key);

				const bool is_selected = (iSelectedIndex == iIndex);

				if (ImGui::Selectable(strKey.c_str(), is_selected))
				{
					if (is_selected)
					{
						m_pParticleTexture = nullptr;
						iSelectedIndex = -1; // 선택 해제
						wstrSelectedKey = L"";
					}
					else
					{
						m_pParticleTexture = pTexture;
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

		ImGui::InputText("Texture Directory", m_szParticleTexturePath, MAX_PATH);

		if (ImGui::Button("Load_DDSTextures"))
		{
			Load_Textures(".dds", m_szParticleTexturePath, m_ParticleTextures);
		}
		ImGui::SameLine();
		if (ImGui::Button("Load_PNGTextures"))
		{
			Load_Textures(".png", m_szParticleTexturePath, m_ParticleTextures);
		}


		ImGui::TreePop();
	}

	ImGui::End();

	ImGui::Begin("Effect Textures");

	if (ImGui::TreeNode("Preview Effect Texture"))
	{
		if (m_pEffectTexture)
		{
			ImVec2 imageSize(300, 300); // 이미지 크기 설정
			ID3D11ShaderResourceView* pSelectImage = m_pEffectTexture->Get_SRV(0);
			if (nullptr != pSelectImage)
			{
				ImGui::Image((ImTextureID)pSelectImage, imageSize);
			}

			if (m_pNowItem)
			{
				if (ImGui::Button("Texture0 Set"))
				{
					m_pNowItem->Set_Texture(m_pEffectTexture, 0);
				}
				ImGui::SameLine();
				if (ImGui::Button("Texture1 Set"))
				{
					m_pNowItem->Set_Texture(m_pEffectTexture, 1);
				}
				ImGui::SameLine();
				if (ImGui::Button("Texture2 Set"))
				{
					m_pNowItem->Set_Texture(m_pEffectTexture, 2);
				}
			}

		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Effect Texture"))
	{
		static _wstring wstrSelectedKey = L"";
		static _int iSelectedIndex = -1;

		if (ImGui::BeginListBox("Textures List"))
		{
			_int iIndex = 0;
			for (const auto& [key, pTexture] : m_EffectTextures)
			{
				std::string strKey = WSTRINGTOSTRING(key);

				const bool is_selected = (iSelectedIndex == iIndex);

				if (ImGui::Selectable(strKey.c_str(), is_selected))
				{
					if (is_selected)
					{
						m_pEffectTexture = nullptr;
						iSelectedIndex = -1; // 선택 해제
						wstrSelectedKey = L"";
					}
					else
					{
						m_pEffectTexture = pTexture;
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

		ImGui::InputText("Texture Directory", m_szEffectTexturePath, MAX_PATH);

		if (ImGui::Button("Load_DDSTextures"))
		{
			Load_Textures(".dds", m_szEffectTexturePath, m_EffectTextures);
		}
		ImGui::SameLine();
		if (ImGui::Button("Load_PNGTextures"))
		{
			Load_Textures(".png", m_szEffectTexturePath, m_EffectTextures);
		}


		ImGui::TreePop();
	}


	ImGui::End();

}

void CLevel_EffectTool::Tool_ControlModel(_float _fTimeDelta)
{

	for (auto& pModel : m_ModelObjects)
	{
		if (pModel->Is_Active())
		{
			ImGui::Begin("Control Model");

			_float fTime = static_cast<C3DModel*>(pModel->Get_Model(COORDINATE_3D))->Get_AnimTime();
			ImGui::Text("Current Anim Time : %.4f", fTime);
			
			static _float fTimeScale = 1.f;
			if (ImGui::DragFloat("LifeTime", &fTimeScale, 0.01f))
			{
				m_pGameInstance->Set_TimeScale(fTimeScale, TEXT("Timer_Default"));
			}

			static _bool isLoop = { true };
			if (ImGui::RadioButton("Loop", isLoop))
			{
				isLoop = !isLoop;
			}

			static _int iAnim = { 0 };
			ImGui::InputInt("Anim Index", &iAnim);

			static _int iReset = { 0 };
			ImGui::InputInt("Event ID", &iReset);

			if (ImGui::Button("Reset All"))
			{
				pModel->Set_AnimationLoop(COORDINATE_3D, iAnim, isLoop);
				pModel->Set_Animation(iAnim);
				if (m_pNowItem)
				{
					m_pNowItem->Tool_Reset(iReset);
				}
			}

			static _char szInputBoneName[MAX_PATH] = "";
			
			ImGui::InputText("Bone Name", szInputBoneName, MAX_PATH);

			if (ImGui::Button("Connect to Bone"))
			{
				m_pNowItem->Set_SpawnMatrix(static_cast<C3DModel*>(pModel->Get_Model(COORDINATE_3D))->Get_BoneMatrix(szInputBoneName));
			}

			if (ImGui::Button("Detach Effect"))
			{
				m_pNowItem->Set_SpawnMatrix(nullptr);
			}


		
			ImGui::End();
			break;
		}
	}
	


}



HRESULT CLevel_EffectTool::Load_Textures(const _char* _szExtension, const _char* _szPath, map<const _wstring, class CTexture*>& _TextureMaps)
{
	std::filesystem::path path;
	path = _szPath;
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
			_TextureMaps.emplace(entry.path().c_str(), pTexture);
			

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
	CEffect_System* pOut;
	CEffect_System::EFFECT_SYSTEM_DESC Desc = {};

	Desc.eStartCoord = COORDINATE_3D;
	Desc.iCurLevelID = LEVEL_TOOL;
	Desc.isCoordChangeEnable = false;
	Desc.iSpriteShaderLevel = LEVEL_STATIC;
	Desc.szSpriteShaderTags = L"Prototype_Component_Shader_VtxPointInstance";
	Desc.iModelShaderLevel = LEVEL_STATIC;
	Desc.szModelShaderTags = L"Prototype_Component_Shader_VtxModelInstance";
	Desc.iEffectShaderLevel = LEVEL_STATIC;
	Desc.szEffectShaderTags = L"Prototype_Component_Shader_Effect";
	Desc.iSingleSpriteShaderLevel = LEVEL_STATIC;
	Desc.szSingleSpriteShaderTags = L"Prototype_Component_Shader_VtxPoint";
	Desc.iSingleSpriteBufferLevel = LEVEL_STATIC;
	Desc.szSingleSpriteBufferTags = L"Prototype_Component_VIBuffer_Point";
	Desc.szSpriteComputeShaderTag = L"Prototype_ComputeShader";
	Desc.szMeshComputeShaderTag = L"Prototype_ComputeShader_Mesh";

	
	std::filesystem::path path;
	path = _szPath;

	for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
		if (entry.path().extension() == ".json") {
			//cout << entry.path().string() << endl;

			CEffect_System* pParticleSystem = CEffect_System::Create(m_pDevice, m_pContext, entry.path().c_str());

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

	for (auto& Pair : m_ParticleTextures)
	{
		Safe_Release(Pair.second);
	}
	m_ParticleTextures.clear();

	for (auto& Pair : m_EffectTextures)
	{
		Safe_Release(Pair.second);
	}
	m_EffectTextures.clear();

	__super::Free();


}
