#include "stdafx.h"
#include "Zippy.h"
#include "GameInstance.h"
#include "FSM.h"
#include "ModelObject.h"
#include "Section_Manager.h"

CZippy::CZippy(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CMonster(_pDevice, _pContext)
{
}

CZippy::CZippy(const CZippy& _Prototype)
    : CMonster(_Prototype)
{
}

HRESULT CZippy::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CZippy::Initialize(void* _pArg)
{
    CZippy::MONSTER_DESC* pDesc = static_cast<CZippy::MONSTER_DESC*>(_pArg);
    pDesc->eStartCoord = COORDINATE_2D;
    pDesc->isCoordChangeEnable = false;
    pDesc->iNumPartObjects = PART_END;

    pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(180.f);
    pDesc->tTransform2DDesc.fSpeedPerSec = 50.f;

    pDesc->fAlert2DRange = 300.f;
    pDesc->fChase2DRange = 600.f;
    pDesc->fAttack2DRange = 200.f;
    pDesc->fDelayTime = 2.f;
    
    pDesc->fHP = 5.f;

    pDesc->fFOVX = 90.f;
    pDesc->fFOVY = 30.f;

    m_fSpeed = pDesc->tTransform2DDesc.fSpeedPerSec;
    m_fDashDistance = 100.f;
    m_fElecTime = 5.f;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    m_pFSM->Add_State((_uint)MONSTER_STATE::IDLE);
    m_pFSM->Add_State((_uint)MONSTER_STATE::PATROL);
    m_pFSM->Add_State((_uint)MONSTER_STATE::ALERT);
    m_pFSM->Add_State((_uint)MONSTER_STATE::STANDBY);
    m_pFSM->Add_State((_uint)MONSTER_STATE::CHASE);
    m_pFSM->Add_State((_uint)MONSTER_STATE::ATTACK);
    m_pFSM->Add_State((_uint)MONSTER_STATE::HIT);
    m_pFSM->Set_State((_uint)MONSTER_STATE::IDLE);
    m_pFSM->Set_PatrolBound();

    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);

    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, IDLE_DOWN, true);
    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, IDLE_RIGHT, true);
    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, IDLE_UP, true);

    pModelObject->Set_Animation(IDLE_DOWN);

    pModelObject->Register_OnAnimEndCallBack(bind(&CZippy::Animation_End, this, placeholders::_1, placeholders::_2));

    CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this);

    return S_OK;
}

void CZippy::Priority_Update(_float _fTimeDelta)
{
    if (true == m_isDelay)
    {
        m_fAccTime += _fTimeDelta;
        if (m_fDelayTime <= m_fAccTime)
        {
            m_fAccTime = 0.f;
            m_isDelay = false;
        }
    }
    //공격받으면 풀려야함
    if (true == m_isElectric)
    {
        m_fAccElecTime += _fTimeDelta;
        if (m_fElecTime <= m_fAccElecTime)
        {
            m_fAccElecTime = 0.f;
            m_isElectric = false;
        }
    }

    __super::Priority_Update(_fTimeDelta); /* Part Object Priority_Update */
}

void CZippy::Update(_float _fTimeDelta)
{
    if (true == m_isDash)
    {
        //대시이동
        Get_ControllerTransform()->Go_Direction(XMLoadFloat3(&m_vDashDirection), _fTimeDelta);
        m_fAccDistance += Get_ControllerTransform()->Get_SpeedPerSec() * _fTimeDelta;

        if (m_fDashDistance <= m_fAccDistance)
        {
            ++m_iAttackCount;
            m_fAccDistance = 0.f;

            if (4 <= m_iAttackCount)
            {
                Get_ControllerTransform()->Set_SpeedPerSec(m_fSpeed);
                Attack_End();
                m_iAttackCount = 0;
                m_isDash = false;
            }
            else
                Attack();
        }
    }

    __super::Update(_fTimeDelta); /* Part Object Update */
}

void CZippy::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta); /* Part Object Late_Update */
}

HRESULT CZippy::Render()
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

void CZippy::Attack()
{
    if (true == m_isDash)
    {
        Get_ControllerTransform()->Set_SpeedPerSec(m_fSpeed * 4.f); //속도 임시
        XMStoreFloat3(&m_vDashDirection, XMVector3Normalize(m_pTarget->Get_FinalPosition() - Get_FinalPosition()));
    }

    if (true == m_isElectric)
    {

    }


    Set_AnimChangeable(true);
    Delay_On();
}

void CZippy::Change_Animation()
{
    if (COORDINATE_3D == Get_CurCoord())
        return;

    if (m_iState != m_iPreState)
    {
		CZippy::Animation2D eAnim = ANIM2D_LAST;
		switch (MONSTER_STATE(m_iState))
		{
		case Client::MONSTER_STATE::IDLE:
        case Client::MONSTER_STATE::PATROL:
        case Client::MONSTER_STATE::STANDBY:
        case Client::MONSTER_STATE::CHASE:
            if (true == m_isElectric)
            {
                if (F_DIRECTION::UP == m_e2DDirection)
                    eAnim = ELEC_UP;
                else if (F_DIRECTION::DOWN == m_e2DDirection)
                    eAnim = ELEC_DOWN;
                else if (F_DIRECTION::RIGHT == m_e2DDirection || F_DIRECTION::LEFT == m_e2DDirection)
                    eAnim = ELEC_RIGHT;
            }
            else if (false == m_isElectric)
            {
                if (F_DIRECTION::UP == m_e2DDirection)
                    eAnim = IDLE_UP;
                else if (F_DIRECTION::DOWN == m_e2DDirection)
                    eAnim = IDLE_DOWN;
                else if (F_DIRECTION::RIGHT == m_e2DDirection || F_DIRECTION::LEFT == m_e2DDirection)
                    eAnim = IDLE_RIGHT;
            }
			break;
		case Client::MONSTER_STATE::ATTACK:
        {
            if (true == m_isElectric)
                return;

            //대시 중일 때는 다음 대시로
            _uint iNum;
            if (true == m_isDash)
                iNum = 2;
            else
                 iNum = (_uint)ceil(m_pGameInstance->Compute_Random(0.f, 2.f));

            switch (iNum)
            {
            case 1:
                if (F_DIRECTION::UP == m_e2DDirection)
                    eAnim = ELEC_IN_UP;
                else if (F_DIRECTION::DOWN == m_e2DDirection)
                    eAnim = ELEC_IN_DOWN;
                else if (F_DIRECTION::RIGHT == m_e2DDirection || F_DIRECTION::LEFT == m_e2DDirection)
                    eAnim = ELEC_IN_RIGHT;
                m_isElectric = true;
                break;
            case 2:
                if (F_DIRECTION::UP == m_e2DDirection)
                    eAnim = DASH_IN_UP;
                else if (F_DIRECTION::DOWN == m_e2DDirection)
                    eAnim = DASH_IN_DOWN;
                else if (F_DIRECTION::RIGHT == m_e2DDirection || F_DIRECTION::LEFT == m_e2DDirection)
                    eAnim = DASH_IN_RIGHT;
                m_isDash = true;
                break;

            default:
                break;
            }
        }
        break;

		case Client::MONSTER_STATE::HIT:
			if (F_DIRECTION::UP == m_e2DDirection)
				eAnim = HIT_UP;
			else if (F_DIRECTION::DOWN == m_e2DDirection)
				eAnim = HIT_DOWN;
			else if (F_DIRECTION::RIGHT == m_e2DDirection || F_DIRECTION::LEFT == m_e2DDirection)
				eAnim = HIT_RIGHT;
			break;
		case Client::MONSTER_STATE::DEAD:
			if (F_DIRECTION::UP == m_e2DDirection)
				eAnim = DIE_UP;
			else if (F_DIRECTION::DOWN == m_e2DDirection)
				eAnim = DIE_DOWN;
			else if (F_DIRECTION::RIGHT == m_e2DDirection || F_DIRECTION::LEFT == m_e2DDirection)
				eAnim = DIE_RIGHT;
			break;
		default:
			break;
		}

		static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(eAnim);
    }
}

void CZippy::Animation_End(COORDINATE _eCoord, _uint iAnimIdx)
{
    if (COORDINATE_3D == Get_CurCoord())
        return;

    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);

    switch ((CZippy::Animation2D)pModelObject->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
    {
    case DASH_IN_DOWN:
        pModelObject->Switch_Animation(DASH_DOWN);
        break;
    case DASH_IN_RIGHT:
        pModelObject->Switch_Animation(DASH_RIGHT);
        break;
    case DASH_IN_UP:
        pModelObject->Switch_Animation(DASH_UP);
        break;

    case ELEC_IN_DOWN:
        pModelObject->Switch_Animation(ELEC_DOWN);
        break;
    case ELEC_IN_RIGHT:
        pModelObject->Switch_Animation(ELEC_RIGHT);
        break;
    case ELEC_IN_UP:
        pModelObject->Switch_Animation(ELEC_UP);
        break;


    case DASH_OUT_DOWN:
    case DASH_OUT_RIGHT:
    case DASH_OUT_UP:
        Set_AnimChangeable(true);
        break;

    case ELEC_OUT_DOWN:
    case ELEC_OUT_RIGHT:
    case ELEC_OUT_UP:
        Set_AnimChangeable(true);
        break;

    case HIT_DOWN:
    case HIT_RIGHT:
    case HIT_UP:
        Set_AnimChangeable(true);
        break;

    default:
        break;
    }
}

void CZippy::Attack_End()
{
    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);
    if (true == m_isDash)
    {
        if (F_DIRECTION::UP == m_e2DDirection)
            pModelObject->Switch_Animation(DASH_OUT_UP);
        else if (F_DIRECTION::DOWN == m_e2DDirection)
            pModelObject->Switch_Animation(DASH_OUT_DOWN);
        else if (F_DIRECTION::RIGHT == m_e2DDirection || F_DIRECTION::LEFT == m_e2DDirection)
            pModelObject->Switch_Animation(DASH_OUT_RIGHT);
    }

    if (true == m_isElectric)
    {
        if (F_DIRECTION::UP == m_e2DDirection)
            pModelObject->Switch_Animation(ELEC_OUT_UP);
        else if (F_DIRECTION::DOWN == m_e2DDirection)
            pModelObject->Switch_Animation(ELEC_OUT_DOWN);
        else if (F_DIRECTION::RIGHT == m_e2DDirection || F_DIRECTION::LEFT == m_e2DDirection)
            pModelObject->Switch_Animation(ELEC_OUT_RIGHT);
    }
    
}

void CZippy::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    if (OBJECT_GROUP::PLAYER & _pOtherObject->Get_ObjectGroupID())
    {
        if (true == m_isElectric)
        {
            //플레이어 setstate 부르기
            Event_Hit(this, _pOtherObject, Get_Stat().iDamg);
            m_isElectric = false;
        }
    }
}

void CZippy::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CZippy::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    if (OBJECT_GROUP::PLAYER & _pOtherObject->Get_ObjectGroupID())
    {
        if (true == m_isContactToTarget)
        {
            m_isContactToTarget = false;
        }
    }
}

void CZippy::On_Hit(CGameObject* _pHitter, _int _iDamg)
{
    __super::On_Hit(_pHitter, _iDamg);
}

_bool CZippy::Has_StateAnim(MONSTER_STATE _eState)
{
    _bool HasAnim = true;
    MONSTER_STATE eState = _eState;
    //현재 상태에 대해 확인
    if (MONSTER_STATE::LAST == _eState)
    {
        eState = (MONSTER_STATE)m_iState;
    }

    switch ((MONSTER_STATE)eState)
    {
    case MONSTER_STATE::ALERT:
        HasAnim = false;
        break;
    }

    return HasAnim;
}



HRESULT CZippy::Ready_Components()
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
    CircleDesc.fRadius = { 40.f };
    CircleDesc.vScale = { 1.0f, 1.0f };
    CircleDesc.vOffsetPosition = { 0.f, CircleDesc.fRadius };
    CircleDesc.isBlock = false;
    CircleDesc.iCollisionGroupID = OBJECT_GROUP::MONSTER;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &CircleDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CZippy::Ready_PartObjects()
{
    /* Part Body */
    CModelObject::MODELOBJECT_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    BodyDesc.strModelPrototypeTag_2D = TEXT("Zippy");
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

CZippy* CZippy::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CZippy* pInstance = new CZippy(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CZippy");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CZippy::Clone(void* _pArg)
{
    CZippy* pInstance = new CZippy(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CZippy");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CZippy::Free()
{

    __super::Free();
}
