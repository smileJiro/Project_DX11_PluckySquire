#include "stdafx.h"
#include "Player.h"
#include "GameInstance.h"
#include "Camera_Manager.h"
#include "StateMachine.h"
#include "ModelObject.h"
#include "Controller_Transform.h"
#include "PlayerState_Idle.h"
#include "PlayerState_Run.h"
#include "PlayerState_Attack.h"

CPlayer::CPlayer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CContainerObject(_pDevice, _pContext)
{
}

CPlayer::CPlayer(const CPlayer& _Prototype)
    :CContainerObject(_Prototype)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPlayer::Initialize(void* _pArg)
{
    CPlayer::CONTAINEROBJ_DESC* pDesc = static_cast<CPlayer::CONTAINEROBJ_DESC*>(_pArg);
    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = true;
    pDesc->iNumPartObjects = PART_LAST;
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
    Set_Position(XMVectorSet(-3.f, 0.35f, -19.3f,1.f));
    return S_OK;
}

HRESULT CPlayer::Ready_Components()
{
    CStateMachine::STATEMACHINE_DESC tStateMachineDesc{};
    tStateMachineDesc.pOwner = this;

    m_pStateMachine = static_cast<CStateMachine*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"), &tStateMachineDesc));
    m_pStateMachine->Transition_To(new CPlayerState_Idle(this));
    return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
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

    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Register_OnAnimEndCallBack(bind(&CPlayer::On_AnimEnd, this, placeholders::_1, placeholders::_2));
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
void CPlayer::Priority_Update(_float _fTimeDelta)
{

    CContainerObject::Priority_Update(_fTimeDelta); /* Part Object Priority_Update */
}

void CPlayer::Update(_float _fTimeDelta)
{
    Key_Input(_fTimeDelta);
    m_pStateMachine->Update(_fTimeDelta);
    __super::Update(_fTimeDelta); /* Part Object Update */
}

void CPlayer::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta); /* Part Object Late_Update */
}

HRESULT CPlayer::Render()
{
    /* Model이 없는 Container Object 같은 경우 Debug 용으로 사용하거나, 폰트 렌더용으로. */

#ifdef _DEBUG

#endif // _DEBUG

    /* Font Render */

    return S_OK;
}

void CPlayer::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	m_pStateMachine->Get_CurrentState()->On_AnimEnd(_eCoord, iAnimIdx);
}

HRESULT CPlayer::Change_Coordinate(COORDINATE _eCoordinate, const _float3& _vPosition)
{
    if (FAILED(m_PartObjects[PART_BODY]->Change_Coordinate(_eCoordinate, _float3(0.0f, 0.0f, 0.0f))))
        return E_FAIL;
    return __super::Change_Coordinate(_eCoordinate, _vPosition);
}

void CPlayer::On_CoordinateChange()
{
    if (COORDINATE_2D == Get_CurCoord())
		Set_2DDirection(F_DIRECTION::DOWN);

	Set_State(IDLE);
}

void CPlayer::Move(_vector _vDir, _float _fTimeDelta)
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

void CPlayer::Attack(_uint _iCombo)
{
    COORDINATE eCoord = Get_CurCoord();

    if (COORDINATE_2D == eCoord)
    {
        F_DIRECTION eOldDir = Get_2DDirection();
        switch (eOldDir)
        {
        case Client::F_DIRECTION::LEFT:
        case Client::F_DIRECTION::RIGHT:
            switch (_iCombo)
            {
            case 0:
                Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACK_RIGHT);
                break;
			case 1:
				Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACKCOMBO_01_RIGHT);
				break;
            case 2:
                Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACKCOMBO_03_RIGHT);
                break;
            default:
                break;
            }
            break;
        case Client::F_DIRECTION::UP:
            switch (_iCombo)
            {
            case 0:
                Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACK_UP);
                break;
            case 1:
                Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACKCOMBO_01_UP);
                break;
            case 2:
                Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACKCOMBO_03_UP);
                break;
            default:
                break;
            }
            break;
        case Client::F_DIRECTION::DOWN:
            switch (_iCombo)
            {
            case 0:
                Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACK_DOWN);
                break;
            case 1:
                Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACKCOMBO_01_DOWN);
                break;
            case 2:
                Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACKCOMBO_03_DOWN);
                break;
            default:
                break;
            }
            break;
        case Client::F_DIRECTION::F_DIR_LAST:
        default:
            break;
        }
    }
    else
    {
        switch (_iCombo)
        {
        case 0:
            Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_ATTACK_01_GT_EDIT);
            break;
        case 1:
            Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_ATTACK_02_GT);
            break;
        case 2:
            Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_ATTACK_03_GT);
            break;
        default:
            break;
        }
    }
}


void CPlayer::Switch_Animation(_uint _iAnimIndex)
{
	static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(_iAnimIndex);
}

void CPlayer::Set_State(STATE _eState)
{
    _uint iAnimIdx;
    switch (_eState)
    {
    case Client::CPlayer::IDLE:
        m_pStateMachine->Transition_To(new CPlayerState_Idle(this));
        break;
    case Client::CPlayer::RUN:
        m_pStateMachine->Transition_To(new CPlayerState_Run(this));
        break;
    case Client::CPlayer::JUMP:
        break;
    case Client::CPlayer::ATTACK:
        m_pStateMachine->Transition_To(new CPlayerState_Attack(this));
        break;
    case Client::CPlayer::STATE_LAST:
        break;
    default:
        break;
    }
}



void CPlayer::Set_2DDirection(F_DIRECTION _eFDir)
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

void CPlayer::Key_Input(_float _fTimeDelta)
{
    if (KEY_DOWN(KEY::F1))
    {
        _int iCurCoord = (_int)Get_CurCoord();
        (_int)iCurCoord ^= 1;
        Change_Coordinate((COORDINATE)iCurCoord, _float3(0.0f, 0.0f, 0.0f));
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


CPlayer* CPlayer::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CPlayer* pInstance = new CPlayer(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CPlayer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayer::Clone(void* _pArg)
{
    CPlayer* pInstance = new CPlayer(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CPlayer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer::Free()
{
	Safe_Release(m_pStateMachine);
    __super::Free();
}
