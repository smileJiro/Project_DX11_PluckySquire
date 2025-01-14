#include "stdafx.h"
#include "Test_Player.h"

CTest_Player::CTest_Player(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CContainerObject(_pDevice, _pContext)
{
}

CTest_Player::CTest_Player(const CTest_Player& _Prototype)
    :CContainerObject(_Prototype)
{
}

HRESULT CTest_Player::Initialize_Prototype()
{
    return E_NOTIMPL;
}

HRESULT CTest_Player::Initialize(void* _pArg)
{
    return E_NOTIMPL;
}

void CTest_Player::Priority_Update(_float _fTimeDelta)
{
}

void CTest_Player::Update(_float _fTimeDelta)
{
}

void CTest_Player::Late_Update(_float _fTimeDelta)
{
}

HRESULT CTest_Player::Render()
{
    return E_NOTIMPL;
}

void CTest_Player::Key_Input(_float _fTimeDelta)
{
}

HRESULT CTest_Player::Ready_Components()
{
    return E_NOTIMPL;
}

HRESULT CTest_Player::Ready_PartObjects()
{
    return E_NOTIMPL;
}

CTest_Player* CTest_Player::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    return nullptr;
}

CGameObject* CTest_Player::Clone(void* _pArg)
{
    return nullptr;
}

void CTest_Player::Free()
{
}
