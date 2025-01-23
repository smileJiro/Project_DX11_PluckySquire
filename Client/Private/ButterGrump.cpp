#include "stdafx.h"
#include "ButterGrump.h"
#include "GameInstance.h"
#include "FSM.h"
#include "ModelObject.h"
#include "Pooling_Manager.h"
#include "Projectile_BarfBug.h"
#include "Boss_HomingBall.h"
#include "Boss_EnergyBall.h"
#include "FSM_Boss.h"

CButterGrump::CButterGrump(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CMonster(_pDevice, _pContext)
{
}

CButterGrump::CButterGrump(const CButterGrump& _Prototype)
    : CMonster(_Prototype)
{
}

HRESULT CButterGrump::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CButterGrump::Initialize(void* _pArg)
{
    CButterGrump::MONSTER_DESC* pDesc = static_cast<CButterGrump::MONSTER_DESC*>(_pArg);
    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = false;
    pDesc->iNumPartObjects = PART_END;

    pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    pDesc->tTransform3DDesc.fSpeedPerSec = 3.f;

    pDesc->fDelayTime = 0.5f;
    pDesc->fCoolTime = 3.f;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    if (FAILED(Ready_Projectiles()))
        return E_FAIL;

    m_pBossFSM->Add_State((_uint)BOSS_STATE::SCENE);
    m_pBossFSM->Add_State((_uint)BOSS_STATE::HOMINGBALL);
    m_pBossFSM->Set_State((_uint)BOSS_STATE::SCENE);

    //static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(COORDINATE::COORDINATE_3D, IDLE, true);
    //static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_Animation(IDLE);

    //static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Register_OnAnimEndCallBack(bind(&CButterGrump::Alert_End, this, COORDINATE_3D, ALERT));
    //static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Register_OnAnimEndCallBack(bind(&CButterGrump::Attack_End, this, COORDINATE_3D, BARF));
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Register_OnAnimEndCallBack(bind(&CButterGrump::Intro_First_End, this, COORDINATE_3D, LB_INTRO_SH01));
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Register_OnAnimEndCallBack(bind(&CButterGrump::Intro_Second_End, this, COORDINATE_3D, LB_ENGAGE_IDLE_SH02));
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Register_OnAnimEndCallBack(bind(&CButterGrump::Intro_End, this, COORDINATE_3D, LB_INTRO_SH04));

    m_pControllerTransform->Rotation(XMConvertToRadians(180.f), XMVectorSet(0.f, 1.f, 0.f, 0.f));

    return S_OK;
}

void CButterGrump::Priority_Update(_float _fTimeDelta)
{
    if (true == m_isDelay)
    {
        m_fAccTime += _fTimeDelta;

        if (m_fDelayTime <= m_fAccTime)
        {
            Delay_Off();
            if (3 <= m_iAttackCount)
            {
                CoolTime_On();
            }
        }
    }

    if (true == m_isCool)
    {
        m_fAccTime += _fTimeDelta;

        if (m_fCoolTime <= m_fAccTime)
        {
            CoolTime_Off();
            m_iAttackCount = 0;
        }
    }

    __super::Priority_Update(_fTimeDelta); /* Part Object Priority_Update */
}

void CButterGrump::Update(_float _fTimeDelta)
{
    if ((KEY_DOWN(KEY::F3)))
    {
        static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(LB_INTRO_SH01);
    }

    m_pBossFSM->Update(_fTimeDelta);
    __super::Update(_fTimeDelta); /* Part Object Update */
}

void CButterGrump::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta); /* Part Object Late_Update */
}

HRESULT CButterGrump::Render()
{
    /* Model이 없는 Container Object 같은 경우 Debug 용으로 사용하거나, 폰트 렌더용으로. */

#ifdef _DEBUG

#endif // _DEBUG

    /* Font Render */

    return S_OK;
}

void CButterGrump::Change_Animation()
{
    if(m_iState != m_iPreState)
    {
        switch ((BOSS_STATE)m_iState)
        {
        case BOSS_STATE::SCENE:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(LB_INTRO_SH01);
            break;

        case BOSS_STATE::ENERGYBALL:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(FIREBALL_SPIT_SMALL);
            break;

        case BOSS_STATE::HOMINGBALL:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(FIREBALL_SPIT_SMALL);
            break;

        /*

        case BOSS_STATE::CHASE:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(WALK);
            break;

        case BOSS_STATE::ATTACK:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(BARF);
            break;*/

        default:
            break;
        }
    }
}

void CButterGrump::Attack(_float _fTimeDelta)
{
    if (false == m_isDelay && false == m_isCool)
    {
        _float3 vScale, vPosition;
        _float4 vRotation;
        if (false == m_pGameInstance->MatrixDecompose(&vScale, &vRotation, &vPosition, m_pControllerTransform->Get_WorldMatrix()))
            return;
        switch ((BOSS_STATE)m_iState)
        {
        case BOSS_STATE::ENERGYBALL:
            vPosition.y += vScale.y * 0.5f;
            vPosition.x += m_pGameInstance->Compute_Random(-15.f, 15.f);
            vPosition.y += m_pGameInstance->Compute_Random(-15.f, 15.f);
            vPosition.z += m_pGameInstance->Compute_Random(-15.f, 15.f);
            CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Boss_EnergyBall"), &vPosition, &vRotation);
            Delay_On();
            ++m_iAttackCount;
            break;
        case BOSS_STATE::HOMINGBALL:
        {
            _vector Rot = XMLoadFloat4(&vRotation);

            Rot = XMQuaternionMultiply(Rot, XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f)));
            XMStoreFloat4(&vRotation, Rot);
            vPosition.y += vScale.y * 0.5f;

            CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Boss_HomingBall"), &vPosition, &vRotation);
            Delay_On();
            ++m_iAttackCount;
            break;
        }
        default:
            break;
        }
    }
}

void CButterGrump::Alert_End(COORDINATE _eCoord, _uint iAnimIdx)
{
    Set_AnimChangeable(true);
}

void CButterGrump::Attack_End(COORDINATE _eCoord, _uint iAnimIdx)
{
    //딜레이 동안은 애니 전환 안됨. 따라서 상태 전환도 불가
    if (false == m_isDelay)
    {
        Set_AnimChangeable(true);
    }
}

void CButterGrump::Intro_First_End(COORDINATE _eCoord, _uint iAnimIdx)
{
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(LB_INTRO_SH04);
}

void CButterGrump::Intro_Second_End(COORDINATE _eCoord, _uint iAnimIdx)
{
    //static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(LB_INTRO_SH04);
}

void CButterGrump::Intro_End(COORDINATE _eCoord, _uint iAnimIdx)
{
    Event_ChangeBossState(BOSS_STATE::HOMINGBALL, m_pBossFSM);
}

void CButterGrump::Play_Intro()
{
}

HRESULT CButterGrump::Ready_Components()
{
    /* Com_FSM */
    CFSM_Boss::FSMBOSSDESC Desc;

    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_FSM_Boss"),
        TEXT("Com_FSM_Boss"), reinterpret_cast<CComponent**>(&m_pBossFSM), &Desc)))
        return E_FAIL;
    m_pBossFSM->Set_Owner(this);


    return S_OK;
}

HRESULT CButterGrump::Ready_PartObjects()
{
    /* Part Body */
    CModelObject::MODELOBJECT_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    BodyDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxAnimMesh");
    BodyDesc.strModelPrototypeTag_3D = TEXT("buttergrump_Rig");
	BodyDesc.iModelPrototypeLevelID_3D = LEVEL_GAMEPLAY;
    BodyDesc.iShaderPass_3D = (_uint)PASS_VTXANIMMESH::DEFAULT;

    BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

    BodyDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    BodyDesc.tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
    BodyDesc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    BodyDesc.tTransform3DDesc.fSpeedPerSec = 10.f;

    m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &BodyDesc));
    if (nullptr == m_PartObjects[PART_BODY])
        return E_FAIL;

    return S_OK;
}

HRESULT CButterGrump::Ready_Projectiles()
{
    Pooling_DESC Pooling_Desc;
    Pooling_Desc.iPrototypeLevelID = LEVEL_GAMEPLAY;
    Pooling_Desc.strLayerTag = TEXT("Layer_Monster");
    Pooling_Desc.strPrototypeTag = TEXT("Prototype_GameObject_Boss_HomingBall");

    CBoss_HomingBall::BOSS_HOMINGBALL_DESC* pHomingBallDesc = new CBoss_HomingBall::BOSS_HOMINGBALL_DESC;
    pHomingBallDesc->fLifeTime = 5.f;
    pHomingBallDesc->eStartCoord = COORDINATE_3D;
    pHomingBallDesc->isCoordChangeEnable = false;
    pHomingBallDesc->iNumPartObjects = PART_LAST;
    pHomingBallDesc->iCurLevelID = m_iCurLevelID;

    pHomingBallDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    pHomingBallDesc->tTransform3DDesc.fSpeedPerSec = 10.f;

    CPooling_Manager::GetInstance()->Register_PoolingObject(TEXT("Pooling_Boss_HomingBall"), Pooling_Desc, pHomingBallDesc);


    Pooling_Desc.strPrototypeTag = TEXT("Prototype_GameObject_Boss_EnergyBall");

    CBoss_EnergyBall::BOSS_ENERGYBALL_DESC* pEnergyBallDesc = new CBoss_EnergyBall::BOSS_ENERGYBALL_DESC;
    pEnergyBallDesc->fLifeTime = 5.f;
    pEnergyBallDesc->eStartCoord = COORDINATE_3D;
    pEnergyBallDesc->isCoordChangeEnable = false;
    pEnergyBallDesc->iNumPartObjects = PART_LAST;
    pEnergyBallDesc->iCurLevelID = m_iCurLevelID;

    pEnergyBallDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    pEnergyBallDesc->tTransform3DDesc.fSpeedPerSec = 10.f;

    CPooling_Manager::GetInstance()->Register_PoolingObject(TEXT("Pooling_Boss_EnergyBall"), Pooling_Desc, pEnergyBallDesc);

    return S_OK;
}

CButterGrump* CButterGrump::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CButterGrump* pInstance = new CButterGrump(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CButterGrump");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CButterGrump::Clone(void* _pArg)
{
    CButterGrump* pInstance = new CButterGrump(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CButterGrump");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CButterGrump::Free()
{
    Safe_Release(m_pBossFSM);
    
    __super::Free();
}
