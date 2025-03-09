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
	pDesc->_iInitAnim = TRAP_CLOSED;
	pDesc->_iFlipAnim1 = TRAP_CLOSE;
	pDesc->_iFlipAnim1End = TRAP_CLOSED;
	pDesc->_iFlipAnim2 = TRAP_OPEN;
	pDesc->_iFlipAnim2End = TRAP_OPENDED;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;
	Register_OnAnimEndCallBack(bind(&CSneak_Trap_Tile::On_AnimEnd, this, placeholders::_1, placeholders::_2));

	m_eTileType = TRAP;


	return S_OK;
}

void CSneak_Trap_Tile::Restart()
{
	m_eCurState = CLOSE;
	m_iDetectorCount = 0;

	m_iCurAnim = TRAP_CLOSED;
	Switch_Animation(TRAP_CLOSED);
}

void CSneak_Trap_Tile::Active_Detection()
{
	if (OPEN == m_eCurState || TRAP_OPEN == m_iCurAnim || TRAP_OPENDED == m_iCurAnim)
		return;

	// TODO: RED Detection 魄窜
	if (m_eCurState != YELLOW && 0 < m_iDetectorCount)
	{
		m_eCurState = YELLOW;
		//m_iCurAnim = DEFAULT_CLOSED;
		Switch_Animation(TRAP_YELLOW);
	}
	else if (m_eCurState == YELLOW && 0 >= m_iDetectorCount)
	{
		m_eCurState = CLOSE;
		Switch_Animation(TRAP_CLOSED);
	}
}

void CSneak_Trap_Tile::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if (TRAP_CLOSE == iAnimIdx)
	{
		m_eCurState = CLOSE;
		// TODO : Sneak_Manager 贸府
	}
	else if (TRAP_OPEN == iAnimIdx)
	{
		m_eCurState = OPEN;
		// TODO : Sneak_Manager 贸府
	}

	__super::On_AnimEnd(_eCoord, iAnimIdx);
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
