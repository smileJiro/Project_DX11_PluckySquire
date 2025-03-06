#include "stdafx.h"
#include "Sneak_Tile.h"
#include "GameInstance.h"

CSneak_Tile::CSneak_Tile(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CModelObject(_pDevice, _pContext)
{
}

CSneak_Tile::CSneak_Tile(const CSneak_Tile& _Prototype)
	: CModelObject(_Prototype)
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
	pDesc->iModelPrototypeLevelID_2D = m_iCurLevelID;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;


	m_iTileIndex = m_iTileIndex;

	return S_OK;
}


void CSneak_Tile::Free()
{
	__super::Free();
}
