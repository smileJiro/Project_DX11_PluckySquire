#include "Component.h"
#include "GameInstance.h"
CComponent::CComponent(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice)
	, m_pContext(_pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
	, m_isCloned(false)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

CComponent::CComponent(const CComponent& Prototype)
	: m_pDevice(Prototype.m_pDevice)
	, m_pContext(Prototype.m_pContext)
	, m_pGameInstance(Prototype.m_pGameInstance)
	, m_isCloned(true)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CComponent::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CComponent::Initialize(void* _pArg)
{
    return S_OK;
}

void CComponent::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	__super::Free();
}
