#include "Emitter.h"
#include "GameInstance.h"

_int			CEmitter::s_iRG_3D = 0;
_int			CEmitter::s_iRG_2D = 0;
_int			CEmitter::s_iRGP_EFFECT = 0;

CEmitter::CEmitter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CPartObject(_pDevice, _pContext)
{
	XMStoreFloat4x4(&m_IdentityMatrix, XMMatrixIdentity());
}

CEmitter::CEmitter(const CEmitter& _Prototype)
	: CPartObject(_Prototype)
	, m_eEffectType(_Prototype.m_eEffectType)
	, m_iShaderPass(_Prototype.m_iShaderPass)
	, m_isFollowParent(_Prototype.m_isFollowParent)
	//, m_isProgress(_Prototype.m_isProgress)
	, m_iEventID(_Prototype.m_iEventID)
	, m_fEventTime(_Prototype.m_fEventTime)
	, m_fAccTime(_Prototype.m_fAccTime)
	, m_iLoopTime(_Prototype.m_iLoopTime)
	, m_IdentityMatrix(_Prototype.m_IdentityMatrix)
{
	m_isActive = _Prototype.m_isActive;
}

HRESULT CEmitter::Initialize_Prototype(const json& _jsonInfo)
{
	if (_jsonInfo.contains("Parent"))
		m_isFollowParent = _jsonInfo["Parent"];
	if (_jsonInfo.contains("Event"))
		m_iEventID = _jsonInfo["Event"];
	if (_jsonInfo.contains("Progress"))
		m_isActive = _jsonInfo["Progress"];
	if (_jsonInfo.contains("Time"))
		m_fEventTime = _jsonInfo["Time"];
	if (_jsonInfo.contains("LoopTime"))
		m_iLoopTime = _jsonInfo["LoopTime"];

	return S_OK;
}

HRESULT CEmitter::Initialize(void* _pArg)
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

void CEmitter::Update(_float _fTimeDelta)
{
	if (m_isActive)
	{
		m_fAccTime += _fTimeDelta;

		if (0 != m_iLoopTime && m_fEventTime <= m_fAccTime)
		{
			++m_iAccLoop;

			if (m_iAccLoop >= m_iLoopTime)
				Set_Active(false);
			else if (0 != m_iLoopTime)
			{
				Reset();
			}

			m_fAccTime = 0.f;

		}

		Update_Component(_fTimeDelta);
	}
}

void CEmitter::Late_Update(_float _fTimeDelta)
{
	// TODO: 할거 많다라!!
	if (m_isActive)
		__super::Late_Update(_fTimeDelta);
		

}

void CEmitter::Reset()
{

}

void CEmitter::Active_OnEnable()
{
	m_fAccTime = 0.f;

	Reset();
}

void CEmitter::Active_OnDisable()
{
}

HRESULT CEmitter::Ready_Components(const PARTICLE_EMITTER_DESC* _pDesc)
{
	if (FAILED(Add_Component(_pDesc->iProtoShaderLevel, _pDesc->szShaderTag,
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

void CEmitter::Free()
{
	Safe_Release(m_pShaderCom);

	__super::Free();
}

#ifdef _DEBUG
void CEmitter::Tool_Update(_float fTimeDelta)
{

	if (ImGui::RadioButton("Follow Parent", m_isFollowParent))
	{
		m_isFollowParent = !m_isFollowParent;
	}

	if (ImGui::RadioButton("Initially Start", m_isToolProgress))
	{
		m_isToolProgress = !m_isToolProgress;
	}

	if (ImGui::InputInt("Event", (_int*)&m_iEventID))
		m_isToolChanged = true;
	if (ImGui::InputFloat("Time", &m_fEventTime))
		m_isToolChanged = true;
	if (ImGui::InputInt("Loop Count", (_int*)&m_iLoopTime))
		m_isToolChanged = true;
	


}
void CEmitter::Tool_Setting()
{

}

HRESULT CEmitter::Save(json& _jsonOut)
{
	_jsonOut["Parent"] = m_isFollowParent;
	_jsonOut["Event"] = m_iEventID;
	_jsonOut["Progress"] = m_isToolProgress;
	_jsonOut["Time"] = m_fEventTime;
	_jsonOut["LoopTime"] = m_iLoopTime;
	_jsonOut["Type"] = m_eEffectType;


	return S_OK;
}
#endif

