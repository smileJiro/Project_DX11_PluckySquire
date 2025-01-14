#include "stdafx.h"
#include "Test_Body_Player.h"

#include "GameInstance.h"

CTest_Body_Player::CTest_Body_Player(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CModelObject(_pDevice, _pContext)
{
}

CTest_Body_Player::CTest_Body_Player(const CTest_Body_Player& _Prototype)
    :CModelObject(_Prototype)
{
}

HRESULT CTest_Body_Player::Initialize_Prototype()
{
    return E_NOTIMPL;
}

HRESULT CTest_Body_Player::Initialize(void* _pArg)
{
    return E_NOTIMPL;
}

void CTest_Body_Player::Priority_Update(_float _fTimeDelta)
{
}

void CTest_Body_Player::Update(_float _fTimeDelta)
{
}

void CTest_Body_Player::Late_Update(_float _fTimeDelta)
{
}

HRESULT CTest_Body_Player::Render()
{
    return E_NOTIMPL;
}

HRESULT CTest_Body_Player::Render_Shadow()
{
    return E_NOTIMPL;
}

HRESULT CTest_Body_Player::Ready_Components()
{
    return E_NOTIMPL;
}

CTest_Body_Player* CTest_Body_Player::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    return nullptr;
}

CGameObject* CTest_Body_Player::Clone(void* _pArg)
{
    return nullptr;
}

void CTest_Body_Player::Free()
{
}
