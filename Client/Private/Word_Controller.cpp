#include "stdafx.h"
#include "Word_Controller.h"

CWord_Controller::CWord_Controller(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CContainerObject(_pDevice,_pContext)
{
}

CWord_Controller::CWord_Controller(const CWord_Controller& _Prototype)
	: CContainerObject(_Prototype)
{
}

HRESULT CWord_Controller::Initialize_Prototype()
{
	return __super::Initialize_Prototype();
}

HRESULT CWord_Controller::Initialize(void* _pArg)
{
	return __super::Initialize(_pArg);
}

void CWord_Controller::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CWord_Controller::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CWord_Controller::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);

}

HRESULT CWord_Controller::Render()
{
	__super::Render();
	return S_OK;
}

CWord_Controller* CWord_Controller::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	return nullptr;
}

CGameObject* CWord_Controller::Clone(void* _pArg)
{
	return nullptr;
}

void CWord_Controller::Free()
{
}
