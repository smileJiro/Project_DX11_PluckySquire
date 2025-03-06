#include "stdafx.h"
#include "Sneak_Trap_Tile.h"
#include "GameInstance.h"
#include "Section_Manager.h"
CSneak_Trap_Tile::CSneak_Trap_Tile(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CSneak_Tile(_pDevice, _pContext)
{
}

CSneak_Trap_Tile::CSneak_Trap_Tile(const CSneak_Trap_Tile& _Prototype)
    : CSneak_Tile(_Prototype)
{
}

HRESULT CSneak_Trap_Tile::Initialize(void* _pArg)
{
	SNEAK_TILEDESC* pDesc = static_cast<SNEAK_TILEDESC*>(_pArg);

	pDesc->strModelPrototypeTag_2D = TEXT("Sneak_Tile");

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	m_eTileType = TRAP;

	Switch_Animation(TRAP_CLOSED);

	return S_OK;
}



CSneak_Trap_Tile* CSneak_Trap_Tile::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CSneak_Trap_Tile* pInstance = new CSneak_Trap_Tile(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSneak_Trap_Tile");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSneak_Trap_Tile::Clone(void* _pArg)
{
	CSneak_Trap_Tile* pInstance = new CSneak_Trap_Tile(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CSneak_Trap_Tile");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSneak_Trap_Tile::Free()
{
	__super::Free();
}
