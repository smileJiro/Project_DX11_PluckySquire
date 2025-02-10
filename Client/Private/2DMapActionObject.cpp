#include "stdafx.h"
#include "2DMapActionObject.h"
#include "GameInstance.h"


C2DMapActionObject::C2DMapActionObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :C2DMapObject(_pDevice, _pContext)
{
}

C2DMapActionObject::C2DMapActionObject(const C2DMapActionObject& _Prototype)
    :C2DMapObject(_Prototype)
{
}

HRESULT C2DMapActionObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT C2DMapActionObject::Initialize(void* _pArg)
{
    if (nullptr == _pArg)
        return E_FAIL;

    MAPOBJ_DESC* pDesc = static_cast<MAPOBJ_DESC*>(_pArg);

    if (pDesc->is2DImport)
    {
        m_isSorting = pDesc->isSorting;
        m_isBackGround = pDesc->isBackGround;
        m_isCollider = pDesc->isCollider;
        m_isActive = pDesc->isActive;
    }


    m_eType = (MAPOBJ_2D_ACTION_TYPE)pDesc->eActiveType;

    if (FAILED(__super::Initialize(_pArg)))
        return  E_FAIL;

    _bool isBlock = false;

    switch (m_eType)
    {
    case Client::C2DMapActionObject::ACTIVE_TYPE_DIALOG:
    case Client::C2DMapActionObject::ACTIVE_TYPE_ATTACKABLE:
        isBlock = true;
        break;
    case Client::C2DMapActionObject::ACTIVE_TYPE_BREAKABLE:
    case Client::C2DMapActionObject::ACTIVE_TYPE_PATROL:
    case Client::C2DMapActionObject::ACTIVE_TYPE_MODEL_CLOSE:
        isBlock = false;
        break;
    default:
        break;
    }

    if (m_isCollider)
    {
        if (FAILED(Ready_Collider(pDesc, isBlock)))
            return E_FAIL;
    }


    //Ready_MapActionObjectSetting();

    return S_OK;
}

void C2DMapActionObject::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta);
}
void C2DMapActionObject::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta);
    if (true == m_isFadeOut)
    {
        m_fAlpha = std::fmax(m_fAlpha - (_fTimeDelta / m_fFadeOutSecond), 0.f);
    }
}

void C2DMapActionObject::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);
}

HRESULT C2DMapActionObject::Render()
{
    switch (m_eType)
    {
    case Client::C2DMapActionObject::ACTIVE_TYPE_BREAKABLE:
        break;
    case Client::C2DMapActionObject::ACTIVE_TYPE_PATROL:
        break;
    case Client::C2DMapActionObject::ACTIVE_TYPE_ATTACKABLE:
        break;
    case Client::C2DMapActionObject::ACTIVE_TYPE_DIALOG:
        break;
    case Client::C2DMapActionObject::ACTIVE_TYPE_MODEL_CLOSE:
        {
            _float4 fColor = { 1.f,1.f,1.f,m_fAlpha };
            if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_RawValue("g_vColors", &fColor, sizeof(_float4))))
                return E_FAIL;
            m_iShaderPasses[COORDINATE_2D] = (_uint)PASS_VTXPOSTEX::MAPOBJECT_MIXCOLOR;
        }
        break;
    case Client::C2DMapActionObject::ACTIVE_TYPE_LAST:
        break;
    default:
        break;
    }

    return __super::Render();
}

HRESULT C2DMapActionObject::Render_Shadow()
{
    return S_OK;
}

void C2DMapActionObject::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    switch (m_eType)
    {
    case Client::C2DMapActionObject::ACTIVE_TYPE_BREAKABLE:
        break;
    case Client::C2DMapActionObject::ACTIVE_TYPE_PATROL:
        break;
    case Client::C2DMapActionObject::ACTIVE_TYPE_ATTACKABLE:
        break;
    case Client::C2DMapActionObject::ACTIVE_TYPE_DIALOG:
        break;
    case Client::C2DMapActionObject::ACTIVE_TYPE_MODEL_CLOSE:
    {
        m_isFadeOut = true;
        m_fAlpha = 1.f;
    }
    break;
    case Client::C2DMapActionObject::ACTIVE_TYPE_LAST:
        break;
    default:
        break;
    }
}

void C2DMapActionObject::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void C2DMapActionObject::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    switch (m_eType)
    {
    case Client::C2DMapActionObject::ACTIVE_TYPE_BREAKABLE:
        break;
    case Client::C2DMapActionObject::ACTIVE_TYPE_PATROL:
        break;
    case Client::C2DMapActionObject::ACTIVE_TYPE_ATTACKABLE:
        break;
    case Client::C2DMapActionObject::ACTIVE_TYPE_DIALOG:
        break;
    case Client::C2DMapActionObject::ACTIVE_TYPE_MODEL_CLOSE:
    {
        m_isFadeOut = false;
        m_fAlpha = 1.f;
    }
    break;
    case Client::C2DMapActionObject::ACTIVE_TYPE_LAST:
        break;
    default:
        break;
    }
}


C2DMapActionObject* C2DMapActionObject::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    C2DMapActionObject* pInstance = new C2DMapActionObject(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : C2DMapActionObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* C2DMapActionObject::Clone(void* _pArg)
{
    C2DMapActionObject* pInstance = new C2DMapActionObject(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : C2DMapActionObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void C2DMapActionObject::Free()
{
    __super::Free();
}
