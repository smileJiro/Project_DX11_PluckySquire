#include "stdafx.h"
#include "Sneak_InteractObject.h"
#include "GameInstance.h"
#include "Sneak_Tile.h"

CSneak_InteractObject::CSneak_InteractObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CSneak_FlipObject(_pDevice, _pContext)
{
}

CSneak_InteractObject::CSneak_InteractObject(const CSneak_InteractObject& _Prototype)
	: CSneak_FlipObject(_Prototype)
{
}

HRESULT CSneak_InteractObject::Initialize(void* _pArg)
{
	if (nullptr == _pArg)
		return E_FAIL;

	SNEAK_INTERACTOBJECT_DESC* pDesc = static_cast<SNEAK_INTERACTOBJECT_DESC*>(_pArg);

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	m_iTileIndex = pDesc->_iTileIndex;
	m_isBlocked = pDesc->_isBlocked;
	m_isBlockChangable = pDesc->_isBlockChangable;
	m_isInteractable = pDesc->_isInteractable;
	m_isCollisionInteractable = pDesc->_isCollisionInteractable;
	m_eBlockDirection = pDesc->_eBlockDirection;

	return S_OK;
}

void CSneak_InteractObject::Register_Tiles(CSneak_Tile* _pTile)
{
	if (nullptr != _pTile)
	{
		Safe_AddRef(_pTile);
		m_pTiles.push_back(_pTile);
	}
}

void CSneak_InteractObject::Register_Objects(CSneak_InteractObject* _pObject)
{
	if (nullptr != _pObject)
	{
		Safe_AddRef(_pObject);
		m_pInteractObjects.push_back(_pObject);
	}
}

void CSneak_InteractObject::Interact()
{
	m_isFlipped = !m_isFlipped;
	Flip();

	for (auto& iter : m_pInteractObjects)
		iter->Interact();
	for (auto& iter : m_pTiles)
		iter->Interact();
}

void CSneak_InteractObject::Restart()
{
	if (m_isFlipped)
	{
		m_isFlipped = false;
		Flip();
	}
}


CSneak_InteractObject* CSneak_InteractObject::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CSneak_InteractObject* pInstance = new CSneak_InteractObject(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSneak_InteractObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSneak_InteractObject::Clone(void* _pArg)
{
	CSneak_InteractObject* pInstance = new CSneak_InteractObject(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CSneak_InteractObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSneak_InteractObject::Free()
{
	for (auto& iter : m_pInteractObjects)
		Safe_Release(iter);
	m_pInteractObjects.clear();

	for (auto& iter : m_pTiles)
		Safe_Release(iter);
	m_pTiles.clear();


	__super::Free();
}