#include "stdafx.h"
#include "SketchSpace_Alien.h"
#include "GameInstance.h"
#include "FSM.h"
#include "ModelObject.h"
#include "Section_Manager.h"
#include "Blocker.h"
#include "Effect2D_Manager.h"
#include "Player.h"

CSketchSpace_Alien::CSketchSpace_Alien(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CMonster(_pDevice, _pContext)
{
}

CSketchSpace_Alien::CSketchSpace_Alien(const CSketchSpace_Alien& _Prototype)
    : CMonster(_Prototype)
{
}

HRESULT CSketchSpace_Alien::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSketchSpace_Alien::Initialize(void* _pArg)
{
    CSketchSpace_Alien::SIDESCROLLDESC* pDesc = static_cast<CSketchSpace_Alien::SIDESCROLLDESC*>(_pArg);
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

    m_pFSM->Add_State((_uint)MONSTER_STATE::SIDESCROLL_PATROL);
    m_pFSM->Add_State((_uint)MONSTER_STATE::SIDESCROLL_HIT);
    m_pFSM->Add_State((_uint)MONSTER_STATE::DEAD);
    m_pFSM->Set_State((_uint)MONSTER_STATE::SIDESCROLL_PATROL);

    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);
    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, IDLE_RIGHT, true);
    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, PATROL_RIGHT, true);
    
    pModelObject->Set_Animation(PATROL_RIGHT);

    pModelObject->Register_OnAnimEndCallBack(bind(&CSketchSpace_Alien::Animation_End, this, placeholders::_1, placeholders::_2));

    return S_OK;
}

void CSketchSpace_Alien::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta); /* Part Object Priority_Update */
}

void CSketchSpace_Alien::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta); /* Part Object Update */
}

void CSketchSpace_Alien::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta); /* Part Object Late_Update */
}

HRESULT CSketchSpace_Alien::Render()
{
    /* Model이 없는 Container Object 같은 경우 Debug 용으로 사용하거나, 폰트 렌더용으로. */

#ifdef _DEBUG
    __super::Render();
#endif

    /* Font Render */

    return S_OK;
}

void CSketchSpace_Alien::Attack()
{
}

void CSketchSpace_Alien::Change_Animation()
{
    if (COORDINATE_3D == Get_CurCoord())
        return;

    if (m_iState != m_iPreState)
    {
		CSketchSpace_Alien::Animation2D eAnim = ANIM2D_LAST;
		switch (MONSTER_STATE(m_iState))
		{
        case Client::MONSTER_STATE::PATROL:
            eAnim = PATROL_RIGHT;
			break;

		case Client::MONSTER_STATE::HIT:
				eAnim = HIT_RIGHT;
			break;

		case Client::MONSTER_STATE::DEAD:
				eAnim = DEATH_RIGHT;
			break;
		default:
			break;
		}

        if (ANIM2D_LAST == eAnim)
            return;

		static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(eAnim);
    }
}

void CSketchSpace_Alien::Animation_End(COORDINATE _eCoord, _uint iAnimIdx)
{
    if (COORDINATE_3D == Get_CurCoord())
        return;

    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);

    switch ((CSketchSpace_Alien::Animation2D)pModelObject->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
    {
    case HIT_RIGHT:
        Set_AnimChangeable(true);
        break;

    case DEATH_RIGHT:
        Set_AnimChangeable(true);
        //풀링에 넣을 시 변경
        //Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
        CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("Death_Burst"), CSection_Manager::GetInstance()->Get_Cur_Section_Key(), Get_ControllerTransform()->Get_WorldMatrix());

        Event_DeleteObject(this);
        break;

    default:
        break;
    }
}

void CSketchSpace_Alien::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    if (OBJECT_GROUP::PLAYER & _pOtherCollider->Get_CollisionGroupID())
    {
        //일단 처리
        if (CPlayer::STATE::JUMP_DOWN != static_cast<CPlayer*>(_pOtherObject)->Get_CurrentStateID())
            __super::On_Collision2D_Enter(_pMyCollider, _pOtherCollider, _pOtherObject);
    }

	if (OBJECT_GROUP::BLOCKER & _pOtherCollider->Get_CollisionGroupID())
    {
        /*if (true == _pMyCollider->Is_Trigger())
            break;*/
        //CGravity::STATE eGravityState = m_pGravityCom->Get_CurState();
        if (true == static_cast<CBlocker*>(_pOtherObject)->Is_Floor())
        {
           /* if (eGravityState == CGravity::STATE_FLOOR)
            {
                F_DIRECTION eDir = Get_2DDirection();

                if(F_DIRECTION::RIGHT== eDir)
                    Set_2D_Direction(F_DIRECTION::LEFT);
                else if (F_DIRECTION::LEFT == eDir)
                    Set_2D_Direction(F_DIRECTION::RIGHT);
            }*/


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

}

void CSketchSpace_Alien::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CSketchSpace_Alien::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    if (OBJECT_GROUP::PLAYER & _pOtherCollider->Get_CollisionGroupID())
    {
        if (true == m_isContactToTarget)
        {
            m_isContactToTarget = false;
        }
    }

    if (OBJECT_GROUP::BLOCKER  &  _pOtherCollider->Get_CollisionGroupID())
    {
        //CCollider_AABB* pOtherCol = static_cast<CCollider_AABB*>(_pOtherCollider);
        //_float2 fLT = pOtherCol->Get_LT();
        //_float2 fRB = pOtherCol->Get_RB();
        //_float3 vPos;  XMStoreFloat3(&vPos, Get_FinalPosition());


        //m_pGravityCom->Change_State(CGravity::STATE_FLOOR);
        //F_DIRECTION eDir = Get_2DDirection();
        ////Set_2D_Direction(eDir);
        //
        ////왼쪽 끝이고 왼쪽 방향으로 가고 있었으면
        //_float fEpsilon = 0.1f;
        //_float fOffsetX = 30.f;
        //_float2 vBound_LR = { fLT.x + fOffsetX , fRB.x - fOffsetX };
        ////if(false == static_cast<CBlocker*> (_pOtherObject)->Is_Floor())
        //if (vBound_LR.x >= vPos.x /*&& XMVector3Equal(Get_ControllerTransform()->Get_State(CTransform::STATE_RIGHT), XMVectorSet(0.f, -1.f, 0.f, 0.f))*/)
        //{
        //    Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, XMVectorSet(vBound_LR.x + fEpsilon, fLT.y, 0.f, 1.f));
        //    Set_2D_Direction(F_DIRECTION::RIGHT);
        //    m_p2DColliderComs[0]->Update_OwnerTransform();
        //}
        ////오른쪽 끝이고 오른쪽 방향으로 가고 있었으면
        //else if (vBound_LR.y <= vPos.x /*&& XMVector3Equal(Get_ControllerTransform()->Get_State(CTransform::STATE_RIGHT), XMVectorSet(0.f, 1.f, 0.f, 0.f)*/)
        //{
        //    Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, XMVectorSet(vBound_LR.y - fEpsilon, fLT.y, 0.f, 1.f));
        //    Set_2D_Direction(F_DIRECTION::LEFT);
        //    m_p2DColliderComs[0]->Update_OwnerTransform();
        //}

        //if(F_DIRECTION::RIGHT==Get_2DDirection())
        //    Set_2D_Direction(F_DIRECTION::LEFT);
        //else if (F_DIRECTION::LEFT == Get_2DDirection())
        //    Set_2D_Direction(F_DIRECTION::RIGHT);
    }
}

void CSketchSpace_Alien::On_Hit(CGameObject* _pHitter, _int _iDamg, _fvector _vForce)
{
    __super::On_Hit(_pHitter, _iDamg, _vForce);
}

void CSketchSpace_Alien::Enter_Section(const _wstring _strIncludeSectionName)
{
    __super::Enter_Section(_strIncludeSectionName);

    //if (TEXT("Chapter2_P0102") == _strIncludeSectionName)
    //{
    //    Set_Position(XMVectorSet(-640.0f, 1360.f, 0.f, 0.f));
    //}
}

HRESULT CSketchSpace_Alien::Ready_Components()
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
    m_p2DColliderComs.resize(2);

    CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
    CircleDesc.pOwner = this;
    CircleDesc.fRadius = { 20.f };
    CircleDesc.vScale = { 1.0f, 1.0f };
    CircleDesc.vOffsetPosition = { 0.f, 0.f };
    CircleDesc.isBlock = true;
    CircleDesc.iCollisionGroupID = OBJECT_GROUP::MONSTER;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &CircleDesc)))
        return E_FAIL;


    CircleDesc.fRadius = { 5.f };
    CircleDesc.vScale = { 1.0f, 1.0f };
    CircleDesc.vOffsetPosition = { 0.f, 20.f };
    CircleDesc.isBlock = false;
    CircleDesc.isTrigger = true;
    CircleDesc.iCollisionGroupID = OBJECT_GROUP::MONSTER;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
        TEXT("Com_ColliderHead"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[1]), &CircleDesc)))
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

HRESULT CSketchSpace_Alien::Ready_PartObjects()
{
    /* Part Body */
    CModelObject::MODELOBJECT_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    BodyDesc.strModelPrototypeTag_2D = TEXT("SketchSpace_Alien");
    BodyDesc.iModelPrototypeLevelID_2D = LEVEL_STATIC;

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

CSketchSpace_Alien* CSketchSpace_Alien::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CSketchSpace_Alien* pInstance = new CSketchSpace_Alien(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CSketchSpace_Alien");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSketchSpace_Alien::Clone(void* _pArg)
{
    CSketchSpace_Alien* pInstance = new CSketchSpace_Alien(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CSketchSpace_Alien");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSketchSpace_Alien::Free()
{

    __super::Free();
}
