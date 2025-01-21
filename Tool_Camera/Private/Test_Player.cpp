#include "stdafx.h"
#include "Test_Player.h"
#include "GameInstance.h"
#include "Camera_Manager_Tool.h"
#include "StateMachine.h"
#include "ModelObject.h"
#include "Controller_Transform.h"
#include "PlayerState_Idle.h"
#include "PlayerState_Run.h"
#include "PlayerState_Attack.h"

CTest_Player::CTest_Player(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CContainerObject(_pDevice, _pContext)
{
}

CTest_Player::CTest_Player(const CTest_Player& _Prototype)
    :CContainerObject(_Prototype)
{
}

HRESULT CTest_Player::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTest_Player::Initialize(void* _pArg)
{
    CTest_Player::CONTAINEROBJ_DESC* pDesc = static_cast<CTest_Player::CONTAINEROBJ_DESC*>(_pArg);
    pDesc->iNumPartObjects = CTest_Player::PLAYER_PART_LAST;
    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = true;
    pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(180.f);
    pDesc->tTransform2DDesc.fSpeedPerSec = 200.f;

    pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(720);
    pDesc->tTransform3DDesc.fSpeedPerSec = 8.f;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;
    // TODO ::임시 위치
    Set_Position(XMVectorSet(-3.f, 0.35f, -19.3f, 1.f));
    return S_OK;
}

HRESULT CTest_Player::Ready_Components()
{
    CStateMachine::STATEMACHINE_DESC tStateMachineDesc{};
    tStateMachineDesc.pOwner = this;

    m_pStateMachine = static_cast<CStateMachine*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"), &tStateMachineDesc));
    m_pStateMachine->Transition_To(new CPlayerState_Idle(this));
    return S_OK;
}

HRESULT CTest_Player::Ready_PartObjects()
{
    /* Part Body */
    CModelObject::MODELOBJECT_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    BodyDesc.iModelPrototypeLevelID_2D = m_iCurLevelID;
    BodyDesc.iModelPrototypeLevelID_3D = m_iCurLevelID;
    BodyDesc.strModelPrototypeTag_2D = TEXT("Prototype_Component_player2DAnimation");
    BodyDesc.strModelPrototypeTag_3D = TEXT("Latch_SkelMesh_NewRig");
    BodyDesc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
    BodyDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxAnimMesh");
    BodyDesc.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE_ANIM;
    BodyDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;
    BodyDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
    BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);
    BodyDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    BodyDesc.tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);
    BodyDesc.tTransform2DDesc.fRotationPerSec = XMConvertToRadians(180.f);
    BodyDesc.tTransform2DDesc.fSpeedPerSec = 10.f;
    m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &BodyDesc));
    if (nullptr == m_PartObjects[PART_BODY])
        return E_FAIL;

    //Part Sword
    BodyDesc.isCoordChangeEnable = false;
    BodyDesc.strModelPrototypeTag_3D = TEXT("latch_sword");
    BodyDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
    BodyDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;
    BodyDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 1.0f, 0.0f);
    BodyDesc.tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);
    BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);
    m_PartObjects[PLAYER_PART_SWORD] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &BodyDesc));
    if (nullptr == m_PartObjects[PLAYER_PART_SWORD])
        return E_FAIL;
    C3DModel* p3DModel = static_cast<C3DModel*>(static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_3D));
    static_cast<CPartObject*>(m_PartObjects[PLAYER_PART_SWORD])->Set_SocketMatrix(p3DModel->Get_BoneMatrix("j_glove_hand_attach_r")); /**/
    m_PartObjects[PLAYER_PART_SWORD]->Get_ControllerTransform()->Rotation(XMConvertToRadians(180.f), _vector{ 1,0,0,0 });
    Set_PartActive(PLAYER_PART_SWORD, false);

    //Part Glove
    BodyDesc.strModelPrototypeTag_3D = TEXT("latch_glove");
    BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);
    m_PartObjects[PLAYER_PART_GLOVE] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &BodyDesc));
    if (nullptr == m_PartObjects[PLAYER_PART_GLOVE])
        return E_FAIL;
    static_cast<CPartObject*>(m_PartObjects[PLAYER_PART_GLOVE])->Set_SocketMatrix(p3DModel->Get_BoneMatrix("j_glove_hand_r")); /**/
    m_PartObjects[PLAYER_PART_GLOVE]->Get_ControllerTransform()->Rotation(XMConvertToRadians(180.f), _vector{ 0,1,0,0 });
    Set_PartActive(PLAYER_PART_GLOVE, false);

    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Register_OnAnimEndCallBack(bind(&CTest_Player::On_AnimEnd, this, placeholders::_1, placeholders::_2));
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(COORDINATE::COORDINATE_2D, (_uint)ANIM_STATE_2D::PLAYER_IDLE_RIGHT, true);
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(COORDINATE::COORDINATE_2D, (_uint)ANIM_STATE_2D::PLAYER_IDLE_UP, true);
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(COORDINATE::COORDINATE_2D, (_uint)ANIM_STATE_2D::PLAYER_IDLE_DOWN, true);
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(COORDINATE::COORDINATE_2D, (_uint)ANIM_STATE_2D::PLAYER_RUN_RIGHT, true);
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(COORDINATE::COORDINATE_2D, (_uint)ANIM_STATE_2D::PLAYER_RUN_UP, true);
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(COORDINATE::COORDINATE_2D, (_uint)ANIM_STATE_2D::PLAYER_RUN_DOWN, true);
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(COORDINATE::COORDINATE_3D, (_uint)ANIM_STATE_3D::LATCH_PICKUP_IDLE_GT, true);
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(COORDINATE::COORDINATE_3D, (_uint)ANIM_STATE_3D::LATCH_ANIM_RUN_01_GT, true);
    return S_OK;
}
void CTest_Player::Priority_Update(_float _fTimeDelta)
{

    CContainerObject::Priority_Update(_fTimeDelta); /* Part Object Priority_Update */
}

void CTest_Player::Update(_float _fTimeDelta)
{
    Key_Input(_fTimeDelta);
    m_pStateMachine->Update(_fTimeDelta);
    __super::Update(_fTimeDelta); /* Part Object Update */
}

void CTest_Player::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta); /* Part Object Late_Update */
}

HRESULT CTest_Player::Render()
{
    /* Model이 없는 Container Object 같은 경우 Debug 용으로 사용하거나, 폰트 렌더용으로. */

#ifdef _DEBUG

#endif // _DEBUG

    /* Font Render */

    return S_OK;
}

void CTest_Player::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
    m_pStateMachine->Get_CurrentState()->On_AnimEnd(_eCoord, iAnimIdx);
}

HRESULT CTest_Player::Change_Coordinate(COORDINATE _eCoordinate, const _float3& _vPosition)
{
    if (FAILED(m_PartObjects[PART_BODY]->Change_Coordinate(_eCoordinate, _float3(0.0f, 0.0f, 0.0f))))
        return E_FAIL;
    return __super::Change_Coordinate(_eCoordinate, _vPosition);
}

void CTest_Player::On_CoordinateChange()
{
    if (COORDINATE_2D == Get_CurCoord())
        Set_2DDirection(F_DIRECTION::DOWN);

    Set_State(IDLE);
}

void CTest_Player::Move(_vector _vDir, _float _fTimeDelta)
{
    if (Get_CurCoord() == COORDINATE_3D)
    {
        m_pControllerTransform->Set_AutoRotationYDirection(_vDir);
        m_pControllerTransform->Update_AutoRotation(_fTimeDelta);
    }
    else
    {

    }
    m_pControllerTransform->Go_Direction(_vDir, _fTimeDelta);

}

void CTest_Player::Attack(_uint _iCombo)
{
    COORDINATE eCoord = Get_CurCoord();

    if (COORDINATE_2D == eCoord)
    {
        F_DIRECTION eOldDir = Get_2DDirection();
        switch (eOldDir)
        {
        case Camera_Tool::F_DIRECTION::LEFT:
        case Camera_Tool::F_DIRECTION::RIGHT:
            switch (_iCombo)
            {
            case 0:
                Switch_Animation((_uint)CTest_Player::ANIM_STATE_2D::PLAYER_ATTACK_RIGHT);
                break;
            case 1:
                Switch_Animation((_uint)CTest_Player::ANIM_STATE_2D::PLAYER_ATTACKCOMBO_01_RIGHT);
                break;
            case 2:
                Switch_Animation((_uint)CTest_Player::ANIM_STATE_2D::PLAYER_ATTACKCOMBO_03_RIGHT);
                break;
            default:
                break;
            }
            break;
        case Camera_Tool::F_DIRECTION::UP:
            switch (_iCombo)
            {
            case 0:
                Switch_Animation((_uint)CTest_Player::ANIM_STATE_2D::PLAYER_ATTACK_UP);
                break;
            case 1:
                Switch_Animation((_uint)CTest_Player::ANIM_STATE_2D::PLAYER_ATTACKCOMBO_01_UP);
                break;
            case 2:
                Switch_Animation((_uint)CTest_Player::ANIM_STATE_2D::PLAYER_ATTACKCOMBO_03_UP);
                break;
            default:
                break;
            }
            break;
        case Camera_Tool::F_DIRECTION::DOWN:
            switch (_iCombo)
            {
            case 0:
                Switch_Animation((_uint)CTest_Player::ANIM_STATE_2D::PLAYER_ATTACK_DOWN);
                break;
            case 1:
                Switch_Animation((_uint)CTest_Player::ANIM_STATE_2D::PLAYER_ATTACKCOMBO_01_DOWN);
                break;
            case 2:
                Switch_Animation((_uint)CTest_Player::ANIM_STATE_2D::PLAYER_ATTACKCOMBO_03_DOWN);
                break;
            default:
                break;
            }
            break;
        case Camera_Tool::F_DIRECTION::F_DIR_LAST:
        default:
            break;
        }
    }
    else
    {
        switch (_iCombo)
        {
        case 0:
            Switch_Animation((_uint)CTest_Player::ANIM_STATE_3D::LATCH_ANIM_ATTACK_01_GT_EDIT);
            break;
        case 1:
            Switch_Animation((_uint)CTest_Player::ANIM_STATE_3D::LATCH_ANIM_ATTACK_02_GT);
            break;
        case 2:
            Switch_Animation((_uint)CTest_Player::ANIM_STATE_3D::LATCH_ANIM_ATTACK_03_GT);
            break;
        default:
            break;
        }
    }
}


void CTest_Player::Switch_Animation(_uint _iAnimIndex)
{
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(_iAnimIndex);
}

void CTest_Player::Set_State(STATE _eState)
{
    _uint iAnimIdx;
    switch (_eState)
    {
    case Camera_Tool::CTest_Player::IDLE:
        m_pStateMachine->Transition_To(new CPlayerState_Idle(this));
        break;
    case Camera_Tool::CTest_Player::RUN:
        m_pStateMachine->Transition_To(new CPlayerState_Run(this));
        break;
    case Camera_Tool::CTest_Player::JUMP:
        break;
    case Camera_Tool::CTest_Player::ATTACK:
        m_pStateMachine->Transition_To(new CPlayerState_Attack(this));
        break;
    case Camera_Tool::CTest_Player::STATE_LAST:
        break;
    default:
        break;
    }
}



void CTest_Player::Set_2DDirection(F_DIRECTION _eFDir)
{
    m_e2DDirection = _eFDir;
    if (F_DIRECTION::LEFT == m_e2DDirection)
    {
        _vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
        m_pControllerTransform->Set_State(CTransform::STATE_RIGHT, -XMVectorAbs(vRight));
    }
    else if (F_DIRECTION::RIGHT == m_e2DDirection)
    {
        _vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
        m_pControllerTransform->Set_State(CTransform::STATE_RIGHT, XMVectorAbs(vRight));
    }
}

void CTest_Player::Equip_Part(PLAYER_PART _ePartId)
{
    for (_int i = PLAYER_PART_SWORD; i < PLAYER_PART_LAST; ++i)
        Set_PartActive((_uint)i, false);
    Set_PartActive(_ePartId, true);
}

void CTest_Player::UnEquip_Part(PLAYER_PART _ePartId)
{
    Set_PartActive(_ePartId, false);
}



void CTest_Player::Key_Input(_float _fTimeDelta)
{
    if (KEY_DOWN(KEY::F1))
    {
        _int iCurCoord = (_int)Get_CurCoord();
        iCurCoord ^= 1;
        Change_Coordinate((COORDINATE)iCurCoord, _float3(0.0f, 0.0f, 0.0f));
    }
    if (KEY_DOWN(KEY::F2))
    {
        if (Get_PartObject(PLAYER_PART_SWORD)->Is_Active())
            UnEquip_Part(PLAYER_PART_SWORD);
        else
            Equip_Part(PLAYER_PART_SWORD);
    }
    if (KEY_DOWN(KEY::F4))
    {
        Event_DeleteObject(this);
    }

    if (KEY_DOWN(KEY::M))
    {
        static_cast<CModelObject*>(m_PartObjects[PART_BODY])->To_NextAnimation();
    }

}


CTest_Player* CTest_Player::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CTest_Player* pInstance = new CTest_Player(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CTest_Player");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTest_Player::Clone(void* _pArg)
{
    CTest_Player* pInstance = new CTest_Player(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CTest_Player");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTest_Player::Free()
{
    Safe_Release(m_pStateMachine);
    __super::Free();
}
