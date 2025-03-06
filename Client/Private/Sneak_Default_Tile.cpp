#include "stdafx.h"
#include "Sneak_Default_Tile.h"
#include "GameInstance.h"
#include "Section_Manager.h"

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
	
	pDesc->strModelPrototypeTag_2D = TEXT("Sneak_Tile");

	//pDesc->tTransform2DDesc.vInitialPosition = { 0.f, 0.f, 0.f };
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	m_eTileType = DEFAULT;
	
	//TEMP
	Switch_Animation(DEFAULT_CLOSED);

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

