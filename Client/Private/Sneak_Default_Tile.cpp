#include "stdafx.h"
#include "Sneak_Default_Tile.h"
#include "GameInstance.h"

CSneak_Default_Tile::CSneak_Default_Tile(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CSneak_Tile(_pDevice, _pContext)
{
}

CSneak_Default_Tile::CSneak_Default_Tile(const CSneak_Default_Tile& _Prototype)
    : CSneak_Tile(_Prototype)
{
}

HRESULT CSneak_Default_Tile::Initialize(void* _pArg)
{
	SNEAK_TILEDESC* pDesc = static_cast<SNEAK_TILEDESC*>(_pArg);
	
	pDesc->strModelPrototypeTag_2D = TEXT("Sneak_AnimTile");

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


    return S_OK;
}

HRESULT CSneak_Default_Tile::Ready_Components()
{
	if (FAILED(Add_Component(m_iCurLevelID, TEXT("Sneak_tile_lit_yellow_trap"), TEXT("Com_YellowTile"), reinterpret_cast<CComponent**>(&m_pYellowTile), nullptr)))
		return E_FAIL;

	if (FAILED(Add_Component(m_iCurLevelID, TEXT("Sneak_tile_red"), TEXT("Com_RedTile"), reinterpret_cast<CComponent**>(&m_pRedTile), nullptr)))
		return E_FAIL;

	return S_OK;
}

CSneak_Default_Tile* CSneak_Default_Tile::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CSneak_Default_Tile* pInstance = new CSneak_Default_Tile(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSneak_Default_Tile");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSneak_Default_Tile::Clone(void* _pArg)
{
	CSneak_Default_Tile* pInstance = new CSneak_Default_Tile(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CSneak_Default_Tile");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSneak_Default_Tile::Free()
{
	__super::Free();
}

