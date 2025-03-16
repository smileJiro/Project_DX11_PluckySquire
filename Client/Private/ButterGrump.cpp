#include "stdafx.h"
#include "ButterGrump.h"
#include "GameInstance.h"
#include "FSM.h"
#include "ModelObject.h"
#include "Pooling_Manager.h"
#include "PlayerData_Manager.h"
#include "Projectile_BarfBug.h"
#include "Boss_HomingBall.h"
#include "Boss_EnergyBall.h"
#include "Boss_YellowBall.h"
#include "Boss_PurpleBall.h"
#include "Boss_WingSlam.h"
#include "Boss_WingSlice.h"
#include "Boss_Rock.h"
#include "FSM_Boss.h"
#include "ButterGrump_LeftEye.h"
#include "ButterGrump_RightEye.h"
#include "ButterGrump_Tongue.h"
#include "ButterGrump_Shield.h"
#include "Boss_Crystal.h"
#include "Boss_TennisBall.h"
#include "Effect_Manager.h"

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

    pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(360.f);
    pDesc->tTransform3DDesc.fSpeedPerSec = 30.f;

    pDesc->iNumPartObjects = BOSSPART_END;

    pDesc->iObjectGroupID = OBJECT_GROUP::BOSS;

    m_tStat.iHP = 500;
    m_tStat.iMaxHP = 500;
    m_tStat.iDamg = 1;

    m_fDelayTime = 0.5f;
    m_fCoolTime = 3.f;

    m_fMoveHPRatio_Phase1_1 = 0.7f;
    m_fMoveHPRatio_Phase1_2 = 0.3f;
    m_fMoveHPRatio_Phase2 = 0.4f;

    if (FAILED(Ready_ActorDesc(pDesc)))
        return E_FAIL;

    if (FAILED(CContainerObject::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    if (FAILED(Ready_Projectiles()))
        return E_FAIL;
    
    m_pBossFSM->Add_State((_uint)BOSS_STATE::SCENE);
    m_pBossFSM->Add_State((_uint)BOSS_STATE::TRANSITION);
    m_pBossFSM->Add_State((_uint)BOSS_STATE::IDLE);
    m_pBossFSM->Add_State((_uint)BOSS_STATE::MOVE);
    //m_pBossFSM->Add_State((_uint)BOSS_STATE::HOMINGBALL);
    //m_pBossFSM->Add_State((_uint)BOSS_STATE::YELLOWBALL);
    //m_pBossFSM->Add_State((_uint)BOSS_STATE::PURPLEBALL);
    //m_pBossFSM->Add_State((_uint)BOSS_STATE::WINGSLAM);
    //m_pBossFSM->Add_State((_uint)BOSS_STATE::ROCKVOLLEY);
    m_pBossFSM->Add_State((_uint)BOSS_STATE::WINGSLICE);
    m_pBossFSM->Add_State((_uint)BOSS_STATE::SHIELD);
    m_pBossFSM->Add_State((_uint)BOSS_STATE::HIT);
    m_pBossFSM->Add_State((_uint)BOSS_STATE::DEAD);

    m_pBossFSM->Set_State((_uint)BOSS_STATE::IDLE);

    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);

    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, IDLE, true);
    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, FIREBALL_SPIT_LOOP, true);
    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, WINGSHIELD_ROCK_VOLLEY_LOOP, true);
    pModelObject->Set_Animation(IDLE);
    
    pModelObject->Register_OnAnimEndCallBack(bind(&CButterGrump::Animation_End, this, placeholders::_1, placeholders::_2));

    Bind_AnimEventFunc("On_Attack", bind(&CButterGrump::On_Attack, this));
    Bind_AnimEventFunc("On_Move", bind(&CButterGrump::On_Move, this));

    /* Com_AnimEventGenerator */
    CAnimEventGenerator::ANIMEVTGENERATOR_DESC tAnimEventDesc{};
    tAnimEventDesc.pReceiver = this;
    tAnimEventDesc.pSenderModel = static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_3D);
    m_pAnimEventGenerator = static_cast<CAnimEventGenerator*> (m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, m_iCurLevelID, TEXT("Prototype_Component_BossAttackAnimEvent"), &tAnimEventDesc));
    Add_Component(TEXT("AnimEventGenerator"), m_pAnimEventGenerator);

    //m_pControllerTransform->Rotation(XMConvertToRadians(180.f), XMVectorSet(0.f, 1.f, 0.f, 0.f));

    /* Actor Desc 채울 때 쓴 데이터 할당해제 */

    for (_uint i = 0; i < pDesc->pActorDesc->ShapeDatas.size(); i++)
    {
        Safe_Delete(pDesc->pActorDesc->ShapeDatas[i].pShapeDesc);
    }
    Safe_Delete(pDesc->pActorDesc);

    static_cast<CActor_Dynamic*>(Get_ActorCom())->Set_Gravity(false);


    //m_PartObjects[BOSSPART_SHIELD]->Get_ControllerTransform()->RotationXYZ(_float3(0.f, -90.f, 0.f));
    m_PartObjects[BOSSPART_SHIELD]->Set_Active(false);

    Get_ControllerTransform()->Rotation(XMConvertToRadians(180.f), XMVectorSet(0.f, 1.f, 0.f, 0.f));


    m_fMoveAnimationProgress[DASH_DOWN] = 0.9f-0.285f;
    m_fMoveAnimationProgress[DASH_LEFT] = 0.76f-0.345f;
    m_fMoveAnimationProgress[DASH_RIGHT] = 0.76f-0.345f;
    m_fMoveAnimationProgress[DASH_UP] = 0.705f-0.343f;


    //플레이어 위치 가져오기
    m_pTarget = CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr();
    if (nullptr == m_pTarget)
    {
#ifdef _DEBUG
        cout << "BOSSINIT : NO PLAYER" << endl;
#endif // _DEBUG
        return S_OK;
    }

    Safe_AddRef(m_pTarget);

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
        }
    }

    __super::Priority_Update(_fTimeDelta); /* Part Object Priority_Update */
}

void CButterGrump::Update(_float _fTimeDelta)
{
#ifdef _DEBUG

    //if ((KEY_DOWN(KEY::NUMPAD7)))
    //{
    //    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(LB_INTRO_SH01);
    //    Set_AnimChangeable(true);
    //    m_pBossFSM->Change_State((_uint)BOSS_STATE::SCENE);
    //}
    //if ((KEY_DOWN(KEY::NUMPAD8)))
    //{
    //    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(EXPLOSION_INTO);
    //    Set_AnimChangeable(true);
    //    m_pBossFSM->Change_State((_uint)BOSS_STATE::SCENE);
    //}
    //if ((KEY_DOWN(KEY::NUMPAD9)))
    //{
    //    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(TRANSITION_PHASE2);
    //    Set_AnimChangeable(true);
    //    m_pBossFSM->Change_State((_uint)BOSS_STATE::SCENE);
    //}

    if (KEY_DOWN(KEY::F5))
    {
        m_isInvincible ^= 1;
        m_PartObjects[BOSSPART_SHIELD]->Set_Active(m_isInvincible);
        if (nullptr != m_pShieldEffect)
            m_pShieldEffect->Active_All(true);
    }

    if (KEY_PRESSING(KEY::CTRL))
    {
        if(KEY_DOWN(KEY::NUMPAD2))
        {
            Event_Hit(this, this, 30, XMVectorZero());
        }
    }

#endif // _DEBUG


   /* if (true == Get_PreAttack())
    {
        Get_ControllerTransform()->Set_AutoRotationYDirection(m_pTarget->Get_FinalPosition() - Get_FinalPosition());
        Get_ControllerTransform()->Update_AutoRotation(_fTimeDelta);
    }*/


    if (true == m_isAttack)
    {
        if(false == m_isDelay)
        {
            Attack();
        }
    }

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

        case BOSS_STATE::TRANSITION:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(TRANSITION_PHASE2);
            break;

        case BOSS_STATE::IDLE:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(IDLE);
            break;

        case BOSS_STATE::MOVE:
            switch (m_iCurMoveIndex)
            {
            case 0:
                static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(DASH_LEFT);
                break;

            case 1:
                static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(DASH_RIGHT);
                break;

            case 2:
                static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(DASH_LEFT);
                break;
            }
            break;

        case BOSS_STATE::ENERGYBALL:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(FIREBALL_SPIT_SMALL);
            break;

        case BOSS_STATE::HOMINGBALL:
            if (false == Is_Phase2())
            {
                static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(FIREBALL_SPIT_SMALL);
                if (nullptr != m_pHomingEffect)
                    m_pHomingEffect->Active_Effect(true);
            }
            else
            {
                static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(FIREBALL_SPIT_BIG);
                if (nullptr != m_pHomingEffect)
                    m_pHomingEffect->Active_Effect(true);
            }


            break;

        case BOSS_STATE::YELLOWBALL:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(FIREBALL_SPIT_SMALL);
            if (nullptr != m_pYellowEffect)
                m_pYellowEffect->Active_Effect(true);
            break;

        case BOSS_STATE::PURPLEBALL:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(WINGSHIELD_ROCK_VOLLEY_INTO);
            if (nullptr != m_pPurpleEffect)
                m_pPurpleEffect->Active_Effect(true);
            break;

        case BOSS_STATE::WINGSLAM:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(WING_SLAM_INTO);
            break;

        case BOSS_STATE::ROCKVOLLEY:
            if(false == Is_Phase2())
                static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(FIREBALL_SPIT_BIG);
            else
                static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(WINGSHIELD_ROCK_VOLLEY_INTO);
            break;

        case BOSS_STATE::WINGSLICE:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(WING_SLICE_INTO);
            break;

        case BOSS_STATE::SHIELD:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(ROAR);
            if (nullptr != m_pShieldEffect)
                m_pShieldEffect->Active_All(true);
            break;

        case BOSS_STATE::HIT:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(RECEIVE_DAMAGE);
            break;

        case BOSS_STATE::DEAD:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(EXPLOSION_INTO);
            break;

        default:
            break;
        }
    }
}

void CButterGrump::Attack()
{
    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);

    _float3 vScale, vPosition;
    _float4 vRotation;
    _matrix matMuzzle = XMLoadFloat4x4(static_cast<C3DModel*>(static_cast<CModelObject*>(m_PartObjects[BOSSPART_BODY])->Get_Model(COORDINATE_3D))->Get_BoneMatrix("buttergrump_rigtonsils_01_03")) * Get_FinalWorldMatrix();
    //_matrix matMuzzle = XMLoadFloat4x4(static_cast<C3DModel*>(static_cast<CModelObject*>(m_PartObjects[BOSSPART_BODY])->Get_Model(COORDINATE_3D))->Get_BoneMatrix("buttergrump_righead_tongue_10")) * Get_FinalWorldMatrix();
    //if (false == m_pGameInstance->MatrixDecompose(nullptr, nullptr, &vPosition, matMuzzle))

    _vector vUp = XMVector3Cross(XMVector3Normalize(Get_ControllerTransform()->Get_State(CTransform::STATE_LOOK)), XMVector3Normalize(Get_ControllerTransform()->Get_State(CTransform::STATE_RIGHT)));

    //목젖 기준이므로 룩 벡터만큼 앞으로 공격위치 조정함(투사체 점진적으로 크게 만들거면 수정)
	XMStoreFloat3(&vPosition, matMuzzle.r[3] + XMVector3Normalize(Get_ControllerTransform()->Get_State(CTransform::STATE_LOOK)) * 7.f - XMVector3Normalize(vUp) * 3.f);
    if (false == m_pGameInstance->MatrixDecompose(&vScale, &vRotation, nullptr, Get_FinalWorldMatrix()))
        return;


    vUp = {0.f,1.f,0.f,0.f};
    _vector vLook = XMVector3Normalize(m_pTarget->Get_FinalPosition() - XMLoadFloat3(&vPosition));
    _vector vRight = XMVector3Cross(vUp, vLook);

    vUp = XMVector3Cross(vLook, vRight);

    _matrix matRot = XMMatrixIdentity();

    matRot.r[0] = vRight;
    matRot.r[1] = vUp;
    matRot.r[2] = vLook;

    
    m_pGameInstance->MatrixDecompose(nullptr, &vRotation, nullptr, matRot);
    //XMStoreFloat4(&vRotation, m_pGameInstance->Direction_To_Quaternion(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMVector4Normalize(m_pTarget->Get_FinalPosition() - XMLoadFloat3(&vPosition))));

    switch ((BOSS_STATE)m_iState)
    {
    //case BOSS_STATE::ENERGYBALL:
    //{
    //    //vPosition.y += vScale.y * 0.5f;
    //    CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Boss_EnergyBall"), COORDINATE_3D,  &vPosition, &vRotation);
    //    m_isAttack = false;
    //    break;
    //}

    case BOSS_STATE::HOMINGBALL:
    {
        //_vector Rot = XMLoadFloat4(&vRotation);

        //Rot = XMQuaternionMultiply(Rot, XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f)));
        //XMStoreFloat4(&vRotation, Rot);
        //vPosition.y += vScale.y * 0.5f;

        _float fVerticalAngle=0.f;
        _float fHorizontalAngle=0.f;

        _vector vLook = { 0.f,0.f,1.f };
        //쿼터니언을 통해 룩벡터 구함
        vLook = XMVector3Rotate(vLook, XMLoadFloat4(&vRotation));
        //업벡터와 외적으로 수직 벡터 구하고 회전축으로 씀
        _vector vRight = XMVector3Normalize(XMVector3Cross(vLook, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
        _vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

        //vScale = { 0.2f, 0.2f, 1.f };
        vScale = { 2.f, 2.f, 5.f };

        if(false == Is_Phase2())
        {
            switch ((_uint)ceil(m_pGameInstance->Compute_Random(0.f, 3.f)))
            {
            case 1:
                fVerticalAngle = XMConvertToRadians(30.f);
                fHorizontalAngle = XMConvertToRadians(0.f);
                break;

            case 2:
                fVerticalAngle = XMConvertToRadians(0.f);
                fHorizontalAngle = XMConvertToRadians(30.f);
                break;

            case 3:
                fVerticalAngle = XMConvertToRadians(0.f);
                fHorizontalAngle = XMConvertToRadians(-30.f);
                break;
            }

            _vector vRot = XMQuaternionMultiply(XMQuaternionRotationAxis(vRight, fVerticalAngle), XMQuaternionRotationAxis(vUp, fHorizontalAngle));
            _float4 vQuat; XMStoreFloat4(&vQuat, XMQuaternionMultiply(vRot, XMLoadFloat4(&vRotation)));

            CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Boss_HomingBall"), COORDINATE_3D, &vPosition, &vQuat, &vScale);
        }
        else if (true == Is_Phase2())
        {
            _float Vertical_Angle_Array[5] = {30.f, 15.f, 15.f, -15.f, -15.f};
            _float Horizontal_Angle_Array[5] = {0.f, 20.f, -20.f, 15.f, -15.f };
            for (_uint i = 0; i < 5; i++)
            {
                fVerticalAngle = XMConvertToRadians(Vertical_Angle_Array[i]);
                fHorizontalAngle = XMConvertToRadians(Horizontal_Angle_Array[i]);

                _vector vRot = XMQuaternionMultiply(XMQuaternionRotationAxis(vRight, fVerticalAngle), XMQuaternionRotationAxis(vUp, fHorizontalAngle));
                _float4 vQuat; XMStoreFloat4(&vQuat, XMQuaternionMultiply(vRot, XMLoadFloat4(&vRotation)));

                CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Boss_HomingBall"), COORDINATE_3D, &vPosition, &vQuat, &vScale);
            }
        }

        ++m_iAttackCount;

        if (m_iNumAttack <= m_iAttackCount)
        {
            m_iAttackCount = 0;
            m_isAttack = false;

            if (true == m_isAttackChained)
            {
                ++m_iAttackChainCount;
                if (m_iNumAttackChain <= m_iAttackChainCount)
                {
                    m_iAttackChainCount = 0;
                    m_isAttackChained = false;
                }
            }
        }
        else
        {
            Delay_On();
        }
        break;
    }

    case BOSS_STATE::YELLOWBALL:
    {
        _vector Rot;

        //vPosition.y += vScale.y * 0.5f;

        _float fAngle = 15.f;
        _float Array[8] = { 1,0,-1,0,1,1,-1,-1 };
        _float Array2[8] = { 0,1,0,-1,1,-1,1,-1 };
        for (_uint i = 0; i < 8; i++)
        {
            _float4 vRot;
            _float3 vPos=vPosition;
            Rot = XMQuaternionMultiply(XMLoadFloat4(&vRotation), XMQuaternionRotationAxis(XMVectorSet(Array[i], Array2[i], 0.f, 0.f), XMConvertToRadians(fAngle)));
            XMStoreFloat4(&vRot, Rot);
            /*vPos.x += 2.f * Array[i];
            vPos.y += 2.f * Array2[i];*/
            CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Boss_YellowBall"), COORDINATE_3D, &vPos, &vRot);
        }

        CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Boss_YellowBall"), COORDINATE_3D, &vPosition, &vRotation);

        ++m_iAttackCount;

        if (m_iNumAttack <= m_iAttackCount)
        {
            m_iAttackCount = 0;
            m_isAttack = false;

            if (true == m_isAttackChained)
            {
                ++m_iAttackChainCount;
                if (m_iNumAttackChain <= m_iAttackChainCount)
                {
                    m_iAttackChainCount = 0;
                    m_isAttackChained = false;
                }
            }
        }
        else
        {
            Delay_On();
        }
        break;
    }

    case BOSS_STATE::PURPLEBALL:
    {
        _float fVerticalAngle = 0.f;
        _float fHorizontalAngle = 0.f;

        _vector vLook = { 0.f,0.f,1.f };
        //쿼터니언을 통해 룩벡터 구함
        vLook = XMVector3Rotate(vLook, XMLoadFloat4(&vRotation));
        //업벡터와 외적으로 수직 벡터 구하고 회전축으로 씀
        _vector vRight = XMVector3Normalize(XMVector3Cross(vLook, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
        _vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

        //vPosition.y += vScale.y * 0.5f;

        //_float Array[8] = { 1,0,-1,0,1,1,-1,-1 };
        //_float Array2[8] = { 0,1,0,-1,1,-1,1,-1 };
        //for (_uint i = 0; i < 8; i++)
        //{
        //    _float4 vRot;
        //    _float3 vPos = vPosition;
        //    Rot = XMQuaternionMultiply(XMLoadFloat4(&vRotation), XMQuaternionRotationAxis(XMVectorSet(Array[i], Array2[i], 0.f, 0.f), XMConvertToRadians(10.f)));
        //    XMStoreFloat4(&vRot, Rot);
        //    /*vPos.x += 2.f * Array[i];
        //    vPos.y += 2.f * Array2[i];*/
        //    CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Boss_PurpleBall"),COORDINATE_3D ,&vPos, &vRot);
        //}

        _float Vertical_Angle_Array[3] = { 5.f, -5.f, -5.f, };
        _float Horizontal_Angle_Array[3] = { 0.f, 5.f, -5.f };
        _float fRadius = 3.f;
        _float fAngleOffset = 20.f;
        for (_uint i = 0; i < 3; i++)
        {
            fVerticalAngle = XMConvertToRadians(Vertical_Angle_Array[i]);
            fHorizontalAngle = XMConvertToRadians(Horizontal_Angle_Array[i]);

            //x, y 위치만 조정
            _float3 fOffSet = { cos(XMConvertToRadians(120.f * (_float)i + fAngleOffset * m_iAttackCount)), sin(XMConvertToRadians(120.f * (_float)i + fAngleOffset * m_iAttackCount)), 0.f };
			_float3 vPos; XMStoreFloat3(&vPos, XMLoadFloat3(&vPosition) + fRadius * XMVector3Rotate(XMLoadFloat3(&fOffSet), XMLoadFloat4(&vRotation)));

            //설정한 수평,수직 각도만큼 회전
            _vector vRot = XMQuaternionMultiply(XMQuaternionRotationAxis(vRight, fVerticalAngle), XMQuaternionRotationAxis(vUp, fHorizontalAngle));
            _float4 vQuat; XMStoreFloat4(&vQuat, XMQuaternionMultiply(vRot, XMLoadFloat4(&vRotation)));

            CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Boss_PurpleBall"), COORDINATE_3D, &vPos, &vRotation);
        }

        ++m_iAttackCount;

        if (m_iNumAttack <= m_iAttackCount)
        {
            pModelObject->Switch_Animation(WINGSHIELD_ROCK_VOLLEY_OUT);
            m_iAttackCount = 0;
            m_isAttack = false;
        }
        else
        {
            Delay_On();
        }
        break;
    }

    case BOSS_STATE::WINGSLAM:
    {
        /*_float4 vRot;
        XMStoreFloat4(&vRot, m_pGameInstance->Direction_To_Quaternion(XMVectorSet(0.f, 0.f, 1.f, 0.f), m_pTarget->Get_FinalPosition() - XMLoadFloat3(&vPosition)));*/
        CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Boss_WingSlam"), COORDINATE_3D, &vPosition, &vRotation);
        m_isAttack = false;
        break;
    }

    case BOSS_STATE::WINGSLICE:
    {
  //      _float4 vRot;
		//XMStoreFloat4(&vRot, m_pGameInstance->Direction_To_Quaternion(XMVectorSet(0.f, 0.f, 1.f, 0.f), m_pTarget->Get_FinalPosition() - XMLoadFloat3(&vPosition)));
        CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Boss_WingSlice"), COORDINATE_3D, &vPosition, &vRotation);
        m_isAttack = false;
        break;
    }

    case BOSS_STATE::ROCKVOLLEY:
    {
        _vector vLook = { 0.f,0.f,1.f };
        //쿼터니언을 통해 룩벡터 구함
        vLook = XMVector3Rotate(vLook, XMLoadFloat4(&vRotation));
        //업벡터와 외적으로 수직 벡터 구하고 회전축으로 씀
        _vector vRight = XMVector3Normalize(XMVector3Cross(vLook, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
        _vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));


        if (false == Is_Phase2())
        {
            for (_int i = 0; i < 10; ++i)
            {
                _float fVerticalAngle = XMConvertToRadians(m_pGameInstance->Compute_Random(-20.f, 20.f));
                _float fHorizontalAngle = XMConvertToRadians(m_pGameInstance->Compute_Random(-20.f, 20.f));

                _vector vRot = XMQuaternionMultiply(XMQuaternionRotationAxis(vRight, fVerticalAngle), XMQuaternionRotationAxis(vUp, fHorizontalAngle));
                _float4 vQuat; XMStoreFloat4(&vQuat, XMQuaternionMultiply(vRot, XMLoadFloat4(&vRotation)));

                CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Boss_Rock"), COORDINATE_3D, &vPosition, &vQuat);
            }
        }

        else if(true == Is_Phase2())
        {
            for (_int i = 0; i < 3; ++i)
            {
                _float fVerticalAngle = XMConvertToRadians(m_pGameInstance->Compute_Random(-15.f, 15.f));
                _float fHorizontalAngle = XMConvertToRadians(m_pGameInstance->Compute_Random(-15.f, 15.f));
                //_vector vVerticalRot = XMQuaternionRotationAxis(vRight, fVerticalAngle);
                //_vector vHorizontalRot = XMQuaternionRotationAxis(vUp, fHorizontalAngle);
                _vector vRot = XMQuaternionMultiply(XMQuaternionRotationAxis(vRight, fVerticalAngle), XMQuaternionRotationAxis(vUp, fHorizontalAngle));
                _float4 vQuat; XMStoreFloat4(&vQuat, XMQuaternionMultiply(vRot, XMLoadFloat4(&vRotation)));

                //_float3 vPos; XMStoreFloat3(&vPos, XMLoadFloat3(&vPosition));
                //vPos.x += m_pGameInstance->Compute_Random(-5.f, 5.f);
                //vPos.y += m_pGameInstance->Compute_Random(-5.f, 5.f);
                CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Boss_Rock"), COORDINATE_3D, &vPosition, &vQuat);
            }
        }

        ++m_iAttackCount;

        if (m_iNumAttack <= m_iAttackCount)
        {
            m_iAttackCount = 0;
            m_isAttack = false;

            if (true == Is_Phase2())
                pModelObject->Switch_Animation(WINGSHIELD_ROCK_VOLLEY_OUT);
        }
        else
        {
            Delay_On();
        }

        break;
    }

    default:
        break;
    }

    if(true == m_PartObjects[BOSSPART_SHIELD]->Is_Active() && false == m_isSpawnOrb)
    {
		if ((_uint)BOSS_STATE::WINGSLAM != m_iState)
        {
            // 1/3 확률로 쉴드파괴용 크리스탈 생성
            if (2 == (_uint)ceil(m_pGameInstance->Compute_Random(0.f, 3.f)))
            {
                CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Boss_Crystal"), COORDINATE_3D, &vPosition, &vRotation);
                m_isSpawnOrb = true;
            }
        }
    }
}

void CButterGrump::On_Attack()
{
    m_isAttack = true;

    switch ((BOSS_STATE)m_iState)
    {
    //case BOSS_STATE::ENERGYBALL:
    //    m_fDelayTime = 0.3f;
    //    m_iNumAttack = 10;
    //    break;

    case BOSS_STATE::HOMINGBALL:
        m_fDelayTime = 0.5f;
        if (false == Is_Phase2())
        {
            if (false == Is_Enforce())
                m_iNumAttack = 1;
            else
                m_iNumAttack = 3;
        }
        else
        {
            m_iNumAttack = 1;
            m_isAttackChained = true;
            m_iNumAttackChain = 3;
            m_fChainDelayTime = 0.5f;
        }
        break;

    case BOSS_STATE::YELLOWBALL:
        m_fDelayTime = 0.5f;
        m_iNumAttack = 1;
        
        if (true == Is_Enforce() || true == Is_Phase2())
        {
            m_isAttackChained = true;
            m_iNumAttackChain = 3;
            m_fChainDelayTime = 1.2f;
        }
        break;

    case BOSS_STATE::PURPLEBALL:
		if (false == Is_Phase2())
        {
            m_fDelayTime = 0.2f;
            m_iNumAttack = 50;
        }
        else
        {
            m_fDelayTime = 0.2f;
            m_iNumAttack = 50;
        }
        break;

    case BOSS_STATE::WINGSLAM:
        m_iNumAttack = 1;
        break;

    case BOSS_STATE::ROCKVOLLEY:
        if (false == Is_Phase2())
        {
            m_fDelayTime = 0.2f;
            m_iNumAttack = 2;
        }
        else
        {
            m_fDelayTime = 0.3f;
            m_iNumAttack = 30;
        }
        break;

    case BOSS_STATE::WINGSLICE:
        m_iNumAttack = 1;
        break;

    case BOSS_STATE::SHIELD:
    {
        if (false == m_PartObjects[BOSSPART_SHIELD]->Is_Active())
            m_PartObjects[BOSSPART_SHIELD]->Set_Active(true);

        m_isAttack = false;
    }
    break;

    default:
        break;
    }
}

void CButterGrump::On_Move()
{
    m_isOnMove = true;
}

void CButterGrump::Shield_Break()
{
    //쉴드 끄고 애니메이션 전환
    Event_SetActive(m_PartObjects[BOSSPART_SHIELD], false);
    Hit();
    Event_ChangeBossState(BOSS_STATE::HIT, m_pBossFSM);

    if (nullptr != m_pShieldEffect)
        m_pShieldEffect->Stop_SpawnAll(1.f);
}

void CButterGrump::Activate_Invinciblility(_bool _isActivate)
{
    m_isInvincible = _isActivate;
    m_PartObjects[BOSSPART_LEFTEYE]->Set_Active(_isActivate);
    m_PartObjects[BOSSPART_RIGHTEYE]->Set_Active(_isActivate);
    m_PartObjects[BOSSPART_TONGUE]->Set_Active(_isActivate);
}

void CButterGrump::Animation_End(COORDINATE _eCoord, _uint iAnimIdx)
{
    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);
    //딜레이 동안은 애니 전환 안됨. 따라서 상태 전환도 불가
    switch ((CButterGrump::Animation)pModelObject->Get_Model(COORDINATE_3D)->Get_CurrentAnimIndex())
    {
    case LB_INTRO_SH01 :
        pModelObject->Switch_Animation(LB_INTRO_SH04);
        break;

    case LB_INTRO_SH04 :
        Set_AnimChangeable(true);
        break;

    case EXPLOSION_INTO:
        if((_uint)BOSS_STATE::DEAD == m_iState)
            pModelObject->Switch_Animation(EXPLOSION_FALL);
        else
            pModelObject->Switch_Animation(EXPLOSION_OUT);
        break;

    case EXPLOSION_OUT:
        Set_AnimChangeable(true);
        break;

    case TRANSITION_PHASE2:
        Set_AnimChangeable(true);
        break;

    case EXPLOSION_FALL:
        break;

    case DASH_DOWN:
    case DASH_LEFT:
    case DASH_RIGHT:
    case DASH_UP:
        m_isMove = false;
        Set_AnimChangeable(true);
        break;

    case FIREBALL_SPIT_SMALL:
        Set_AnimChangeable(true);
        break;

    case FIREBALL_SPIT_BIG:
        Set_AnimChangeable(true);
        break;

    case WING_SLAM_INTO:
        pModelObject->Switch_Animation(WING_SLAM_OUT);
        break;

    case WING_SLAM_OUT:
        Set_AnimChangeable(true);
        break;

    case WING_SLICE_INTO:
        pModelObject->Switch_Animation(WING_SLICE_OUT);
        break;

    case WING_SLICE_OUT:
        Set_AnimChangeable(true);
        break;

    case WINGSHIELD_ROCK_VOLLEY_INTO:
        pModelObject->Switch_Animation(WINGSHIELD_ROCK_VOLLEY_LOOP);
        On_Attack();
        break;

    case WINGSHIELD_ROCK_VOLLEY_OUT:
        Set_AnimChangeable(true);
        break;

    case ROAR:
        Set_AnimChangeable(true);
        if (true == Is_Converse())
            m_isConverse = false;
        break;

    case RECEIVE_DAMAGE:
        Set_AnimChangeable(true);
        break;

    default:
        break;
    }
}

//void CButterGrump::Intro_First_End(COORDINATE _eCoord, _uint iAnimIdx)
//{
//    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(LB_INTRO_SH04);
//}
//
//void CButterGrump::Intro_Second_End(COORDINATE _eCoord, _uint iAnimIdx)
//{
//    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(LB_INTRO_SH04);
//}

void CButterGrump::Play_Intro()
{
}

void CButterGrump::On_Hit(CGameObject* _pHitter, _int _iDamg, _fvector _vForce)
{
    if ((_uint)BOSS_STATE::DEAD == m_iState)
        return;

    m_tStat.iHP -= _iDamg;
    if (m_tStat.iHP < 0)
    {
        m_tStat.iHP = 0;
    }
    cout << "BOSS_HP : " << m_tStat.iHP << endl;
    if (0 >= m_tStat.iHP)
    {
        if(false == Is_Phase2())
        {
            m_tStat.iHP = m_tStat.iMaxHP;
            m_isPhase2 = true;
            m_isConverse = false;
            Hit();
            Event_ChangeBossState(BOSS_STATE::TRANSITION, m_pBossFSM);
        }
        else
        {
            Hit();
            Event_ChangeBossState(BOSS_STATE::DEAD, m_pBossFSM);
        }
    }
    else if (m_tStat.iMaxHP * 0.5 >= m_tStat.iHP)
    {
        if(false == Is_Enforce())
            m_isEnforce = true;

        if(false == Is_Phase2())
        {
            if (0 == m_iNumConverse)
            {
                ++m_iNumConverse;
                m_isConverse = true;
            }
        }
        else if (true == Is_Phase2())
        {
            if (1 == m_iNumConverse)
            {
                ++m_iNumConverse;
                m_isConverse = true;
            }
        }
    }
    
    //일정 체력이하일 때 한번 씩 움직임
    if (false == Is_Phase2())
    {
        if (m_tStat.iMaxHP * m_fMoveHPRatio_Phase1_2 >= m_tStat.iHP)
        {
            if(true == m_isAlreadyMove)
            {
                m_isMove = true;
                m_isAlreadyMove = false;
            }
        }

        else if (m_tStat.iMaxHP * m_fMoveHPRatio_Phase1_1 >= m_tStat.iHP)
        {
            if(false == m_isAlreadyMove)
            {
                m_isMove = true;
                m_isAlreadyMove = true;
            }
        }
    }
    else if (true == Is_Phase2())
    {
        if (m_tStat.iMaxHP * 0.4 >= m_tStat.iHP)
        {
            m_isMove = true;
        }
    }
}

void CButterGrump::Hit()
{
    m_isAttack = false;
    m_isAttackChained = false;
    m_isSpawnOrb = false;
    Set_AnimChangeable(true);
}

HRESULT CButterGrump::Ready_ActorDesc(void* _pArg)
{
    CButterGrump::MONSTER_DESC* pDesc = static_cast<CButterGrump::MONSTER_DESC*>(_pArg);

    pDesc->eActorType = ACTOR_TYPE::KINEMATIC;
    CActor::ACTOR_DESC* ActorDesc = new CActor::ACTOR_DESC;

    /* Actor의 주인 오브젝트 포인터 */
    ActorDesc->pOwner = this;

    /* Actor의 회전축을 고정하는 파라미터 */
    ActorDesc->FreezeRotation_XYZ[0] = true;
    ActorDesc->FreezeRotation_XYZ[1] = false;
    ActorDesc->FreezeRotation_XYZ[2] = true;

    /* Actor의 이동축을 고정하는 파라미터 (이걸 고정하면 중력도 영향을 받지 않음. 아예 해당 축으로의 이동을 제한하는)*/
    ActorDesc->FreezePosition_XYZ[0] = false;
    ActorDesc->FreezePosition_XYZ[1] = false;
    ActorDesc->FreezePosition_XYZ[2] = false;


    /* 사용하려는 Shape의 형태를 정의 */
    SHAPE_CAPSULE_DESC* ShapeDesc = new SHAPE_CAPSULE_DESC;
    ShapeDesc->fRadius = 0.1f;
    ShapeDesc->fHalfHeight = 0.1f;

    SHAPE_DATA* ShapeData = new SHAPE_DATA;
    ShapeData->pShapeDesc = ShapeDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    ShapeData->eShapeType = SHAPE_TYPE::CAPSULE;     // Shape의 형태.
    ShapeData->eMaterial = ACTOR_MATERIAL::CHARACTER_FOOT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    ShapeData->isTrigger = false;                    // Trigger 알림을 받기위한 용도라면 true
    ShapeData->iShapeUse = (_uint)SHAPE_USE::SHAPE_BODY;
    XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixTranslation(0.0f, ShapeDesc->fHalfHeight + ShapeDesc->fRadius, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.

    /* 최종으로 결정 된 ShapeData를 PushBack */
    ActorDesc->ShapeDatas.push_back(*ShapeData);


    ///* 사용하려는 Shape의 형태를 정의 */
    //SHAPE_SPHERE_DESC* ShieldShapeDesc = new SHAPE_SPHERE_DESC;
    //ShieldShapeDesc->fRadius = 15.f;

    //ShapeData->pShapeDesc = ShieldShapeDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    //ShapeData->eShapeType = SHAPE_TYPE::SPHERE;     // Shape의 형태.
    //ShapeData->eMaterial = ACTOR_MATERIAL::CHARACTER_FOOT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    //ShapeData->isTrigger = false;                    // Trigger 알림을 받기위한 용도라면 true
    //ShapeData->iShapeUse = (_uint)SHAPE_USE::SHAPE_TRIGER;
    //XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixTranslation(0.0f, ShapeDesc->fRadius, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.

    ///* 최종으로 결정 된 ShapeData를 PushBack */
    //ActorDesc->ShapeDatas.push_back(*ShapeData);


       /* 충돌 필터에 대한 세팅 ()*/
    ActorDesc->tFilterData.MyGroup = OBJECT_GROUP::BOSS;
    ActorDesc->tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::PLAYER | OBJECT_GROUP::PLAYER_PROJECTILE | OBJECT_GROUP::EXPLOSION;

    /* Actor Component Finished */
    pDesc->pActorDesc = ActorDesc;

    /* Shapedata 할당해제 */
    Safe_Delete(ShapeData);

    return S_OK;
}

HRESULT CButterGrump::Ready_Components()
{
    /* Com_FSM */
    CFSM_Boss::FSMBOSSDESC FSMDesc;
    FSMDesc.pOwner = this;
    FSMDesc.iCurLevel = m_iCurLevelID;

    if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_FSM_Boss"),
        TEXT("Com_FSM_Boss"), reinterpret_cast<CComponent**>(&m_pBossFSM), &FSMDesc)))
        return E_FAIL;


    return S_OK;
}

HRESULT CButterGrump::Ready_PartObjects()
{
    /* Part Body */
    CModelObject::MODELOBJECT_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    BodyDesc.strModelPrototypeTag_3D = TEXT("Prototype_Model_ButterGrump");
	BodyDesc.iModelPrototypeLevelID_3D = m_iCurLevelID;

    BodyDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxAnimMesh");
    BodyDesc.iShaderPass_3D = (_uint)PASS_VTXANIMMESH::DEFAULT;
    BodyDesc.iRenderGroupID_3D = RG_3D;
    BodyDesc.iPriorityID_3D = PR3D_GEOMETRY;

    BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

    BodyDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    BodyDesc.tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
    BodyDesc.tTransform3DDesc.fRotationPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_RotationPerSec();
    BodyDesc.tTransform3DDesc.fSpeedPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_SpeedPerSec();

    m_PartObjects[BOSSPART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &BodyDesc));
    if (nullptr == m_PartObjects[BOSSPART_BODY])
        return E_FAIL;


    /* Parts */

    CButterGrump_LeftEye::BUTTERGRUMP_LEFTEYE_DESC LeftEyeDesc{};

    LeftEyeDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

    LeftEyeDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    LeftEyeDesc.tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
    LeftEyeDesc.tTransform3DDesc.fRotationPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_RotationPerSec();
    LeftEyeDesc.tTransform3DDesc.fSpeedPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_SpeedPerSec();

    LeftEyeDesc.pParent = this;

    m_PartObjects[BOSSPART_LEFTEYE] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, m_iCurLevelID, TEXT("Prototype_GameObject_ButterGrump_LeftEye"), &LeftEyeDesc));
    if (nullptr == m_PartObjects[BOSSPART_LEFTEYE])
        return E_FAIL;

    C3DModel* p3DModel = static_cast<C3DModel*>(static_cast<CModelObject*>(m_PartObjects[BOSSPART_BODY])->Get_Model(COORDINATE_3D));
    m_PartObjects[BOSSPART_LEFTEYE]->Set_SocketMatrix(COORDINATE_3D, p3DModel->Get_BoneMatrix("buttergrump_righead_left_eye"));


    CButterGrump_RightEye::BUTTERGRUMP_RIGHTEYE_DESC RightEyeDesc{};

    RightEyeDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

    RightEyeDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    RightEyeDesc.tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
    RightEyeDesc.tTransform3DDesc.fRotationPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_RotationPerSec();
    RightEyeDesc.tTransform3DDesc.fSpeedPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_SpeedPerSec();

    RightEyeDesc.pParent = this;

    m_PartObjects[BOSSPART_RIGHTEYE] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, m_iCurLevelID, TEXT("Prototype_GameObject_ButterGrump_RightEye"), &RightEyeDesc));
    if (nullptr == m_PartObjects[BOSSPART_RIGHTEYE])
        return E_FAIL;

    m_PartObjects[BOSSPART_RIGHTEYE]->Set_SocketMatrix(COORDINATE_3D, p3DModel->Get_BoneMatrix("buttergrump_righead_right_eye"));


    CButterGrump_Tongue::BUTTERGRUMP_TONGUE_DESC TongueDesc{};

    TongueDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

    TongueDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    TongueDesc.tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
    TongueDesc.tTransform3DDesc.fRotationPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_RotationPerSec();
    TongueDesc.tTransform3DDesc.fSpeedPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_SpeedPerSec();

    TongueDesc.pParent = this;

    m_PartObjects[BOSSPART_TONGUE] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, m_iCurLevelID, TEXT("Prototype_GameObject_ButterGrump_Tongue"), &TongueDesc));
    if (nullptr == m_PartObjects[BOSSPART_TONGUE])
        return E_FAIL;

    m_PartObjects[BOSSPART_TONGUE]->Set_SocketMatrix(COORDINATE_3D, p3DModel->Get_BoneMatrix("buttergrump_righead_tongue_01"));


    CButterGrump_Shield::BUTTERGRUMP_SHIELD_DESC ShieldDesc{};

    ShieldDesc.strModelPrototypeTag_3D = TEXT("S_FX_CMN_Sphere_03");
    ShieldDesc.iModelPrototypeLevelID_3D = m_iCurLevelID;
    ShieldDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

    ShieldDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
    ShieldDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::NOISEFRESNEL;
    ShieldDesc.iCurLevelID = m_iCurLevelID;
    _float fScale = 30.f;

	//ShieldDesc.tTransform3DDesc.vInitialPosition = _float3(fScale * (0.3f), 0.0f, fScale * (-0.3f));
	ShieldDesc.tTransform3DDesc.vInitialPosition = _float3(0.f, 0.0f, fScale * (0.4f));
	//ShieldDesc.tTransform3DDesc.vInitialPosition = _float3(0.f, 0.f, 0.f);
    ShieldDesc.tTransform3DDesc.vInitialScaling = _float3(fScale, fScale, fScale);
    ShieldDesc.tTransform3DDesc.fRotationPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_RotationPerSec();
    ShieldDesc.tTransform3DDesc.fSpeedPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_SpeedPerSec();

    ShieldDesc.iRenderGroupID_3D = RG_3D;
    ShieldDesc.iPriorityID_3D = PR3D_PARTICLE;

    ShieldDesc.pParent = this;

    m_PartObjects[BOSSPART_SHIELD] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, m_iCurLevelID, TEXT("Prototype_GameObject_ButterGrump_Shield"), &ShieldDesc));
    if (nullptr == m_PartObjects[BOSSPART_SHIELD])
        return E_FAIL;

    m_PartObjects[BOSSPART_SHIELD]->Set_SocketMatrix(COORDINATE_3D, p3DModel->Get_BoneMatrix("buttergrump_righead_nose"));


    /* Effects */
    CEffect_System::EFFECT_SYSTEM_DESC EffectDesc = {};
    EffectDesc.eStartCoord = COORDINATE_3D;
    EffectDesc.isCoordChangeEnable = false;
    EffectDesc.iSpriteShaderLevel = LEVEL_STATIC;
    EffectDesc.szSpriteShaderTags = L"Prototype_Component_Shader_VtxPointInstance";
    EffectDesc.iEffectShaderLevel = LEVEL_STATIC;
    EffectDesc.szEffectShaderTags = L"Prototype_Component_Shader_VtxMeshEffect";
    EffectDesc.szSpriteComputeShaderTag = L"Prototype_Component_Compute_Shader_SpriteInstance";
    
    m_PartObjects[BOSSPART_HOMING_EFFECT] = m_pHomingEffect =
        static_cast<CEffect_System*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, m_iCurLevelID, TEXT("HomingBallOut.json"), &EffectDesc));

    if (nullptr != m_pHomingEffect)
    {
        m_pHomingEffect->Set_Active(false);
        m_pHomingEffect->Set_ParentMatrix(COORDINATE_3D, m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D));
        m_pHomingEffect->Set_SpawnMatrix(p3DModel->Get_BoneMatrix("buttergrump_righead_jaw_end"));
    }

    Safe_AddRef(m_pHomingEffect);


    m_PartObjects[BOSSPART_PURPLEBALL_EFFECT] = m_pPurpleEffect =
        static_cast<CEffect_System*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, m_iCurLevelID, TEXT("PurpleBallOut.json"), &EffectDesc));

    if (nullptr != m_pPurpleEffect)
    {
        m_pPurpleEffect->Set_Active(false);
        m_pPurpleEffect->Set_ParentMatrix(COORDINATE_3D, m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D));
        m_pPurpleEffect->Set_SpawnMatrix(p3DModel->Get_BoneMatrix("buttergrump_righead_jaw_end"));
    }

    Safe_AddRef(m_pPurpleEffect);

    m_PartObjects[BOSSPART_YELLOWBALL_EFFECT] = m_pYellowEffect =
        static_cast<CEffect_System*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, m_iCurLevelID, TEXT("YellowBallOut.json"), &EffectDesc));

    if (nullptr != m_pYellowEffect)
    {
        m_pYellowEffect->Set_Active(false);
        m_pYellowEffect->Set_ParentMatrix(COORDINATE_3D, m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D));
        m_pYellowEffect->Set_SpawnMatrix(p3DModel->Get_BoneMatrix("buttergrump_righead_jaw_end"));
    }

    Safe_AddRef(m_pYellowEffect);

    m_PartObjects[BOOSPART_SHIELD_EFFECT] = m_pShieldEffect =
        static_cast<CEffect_System*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, m_iCurLevelID, TEXT("ShieldEnv.json"), &EffectDesc));

    if (nullptr != m_pShieldEffect)
    {
        m_pShieldEffect->Set_Active(false);
        m_pShieldEffect->Set_ParentMatrix(COORDINATE_3D, m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D));
    }

    Safe_AddRef(m_pShieldEffect);

    return S_OK;
}

HRESULT CButterGrump::Ready_Projectiles()
{
    Pooling_DESC Pooling_Desc;
    Pooling_Desc.iPrototypeLevelID = m_iCurLevelID;
    Pooling_Desc.strLayerTag = TEXT("Layer_Monster_Projectile");
    Pooling_Desc.strPrototypeTag = TEXT("Prototype_GameObject_Boss_HomingBall");

    CBoss_HomingBall::PROJECTILE_MONSTER_DESC* pHomingBallDesc = new CBoss_HomingBall::PROJECTILE_MONSTER_DESC;
    pHomingBallDesc->fLifeTime = 4.f;
    pHomingBallDesc->eStartCoord = COORDINATE_3D;
    pHomingBallDesc->isCoordChangeEnable = false;
    pHomingBallDesc->iCurLevelID = m_iCurLevelID;

    pHomingBallDesc->tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
    pHomingBallDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(30.f);
    pHomingBallDesc->tTransform3DDesc.fSpeedPerSec = 10.f;

    CPooling_Manager::GetInstance()->Register_PoolingObject(TEXT("Pooling_Boss_HomingBall"), Pooling_Desc, pHomingBallDesc);




    /*Pooling_Desc.strPrototypeTag = TEXT("Prototype_GameObject_Boss_EnergyBall");
    CBoss_EnergyBall::PROJECTILE_MONSTER_DESC* pEnergyBallDesc = new CBoss_EnergyBall::PROJECTILE_MONSTER_DESC;
    pEnergyBallDesc->fLifeTime = 5.f;
    pEnergyBallDesc->eStartCoord = COORDINATE_3D;
    pEnergyBallDesc->isCoordChangeEnable = false;
    pEnergyBallDesc->iCurLevelID = m_iCurLevelID;

    pEnergyBallDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    pEnergyBallDesc->tTransform3DDesc.fSpeedPerSec = 15.f;

    CPooling_Manager::GetInstance()->Register_PoolingObject(TEXT("Pooling_Boss_EnergyBall"), Pooling_Desc, pEnergyBallDesc);*/



    Pooling_Desc.strPrototypeTag = TEXT("Prototype_GameObject_Boss_YellowBall");
    CBoss_YellowBall::PROJECTILE_MONSTER_DESC* pYellowBallDesc = new CBoss_YellowBall::PROJECTILE_MONSTER_DESC;
    pYellowBallDesc->fLifeTime = 5.f;
    pYellowBallDesc->eStartCoord = COORDINATE_3D;
    pYellowBallDesc->isCoordChangeEnable = false;
    pYellowBallDesc->iCurLevelID = m_iCurLevelID;

    pYellowBallDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    pYellowBallDesc->tTransform3DDesc.fSpeedPerSec = 15.f;

    CPooling_Manager::GetInstance()->Register_PoolingObject(TEXT("Pooling_Boss_YellowBall"), Pooling_Desc, pYellowBallDesc);



    Pooling_Desc.strPrototypeTag = TEXT("Prototype_GameObject_Boss_PurpleBall");
    CBoss_PurpleBall::PROJECTILE_MONSTER_DESC* pPurpleBallDesc = new CBoss_PurpleBall::PROJECTILE_MONSTER_DESC;
    pPurpleBallDesc->fLifeTime = 5.f;
    pPurpleBallDesc->eStartCoord = COORDINATE_3D;
    pPurpleBallDesc->isCoordChangeEnable = false;
    pPurpleBallDesc->iCurLevelID = m_iCurLevelID;

    pPurpleBallDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    pPurpleBallDesc->tTransform3DDesc.fSpeedPerSec = 30.f;

    CPooling_Manager::GetInstance()->Register_PoolingObject(TEXT("Pooling_Boss_PurpleBall"), Pooling_Desc, pPurpleBallDesc);



    Pooling_Desc.strPrototypeTag = TEXT("Prototype_GameObject_Boss_WingSlam");
    CBoss_WingSlam::PROJECTILE_MONSTER_DESC* pWingSlamDesc = new CBoss_WingSlam::PROJECTILE_MONSTER_DESC;
    pWingSlamDesc->fLifeTime = 3.f;
    pWingSlamDesc->eStartCoord = COORDINATE_3D;
    pWingSlamDesc->isCoordChangeEnable = false;
    pWingSlamDesc->iCurLevelID = m_iCurLevelID;

    pWingSlamDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    pWingSlamDesc->tTransform3DDesc.fSpeedPerSec = 70.f;

    CPooling_Manager::GetInstance()->Register_PoolingObject(TEXT("Pooling_Boss_WingSlam"), Pooling_Desc, pWingSlamDesc);


    Pooling_Desc.strPrototypeTag = TEXT("Prototype_GameObject_Boss_WingSlice");
    CBoss_WingSlice::PROJECTILE_MONSTER_DESC* pWingSliceDesc = new CBoss_WingSlice::PROJECTILE_MONSTER_DESC;
    pWingSliceDesc->fLifeTime = 3.f;
    pWingSliceDesc->eStartCoord = COORDINATE_3D;
    pWingSliceDesc->isCoordChangeEnable = false;
    pWingSliceDesc->iCurLevelID = m_iCurLevelID;

    pWingSliceDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    pWingSliceDesc->tTransform3DDesc.fSpeedPerSec = 70.f;

    CPooling_Manager::GetInstance()->Register_PoolingObject(TEXT("Pooling_Boss_WingSlice"), Pooling_Desc, pWingSliceDesc);



    Pooling_Desc.strPrototypeTag = TEXT("Prototype_GameObject_Boss_Rock");
    CBoss_Rock::PROJECTILE_MONSTER_DESC* pRockDesc = new CBoss_Rock::PROJECTILE_MONSTER_DESC;
    pRockDesc->fLifeTime = 10.f;
    pRockDesc->eStartCoord = COORDINATE_3D;
    pRockDesc->isCoordChangeEnable = false;
    pRockDesc->iCurLevelID = m_iCurLevelID;

    pRockDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    pRockDesc->tTransform3DDesc.fSpeedPerSec = 10.f;

    CPooling_Manager::GetInstance()->Register_PoolingObject(TEXT("Pooling_Boss_Rock"), Pooling_Desc, pRockDesc);


    Pooling_Desc.strPrototypeTag = TEXT("Prototype_GameObject_Boss_Crystal");
    CBoss_Crystal::BOSS_CRYSTAL_DESC* pCrystalDesc = new CBoss_Crystal::BOSS_CRYSTAL_DESC;
    pCrystalDesc->fLifeTime = 20.f;
    pCrystalDesc->eStartCoord = COORDINATE_3D;
    pCrystalDesc->isCoordChangeEnable = false;
    pCrystalDesc->iCurLevelID = m_iCurLevelID;
    pCrystalDesc->pSpawner = this;

    pCrystalDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    pCrystalDesc->tTransform3DDesc.fSpeedPerSec = 10.f;

    CPooling_Manager::GetInstance()->Register_PoolingObject(TEXT("Pooling_Boss_Crystal"), Pooling_Desc, pCrystalDesc);


    Pooling_Desc.strPrototypeTag = TEXT("Prototype_GameObject_Boss_TennisBall");
    CBoss_TennisBall::BOSS_TENNISBALL_DESC* pTennisBallDesc = new CBoss_TennisBall::BOSS_TENNISBALL_DESC;
    pTennisBallDesc->fLifeTime = 20.f;
    pTennisBallDesc->eStartCoord = COORDINATE_3D;
    pTennisBallDesc->isCoordChangeEnable = false;
    pTennisBallDesc->iCurLevelID = m_iCurLevelID;
    pTennisBallDesc->pSpawner = this;

    pTennisBallDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    pTennisBallDesc->tTransform3DDesc.fSpeedPerSec = 10.f;

    CPooling_Manager::GetInstance()->Register_PoolingObject(TEXT("Pooling_Boss_TennisBall"), Pooling_Desc, pTennisBallDesc);


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
    
    Safe_Release(m_pHomingEffect);
    Safe_Release(m_pPurpleEffect);
    Safe_Release(m_pYellowEffect);
    Safe_Release(m_pShieldEffect);

    __super::Free();
}
