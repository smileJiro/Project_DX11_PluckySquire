#include "stdafx.h"
#include "2DMapActionObject.h"
#include "GameInstance.h"
#include "Effect2D_Manager.h"
#include "Section_Manager.h"
#include "Pooling_Manager.h"
#include "Character.h"


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

#pragma region 콜라이더 개별설정


    if (m_isCollider)
    {
        _bool isBlock = false;

        switch (m_eType)
        {
        case Client::C2DMapActionObject::ACTIVE_TYPE_DIALOG:
        case Client::C2DMapActionObject::ACTIVE_TYPE_ATTACKABLE:
            isBlock = true;
            break;
        default:
            isBlock = false;
            break;
        }

        if (FAILED(Ready_Collider(pDesc, isBlock)))
            return E_FAIL;
    }
#pragma endregion
#pragma region 기초 초기화


    switch (m_eType)
    {
    case Client::C2DMapActionObject::ACTIVE_TYPE_ACTIONANIM:
        Set_PlayingAnim(false);
        Set_AnimationLoop(COORDINATE_2D,0,false);
        break;
    default:
        break;
    }

#pragma endregion



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
        if (true == m_isFadeOut)
        {
            m_fAlpha = std::fmax(m_fAlpha - (_fTimeDelta / m_fFadeOutSecond), 0.f);
        }
        break;
    case Client::C2DMapActionObject::ACTIVE_TYPE_DROPBLOCK:
        break;
    case Client::C2DMapActionObject::ACTIVE_TYPE_DYNAMIC_BACKGROUND:
        if (true == m_isAction)
        {
            m_fAlpha = std::fmax(m_fAlpha - (_fTimeDelta / m_fFadeOutSecond), 0.01f);
        }
        break;
    case Client::C2DMapActionObject::ACTIVE_TYPE_DAMEGED:
        break;
    case Client::C2DMapActionObject::ACTIVE_TYPE_LAST:
        break;
    default:
        break;
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
            _float4 fColor = { 0.f,0.f,0.f,m_fAlpha };
            _uint iFlag = 0;
            if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_RawValue("g_vColors", &fColor, sizeof(_float4))))
                return E_FAIL;
            if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_RawValue("g_iFlag", &iFlag, sizeof(_uint))))
                return E_FAIL;
            m_iShaderPasses[COORDINATE_2D] = (_uint)PASS_VTXPOSTEX::MAPOBJECT_MIXCOLOR;
        }
        break;
    case Client::C2DMapActionObject::ACTIVE_TYPE_DYNAMIC_BACKGROUND:
        if(m_isAction)
        {
            _float4 fColor = { m_fAlpha, m_fAlpha, m_fAlpha,1.f };
            _uint iFlag = 1;
            if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_RawValue("g_vColors", &fColor, sizeof(_float4))))
                return E_FAIL;
            if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_RawValue("g_iFlag", &iFlag, sizeof(_uint))))
                return E_FAIL;
            m_iShaderPasses[COORDINATE_2D] = (_uint)PASS_VTXPOSTEX::MAPOBJECT_MIXCOLOR;
        }
        break;
    case Client::C2DMapActionObject::ACTIVE_TYPE_LAST:
        break;
    default:
        break;
    }

#ifdef _DEBUG
    if (!m_p2DColliderComs.empty())
        m_p2DColliderComs[0]->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
#endif // _DEBUG

    return __super::Render();
}

HRESULT C2DMapActionObject::Render_Shadow()
{
    return S_OK;
}

void C2DMapActionObject::Ready_Action()
{
    switch (m_eType)
    {
    case Client::C2DMapActionObject::ACTIVE_TYPE_DYNAMIC_BACKGROUND:
        m_isAction = true;
        m_fAlpha = 1.f;
        m_fFadeOutSecond = 2.f;
        break;
    case Client::C2DMapActionObject::ACTIVE_TYPE_ACTIONANIM:
        Set_PlayingAnim(true);
        break;
    default:
        break;
    }
}

void C2DMapActionObject::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    switch (m_eType)
    {
    case Client::C2DMapActionObject::ACTIVE_TYPE_BREAKABLE:
    {
        if (true == ContainWstring(m_strModelPrototypeTag[COORDINATE_2D], L"bush"))
        {
            if (PLAYER_PROJECTILE & _pOtherCollider->Get_CollisionGroupID())
            {
                //이펙트 애니메이션 재생
                //_matrix matFX = Get_ControllerTransform()->Get_WorldMatrix();

                //_wstring strFXTag = L"bushburst_leaves";
                //strFXTag += to_wstring((_int)ceil(m_pGameInstance->Compute_Random(0.f, 2.f)));
                //CEffect2D_Manager::GetInstance()->Play_Effect(strFXTag, CSection_Manager::GetInstance()->Get_Cur_Section_Key(), matFX);

                //strFXTag = L"bushburst_dust";
                //strFXTag += to_wstring((_int)ceil(m_pGameInstance->Compute_Random(0.f, 2.f)));
                //CEffect2D_Manager::GetInstance()->Play_Effect(strFXTag, CSection_Manager::GetInstance()->Get_Cur_Section_Key(), matFX);


                //확률로 전구 생성
                if (2 == (_int)ceil(m_pGameInstance->Compute_Random(0.f, 3.f)))
                {
                    _float3 vPos; XMStoreFloat3(&vPos, Get_FinalPosition());
                    _wstring strCurSection = CSection_Manager::GetInstance()->Get_Cur_Section_Key();
                    CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_2DBulb"), COORDINATE_2D, &vPos, nullptr, nullptr, &strCurSection);
                }

                //삭제
                Event_DeleteObject(this);
            }
        }
    }
        break;
    case Client::C2DMapActionObject::ACTIVE_TYPE_PATROL:
        break;
    case Client::C2DMapActionObject::ACTIVE_TYPE_ATTACKABLE:
        break;
    case Client::C2DMapActionObject::ACTIVE_TYPE_DIALOG:
        break;
    case Client::C2DMapActionObject::ACTIVE_TYPE_DAMEGED:
    {
        Event_Hit(this, static_cast<CCharacter*>(_pOtherObject), 1, XMVectorZero());
    }
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
