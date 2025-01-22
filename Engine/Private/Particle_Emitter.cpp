#include "Particle_Emitter.h"
#include "GameInstance.h"

CParticle_Emitter::CParticle_Emitter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CParticle_Emitter::CParticle_Emitter(const CParticle_Emitter& _Prototype)
	: CGameObject(_Prototype), m_eType(_Prototype.m_eType)
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


