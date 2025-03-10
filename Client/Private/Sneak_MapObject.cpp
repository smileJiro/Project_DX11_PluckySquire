#include "stdafx.h"
#include "Sneak_MapObject.h"
#include "GameInstance.h"

CSneak_MapObject::CSneak_MapObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CSneak_FlipObject(_pDevice, _pContext)
{
}

CSneak_MapObject::CSneak_MapObject(const CSneak_MapObject& _Prototype)
	: CSneak_FlipObject(_Prototype)
{
}

HRESULT CSneak_MapObject::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	return S_OK;
}

CSneak_MapObject* CSneak_MapObject::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CSneak_MapObject* pInstance = new CSneak_MapObject(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSneak_MapObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSneak_MapObject::Clone(void* _pArg)
{
	CSneak_MapObject* pInstance = new CSneak_MapObject(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CSneak_MapObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSneak_MapObject::Free()
{
	__super::Free();
}
