#include "stdafx.h"
#include "2DTrigger_Sample.h"
#include "GameInstance.h"
#include "Controller_Model.h"
#include "Material.h"

#include "Bone.h"
#include "Engine_Struct.h"
#include <gizmo/ImGuizmo.h>

C2DTrigger_Sample::C2DTrigger_Sample(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CGameObject(_pDevice, _pContext)
{
}

C2DTrigger_Sample::C2DTrigger_Sample(const C2DTrigger_Sample& _Prototype)
    :CGameObject(_Prototype)
{
}

HRESULT C2DTrigger_Sample::Initialize_Prototype()
{

    return S_OK;
}

HRESULT C2DTrigger_Sample::Initialize(void* _pArg)
{
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    CCollider_AABB::COLLIDER_AABB_DESC AABBDesc = {};
    AABBDesc.pOwner = this;
    AABBDesc.vExtents = { 1.f, 1.f };
    AABBDesc.vScale = { 1.f, 1.f };
    AABBDesc.vOffsetPosition = { 0.f, 0.f };
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
        return E_FAIL;
    

    XMStoreFloat4x4(&m_matWorld,Get_WorldMatrix());


    return S_OK;
}

void C2DTrigger_Sample::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta);
}
void C2DTrigger_Sample::Update(_float _fTimeDelta)
{
    /* Update Parent Matrix */
    __super::Update(_fTimeDelta);
}

void C2DTrigger_Sample::Late_Update(_float _fTimeDelta)
{
    if (COORDINATE_3D == m_pControllerTransform->Get_CurCoord())
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
    else if (COORDINATE_2D == m_pControllerTransform->Get_CurCoord())
        m_pGameInstance->Add_RenderObject(CRenderer::RG_BOOK_2D, this);

    /* Update Parent Matrix */
    __super::Late_Update(_fTimeDelta);
}

HRESULT C2DTrigger_Sample::Render()
{

#ifdef _DEBUG
    m_pColliderCom->Render();
#endif // _DEBUG


    CGameObject::Render();
    return S_OK;
}




C2DTrigger_Sample* C2DTrigger_Sample::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    C2DTrigger_Sample* pInstance = new C2DTrigger_Sample(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : C2DTrigger_Sample");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* C2DTrigger_Sample::Clone(void* _pArg)
{
    C2DTrigger_Sample* pInstance = new C2DTrigger_Sample(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : C2DTrigger_Sample");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void C2DTrigger_Sample::Free()
{
    Safe_Release(m_pColliderCom);

    __super::Free();
}

HRESULT C2DTrigger_Sample::Cleanup_DeadReferences()
{
    return E_NOTIMPL;
}
