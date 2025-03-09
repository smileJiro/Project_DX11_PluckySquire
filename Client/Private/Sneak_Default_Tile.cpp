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
	pDesc->_iInitAnim = DEFAULT_CLOSED;
	pDesc->_iFlipAnim1 = DEFAULT_CLOSE;
	pDesc->_iFlipAnim1End = DEFAULT_CLOSED;
	pDesc->_iFlipAnim2 = DEFAULT_OPEN;
	pDesc->_iFlipAnim2End = DEFAULT_OPENED;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	Register_OnAnimEndCallBack(bind(&CSneak_Default_Tile::On_AnimEnd, this, placeholders::_1, placeholders::_2));


	m_eTileType = DEFAULT;
	
    return S_OK;
}

void CSneak_Default_Tile::Restart()
{
	m_eCurState = CLOSE;
	m_iDetectorCount = 0;

	m_iCurAnim = DEFAULT_CLOSED;
	Switch_Animation(DEFAULT_CLOSED);
}

void CSneak_Default_Tile::Active_Detection()
{
	if (OPEN == m_eCurState)
		return;

	// TODO: RED Detection 魄窜
	if (m_eCurState != YELLOW && 0 < m_iDetectorCount)
	{
		m_eCurState = YELLOW;
		//m_iCurAnim = DEFAULT_CLOSED;
		Switch_Animation(DEFAULT_YELLOW);
	}
	else if (m_eCurState == YELLOW && 0 >= m_iDetectorCount)
	{
		m_eCurState = CLOSE;
		Switch_Animation(DEFAULT_CLOSED);
	}
}

void CSneak_Default_Tile::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if (DEFAULT_CLOSE == iAnimIdx)
	{
		m_eCurState = CLOSE;
		// TODO : Sneak_Manager 贸府
	}
	else if (DEFAULT_OPEN == iAnimIdx)
	{
		m_eCurState = OPEN;		
		// TODO : Sneak_Manager 贸府
	}

	__super::On_AnimEnd(_eCoord, iAnimIdx);
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

