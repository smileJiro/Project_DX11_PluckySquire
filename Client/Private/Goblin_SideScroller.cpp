#include "stdafx.h"
#include "Goblin_SideScroller.h"
#include "GameInstance.h"
#include "FSM.h"
#include "ModelObject.h"
#include "Section_Manager.h"
#include "Blocker.h"

CGoblin_SideScroller::CGoblin_SideScroller(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CMonster(_pDevice, _pContext)
{
}

CGoblin_SideScroller::CGoblin_SideScroller(const CGoblin_SideScroller& _Prototype)
    : CMonster(_Prototype)
{
}

HRESULT CGoblin_SideScroller::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGoblin_SideScroller::Initialize(void* _pArg)
{
    CGoblin_SideScroller::MONSTER_DESC* pDesc = static_cast<CGoblin_SideScroller::MONSTER_DESC*>(_pArg);
    pDesc->eStartCoord = COORDINATE_2D;
    pDesc->isCoordChangeEnable = false;
    pDesc->iNumPartObjects = PART_END;

    pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(180.f);
    pDesc->tTransform2DDesc.fSpeedPerSec = 100.f;

    //pDesc->fAlert2DRange = 300.f;
    //pDesc->fChase2DRange = 600.f;
    //pDesc->fAttack2DRange = 300.f;
    //pDesc->fDelayTime = 2.f;

    m_tStat.iHP = 2;
    m_tStat.iMaxHP = 2;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    m_pFSM->Add_State((_uint)MONSTER_STATE::SIDESCROLL_PATROL);
    m_pFSM->Add_State((_uint)MONSTER_STATE::SIDESCROLL_HIT);
    m_pFSM->Add_State((_uint)MONSTER_STATE::DEAD);
    m_pFSM->Set_State((_uint)MONSTER_STATE::SIDESCROLL_PATROL);

    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);
    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, PATROL, true);

    pModelObject->Set_Animation(PATROL);

    pModelObject->Register_OnAnimEndCallBack(bind(&CGoblin_SideScroller::Animation_End, this, placeholders::_1, placeholders::_2));

    return S_OK;
}

void CGoblin_SideScroller::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta); /* Part Object Priority_Update */
}

void CGoblin_SideScroller::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta); /* Part Object Update */
}

void CGoblin_SideScroller::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta); /* Part Object Late_Update */
}

HRESULT CGoblin_SideScroller::Render()
{
    /* Model이 없는 Container Object 같은 경우 Debug 용으로 사용하거나, 폰트 렌더용으로. */

#ifdef _DEBUG
    if (COORDINATE_2D == Get_CurCoord())
    {
        for (_uint i = 0; i < m_p2DColliderComs.size(); ++i)
        {
            m_p2DColliderComs[i]->Render();
        }
    }
#endif

    /* Font Render */

    return S_OK;
}

void CGoblin_SideScroller::Attack()
{
}

void CGoblin_SideScroller::Change_Animation()
{
    if (COORDINATE_3D == Get_CurCoord())
        return;

    if (m_iState != m_iPreState)
    {
		CGoblin_SideScroller::Animation2D eAnim = ANIM2D_LAST;
		switch (MONSTER_STATE(m_iState))
		{
        case Client::MONSTER_STATE::PATROL:
            eAnim = PATROL;
			break;

		case Client::MONSTER_STATE::HIT:
				eAnim = HIT;
			break;

		case Client::MONSTER_STATE::DEAD:
				eAnim = DEATH;
			break;
		default:
			break;
		}

        if (ANIM2D_LAST == eAnim)
            return;

		static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(eAnim);
    }
}

void CGoblin_SideScroller::Animation_End(COORDINATE _eCoord, _uint iAnimIdx)
{
    if (COORDINATE_3D == Get_CurCoord())
        return;

    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);

    switch ((CGoblin_SideScroller::Animation2D)pModelObject->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
    {
    case HIT:
        Set_AnimChangeable(true);
        break;

    case DEATH:
        Set_AnimChangeable(true);
        //풀링에 넣을 시 변경
        //Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);

        Event_DeleteObject(this);
        break;

    default:
        break;
    }
}

void CGoblin_SideScroller::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    if (OBJECT_GROUP::PLAYER & _pOtherCollider->Get_CollisionGroupID())
    {
        int a = 10;
    }

	if (OBJECT_GROUP::BLOCKER & _pOtherCollider->Get_CollisionGroupID())
    {
        /*if (true == _pMyCollider->Is_Trigger())
            break;*/
        if (true == static_cast<CBlocker*>(_pOtherObject)->Is_Floor())
        {
            /* 1. Blocker은 항상 AABB임을 가정. */

            /* 2. 나의 Collider 중점 기준, AABB에 가장 가까운 점을 찾는다. */
            _bool isResult = false;
            _float fEpsilon = 0.01f;
            _float2 vContactVector = {};
            isResult = static_cast<CCollider_Circle*>(_pMyCollider)->Compute_NearestPoint_AABB(static_cast<CCollider_AABB*>(_pOtherCollider), nullptr, &vContactVector);
            if (true == isResult)
            {
                /* 3. 충돌지점 벡터와 중력벡터를 내적하여 그 결과를 기반으로 Floor 인지 체크. */
                _float3 vGravityDir = m_pGravityCom->Get_GravityDirection();
                _float2 vGravityDirection = _float2(vGravityDir.x, vGravityDir.y);
                _float fGdotC = XMVectorGetX(XMVector2Dot(XMLoadFloat2(&vGravityDirection), XMVector2Normalize(XMLoadFloat2(&vContactVector))));
                if (1.0f - fEpsilon <= fGdotC)
                {
                    /* 결과가 1에 근접한다면 이는 floor로 봐야겠지. */
                    m_pGravityCom->Change_State(CGravity::STATE_FLOOR);

                }
            }
        }
    }

    __super::On_Collision2D_Enter(_pMyCollider, _pOtherCollider, _pOtherObject);
}

void CGoblin_SideScroller::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    int a = 10;
}

void CGoblin_SideScroller::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    if (OBJECT_GROUP::PLAYER & _pOtherCollider->Get_CollisionGroupID())
    {
        if (true == m_isContactToTarget)
        {
            m_isContactToTarget = false;
        }
    }

    if (OBJECT_GROUP::BLOCKER & _pOtherCollider->Get_CollisionGroupID())
    {
        CCollider_AABB* pOtherCol = static_cast<CCollider_AABB*>(_pOtherCollider);
        _float2 fLT = pOtherCol->Get_LT();
        _float2 fRB = pOtherCol->Get_RB();
        _float3 vPos;  XMStoreFloat3(&vPos, Get_FinalPosition());

        //왼쪽 끝이고 왼쪽 방향으로 가고 있었으면
        if (fLT.x >= vPos.x && XMVector3Equal(Get_ControllerTransform()->Get_State(CTransform::STATE_RIGHT), XMVectorSet(0.f, -1.f, 0.f, 0.f)))
        {
            Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, XMVectorSet(fLT.x, vPos.y, 0.f, 1.f));
            Set_2D_Direction(F_DIRECTION::RIGHT);
        }

        //오른쪽 끝이고 오른쪽 방향으로 가고 있었으면
        if (fRB.x <= vPos.x && XMVector3Equal(Get_ControllerTransform()->Get_State(CTransform::STATE_RIGHT), XMVectorSet(0.f, 1.f, 0.f, 0.f)))
        {
            Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, XMVectorSet(fRB.x, vPos.y, 0.f, 1.f));
            Set_2D_Direction(F_DIRECTION::LEFT);
        }

        //if(F_DIRECTION::RIGHT==Get_2DDirection())
        //    Set_2D_Direction(F_DIRECTION::LEFT);
        //else if (F_DIRECTION::LEFT == Get_2DDirection())
        //    Set_2D_Direction(F_DIRECTION::RIGHT);
    }
}

void CGoblin_SideScroller::On_Hit(CGameObject* _pHitter, _int _iDamg)
{
    __super::On_Hit(_pHitter, _iDamg);
}

HRESULT CGoblin_SideScroller::Ready_Components()
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

    if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_FSM"),
        TEXT("Com_FSM"), reinterpret_cast<CComponent**>(&m_pFSM), &FSMDesc)))
        return E_FAIL;

    /* 2D Collider */
    m_p2DColliderComs.resize(1);

    CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
    CircleDesc.pOwner = this;
    CircleDesc.fRadius = { 50.f };
    CircleDesc.vScale = { 1.0f, 1.0f };
    CircleDesc.vOffsetPosition = { 0.f, CircleDesc.fRadius };
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

HRESULT CGoblin_SideScroller::Ready_PartObjects()
{
    /* Part Body */
    CModelObject::MODELOBJECT_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    BodyDesc.strModelPrototypeTag_2D = TEXT("Goblin_SideScroller");
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

CGoblin_SideScroller* CGoblin_SideScroller::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CGoblin_SideScroller* pInstance = new CGoblin_SideScroller(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CGoblin_SideScroller");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CGoblin_SideScroller::Clone(void* _pArg)
{
    CGoblin_SideScroller* pInstance = new CGoblin_SideScroller(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CGoblin_SideScroller");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGoblin_SideScroller::Free()
{

    __super::Free();
}
