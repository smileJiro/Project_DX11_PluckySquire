#include "stdafx.h"
#include "FriendBody.h"
#include "GameInstance.h"

CFriendBody::CFriendBody(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CModelObject(_pDevice, _pContext)
{
}

CFriendBody::CFriendBody(const CFriendBody& _Prototype)
    :CModelObject(_Prototype)
{
}

HRESULT CFriendBody::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFriendBody::Initialize(void* _pArg)
{

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    return S_OK;
}

void CFriendBody::Priority_Update(_float _fTimeDelta)
{

    __super::Priority_Update(_fTimeDelta);
}

void CFriendBody::Update(_float _fTimeDelta)
{

    __super::Update(_fTimeDelta);
}

void CFriendBody::Late_Update(_float _fTimeDelta)
{

    __super::Late_Update(_fTimeDelta);
}

CFriendBody* CFriendBody::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CFriendBody* pInstance = new CFriendBody(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CFriendBody");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CFriendBody::Clone(void* _pArg)
{
    CFriendBody* pInstance = new CFriendBody(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CFriendBody");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFriendBody::Free()
{

    __super::Free();
}
