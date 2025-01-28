#include "Particle_System.h"
#include "GameInstance.h"

#include "Particle_Sprite_Emitter.h"
#include "Particle_Mesh_Emitter.h"
CParticle_System::CParticle_System(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CPartObject(_pDevice, _pContext)
{
}

CParticle_System::CParticle_System(const CParticle_System& _Prototype)
	: CPartObject(_Prototype)
	, m_strFilePath(_Prototype.m_strFilePath)
{
	m_strName = _Prototype.m_strName;
}

// 파티클 시스템이 Prototype 시점에 모든 Emitter을 다 불러옵니다.
HRESULT CParticle_System::Initialize_Prototype(const _tchar* _szFilePath)
{
	json jsonEffectInfo;
	if (FAILED(m_pGameInstance->Load_Json(_szFilePath, &jsonEffectInfo)))
		return E_FAIL;

	if (false == jsonEffectInfo.contains("Name"))
	{
		return E_FAIL;
	}
	_string strName = jsonEffectInfo["Name"];
	m_strName = STRINGTOWSTRING(strName);

	// 갖고있는 Emitter 정보를 불러들입니다.
	for (_int i = 0; i < jsonEffectInfo["Emitters"].size(); ++i)
	{
		if (false == jsonEffectInfo["Emitters"][i].contains("Type"))
			return E_FAIL;
		
		CParticle_Emitter::PARTICLE_TYPE eType = jsonEffectInfo["Emitters"][i]["Type"];

		if (CParticle_Emitter::SPRITE == eType)
		{
			CParticle_Sprite_Emitter* pSpriteEmitter = CParticle_Sprite_Emitter::Create(m_pDevice, m_pContext, jsonEffectInfo["Emitters"][i]);
			if (nullptr == pSpriteEmitter)
				return E_FAIL;
			m_ParticleEmitters.push_back(pSpriteEmitter);
		}

		else if (CParticle_Emitter::MESH == eType)
		{
			//const jsonEffectInfo["Emitters"][]

			CParticle_Mesh_Emitter* pMeshEmitter = CParticle_Mesh_Emitter::Create(m_pDevice, m_pContext, jsonEffectInfo["Emitters"][i]);
			if (nullptr == pMeshEmitter)
				return E_FAIL;
			m_ParticleEmitters.push_back(pMeshEmitter);
		}

	}

#ifdef _DEBUG
	m_strFilePath = WSTRINGTOSTRING(_szFilePath);
#endif


	return S_OK;
}

HRESULT CParticle_System::Initialize_Prototype()
{
	return S_OK;
}

// Prototype의 Emitter을 다 Clone 해옵니다.
// Shader에 대한 Desc을 채워줘야 합니다.
HRESULT CParticle_System::Initialize(void* _pArg, const CParticle_System* _pPrototype)
{
	if (nullptr == _pArg)
		return E_FAIL;

	PARTICLE_SYSTEM_DESC* pDesc = static_cast<PARTICLE_SYSTEM_DESC*>(_pArg);

	// TODO: Particle Coordinate는 2D도 해야할까 ? 
	pDesc->eStartCoord = COORDINATE_3D;
	pDesc->isCoordChangeEnable = false;

	
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	//if (_pPrototype->m_ParticleEmitters.size() != pDesc->EmitterShaderLevels.size())
	//	return E_FAIL;
	//if (_pPrototype->m_ParticleEmitters.size() != pDesc->ShaderTags.size())
	//	return E_FAIL;

	for (_uint i = 0; i < _pPrototype->m_ParticleEmitters.size(); ++i)
	{
		CParticle_Emitter::PARTICLE_EMITTER_DESC Desc = {};

		if (CParticle_Emitter::MESH == _pPrototype->m_ParticleEmitters[i]->Get_Type())
		{
			Desc.iProtoShaderLevel = pDesc->iModelShaderLevel;
			Desc.szShaderTag = pDesc->szModelShaderTags;
		}

		else if (CParticle_Emitter::SPRITE == _pPrototype->m_ParticleEmitters[i]->Get_Type())
		{
			Desc.iProtoShaderLevel = pDesc->iSpriteShaderLevel;
			Desc.szShaderTag = pDesc->szSpriteShaderTags;
		}

		Desc.iCurLevelID = pDesc->iCurLevelID;
		Desc.eStartCoord = COORDINATE_3D;
		Desc.isCoordChangeEnable = false;
		Desc.pParentMatrices[COORDINATE_3D] = &m_WorldMatrices[COORDINATE_3D];

		CParticle_Emitter* pEmitter = static_cast<CParticle_Emitter*>(_pPrototype->m_ParticleEmitters[i]->Clone(&Desc));
		if (nullptr == pEmitter)
			return E_FAIL;
		m_ParticleEmitters.push_back(pEmitter);
	}



	return S_OK;
}

// TOOL 전용의 Particle Initialize입니다.
// 새로운 파티클을 툴에서 만들 때 사용합니다.
HRESULT CParticle_System::Initialize()
{
	return S_OK;
}


// TODO : 여러 로직들 구현하기
void CParticle_System::Priority_Update(_float _fTimeDelta)
{
	for (auto& pEmitter : m_ParticleEmitters)
		pEmitter->Priority_Update(_fTimeDelta);


}

void CParticle_System::Update(_float _fTimeDelta)
{
	for (auto& pEmitter : m_ParticleEmitters)
		pEmitter->Update(_fTimeDelta);
}

void CParticle_System::Late_Update(_float _fTimeDelta)
{
	// 부모 오브젝트의 World에 따라 먼저 Update된 이후에 갱신됩니다.
	__super::Late_Update(_fTimeDelta);

	for (auto& pEmitter : m_ParticleEmitters)
		pEmitter->Late_Update(_fTimeDelta);

}

HRESULT CParticle_System::Render()
{
	return S_OK;
}

void CParticle_System::Play_Effect(_uint _iEventID)
{
	for (auto& pEmitter : m_ParticleEmitters)
	{
		if (_iEventID == pEmitter->Get_EventID())
			pEmitter->Set_Active(true);
	}
			
}

CParticle_System* CParticle_System::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _szFilePath)
{
	CParticle_System* pInstance = new CParticle_System(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_szFilePath)))
	{
		MSG_BOX("Failed to Cloned : CParticle_System");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CParticle_System::Clone(void* _pArg)
{
	CParticle_System* pInstance = new CParticle_System(*this);

#ifdef _DEBUG
	// 툴 전용
	if (0 == m_ParticleEmitters.size())
	{
		if (FAILED(pInstance->Initialize()))
		{
			MSG_BOX("Failed to Cloned : CParticle_System");
			Safe_Release(pInstance);
		}

		return pInstance;
	}

#endif

	// 이미 Emitter 존재하는 상태 -> Emitter 복사
	if (FAILED(pInstance->Initialize(_pArg, this)))
	{
		MSG_BOX("Failed to Cloned : CParticle_System");
		Safe_Release(pInstance);
	}


	return pInstance;
}

void CParticle_System::Free()
{
	for (auto& pEmitter : m_ParticleEmitters)
	{
		Safe_Release(pEmitter);
	}
	m_ParticleEmitters.clear();

	__super::Free();
}

HRESULT CParticle_System::Cleanup_DeadReferences()
{
	return S_OK;
}


#ifdef _DEBUG
HRESULT CParticle_System::Add_New_Emitter(CParticle_Emitter::PARTICLE_TYPE _eType, void* _pArg)
{
	if (CParticle_Emitter::SPRITE == _eType)
	{
		CParticle_Sprite_Emitter* pOut = CParticle_Sprite_Emitter::Create(m_pDevice, m_pContext, _pArg);
		if (nullptr == pOut)
			return E_FAIL;

		m_ParticleEmitters.push_back(pOut);
	}

	return S_OK;
}

void CParticle_System::Tool_ShowList()
{
	if (ImGui::TreeNode("Emitter List"))
	{
		if (ImGui::BeginListBox("List"))
		{
			_int n = 0;
			static _int iNowIndex = 0;
			for (auto& Item : m_ParticleEmitters)
			{
				_char _szNumber[10] = "";
				_itoa_s(n, _szNumber, 10);
				const _bool is_selected = (iNowIndex == n);
				if (ImGui::Selectable(_szNumber, is_selected))
				{
					iNowIndex = n;
					m_pNowItem = m_ParticleEmitters[n];
					m_pNowItem->Tool_Setting();
				}
				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
				++n;
			}
			ImGui::EndListBox();


		}

		ImGui::TreePop();
	}


}

void CParticle_System::Tool_InputText()
{
	static _char szInputName[MAX_PATH] = "";

	strcpy_s(szInputName, WSTRINGTOSTRING(m_strName).c_str());
	if (ImGui::InputText("Name", szInputName, MAX_PATH))
	{
		if (0 != strcmp(szInputName, ""))
			m_strName = STRINGTOWSTRING(szInputName);
	}

	static _char szInputPath[MAX_PATH] = "";
	
	strcpy_s(szInputPath, m_strFilePath.c_str());
	if (ImGui::InputText("File Path", szInputPath, MAX_PATH))
	{
		m_strFilePath = szInputPath;
	}

	if (ImGui::InputFloat("Debug Loop Time", &m_fToolRepeatTime))
	{
		if (0.f > m_fToolRepeatTime)
			m_fToolRepeatTime = 1.f;
	}



}

void CParticle_System::Tool_Update(_float _fTimeDelta)
{
	m_fToolAccTime += _fTimeDelta;
	if (m_fToolRepeatTime <= m_fToolAccTime)
	{
		// TODO: Tool Reset 설정
		m_fToolAccTime = 0.f;
	}

	
	Tool_InputText();
	Tool_ShowList();

	
	if (m_pNowItem)
	{
		m_pNowItem->Tool_Update(_fTimeDelta);
	}


}
void CParticle_System::Set_Texture(CTexture* _pTextureCom)
{
	if (m_pNowItem)
	{
		if (CParticle_Emitter::SPRITE == m_pNowItem->Get_Type())
		{
			static_cast<CParticle_Sprite_Emitter*>(m_pNowItem)->Set_Texture(_pTextureCom);
		}
	}
}

HRESULT CParticle_System::Save_File()
{
	std::ofstream OutputFile(m_strFilePath);

	if (OutputFile.is_open())
	{
		json jsonRoot;

		jsonRoot["Name"] = WSTRINGTOSTRING(m_strName).c_str();

		
		for (auto& pEmitter : m_ParticleEmitters)
		{
			json jsonLeaf;
			if (SUCCEEDED(pEmitter->Save(jsonLeaf)))
				jsonRoot["Emitters"].push_back(jsonLeaf);
		}
		
		
		OutputFile << jsonRoot.dump(4);
		OutputFile.close();
	}
	else
	{
		MessageBoxA(NULL, m_strFilePath.c_str(), "파일 경로 없음", MB_OK);
	}

	return S_OK;
}

CParticle_System* CParticle_System::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CParticle_System* pInstance = new CParticle_System(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Cloned : CParticle_System");
		Safe_Release(pInstance);
	}

	return pInstance;
}
#endif