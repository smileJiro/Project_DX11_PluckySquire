#include "stdafx.h"
#include "Sneak_Drawer.h"
#include "GameInstance.h"

CSneak_Drawer::CSneak_Drawer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CSneak_DefaultObject(_pDevice, _pContext)
{
}

CSneak_Drawer::CSneak_Drawer(const CSneak_Drawer& _Prototype)
    : CSneak_DefaultObject(_Prototype)
{
}

HRESULT CSneak_Drawer::Initialize(void* _pArg)
{
	DRAWER_DESC* pDesc = static_cast<DRAWER_DESC*>(_pArg);
    
	pDesc->strModelPrototypeTag_2D = TEXT("Sneak_Drawer");

	if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

	m_isBright = pDesc->isBright;

	if (m_isBright)
		Switch_Animation(B_CLOSED);
	else
		Switch_Animation(D_CLOSED);

    return S_OK;
}

void CSneak_Drawer::Update(_float _fTimeDelta)
{
#ifdef _DEBUG
	if (KEY_PRESSING(KEY::CTRL) && KEY_DOWN(KEY::NUM7))
	{
		Move();
	}
#endif
	__super::Update(_fTimeDelta);
}

void CSneak_Drawer::Move()
{
	m_isClosed = !m_isClosed;

	if (m_isBright)
	{
		if (m_isClosed)
		{
			Switch_Animation(B_OPEN);
		}
		else
			Switch_Animation(B_CLOSE);
	}
	else
	{
		if (m_isClosed)
		{
			Switch_Animation(D_OPEN);
		}
		else
			Switch_Animation(D_CLOSE);
	}


}

CSneak_Drawer* CSneak_Drawer::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CSneak_Drawer* pInstance = new CSneak_Drawer(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSneak_Drawer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSneak_Drawer::Clone(void* _pArg)
{
	CSneak_Drawer* pInstance = new CSneak_Drawer(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CSneak_Drawer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSneak_Drawer::Free()
{
	__super::Free();
}
