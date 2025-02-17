#include "stdafx.h"
#include "Word_Container.h"

HRESULT CWord_Container::Initialize_Prototype()
{
	return __super::Initialize_Prototype();
}

HRESULT CWord_Container::Initialize(void* _pArg)
{
	return __super::Initialize(_pArg);
}

void CWord_Container::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CWord_Container::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CWord_Container::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);

}

HRESULT CWord_Container::Render()
{
	__super::Render();
	return S_OK;

}

CWord_Container* CWord_Container::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	return nullptr;
}

CGameObject* CWord_Container::Clone(void* _pArg)
{
	return nullptr;
}

void CWord_Container::Free()
{
}
