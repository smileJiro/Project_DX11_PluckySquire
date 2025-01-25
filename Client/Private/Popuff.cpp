#include "stdafx.h"
#include "Popuff.h"
#include "GameInstance.h"
#include "FSM.h"
#include "ModelObject.h"

CPopuff::CPopuff(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CMonster(_pDevice, _pContext)
{
}

CPopuff::CPopuff(const CPopuff& _Prototype)
    : CMonster(_Prototype)
{
}

HRESULT CPopuff::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPopuff::Initialize(void* _pArg)
{
    CPopuff::MONSTER_DESC* pDesc = static_cast<CPopuff::MONSTER_DESC*>(_pArg);
    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = false;
    pDesc->iNumPartObjects = PART_END;

    pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
    pDesc->tTransform3DDesc.fSpeedPerSec = 6.f;

    pDesc->fAlertRange = 5.f;
    pDesc->fChaseRange = 12.f;
    pDesc->fAttackRange = 2.f;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    m_pFSM->Add_State((_uint)MONSTER_STATE::IDLE);
    m_pFSM->Add_State((_uint)MONSTER_STATE::ALERT);
    m_pFSM->Add_State((_uint)MONSTER_STATE::CHASE);
    m_pFSM->Add_State((_uint)MONSTER_STATE::ATTACK);
    m_pFSM->Set_State((_uint)MONSTER_STATE::IDLE);

    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);

    //pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, IDLE, true);
    //pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, RUN, true);
    //pModelObject->Set_Animation(IDLE);

    pModelObject->Register_OnAnimEndCallBack(bind(&CPopuff::Animation_End, this, placeholders::_1, placeholders::_2));

    return S_OK;
}

void CPopuff::Priority_Update(_float _fTimeDelta)
{

    __super::Priority_Update(_fTimeDelta); /* Part Object Priority_Update */
}

void CPopuff::Update(_float _fTimeDelta)
{
    m_pFSM->Update(_fTimeDelta);
    __super::Update(_fTimeDelta); /* Part Object Update */
}

void CPopuff::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta); /* Part Object Late_Update */
}

HRESULT CPopuff::Render()
{
    /* Model이 없는 Container Object 같은 경우 Debug 용으로 사용하거나, 폰트 렌더용으로. */

#ifdef _DEBUG

#endif // _DEBUG

    /* Font Render */

    return S_OK;
}

void CPopuff::Change_Animation()
{
    if(m_iState != m_iPreState)
    {
        switch (MONSTER_STATE(m_iState))
        {
        case MONSTER_STATE::IDLE:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(IDLE);
            break;

        case MONSTER_STATE::ALERT:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(IDLE);
            break;

        case MONSTER_STATE::CHASE:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(WALK);
            break;

        case MONSTER_STATE::ATTACK:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(ATTACK);
            break;

        default:
            break;
        }
    }
}

void CPopuff::Animation_End(COORDINATE _eCoord, _uint iAnimIdx)
{
    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);
    switch ((CPopuff::Animation)pModelObject->Get_Model(COORDINATE_3D)->Get_CurrentAnimIndex())
    {

    case ATTACK:
        Set_AnimChangeable(true);
        break;

    default:
        break;
    }
}

HRESULT CPopuff::Ready_Components()
{
    /* Com_FSM */
    CFSM::FSMDESC Desc;
    Desc.fAlertRange = m_fAlertRange;
    Desc.fChaseRange = m_fChaseRange;
    Desc.fAttackRange = m_fAttackRange;

    if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_FSM"),
        TEXT("Com_FSM"), reinterpret_cast<CComponent**>(&m_pFSM), &Desc)))
        return E_FAIL;
    m_pFSM->Set_Owner(this);

    return S_OK;
}

HRESULT CPopuff::Ready_PartObjects()
{
    /* Part Body */
    CModelObject::MODELOBJECT_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    BodyDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxAnimMesh");
    BodyDesc.strModelPrototypeTag_3D = TEXT("Popuff_Rig");
	BodyDesc.iModelPrototypeLevelID_3D = m_iCurLevelID;
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

CPopuff* CPopuff::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CPopuff* pInstance = new CPopuff(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CPopuff");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPopuff::Clone(void* _pArg)
{
    CPopuff* pInstance = new CPopuff(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CPopuff");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPopuff::Free()
{

    __super::Free();
}
