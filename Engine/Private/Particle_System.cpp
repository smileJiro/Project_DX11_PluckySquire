#include "Particle_System.h"
#include "GameInstance.h"

CParticle_System::CParticle_System(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CParticle_System::CParticle_System(const CParticle_System& _Prototype)
	: CGameObject(_Prototype)
{
}

CParticle_System* CParticle_System::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _szFilePath)
{
	return nullptr;
}

CGameObject* CParticle_System::Clone(void* _pArg)
{
	return nullptr;
}

void CParticle_System::Free()
{
	__super::Free();
}

HRESULT CParticle_System::Cleanup_DeadReferences()
{
	return E_NOTIMPL;
}
