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

    case Client::C2DMapActionObject::ACTIVE_TYPE_SKSP_HUMGRUMP:
        Set_PlayingAnim(false);
        Set_AnimationLoop(COORDINATE_2D, 0, false);
        Set_AnimationLoop(COORDINATE_2D, 1, false);
        Set_AnimationLoop(COORDINATE_2D, 2, false);
        Set_AnimationLoop(COORDINATE_2D, 3, false);
        Switch_Animation(3);
        break;    
    case Client::C2DMapActionObject::ACTIVE_TYPE_SPIKE:
    {

        _int eState = ((_int)ceil(m_pGameInstance->Compute_Random(0.f, 4.f))) - 1;

        Set_PlayingAnim(true);
        Set_AnimationLoop(COORDINATE_2D, 0, false); // SPKIE - DOWN
        Set_AnimationLoop(COORDINATE_2D, 1, false); // SPKIE - DownAnim
        Set_AnimationLoop(COORDINATE_2D, 2, false); // SPKIE - UPANIM
        Set_AnimationLoop(COORDINATE_2D, 3, false); // SPKIE - UP
        Switch_Animation(eState);
        m_eCurPattern = eState;
    }
        break;
    default:
        break;
    }

#pragma endregion

    Register_OnAnimEndCallBack(bind(&C2DMapActionObject::MapActionObject_AnimEnd, this, placeholders::_1, placeholders::_2));


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
    case Client::C2DMapActionObject::ACTIVE_TYPE_MODEL_CLOSE:

        if (true == m_isFadeOut)
            m_fAlpha = std::fmax(m_fAlpha - (_fTimeDelta / m_fFadeOutSecond), 0.f);

        break;
    case Client::C2DMapActionObject::ACTIVE_TYPE_MODEL_CLOSE_DELETE:

        if (true == m_isFadeOut)
        {
            m_fAlpha = std::fmax(m_fAlpha - (_fTimeDelta / m_fFadeOutSecond), 0.f);
            if (0.f >= m_fAlpha && false == Is_Dead())
                Event_DeleteObject(this);
        }
        break;

    case Client::C2DMapActionObject::ACTIVE_TYPE_DYNAMIC_BACKGROUND:

        if (true == m_isAction)
            m_fAlpha = std::fmax(m_fAlpha - (_fTimeDelta / m_fFadeOutSecond), m_fDynamicBackGround_MinBright);

        break;
    case Client::C2DMapActionObject::ACTIVE_TYPE_SPIKE:
        Spike_Pattern_UpdateProcess(_fTimeDelta);
        
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
    case Client::C2DMapActionObject::ACTIVE_TYPE_MODEL_CLOSE_DELETE:
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
    default:
        break;
    }

#ifdef _DEBUG
    if (!m_p2DColliderComs.empty())
        m_p2DColliderComs[0]->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
#endif // _DEBUG

    return __super::Render();
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

void C2DMapActionObject::MapActionObject_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
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
    case Client::C2DMapActionObject::ACTIVE_TYPE_SKSP_HUMGRUMP:
        Set_PlayingAnim(false);
        break;
    case Client::C2DMapActionObject::ACTIVE_TYPE_SPIKE:
        Spike_Pattern_EndProcess();
        break;
    }
}

void C2DMapActionObject::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    switch (m_eType)
    {
    case Client::C2DMapActionObject::ACTIVE_TYPE_BREAKABLE:
    {
        if (PLAYER_PROJECTILE & _pOtherCollider->Get_CollisionGroupID())
        {
            _wstring strFXTag = L"";
            _wstring strCurSection = m_strSectionName;
            _matrix matFX = Get_ControllerTransform()->Get_WorldMatrix();

            // 풀숲 죽는이펙트
            if (true == ContainWstring(m_strModelPrototypeTag[COORDINATE_2D], L"bush"))
            {
                strFXTag = L"bushburst_leaves";
                strFXTag += to_wstring((_int)ceil(m_pGameInstance->Compute_Random(0.f, 2.f)));
                CEffect2D_Manager::GetInstance()->Play_Effect(strFXTag, m_strSectionName, matFX);

                strFXTag = L"bushburst_dust";
                strFXTag += to_wstring((_int)ceil(m_pGameInstance->Compute_Random(0.f, 2.f)));
                CEffect2D_Manager::GetInstance()->Play_Effect(strFXTag, m_strSectionName, matFX);

                m_pGameInstance->Start_SFX(_wstring(L"A_sfx_hit_bushes-") + to_wstring(rand() % 8), 30.f);


            }
            // 나무통 죽는이펙트
            else if (true == ContainWstring(m_strModelPrototypeTag[COORDINATE_2D], L"barrel"))
            {
                matFX = Get_ControllerTransform()->Get_WorldMatrix();
                strFXTag = L"Barrel_Break";
                CEffect2D_Manager::GetInstance()->Play_Effect(strFXTag, m_strSectionName, matFX);
                m_pGameInstance->Start_SFX(_wstring(L"A_sfx_sword_hit_barrel_") + to_wstring(rand() % 4), 30.f);

            }

            //확률로 전구 생성
            if (2 == (_int)ceil(m_pGameInstance->Compute_Random(0.f, 3.f)))
            {
                _float3 vPos; XMStoreFloat3(&vPos, Get_FinalPosition());
                CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_2DBulb"), COORDINATE_2D, &vPos, nullptr, nullptr, &strCurSection);
            }
            Event_DeleteObject(this);
        }

    }
        break;
    case Client::C2DMapActionObject::ACTIVE_TYPE_DAMEGED:
        Event_Hit(this, static_cast<CCharacter*>(_pOtherObject), 1, XMVectorZero());
        break;
    case Client::C2DMapActionObject::ACTIVE_TYPE_MODEL_CLOSE:
    case Client::C2DMapActionObject::ACTIVE_TYPE_MODEL_CLOSE_DELETE:
    {
        m_isFadeOut = true;
        m_fAlpha = 1.f;
    }
    break;
    case Client::C2DMapActionObject::ACTIVE_TYPE_SKSP_HUMGRUMP:
        Switch_Animation(1);
        Set_PlayingAnim(true);
        break;
    case Client::C2DMapActionObject::ACTIVE_TYPE_SPIKE:
    {
        if (PLAYER & _pOtherCollider->Get_CollisionGroupID())
            if (SPKIE_IDLE_DOWN != m_eCurPattern)
                Event_Hit(this, static_cast<CCharacter*>(_pOtherObject), 1, XMVectorZero());
    }
    default:
        break;
    }
}

void C2DMapActionObject::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    switch (m_eType)
    {
    case Client::C2DMapActionObject::ACTIVE_TYPE_SPIKE:
    {
        if (PLAYER & _pOtherCollider->Get_CollisionGroupID())
            if (SPKIE_IDLE_DOWN != m_eCurPattern)
                Event_Hit(this, static_cast<CCharacter*>(_pOtherObject), 1, XMVectorZero());
    }
    default:
        break;
    }
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
    case Client::C2DMapActionObject::ACTIVE_TYPE_SKSP_HUMGRUMP:
        Switch_Animation(2);
        Set_PlayingAnim(true);
        break;
    default:
        break;
    }
}


void C2DMapActionObject::Spike_Pattern_UpdateProcess(_float _fTimeDelta)
{
    m_fAccTime += _fTimeDelta;
    switch (m_eCurPattern)
    {
    case Client::C2DMapActionObject::SPKIE_IDLE_DOWN:
        if (2.f < m_fAccTime)
        {
            m_fAccTime = 0.f;
            m_eCurPattern = SPKIE_UP;
            Switch_Animation(SPKIE_UP);
        }
        break;
    case Client::C2DMapActionObject::SPKIE_DOWN:
        break;
    case Client::C2DMapActionObject::SPKIE_UP:
        break;
    case Client::C2DMapActionObject::SPKIE_IDLE_UP:
        if (0.5f < m_fAccTime)
        {
            m_fAccTime = 0.f;
            m_eCurPattern = SPKIE_DOWN;
            Switch_Animation(SPKIE_DOWN);
        }
        break;
    default:
        break;
    }

}

void C2DMapActionObject::Spike_Pattern_EndProcess()
{
    switch (m_eCurPattern)
    {
    case Client::C2DMapActionObject::SPKIE_DOWN:
        m_fAccTime = 0.f;
        m_eCurPattern = SPKIE_IDLE_DOWN;
        Switch_Animation(SPKIE_IDLE_DOWN);
        break;
    case Client::C2DMapActionObject::SPKIE_UP:
        m_fAccTime = 0.f;
        m_eCurPattern = SPKIE_IDLE_UP;
        Switch_Animation(SPKIE_IDLE_UP);
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
