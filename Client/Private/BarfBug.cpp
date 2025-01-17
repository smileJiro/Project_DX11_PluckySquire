#include "stdafx.h"
#include "BarfBug.h"
#include "GameInstance.h"
#include "FSM.h"
#include "ModelObject.h"

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

    pDesc->fChaseRange = 5.f;
    pDesc->fAttackRange = 2.f;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    m_pFSM->Add_State(MONSTER_STATE::IDLE);
    m_pFSM->Add_State(MONSTER_STATE::CHASE);
    m_pFSM->Set_State(MONSTER_STATE::IDLE);

    //static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(Idle, true);
    //static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(Run, true);
    //static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_Animation(Idle);

    return S_OK;
}

void CBarfBug::Priority_Update(_float _fTimeDelta)
{

    __super::Priority_Update(_fTimeDelta); /* Part Object Priority_Update */
}

void CBarfBug::Update(_float _fTimeDelta)
{
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
    //if(m_eState != m_ePreState)
    //{
    //    switch (m_eState)
    //    {
    //    case MONSTER_STATE::IDLE:
    //        static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(Idle);
    //        break;

    //    case MONSTER_STATE::CHASE:
    //        static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(Run);
    //        break;

    //    case MONSTER_STATE::ATTACK:
    //        static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(AttackStrike);
    //        break;

    //    default:
    //        break;
    //    }
    //}
}

HRESULT CBarfBug::Ready_Components()
{
    /* Com_FSM */
    CFSM::FSMDESC Desc;
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


    /* Part Weapon */
    CModelObject::MODELOBJECT_DESC WeaponDesc{};

    WeaponDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    WeaponDesc.iCurLevelID = m_iCurLevelID;
    WeaponDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    //WeaponDesc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
    WeaponDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
    //WeaponDesc.strModelPrototypeTag_2D = TEXT("barfBug_Rig");
    WeaponDesc.strModelPrototypeTag_3D = TEXT("S_FX_CMN_Sphere_01");
    //WeaponDesc.iModelPrototypeLevelID_2D = LEVEL_GAMEPLAY;
    WeaponDesc.iModelPrototypeLevelID_3D = LEVEL_GAMEPLAY;
    //WeaponDesc.iShaderPass_2D = (_uint)PASS_VTXMESH::DEFAULT;
    WeaponDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::COLOR;

    //WeaponDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
    WeaponDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

    WeaponDesc.tTransform3DDesc.vPosition = _float3(0.0f, 0.0f, 0.0f);
    WeaponDesc.tTransform3DDesc.vScaling = _float3(1.0f, 1.0f, 1.0f);
    WeaponDesc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    WeaponDesc.tTransform3DDesc.fSpeedPerSec = 10.f;

    //WeaponDesc.tTransform2DDesc.vPosition = _float3(0.0f, 0.0f, 0.0f);
    //WeaponDesc.tTransform2DDesc.vScaling = _float3(1.0f, 1.0f, 1.0f);
    //WeaponDesc.tTransform2DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    //WeaponDesc.tTransform2DDesc.fSpeedPerSec = 10.f;

    m_PartObjects[PART_WEAPON] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Projectile_BarfBug"), &WeaponDesc));
    if (nullptr == m_PartObjects[PART_WEAPON])
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
