#include "stdafx.h"
#include "SketchSpace_SpikeBall.h"
#include "GameInstance.h"
#include "FSM.h"
#include "ModelObject.h"
#include "Section_Manager.h"
#include "Blocker.h"
#include "Effect2D_Manager.h"
#include "Player.h"

CSketchSpace_SpikeBall::CSketchSpace_SpikeBall(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CMonster(_pDevice, _pContext)
{
}

CSketchSpace_SpikeBall::CSketchSpace_SpikeBall(const CSketchSpace_SpikeBall& _Prototype)
    : CMonster(_Prototype)
{
}

HRESULT CSketchSpace_SpikeBall::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSketchSpace_SpikeBall::Initialize(void* _pArg)
{
    CSketchSpace_SpikeBall::SIDESCROLLDESC* pDesc = static_cast<CSketchSpace_SpikeBall::SIDESCROLLDESC*>(_pArg);
    pDesc->isCoordChangeEnable = false;

    pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(180.f);
    pDesc->tTransform2DDesc.fSpeedPerSec = 100.f;

    pDesc->_tStat.iHP = 1;
    pDesc->_tStat.iMaxHP = 1;
    pDesc->_tStat.iDamg = 1;

    m_eSideScroll_Bound = pDesc->eSideScroll_Bound;


    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    if (false == Is_Stay())
    {
        m_pFSM->Add_State((_uint)MONSTER_STATE::SIDESCROLL_PATROL);
        m_pFSM->Set_State((_uint)MONSTER_STATE::SIDESCROLL_PATROL);
    }
    else
    {
        m_pFSM->Add_State((_uint)MONSTER_STATE::IDLE);
        m_pFSM->Set_State((_uint)MONSTER_STATE::IDLE);
    }

    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);
    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, IDLE, true);
    
    pModelObject->Set_Animation(IDLE);

    return S_OK;
}

void CSketchSpace_SpikeBall::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta); /* Part Object Priority_Update */
}

void CSketchSpace_SpikeBall::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta); /* Part Object Update */
}

void CSketchSpace_SpikeBall::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta); /* Part Object Late_Update */
}

HRESULT CSketchSpace_SpikeBall::Render()
{
    /* Model이 없는 Container Object 같은 경우 Debug 용으로 사용하거나, 폰트 렌더용으로. */

#ifdef _DEBUG
    __super::Render();
#endif

    /* Font Render */

    return S_OK;
}

void CSketchSpace_SpikeBall::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    __super::On_Collision2D_Enter(_pMyCollider, _pOtherCollider, _pOtherObject);
}

void CSketchSpace_SpikeBall::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CSketchSpace_SpikeBall::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CSketchSpace_SpikeBall::On_Hit(CGameObject* _pHitter, _int _iDamg, _fvector _vForce)
{
}

void CSketchSpace_SpikeBall::Enter_Section(const _wstring _strIncludeSectionName)
{
    __super::Enter_Section(_strIncludeSectionName);

    //if (TEXT("Chapter2_P0102") == _strIncludeSectionName)
    //{
    //    Set_Position(XMVectorSet(-640.0f, 1360.f, 0.f, 0.f));
    //}
}

HRESULT CSketchSpace_SpikeBall::Ready_Components()
{
    /* Com_FSM */
    CFSM::FSMDESC FSMDesc;
    FSMDesc.fAlertRange = m_fAlertRange;
    FSMDesc.fChaseRange = m_fChaseRange;
    FSMDesc.fAttackRange = m_fAttackRange;
    FSMDesc.fAlert2DRange = m_fAlert2DRange;
    FSMDesc.fChase2DRange = m_fChase2DRange;
    FSMDesc.fAttack2DRange = m_fAttack2DRange;
    FSMDesc.pOwner = this;
    FSMDesc.iCurLevel = m_iCurLevelID;
    FSMDesc.isMelee = true;
    FSMDesc.eSideScroll_Bound = m_eSideScroll_Bound;

    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_FSM"),
        TEXT("Com_FSM"), reinterpret_cast<CComponent**>(&m_pFSM), &FSMDesc)))
        return E_FAIL;

    /* 2D Collider */
    m_p2DColliderComs.resize(1);

    CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
    CircleDesc.pOwner = this;
    CircleDesc.fRadius = { 40.f };
    CircleDesc.vScale = { 1.0f, 1.0f };
    CircleDesc.vOffsetPosition = { 0.f, 0.f};
    CircleDesc.isBlock = true;
    CircleDesc.iCollisionGroupID = OBJECT_GROUP::MONSTER;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &CircleDesc)))
        return E_FAIL;

    /* Com_Gravity */
    CGravity::GRAVITY_DESC GravityDesc = {};
    GravityDesc.fGravity = 9.8f * 150.f;
    GravityDesc.vGravityDirection = { 0.0f, -1.0f, 0.0f };
    GravityDesc.pOwner = this;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Gravity"),
        TEXT("Com_Gravity"), reinterpret_cast<CComponent**>(&m_pGravityCom), &GravityDesc)))
        return E_FAIL;

    m_pGravityCom->Set_Active(true);

    return S_OK;
}

HRESULT CSketchSpace_SpikeBall::Ready_PartObjects()
{
    /* Part Body */
    CModelObject::MODELOBJECT_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    BodyDesc.strModelPrototypeTag_2D = TEXT("sketchspace_spikeyball");
    BodyDesc.iModelPrototypeLevelID_2D = m_iCurLevelID;

    BodyDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);

    BodyDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    BodyDesc.tTransform2DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);

    BodyDesc.tTransform2DDesc.fRotationPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_RotationPerSec();
    BodyDesc.tTransform2DDesc.fSpeedPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_SpeedPerSec();

    m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_Monster_Body"), &BodyDesc));
    if (nullptr == m_PartObjects[PART_BODY])
        return E_FAIL;

    return S_OK;
}

CSketchSpace_SpikeBall* CSketchSpace_SpikeBall::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CSketchSpace_SpikeBall* pInstance = new CSketchSpace_SpikeBall(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CSketchSpace_SpikeBall");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSketchSpace_SpikeBall::Clone(void* _pArg)
{
    CSketchSpace_SpikeBall* pInstance = new CSketchSpace_SpikeBall(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CSketchSpace_SpikeBall");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSketchSpace_SpikeBall::Free()
{

    __super::Free();
}
