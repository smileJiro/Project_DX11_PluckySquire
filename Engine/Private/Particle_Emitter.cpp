#include "Particle_Emitter.h"
#include "GameInstance.h"

CParticle_Emitter::CParticle_Emitter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CPartObject(_pDevice, _pContext)
{
}

CParticle_Emitter::CParticle_Emitter(const CParticle_Emitter& _Prototype)
	: CPartObject(_Prototype)
	, m_eParticleType(_Prototype.m_eParticleType)
	, m_iShaderPass(_Prototype.m_iShaderPass)
	, m_isFollowParent(_Prototype.m_isFollowParent)
	//, m_isProgress(_Prototype.m_isProgress)
	, m_iEventID(_Prototype.m_iEventID)
	, m_fEventTime(_Prototype.m_fEventTime)
	, m_fAccTime(_Prototype.m_fAccTime)
{
	m_isActive = _Prototype.m_isActive;
}

HRESULT CParticle_Emitter::Initialize_Prototype(const json& _jsonInfo)
{
	if (_jsonInfo.contains("Parent"))
		m_isFollowParent = _jsonInfo["Parent"];
	if (_jsonInfo.contains("Event"))
		m_iEventID = _jsonInfo["Event"];
	if (_jsonInfo.contains("Progress"))
		m_isActive = _jsonInfo["Progress"];
	if (_jsonInfo.contains("Time"))
		m_fEventTime = _jsonInfo["Time"];


	return S_OK;
}

HRESULT CParticle_Emitter::Initialize(void* _pArg)
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

void CParticle_Emitter::Update(_float _fTimeDelta)
{
	if (m_isActive)
	{
		m_fAccTime += _fTimeDelta;

		if (m_fEventTime <= m_fAccTime)
			Set_Active(false);

		Update_Component(_fTimeDelta);
	}
}

void CParticle_Emitter::Late_Update(_float _fTimeDelta)
{
	// TODO: 할거 많다라!!
	if (m_isActive)
		__super::Late_Update(_fTimeDelta);
		

}

void CParticle_Emitter::Active_OnEnable()
{
	m_fAccTime = 0.f;

}

void CParticle_Emitter::Active_OnDisable()
{
}

HRESULT CParticle_Emitter::Ready_Components(const PARTICLE_EMITTER_DESC* _pDesc)
{
	if (FAILED(Add_Component(_pDesc->iProtoShaderLevel, _pDesc->szShaderTag,
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

void CParticle_Emitter::Free()
{
	Safe_Release(m_pShaderCom);

	__super::Free();
}

#ifdef _DEBUG
void CParticle_Emitter::Tool_Update(_float fTimeDelta)
{

	if (ImGui::RadioButton("Follow Parent", m_isFollowParent))
	{
		m_isFollowParent = !m_isFollowParent;
	}

	if (ImGui::RadioButton("Initially Start", m_isToolProgress))
	{
		m_isToolProgress = !m_isToolProgress;
	}

	ImGui::InputInt("Event", (_int*)&m_iEventID);
	ImGui::InputFloat("Time", &m_fEventTime);
}
void CParticle_Emitter::Tool_Setting()
{

}

HRESULT CParticle_Emitter::Save(json& _jsonOut)
{
	_jsonOut["Parent"] = m_isFollowParent;
	_jsonOut["Event"] = m_iEventID;
	_jsonOut["Progress"] = m_isToolProgress;
	_jsonOut["Time"] = m_fEventTime;

	return S_OK;
}
#endif

