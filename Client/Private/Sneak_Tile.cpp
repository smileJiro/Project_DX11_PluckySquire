#include "stdafx.h"
#include "Sneak_Tile.h"
#include "GameInstance.h"

CSneak_Tile::CSneak_Tile(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CSneak_FlipObject(_pDevice, _pContext)
{
}

CSneak_Tile::CSneak_Tile(const CSneak_Tile& _Prototype)
	: CSneak_FlipObject(_Prototype)
{
}

HRESULT CSneak_Tile::Initialize(void* _pArg)
{
	if (nullptr == _pArg)
		return E_FAIL;

	SNEAK_TILEDESC* pDesc = static_cast<SNEAK_TILEDESC*>(_pArg);

	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->isCoordChangeEnable = false;
	pDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	pDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
	
	pDesc->tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	
	pDesc->eActorType = ACTOR_TYPE::LAST;
	pDesc->pActorDesc = nullptr;
	pDesc->iModelPrototypeLevelID_2D = pDesc->iCurLevelID;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	m_iTileIndex = pDesc->iTileIndex;
	for (_int i = 0; i < (_int)F_DIRECTION::F_DIR_LAST; ++i)
	{
		m_AdjacentTiles[i] = pDesc->iAdjacents[i];
	}
	m_vTilePosition = _float2(pDesc->tTransform2DDesc.vInitialPosition.x, pDesc->tTransform2DDesc.vInitialPosition.y);


	return S_OK;
}


void CSneak_Tile::Interact()
{
	if (CLOSE == m_eCurState)
	{
		m_eCurState = OPEN;
	}
	else if (OPEN == m_eCurState)
	{
		m_eCurState = CLOSE;
	}

	Flip();
}

void CSneak_Tile::Restart()
{
	if (OPEN == m_eCurState)
	{
		m_eCurState = CLOSE;
		Flip();
	}
}


void CSneak_Tile::Free()
{
	__super::Free();
}
