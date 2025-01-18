#include "stdafx.h"
#include "BarfBug.h"
#include "GameInstance.h"
#include "FSM.h"
#include "ModelObject.h"
#include "Pooling_Manager.h"
#include "Projectile_BarfBug.h"

CBarfBug::CBarfBug(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CMonster(_pDevice, _pContext)
{
}

CBarfBug::CBarfBug(const CBarfBug& _Prototype)
    : CMonster(_Prototype)
{
}

HRESULT CBarfBug::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBarfBug::Initialize(void* _pArg)
{
    CBarfBug::MONSTER_DESC* pDesc = static_cast<CBarfBug::MONSTER_DESC*>(_pArg);
    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = false;
    pDesc->iNumPartObjects = PART_END;

    //pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    //pDesc->tTransform2DDesc.fSpeedPerSec = 3.f;

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

    //static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(Idle, true);
    //static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(Run, true);
    //static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_Animation(Idle);

    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Register_OnAnimEndCallBack(bind(&CBarfBug::Alert_End, this, COORDINATE_3D, ALERT));
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Register_OnAnimEndCallBack(bind(&CBarfBug::Attack_End, this, COORDINATE_3D, BARF));


    /*  Projectile  */
    Pooling_DESC Pooling_Desc;
    Pooling_Desc.iPrototypeLevelID = LEVEL_GAMEPLAY;
    Pooling_Desc.strLayerTag = TEXT("Layer_Monster");
    Pooling_Desc.strPrototypeTag = TEXT("Prototype_GameObject_Projectile_BarfBug");

    CProjectile_BarfBug::PROJECTILE_BARFBUG_DESC* pProjDesc = new CProjectile_BarfBug::PROJECTILE_BARFBUG_DESC;
    pProjDesc->fLifeTime = 5.f;
    pProjDesc->eStartCoord = COORDINATE_3D;
    pProjDesc->isCoordChangeEnable = false;
    pProjDesc->iNumPartObjects = PART_LAST;
    pProjDesc->iCurLevelID = LEVEL_GAMEPLAY;

    //pProjDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    //pProjDesc->tTransform2DDesc.fSpeedPerSec = 3.f;

    pProjDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    pProjDesc->tTransform3DDesc.fSpeedPerSec = 10.f;

    CPooling_Manager::GetInstance()->Register_PoolingObject(TEXT("Pooling_Projectile_BarfBug"), Pooling_Desc, pProjDesc);

    return S_OK;
}

void CBarfBug::Priority_Update(_float _fTimeDelta)
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

void CBarfBug::Update(_float _fTimeDelta)
{
    if (KEY_DOWN(KEY::F3))
    {
        Event_DeleteObject(this);
    }

    m_pFSM->Update(_fTimeDelta);
    __super::Update(_fTimeDelta); /* Part Object Update */
}

void CBarfBug::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta); /* Part Object Late_Update */
}

HRESULT CBarfBug::Render()
{
    /* Model이 없는 Container Object 같은 경우 Debug 용으로 사용하거나, 폰트 렌더용으로. */

#ifdef _DEBUG

#endif // _DEBUG

    /* Font Render */

    return S_OK;
}

void CBarfBug::Change_Animation()
{
    if(m_eState != m_ePreState)
    {
        switch (m_eState)
        {
        case MONSTER_STATE::IDLE:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(IDLE);
            break;

        case MONSTER_STATE::ALERT:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(ALERT);
            break;

        case MONSTER_STATE::CHASE:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(WALK);
            break;

        case MONSTER_STATE::ATTACK:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(BARF);
            break;

        default:
            break;
        }
    }
}

void CBarfBug::Attack(_float _fTimeDelta)
{
    if (false == m_isDelay && false == m_isCool)
    {
        _float3 vPosition;
        _float4 vRotation;
        _vector vvScale, vvRotation, vvPosition;
        XMMatrixDecompose(&vvScale, &vvRotation, &vvPosition, m_pControllerTransform->Get_WorldMatrix());
        XMStoreFloat3(&vPosition, m_pControllerTransform->Get_State(CTransform_3D::STATE_POSITION));
        XMStoreFloat4(&vRotation, vvRotation);

        CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Projectile_BarfBug"),&vPosition, &vRotation);
        Delay_On();
        ++m_iAttackCount;
    }
}

void CBarfBug::Alert_End(COORDINATE _eCoord, _uint iAnimIdx)
{
    Set_AnimChangeable(true);
}

void CBarfBug::Attack_End(COORDINATE _eCoord, _uint iAnimIdx)
{
    //딜레이 동안은 애니 전환 안됨. 따라서 상태 전환도 불가
    if (false == m_isDelay)
    {
        Set_AnimChangeable(true);
    }
}

HRESULT CBarfBug::Ready_Components()
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

HRESULT CBarfBug::Ready_PartObjects()
{
    /* Part Body */
    CModelObject::MODELOBJECT_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    //BodyDesc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
    BodyDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxAnimMesh");
    //BodyDesc.strModelPrototypeTag_2D = TEXT("barfBug_Rig");
    BodyDesc.strModelPrototypeTag_3D = TEXT("barfBug_Rig");
	//BodyDesc.iModelPrototypeLevelID_2D = LEVEL_GAMEPLAY;
	BodyDesc.iModelPrototypeLevelID_3D = LEVEL_GAMEPLAY;
   //BodyDesc.iShaderPass_2D = (_uint)PASS_VTXMESH::DEFAULT;
    BodyDesc.iShaderPass_3D = (_uint)PASS_VTXANIMMESH::DEFAULT;

    //BodyDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
    BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

    BodyDesc.tTransform3DDesc.vPosition = _float3(0.0f, 0.0f, 0.0f);
    BodyDesc.tTransform3DDesc.vScaling = _float3(1.0f, 1.0f, 1.0f);
    BodyDesc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    BodyDesc.tTransform3DDesc.fSpeedPerSec = 10.f;

    //BodyDesc.tTransform2DDesc.vPosition = _float3(0.0f, 0.0f, 0.0f);
    //BodyDesc.tTransform2DDesc.vScaling = _float3(1.0f, 1.0f, 1.0f);
    //BodyDesc.tTransform2DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    //BodyDesc.tTransform2DDesc.fSpeedPerSec = 10.f;

    m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &BodyDesc));
    if (nullptr == m_PartObjects[PART_BODY])
        return E_FAIL;

    return S_OK;
}

CBarfBug* CBarfBug::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBarfBug* pInstance = new CBarfBug(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBarfBug");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBarfBug::Clone(void* _pArg)
{
    CBarfBug* pInstance = new CBarfBug(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CBarfBug");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBarfBug::Free()
{

    __super::Free();
}
