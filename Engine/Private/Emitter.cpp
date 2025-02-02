#include "Emitter.h"
#include "GameInstance.h"

_int			CEmitter::s_iRG_3D = 0;
_int			CEmitter::s_iRG_2D = 0;
_int			CEmitter::s_iRGP_PARTICLE = 0;
_int			CEmitter::s_iRGP_EFFECT = 0;

CEmitter::CEmitter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CPartObject(_pDevice, _pContext)
{
}

CEmitter::CEmitter(const CEmitter& _Prototype)
	: CPartObject(_Prototype)
	, m_eEffectType(_Prototype.m_eEffectType)
	, m_iShaderPass(_Prototype.m_iShaderPass)
	, m_isWorld(_Prototype.m_isWorld)
	//, m_isProgress(_Prototype.m_isProgress)
	, m_iEventID(_Prototype.m_iEventID)
	, m_fEventTime(_Prototype.m_fEventTime)
	, m_fDelayTime(_Prototype.m_fDelayTime)
	, m_fAccTime(_Prototype.m_fAccTime)
	, m_iLoopTime(_Prototype.m_iLoopTime)
	, m_LoadMatrix(_Prototype.m_LoadMatrix)
{
	m_isActive = _Prototype.m_isActive;
}

HRESULT CEmitter::Initialize_Prototype(const json& _jsonInfo)
{
	if (_jsonInfo.contains("World"))
		m_isWorld = _jsonInfo["World"];
	if (_jsonInfo.contains("Event"))
		m_iEventID = _jsonInfo["Event"];
	if (_jsonInfo.contains("Progress"))
		m_isActive = _jsonInfo["Progress"];
	if (_jsonInfo.contains("Time"))
		m_fEventTime = _jsonInfo["Time"];
	if (_jsonInfo.contains("DelayTime"))
		m_fDelayTime = _jsonInfo["DelayTime"];
	if (_jsonInfo.contains("LoopTime"))
		m_iLoopTime = _jsonInfo["LoopTime"];
	if (_jsonInfo.contains("Transform"))
	{
		for (_int i = 0; i < 16; ++i)
		{
			*((_float*)(&m_LoadMatrix) + i) = _jsonInfo["Transform"][i];			
		}
	}
	else
	{
		XMStoreFloat4x4(&m_LoadMatrix, XMMatrixIdentity());
	}
	

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

	m_pControllerTransform->Set_WorldMatrix(XMLoadFloat4x4(&m_LoadMatrix));

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CEmitter::Update(_float _fTimeDelta)
{
	if (m_isActive)
	{
		m_fAccTime += _fTimeDelta;

		// Delay 시간 적용
		if (0 >= m_iAccLoop && m_fDelayTime <= m_fAccTime)
		{
			++m_iAccLoop;
			m_fAccTime = 0.f;
			
			// TODO: OnEvent
			On_Event();
		}

		if (0 < m_iAccLoop)
		{
			if (m_fEventTime <= m_fAccTime)
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
}

void CEmitter::Late_Update(_float _fTimeDelta)
{
	// TODO: 할거 많다라!!
	if (m_isActive)
		__super::Late_Update(_fTimeDelta);
		

}


void CEmitter::Active_OnEnable()
{
	m_fAccTime = 0.f;
	m_iAccLoop = 0;

	Reset();
}

void CEmitter::Active_OnDisable()
{
	m_fAccTime = 0.f;
	m_iAccLoop = 0;

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

	if (ImGui::RadioButton("Follow World", m_isWorld))
	{
		m_isWorld = !m_isWorld;
		m_isToolChanged = true;
	}

	if (ImGui::RadioButton("Initially Start", m_isToolProgress))
	{
		m_isToolProgress = !m_isToolProgress;
		m_isToolChanged = true;

	}

	if (ImGui::InputInt("Event", (_int*)&m_iEventID))
		m_isToolChanged = true;
	if (ImGui::InputFloat("Time", &m_fEventTime))
		m_isToolChanged = true;
	
	if (ImGui::Button("Infinite Time"))
	{
		m_fEventTime = D3D11_FLOAT32_MAX;
		m_isToolChanged = true;
	}

	if (ImGui::InputFloat("Delay Time", &m_fDelayTime))
		m_isToolChanged = true;
	if (ImGui::InputInt("Loop Count", (_int*)&m_iLoopTime))
		m_isToolChanged = true;

	if (ImGui::Button("Infinite Loop"))
	{
		m_iLoopTime = UINT32_MAX;
		m_isToolChanged = true;
	}

	static _float3 vScale = { 1.f, 1.f, 1.f };
	static _float3 vRotaiton = { 0.f, 0.f, 0.f };
	static _float4 vQuaternion = { 0.f, 0.f, 0.f, 1.f };
	static _float3 vPosition = { 0.f, 0.f, 0.f };
	if (ImGui::Button("Get_Transform_Info"))
	{
		_matrix matWorld = m_pControllerTransform->Get_WorldMatrix();

		for (_int i = 0; i < 3; ++i)
		{
			*((_float*)(&vScale) + i) = XMVectorGetX(XMVector3Length(matWorld.r[i]));
			matWorld.r[i] = XMVector3Normalize(matWorld.r[i]);
		}

		XMStoreFloat4(&vQuaternion, XMQuaternionRotationMatrix(matWorld));

		XMStoreFloat3(&vPosition, matWorld.r[3]);
	}

	ImGui::Text("Rotation : %.3f, %.3f, %.3f, %.3f", vQuaternion.x, vQuaternion.y, vQuaternion.z, vQuaternion.w);
	
	if (ImGui::DragFloat3("Scaling", (_float*)(&vScale), 0.01f))
	{
		if (vScale.x != 0.f && vScale.y != 0.f && vScale.z != 0.f)
			m_pControllerTransform->Set_Scale(vScale);
	}

	if (ImGui::DragFloat3("Rotation", (_float*)(&vRotaiton), 0.01f))
	{
		m_pControllerTransform->RotationXYZ(vRotaiton);
	}
	
	if (ImGui::DragFloat3("Position", (_float*)(&vPosition), 0.01f))
	{
		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&vPosition), 1.f));
	}


}
void CEmitter::Tool_Setting()
{

}

HRESULT CEmitter::Save(json& _jsonOut)
{
	_jsonOut["World"] = m_isWorld;
	_jsonOut["Event"] = m_iEventID;
	_jsonOut["Progress"] = m_isToolProgress;
	_jsonOut["Time"] = m_fEventTime;
	_jsonOut["LoopTime"] = m_iLoopTime;
	_jsonOut["Type"] = m_eEffectType;
	_jsonOut["DelayTime"] = m_fDelayTime;

	_float4x4 matWorld;
	XMStoreFloat4x4(&matWorld, m_pControllerTransform->Get_WorldMatrix());
	for (_int i = 0; i < 16; ++i)
		_jsonOut["Transform"].push_back(*((_float*)(&matWorld) + i));


	return S_OK;
}
#endif

