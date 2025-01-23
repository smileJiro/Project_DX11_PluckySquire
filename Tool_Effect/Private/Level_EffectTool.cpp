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

	return S_OK;
}

void CLevel_EffectTool::Update(_float _fTimeDelta)
{
	if (m_isParticle)
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

	CParticle_System* pOut;

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
	m_ParticleSystems.push_back(pOut);

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

void CLevel_EffectTool::Update_Particle_Tool(_float _fTimeDelta)
{
	ImGui::Begin("Particle System");
	Show_System_List();
	ImGui::End();

	ImGui::Begin("Emitter");
	Adjust_System(_fTimeDelta);
	ImGui::End();
}

void CLevel_EffectTool::Show_System_List()
{
	if (ImGui::TreeNode("Particle System List"))
	{
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
				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
				++n;
			}
			ImGui::EndListBox();
		}

		if (ImGui::Button("New System"))
		{
			CParticle_System* pOut;

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_TOOL, TEXT("Prototype_Effect_ForNew"),
				LEVEL_TOOL, TEXT("Layer_Effect"), reinterpret_cast<CGameObject**>(&pOut))))
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

		ImGui::TreePop();
	}

}

void CLevel_EffectTool::Adjust_System(_float _fTimeDelta)
{
	if (m_pNowItem && ImGui::TreeNode("Adjust System"))
	{
		if (ImGui::Button("New_Sprite_Emitter"))
		{
			CParticle_Emitter::PARTICLE_EMITTER_DESC Desc = {};

			Desc.eStartCoord = COORDINATE_3D;
			Desc.iCurLevelID = LEVEL_TOOL;
			Desc.isCoordChangeEnable = false;
			Desc.iProtoShaderLevel = LEVEL_STATIC;
			Desc.szShaderTag = L"Prototype_Component_Shader_VtxPointInstance";
			if (FAILED(m_pNowItem->Add_NewEmitter(CParticle_Emitter::SPRITE, &Desc)))
			{
				MSG_BOX("Emitter 만들기 실패");
			}
		}


		m_pNowItem->Tool_Update(_fTimeDelta);

		ImGui::TreePop();
	}


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

	__super::Free();


}
