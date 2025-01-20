#include "stdafx.h"
#include "Beetle.h"
#include "GameInstance.h"
#include "FSM.h"
#include "ModelObject.h"

CBeetle::CBeetle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CMonster(_pDevice, _pContext)
{
}

CBeetle::CBeetle(const CBeetle& _Prototype)
    : CMonster(_Prototype)
{
}

HRESULT CBeetle::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBeetle::Initialize(void* _pArg)
{
    CBeetle::MONSTER_DESC* pDesc = static_cast<CBeetle::MONSTER_DESC*>(_pArg);
    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = false;
    pDesc->iNumPartObjects = PART_END;

    pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    pDesc->tTransform3DDesc.fSpeedPerSec = 3.f;

    pDesc->fAlertRange = 5.f;
    pDesc->fChaseRange = 12.f;
    pDesc->fAttackRange = 2.f;

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
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(COORDINATE::COORDINATE_3D, RUN, true);
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_Animation(IDLE);

    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Register_OnAnimEndCallBack(bind(&CBeetle::Alert_End, this, COORDINATE_3D, ALERT));
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Register_OnAnimEndCallBack(bind(&CBeetle::Attack_End, this, COORDINATE_3D, ATTACKSTRIKE));
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Register_OnAnimEndCallBack(bind(&CBeetle::Attack_Recovery_End, this, COORDINATE_3D, ATTACKRECOVERY));

    return S_OK;
}

void CBeetle::Priority_Update(_float _fTimeDelta)
{

    __super::Priority_Update(_fTimeDelta); /* Part Object Priority_Update */
}

void CBeetle::Update(_float _fTimeDelta)
{
    m_pFSM->Update(_fTimeDelta);
    __super::Update(_fTimeDelta); /* Part Object Update */
}

void CBeetle::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta); /* Part Object Late_Update */
}

HRESULT CBeetle::Render()
{
    /* Model이 없는 Container Object 같은 경우 Debug 용으로 사용하거나, 폰트 렌더용으로. */

#ifdef _DEBUG

#endif // _DEBUG

    /* Font Render */

    return S_OK;
}

void CBeetle::Change_Animation()
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
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(RUN);
            break;

        case MONSTER_STATE::ATTACK:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(ATTACKSTRIKE);
            break;

        default:
            break;
        }
    }
}

void CBeetle::Alert_End(COORDINATE _eCoord, _uint iAnimIdx)
{
    Set_AnimChangeable(true);
}

void CBeetle::Attack_End(COORDINATE _eCoord, _uint iAnimIdx)
{
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(ATTACKRECOVERY);
}

void CBeetle::Attack_Recovery_End(COORDINATE _eCoord, _uint iAnimIdx)
{
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(ATTACKSTRIKE);
    Set_AnimChangeable(true);
}

HRESULT CBeetle::Ready_Components()
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

HRESULT CBeetle::Ready_PartObjects()
{
    /* Part Body */
    CModelObject::MODELOBJECT_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    BodyDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxAnimMesh");
    BodyDesc.strModelPrototypeTag_3D = TEXT("beetle_01");
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

CBeetle* CBeetle::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBeetle* pInstance = new CBeetle(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBeetle");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBeetle::Clone(void* _pArg)
{
    CBeetle* pInstance = new CBeetle(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CBeetle");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBeetle::Free()
{

    __super::Free();
}
