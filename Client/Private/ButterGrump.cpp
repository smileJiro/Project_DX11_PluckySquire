#include "stdafx.h"
#include "ButterGrump.h"
#include "GameInstance.h"
#include "FSM.h"
#include "ModelObject.h"
#include "Pooling_Manager.h"
#include "Projectile_BarfBug.h"

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

    pDesc->fAlertRange = 5.f;
    pDesc->fChaseRange = 12.f;
    pDesc->fAttackRange = 10.f;
    pDesc->fDelayTime = 1.f;
    pDesc->fCoolTime = 3.f;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    m_pFSM->Add_State(MONSTER_STATE::IDLE);
    m_pFSM->Add_State(MONSTER_STATE::ALERT);
    m_pFSM->Add_State(MONSTER_STATE::CHASE);
    m_pFSM->Add_State(MONSTER_STATE::ATTACK);
    m_pFSM->Set_State(MONSTER_STATE::IDLE);

    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(COORDINATE::COORDINATE_3D, IDLE, true);
    //static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(COORDINATE::COORDINATE_3D, WALK, true);
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_Animation(IDLE);

    //static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Register_OnAnimEndCallBack(bind(&CButterGrump::Alert_End, this, COORDINATE_3D, ALERT));
    //static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Register_OnAnimEndCallBack(bind(&CButterGrump::Attack_End, this, COORDINATE_3D, BARF));


    /*  Projectile  */
   /* Pooling_DESC Pooling_Desc;
    Pooling_Desc.iPrototypeLevelID = LEVEL_GAMEPLAY;
    Pooling_Desc.strLayerTag = TEXT("Layer_Monster");
    Pooling_Desc.strPrototypeTag = TEXT("Prototype_GameObject_Projectile_ButterGrump");

    CProjectile_ButterGrump::PROJECTILE_BARFBUG_DESC* pProjDesc = new CProjectile_ButterGrump::PROJECTILE_BARFBUG_DESC;
    pProjDesc->fLifeTime = 5.f;
    pProjDesc->eStartCoord = COORDINATE_3D;
    pProjDesc->isCoordChangeEnable = false;
    pProjDesc->iNumPartObjects = PART_LAST;
    pProjDesc->iCurLevelID = LEVEL_GAMEPLAY;

    pProjDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    pProjDesc->tTransform3DDesc.fSpeedPerSec = 10.f;

    CPooling_Manager::GetInstance()->Register_PoolingObject(TEXT("Pooling_Projectile_ButterGrump"), Pooling_Desc, pProjDesc);*/

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
    m_pFSM->Update(_fTimeDelta);
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
    if(m_eState != m_ePreState)
    {
        switch (m_eState)
        {
        case MONSTER_STATE::IDLE:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(IDLE);
            break;

        /*case MONSTER_STATE::ALERT:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(ALERT);
            break;

        case MONSTER_STATE::CHASE:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(WALK);
            break;

        case MONSTER_STATE::ATTACK:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(BARF);
            break;*/

        default:
            break;
        }
    }
}

void CButterGrump::Attack(_float _fTimeDelta)
{
  //  if (false == m_isDelay && false == m_isCool)
  //  {
  //      _float3 vPosition;
  //      _float4 vRotation;
  //      _vector vvScale, vvRotation, vvPosition;
  //      XMMatrixDecompose(&vvScale, &vvRotation, &vvPosition, m_pControllerTransform->Get_WorldMatrix());
		//XMStoreFloat3(&vPosition, vvPosition + XMVectorSet(0.f, vvScale.m128_f32[1] * 0.5f, 0.f, 1.f));
  //      XMStoreFloat4(&vRotation, vvRotation);

  //      CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Projectile_ButterGrump"),&vPosition, &vRotation);
  //      Delay_On();
  //      ++m_iAttackCount;
  //  }
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

HRESULT CButterGrump::Ready_Components()
{
    /* Com_FSM */
    CFSM::FSMDESC Desc;
    Desc.fAlertRange = m_fAlertRange;
    Desc.fChaseRange = m_fChaseRange;
    Desc.fAttackRange = m_fAttackRange;

    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_FSM"),
        TEXT("Com_FSM"), reinterpret_cast<CComponent**>(&m_pFSM), &Desc)))
        return E_FAIL;
    m_pFSM->Set_Owner(this);


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

    __super::Free();
}
