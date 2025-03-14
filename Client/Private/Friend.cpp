#include "stdafx.h"
#include "Friend.h"
#include "GameInstance.h"

#include "FriendBody.h"

CFriend::CFriend(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CCharacter(_pDevice, _pContext)
{
}

CFriend::CFriend(const CFriend& _Prototype)
    :CCharacter(_Prototype)
{
}

HRESULT CFriend::Initialize(void* _pArg)
{
    FRIEND_DESC* pDesc = static_cast<FRIEND_DESC*>(_pArg);

    // Save Desc
    m_eCurState = pDesc->eStartState;
    m_strFightLayerTag = pDesc->strFightLayerTag.empty() ? TEXT("") : pDesc->strFightLayerTag;
    m_strDialogueTag = pDesc->strDialogueTag.empty() ? TEXT("") : pDesc->strDialogueTag;
    m_iNumDialogueIndices = pDesc->iNumDialoguesIndices;
    m_iDialogueIndex = pDesc->iStartDialogueIndex;

    // Add Desc
    pDesc->iNumPartObjects = PART_LAST;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Register_OnAnimEndCallBack(bind(&CFriend::On_AnimEnd, this, placeholders::_1, placeholders::_2));
    return S_OK;
}

void CFriend::Priority_Update(_float _fTimeDelta)
{

    __super::Priority_Update(_fTimeDelta);
}

void CFriend::Update(_float _fTimeDelta)
{
    Action_State(_fTimeDelta);

    __super::Update(_fTimeDelta);
}

void CFriend::Late_Update(_float _fTimeDelta)
{
    State_Change();

    __super::Late_Update(_fTimeDelta);
}

HRESULT CFriend::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

void CFriend::Switch_PartAnim(_uint _iPartIndex, _uint _iAnimIndex)
{
    assert(m_PartObjects[_iPartIndex]);

    static_cast<CModelObject*>(m_PartObjects[_iPartIndex])->Switch_Animation(_iAnimIndex);
}

void CFriend::Add_IdleQueue(FRIEND_STATE _eState)
{
    if (FRIEND_STATE::FRIEND_LAST <= _eState)
        return;

    m_StateIdleQueue.push_back(_eState);
}

void CFriend::Pop_IdleQueue()
{
    if (true == m_StateIdleQueue.empty())
        return;

    m_StateIdleQueue.pop_front();
}

CFriend::FRIEND_STATE CFriend::Check_IdleQueue()
{
    if (true == m_StateIdleQueue.empty())
        return FRIEND_STATE::FRIEND_LAST;

    return m_StateIdleQueue.front();
}


void CFriend::Clear_IdleQueue()
{
    m_StateIdleQueue.clear();
}

void CFriend::State_Change()
{
    if (m_ePreState == m_eCurState)
        return;

    switch (m_eCurState)
    {
    case Client::CFriend::FRIEND_IDLE:
        State_Change_Idle();
        break;
    case Client::CFriend::FRIEND_MOVE:
        State_Change_Move();
        break;
    case Client::CFriend::FRIEND_CHASE:
        State_Change_Chase();
        break;
    case Client::CFriend::FRIEND_ATTACK:
        State_Change_Attack();
        break;
    case Client::CFriend::FRIEND_TALK:
        State_Change_Talk();
        break;
    case Client::CFriend::FRIEND_MOJAM:
        State_Change_Mojam();
        break;
    default:
        break;
    }

    m_ePreState = m_eCurState;
}

void CFriend::Set_ChaseTarget(CGameObject* _pChaseTarget)
{
    if (nullptr != m_pChaseTarget)
        Delete_ChaseTarget();

    m_pChaseTarget = _pChaseTarget;
}

void CFriend::State_Change_Idle()
{

}

void CFriend::State_Change_Move()
{

}

void CFriend::State_Change_Chase()
{
    if (m_eCurMode == MODE_DEFAULT)
    {
        if (nullptr == m_pChaseTarget)
        {
            // Friend Controller에서 Chase 대상의 Ptr을 받아온다.
        }
        
    }
    else if (m_eCurMode == MODE_FIGHT)
    {
        // 전투 대상 레이어에서 가장 가까운 적을 탐색한다.
    }
}

void CFriend::State_Change_Attack()
{

}

void CFriend::State_Change_Talk()
{
}

void CFriend::State_Change_Mojam()
{
}

void CFriend::Update_ChaseTargetDirection()
{
    if (nullptr == m_pChaseTarget)
        return;
    _vector vPlayerPos = m_pChaseTarget->Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION);

    Update_MoveTargetDirection(vPlayerPos);
}

void CFriend::Update_MoveTargetDirection(_fvector _vTargetPos)
{
    CTransform* pTransform2D = Get_ControllerTransform()->Get_Transform(COORDINATE_2D);
    _vector vPos = pTransform2D->Get_State(CTransform::STATE_POSITION);
    _vector vDir = _vTargetPos - vPos;
    _float fDiff = XMVectorGetX(XMVector3Length(vDir));
    vDir /= fDiff;

    /* 2. 외적을 통한 2D 애니메이션 방향 판별 */
    _vector vUpVector = { 0.0f, 1.0f, 0.0f, 0.0f };
    _vector vCrossResult = XMVector3Cross(vUpVector, vDir);
    _float fDot = XMVectorGetX(XMVector3Dot(vUpVector, vDir));

#pragma region Check AnimDirection

    /* 2. 외적의 결과 + 내적의 결과로 4방향 판별*/
    if (0.0f > XMVectorGetZ(vCrossResult))
    {
        /* 우측 방향에 플레이어 존재. */
        if (0.0f < fDot)
        {
            // 윗 방향 판별
            if (0.5f < fDot)
            {
                // 위로 본다.
                m_eDirection = DIR_UP;
            }
            else
            {
                // 우측으로 본다.
                m_eDirection = DIR_RIGHT;
            }
        }
        else
        {
            // 아래 방향 판별 
            if (-0.5f * -0.5f > fDot * fDot)
            {
                // 우측으로 본다.
                m_eDirection = DIR_RIGHT;
            }
            else
            {
                // 아래로 본다.
                m_eDirection = DIR_DOWN;
            }
        }


    }
    else
    {
        /* 좌측 방향에 플레이어 존재. */
        if (0.0f < fDot)
        {
            // 윗 방향 판별
            if (0.5f < fDot)
            {
                // 위로 본다.
                m_eDirection = DIR_UP;
            }
            else
            {
                // 좌측으로 본다.
                m_eDirection = DIR_LEFT;
            }
        }
        else
        {
            // 아래 방향 판별 
            if (-0.5f * -0.5f > fDot * fDot)
            {
                // 좌측으로 본다.
                m_eDirection = DIR_LEFT;
            }
            else
            {
                // 아래로 본다.
                m_eDirection = DIR_DOWN;
            }
        }
    }

#pragma endregion // Check AnimDirection


    // 결정된 최종 방향으로 Anim Index 변경.
    Change_AnimIndex_CurDirection();
}

void CFriend::ChaseToTarget(_float _fTimeDelta)
{
    if (nullptr == m_pChaseTarget)
    {
        m_eCurState = FRIEND_IDLE;
        return;
    }
    _vector vChaseTargetPos = m_pChaseTarget->Get_FinalPosition();
    static _bool isMoveArrival = false; // 도착 여부 체크.
    isMoveArrival = Move_To(vChaseTargetPos, _fTimeDelta);

    if (true == isMoveArrival)
        m_eCurState = FRIEND_IDLE;
}

void CFriend::ChaseToTargetPosition(_float2 _vTargetPosition, _float _fTimeDelta)
{
    _vector vChaseTargetPos = XMVectorSetW(XMLoadFloat2(&_vTargetPosition), 1.0f);
    static _bool isMoveArrival = false; // 도착 여부 체크.
    isMoveArrival = Move_To(vChaseTargetPos, _fTimeDelta);

    if (true == isMoveArrival)
        m_eCurState = FRIEND_IDLE;
}

void CFriend::Action_State(_float _fTimeDelta)
{
    switch (m_eCurState)
    {
    case Client::CFriend::FRIEND_IDLE:
        Action_State_Idle(_fTimeDelta); // Idle Queue 검사, Target이 존재한다면 Target과의 거리 확인 후 전환.
        break;
    case Client::CFriend::FRIEND_MOVE:
        Action_State_Move(_fTimeDelta); // 목표 위치까지 이동 후, Idle 로 전환.
        break;
    case Client::CFriend::FRIEND_CHASE:
        Action_State_Chase(_fTimeDelta); // 타겟과의 일정거리까지 이동 후 Idle로 전환.
        break;
    case Client::CFriend::FRIEND_ATTACK:
        Action_State_Attack(_fTimeDelta); // Attack은 단순 공격 모션을 재생하고 콜라이더를 켜는정도 
        break;
    case Client::CFriend::FRIEND_TALK:
        Action_State_Talk(_fTimeDelta); // Dialogue 재생 하는 State인데 어떻게 재생해야 할지 딱히 모르겠음 >> 재생 끝나면 Idle로 가야겠지. 
        break;
    case Client::CFriend::FRIEND_MOJAM:
        Action_State_Mojam(_fTimeDelta); // Mojam 애니메이션 재생 후 Idle 
        break;
    default:
        break;
    }
}

void CFriend::Action_State_Idle(_float _fTimeDelta)
{
    if (false == m_StateIdleQueue.empty())
    {
        /* 1. IdleQueue 에 존재하는 상태로 전환. */
        FRIEND_STATE eNextState = Check_IdleQueue();
        if (FRIEND_LAST != eNextState)
        {
            /* 2. NextState 가 LAST 가 아닌 경우, */
            m_eCurState = eNextState;
            return;
        }
    }

    if (nullptr != m_pChaseTarget)
    {

    }
}

void CFriend::Action_State_Move(_float _fTimeDelta)
{
    ChaseToTargetPosition(m_vMoveTargetPosition, _fTimeDelta);
}

void CFriend::Action_State_Chase(_float _fTimeDelta)
{
    ChaseToTarget(_fTimeDelta);

    Update_ChaseTargetDirection();
}

void CFriend::Action_State_Attack(_float _fTimeDelta)
{
}

void CFriend::Action_State_Talk(_float _fTimeDelta)
{
}

void CFriend::Action_State_Mojam(_float _fTimeDelta)
{
}

HRESULT CFriend::Ready_Components()
{
    m_p2DColliderComs.resize(1);
    //몸통 콜라이더
    CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
    CircleDesc.pOwner = this;
    CircleDesc.fRadius = 20.f;
    CircleDesc.vScale = { 1.0f, 1.0f };
    CircleDesc.vOffsetPosition = { 0.f, CircleDesc.fRadius * 0.5f };
    CircleDesc.isBlock = false;
    CircleDesc.isTrigger = false;
    CircleDesc.iCollisionGroupID = OBJECT_GROUP::FRIEND;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
        TEXT("Com_Body2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &CircleDesc)))
        return E_FAIL;

    /* Com_Gravity */
    CGravity::GRAVITY_DESC GravityDesc = {};
    GravityDesc.fGravity = 9.8f * 290.f;
    GravityDesc.vGravityDirection = { 0.0f, -1.0f, 0.0f };
    GravityDesc.pOwner = this;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Gravity"),
        TEXT("Com_Gravity"), reinterpret_cast<CComponent**>(&m_pGravityCom), &GravityDesc)))
        return E_FAIL;
    Safe_AddRef(m_pGravityCom);

    m_pGravityCom->Set_Active(false);

    return S_OK;
}

HRESULT CFriend::Ready_PartObjects(FRIEND_DESC* _pDesc)
{
    
    {/* Part Body */
        CFriendBody::MODELOBJECT_DESC Desc{};
        Desc.eStartCoord = m_pControllerTransform->Get_CurCoord();
        Desc.iCurLevelID = m_iCurLevelID;
        Desc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

        Desc.iModelPrototypeLevelID_2D = _pDesc->iModelTagLevelID;
        Desc.strModelPrototypeTag_2D = _pDesc->strPartBodyModelTag;
        Desc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
        Desc.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
        Desc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);

        Desc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
        Desc.tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);
        Desc.tTransform2DDesc.fRotationPerSec = XMConvertToRadians(180.f);
        Desc.tTransform2DDesc.fSpeedPerSec = 300.f;

        m_PartObjects[PART_BODY] = static_cast<CFriendBody*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_FriendBody"), &Desc));
        if (nullptr == m_PartObjects[PART_BODY])
        {
            MSG_BOX("CPlayer Body Creation Failed");
            return E_FAIL;
        }
    }/* Part Body */

    return S_OK;
}

void CFriend::Free()
{
    Safe_Release(m_pChaseTarget);

    __super::Free();
}
