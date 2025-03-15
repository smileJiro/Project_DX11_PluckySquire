#include "stdafx.h"
#include "WorldMapNPC.h"
#include "GameInstance.h"

#include "PlayerData_Manager.h"



CWorldMapNPC::CWorldMapNPC(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CContainerObject(_pDevice, _pContext)
{
}

CWorldMapNPC::CWorldMapNPC(const CWorldMapNPC& _Prototype)
	: CContainerObject( _Prototype )
{
}

HRESULT CWorldMapNPC::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWorldMapNPC::Initialize(void* _pArg)
{
	tagContainerObjectDesc* pDesc = static_cast<tagContainerObjectDesc*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	//if (FAILED(Ready_Components()))
	//	return E_FAIL;

	m_isRender = false;

	return S_OK;
}

void CWorldMapNPC::Priority_Update(_float _fTimeDelta)
{
}

void CWorldMapNPC::Update(_float _fTimeDelta)
{


}

void CWorldMapNPC::Late_Update(_float _fTimeDelta)
{
	if (m_isRender)
		__super::Late_Update(_fTimeDelta);
}

HRESULT CWorldMapNPC::Render()
{
	

	return S_OK;
}



//HRESULT CWorldMapNPC::Ready_Components()
//{
//	return S_OK;
//}


CWorldMapNPC* CWorldMapNPC::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CWorldMapNPC* pInstance = new CWorldMapNPC(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CWorldMapNPC Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CWorldMapNPC::Clone(void* _pArg)
{
	CWorldMapNPC* pInstance = new CWorldMapNPC(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CWorldMapNPC Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CWorldMapNPC::Free()
{
	
	__super::Free();
	
}

HRESULT CWorldMapNPC::Cleanup_DeadReferences()
{

	return S_OK;
}
