#include "stdafx.h"
#include "Zippy.h"
#include "GameInstance.h"
#include "FSM.h"
#include "ModelObject.h"
#include "Section_Manager.h"
#include "Player.h"
#include "Effect2D_Manager.h"
#include "Pooling_Manager.h"
#include "PlayerData_Manager.h"

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

    pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(180.f);
    pDesc->tTransform2DDesc.fSpeedPerSec = 100.f;

    pDesc->fAlert2DRange = 300.f;
    pDesc->fChase2DRange = 600.f;
    pDesc->fAttack2DRange = 300.f;
    pDesc->fDelayTime = 2.f;

    pDesc->_tStat.iHP = 5;
    pDesc->_tStat.iMaxHP = 5;
    pDesc->_tStat.iDamg = 1;

    m_fSpeed = pDesc->tTransform2DDesc.fSpeedPerSec;
    m_fDashDistance = 200.f;
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
    m_pFSM->Add_State((_uint)MONSTER_STATE::DEAD);
    m_pFSM->Set_State((_uint)MONSTER_STATE::IDLE);
    m_pFSM->Set_PatrolBound();

    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);

    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, IDLE_DOWN, true);
    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, IDLE_RIGHT, true);
    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, IDLE_UP, true);

    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, DASH_DOWN, true);
    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, DASH_RIGHT, true);
    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, DASH_UP, true);

    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, ELEC_DOWN, true);
    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, ELEC_RIGHT, true);
    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, ELEC_UP, true);

    pModelObject->Set_Animation(IDLE_DOWN);

    pModelObject->Register_OnAnimEndCallBack(bind(&CZippy::Animation_End, this, placeholders::_1, placeholders::_2));

    /* Com_AnimEventGenerator */
    CAnimEventGenerator::ANIMEVTGENERATOR_DESC tAnimEventDesc{};
    tAnimEventDesc.pReceiver = this;
    tAnimEventDesc.pSenderModel = static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_2D);
    m_pAnimEventGenerator = static_cast<CAnimEventGenerator*> (m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, LEVEL_STATIC, TEXT("Prototype_Component_ZippyAttackAnimEvent"), &tAnimEventDesc));
    Add_Component(TEXT("AnimEventGenerator"), m_pAnimEventGenerator);

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
    //priority에서 처리하는게 맞는지 고민
    if (true == m_isElectric)
    {
        m_fAccElecTime += _fTimeDelta;
        if (m_fElecTime <= m_fAccElecTime)
        {
            m_fAccElecTime = 0.f;
            Set_AnimChangeable(false);

            Attack_End();
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
    __super::Render();
#endif

    /* Font Render */

    return S_OK;
}

void CZippy::Attack()
{
    if (true == m_isDash)
    {
        Get_ControllerTransform()->Set_SpeedPerSec(m_fSpeed * 10.f); //속도 임시
        XMStoreFloat3(&m_vDashDirection, XMVector3Normalize(m_pTarget->Get_FinalPosition() - Get_FinalPosition()));
        return;
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
                if (E_DIRECTION::UP == Get_2DDirection())
                    eAnim = ELEC_UP;
                else if (E_DIRECTION::DOWN == Get_2DDirection())
                    eAnim = ELEC_DOWN;
                else if (E_DIRECTION::RIGHT == Get_2DDirection() || E_DIRECTION::LEFT == Get_2DDirection())
                    eAnim = ELEC_RIGHT;

                if (COORDINATE_2D == CPlayerData_Manager::GetInstance()->Get_PlayerCoord())
                {
                    if (CSection_Manager::GetInstance()->Is_PlayerInto(m_strSectionName))
                    {
                        m_pGameInstance->Start_SFX_Distance2D_Delay(_wstring(L"A_sfx_zippi_saw_loop"),
                            m_pControllerTransform->Get_State(CTransform::STATE_POSITION), CPlayerData_Manager::GetInstance()->Get_PlayerPosition(), 0.1f, g_SFXVolume*0.5f, 0.f);
                    }
                }
            }
            else if (false == m_isElectric)
            {
                if (E_DIRECTION::UP == Get_2DDirection())
                    eAnim = IDLE_UP;
                else if (E_DIRECTION::DOWN == Get_2DDirection())
                    eAnim = IDLE_DOWN;
                else if (E_DIRECTION::RIGHT == Get_2DDirection() || E_DIRECTION::LEFT == Get_2DDirection())
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
                if (E_DIRECTION::UP == Get_2DDirection())
                    eAnim = ELEC_IN_UP;
                else if (E_DIRECTION::DOWN == Get_2DDirection())
                    eAnim = ELEC_IN_DOWN;
                else if (E_DIRECTION::RIGHT == Get_2DDirection() || E_DIRECTION::LEFT == Get_2DDirection())
                    eAnim = ELEC_IN_RIGHT;
                m_isElectric = true;
                break;
            case 2:
                if (E_DIRECTION::UP == Get_2DDirection())
                    eAnim = DASH_IN_UP;
                else if (E_DIRECTION::DOWN == Get_2DDirection())
                    eAnim = DASH_IN_DOWN;
                else if (E_DIRECTION::RIGHT == Get_2DDirection() || E_DIRECTION::LEFT == Get_2DDirection())
                    eAnim = DASH_IN_RIGHT;
                break;

            default:
                break;
            }

            if (COORDINATE_2D == CPlayerData_Manager::GetInstance()->Get_PlayerCoord())
            {
                if (CSection_Manager::GetInstance()->Is_PlayerInto(m_strSectionName))
                {
                    m_pGameInstance->Start_SFX_Distance2D_Delay(_wstring(L"A_sfx_zippi_electrify_loop"),
                        m_pControllerTransform->Get_State(CTransform::STATE_POSITION), CPlayerData_Manager::GetInstance()->Get_PlayerPosition(), 0.1f, g_SFXVolume*0.5f, 0.f);
                }
            }

        }
        break;

		case Client::MONSTER_STATE::HIT:
			if (E_DIRECTION::UP == Get_2DDirection())
				eAnim = HIT_UP;
			else if (E_DIRECTION::DOWN == Get_2DDirection())
				eAnim = HIT_DOWN;
			else if (E_DIRECTION::RIGHT == Get_2DDirection() || E_DIRECTION::LEFT == Get_2DDirection())
				eAnim = HIT_RIGHT;

            m_pGameInstance->End_SFX(_wstring(L"A_sfx_zippi_saw_loop"));
            m_pGameInstance->End_SFX(_wstring(L"A_sfx_zippi_electrify_loop"));

            m_pGameInstance->Start_SFX_Delay(_wstring(L"A_sfx_sword_hit_zippi_") + (to_wstring(rand()%3+1)), 0.1f, g_SFXVolume*0.5f);
			break;

		case Client::MONSTER_STATE::DEAD:
			if (E_DIRECTION::UP == Get_2DDirection())
				eAnim = DIE_UP;
			else if (E_DIRECTION::DOWN == Get_2DDirection())
				eAnim = DIE_DOWN;
			else if (E_DIRECTION::RIGHT == Get_2DDirection() || E_DIRECTION::LEFT == Get_2DDirection())
				eAnim = DIE_RIGHT;

            m_pGameInstance->End_SFX(_wstring(L"A_sfx_zippi_saw_loop"));
            m_pGameInstance->End_SFX(_wstring(L"A_sfx_zippi_electrify_loop"));

            m_pGameInstance->Start_SFX(_wstring(L"A_sfx_zippi_death_") + to_wstring(rand() % 2), g_SFXVolume*0.5f);
			break;
		default:
			break;
		}

        if (ANIM2D_LAST == eAnim)
            return;

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
        m_isDash = true;
        break;
    case DASH_IN_RIGHT:
        pModelObject->Switch_Animation(DASH_RIGHT);
        m_isDash = true;
        break;
    case DASH_IN_UP:
        pModelObject->Switch_Animation(DASH_UP);
        m_isDash = true;
        break;

    case ELEC_IN_DOWN:
        pModelObject->Switch_Animation(ELEC_DOWN);
        Set_AnimChangeable(true);
        break;
    case ELEC_IN_RIGHT:
        pModelObject->Switch_Animation(ELEC_RIGHT);
        Set_AnimChangeable(true);
        break;
    case ELEC_IN_UP:
        pModelObject->Switch_Animation(ELEC_UP);
        Set_AnimChangeable(true);
        break;

    //case ELEC_DOWN:
    //case ELEC_RIGHT:
    //case ELEC_UP:
    //    Set_AnimChangeable(true);
    //    break;


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

    case DIE_DOWN:
    case DIE_RIGHT:
    case DIE_UP:
        Monster_Death();

        //Set_AnimChangeable(true);
        ////풀링에 넣을 시 변경
        ////Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
        //CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("Death_Burst"), CSection_Manager::GetInstance()->Get_Cur_Section_Key(), Get_ControllerTransform()->Get_WorldMatrix());
       
        ////확률로 전구 생성
        //if (2 == (_int)ceil(m_pGameInstance->Compute_Random(0.f, 3.f)))
        //{
        //    _float3 vPos; XMStoreFloat3(&vPos, Get_FinalPosition());
        //    _wstring strCurSection = CSection_Manager::GetInstance()->Get_Cur_Section_Key();
        //    CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_2DBulb"), COORDINATE_2D, &vPos, nullptr, nullptr, &strCurSection);
        //}
        //
        //Event_DeleteObject(this);
        break;

    default:
        break;
    }
}

void CZippy::Attack_End()
{
    //현재 애니메이션만 바꿔주므로
    if (m_iState == (_uint)MONSTER_STATE::DEAD)
        return;

    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);
    if (true == m_isDash)
    {
        if (E_DIRECTION::UP == Get_2DDirection())
            pModelObject->Switch_Animation(DASH_OUT_UP);
        else if (E_DIRECTION::DOWN == Get_2DDirection())
            pModelObject->Switch_Animation(DASH_OUT_DOWN);
        else if (E_DIRECTION::RIGHT == Get_2DDirection() || E_DIRECTION::LEFT == Get_2DDirection())
            pModelObject->Switch_Animation(DASH_OUT_RIGHT);
    }

    if (true == m_isElectric)
    {
        if (E_DIRECTION::UP == Get_2DDirection())
            pModelObject->Switch_Animation(ELEC_OUT_UP);
        else if (E_DIRECTION::DOWN == Get_2DDirection())
            pModelObject->Switch_Animation(ELEC_OUT_DOWN);
        else if (E_DIRECTION::RIGHT == Get_2DDirection() || E_DIRECTION::LEFT == Get_2DDirection())
            pModelObject->Switch_Animation(ELEC_OUT_RIGHT);
    }
    
}

void CZippy::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    OBJECT_GROUP eOtherGroupID = (OBJECT_GROUP)_pOtherObject->Get_ObjectGroupID();

    if (OBJECT_GROUP::PLAYER & eOtherGroupID)
    {
        if (true == m_isElectric)
        {
            static_cast<CPlayer*>(_pOtherObject)->Set_State(CPlayer::ELECTRIC);
            Event_Hit(this, static_cast<CCharacter*>(_pOtherObject), Get_Stat().iDamg, XMVectorZero());
            Attack_End();
            m_isElectric = false;
            m_fAccElecTime = 0.f;
            Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
        }
        else
        {
            __super::On_Collision2D_Enter(_pMyCollider, _pOtherCollider, _pOtherObject);
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

void CZippy::On_Hit(CGameObject* _pHitter, _int _iDamg, _fvector _vForce)
{
    if (true == m_isDash)
    {
        //대시 중일 때는 데미지만 입음.
        m_tStat.iHP -= _iDamg;
        if (0 >= m_tStat.iHP)
        {
            if (0 == m_tStat.iHP)
            {
                m_isDash = false;
                Set_AnimChangeable(true);
                m_p2DColliderComs[0]->Set_Active(false);
                Event_ChangeMonsterState(MONSTER_STATE::DEAD, m_pFSM);
                static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Start_HitRender();
            }
        }
        else
        {
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Start_HitRender();
            //KnockBack(_vForce);
            //Effect
            _matrix matFX = Get_ControllerTransform()->Get_WorldMatrix();

            _wstring strFXTag = L"Hit_FX";
            strFXTag += to_wstring((_int)ceil(m_pGameInstance->Compute_Random(0.f, 5.f)));
            CEffect2D_Manager::GetInstance()->Play_Effect(strFXTag, CSection_Manager::GetInstance()->Get_Cur_Section_Key(), matFX);
            matFX.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f);
            switch ((_uint)ceil(m_pGameInstance->Compute_Random(0.f, 4.f)))
            {
            case 1:
                CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("Hit_Words1"), CSection_Manager::GetInstance()->Get_Cur_Section_Key(), matFX);
                break;

            case 2:
                CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("Hit_Words2"), CSection_Manager::GetInstance()->Get_Cur_Section_Key(), matFX);
                break;

            case 3:
                CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("Hit_Words4"), CSection_Manager::GetInstance()->Get_Cur_Section_Key(), matFX);
                break;

            case 4:
                CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("Hit_Words5"), CSection_Manager::GetInstance()->Get_Cur_Section_Key(), matFX);
                break;
            }
        }
    }
    else
    {
        __super::On_Hit(_pHitter, _iDamg, _vForce);
    }

    m_pGameInstance->Start_SFX(_wstring(L"A_sfx_sword_hit_zippi_") + to_wstring(rand() % 3), g_SFXVolume*0.5f);


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

    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_FSM"),
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
    CircleDesc.iColliderUse = (_uint)COLLIDER2D_USE::COLLIDER2D_BODY;
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
