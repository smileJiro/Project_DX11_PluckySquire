#include "stdafx.h"
#include "PlayerSword.h"
#include "Actor_Dynamic.h"
#include "Player.h"
#include "3DModel.h"
#include "GameInstance.h"
#include "Section_Manager.h"     
#include "Camera_Manager.h"


CPlayerSword::CPlayerSword(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CModelObject(_pDevice, _pContext)
    , m_eCurrentState(HANDLING)
    , m_ePastState(HANDLING)
{
}


CPlayerSword::CPlayerSword(const CPlayerSword& _Prototype)
    :CModelObject(_Prototype)
    , m_eCurrentState(HANDLING)
    , m_ePastState(HANDLING)
{
}

HRESULT CPlayerSword::Initialize(void* _pArg)
{
    //Part Sword
    PLAYER_SWORD_DESC* pDesc = static_cast<PLAYER_SWORD_DESC*>(_pArg);
    m_pPlayer = pDesc->pParent;
    pDesc->pParentMatrices[COORDINATE_3D] = m_pPlayer->Get_ControllerTransform()->Get_WorldMatrix_Ptr(COORDINATE_3D);
    C3DModel* p3DModel = static_cast<C3DModel*>(static_cast<CModelObject*>(m_pPlayer->Get_PartObject(CPlayer::PART_BODY))->Get_Model(COORDINATE_3D));
    Set_SocketMatrix(COORDINATE_3D, p3DModel->Get_BoneMatrix("j_glove_hand_attach_r"));

    pDesc->isCoordChangeEnable = true;
    pDesc->strModelPrototypeTag_3D = TEXT("latch_sword");
    pDesc->strModelPrototypeTag_2D = TEXT("player2dsword");
    pDesc->strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
    pDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
    pDesc->iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;
    pDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
    pDesc->tTransform2DDesc.vInitialPosition = _float3(0.0f, 1.0f, 0.0f);
    pDesc->tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);

    pDesc->eActorType = ACTOR_TYPE::KINEMATIC;
    CActor::ACTOR_DESC ActorDesc;

    /* Actor의 주인 오브젝트 포인터 */
    ActorDesc.pOwner = this;

    /* Actor의 회전축을 고정하는 파라미터 */
    ActorDesc.FreezeRotation_XYZ[0] = true;
    ActorDesc.FreezeRotation_XYZ[1] = false;
    ActorDesc.FreezeRotation_XYZ[2] = true;

    /* Actor의 이동축을 고정하는 파라미터 (이걸 고정하면 중력도 영향을 받지 않음. 아예 해당 축으로의 이동을 제한하는)*/
    ActorDesc.FreezePosition_XYZ[0] = false;
    ActorDesc.FreezePosition_XYZ[1] = true;
    ActorDesc.FreezePosition_XYZ[2] = false;


    /* 사용하려는 Shape의 형태를 정의 */
    SHAPE_SPHERE_DESC ShapeDesc = {};
    ShapeDesc.fRadius = 1.f;

    /* 해당 Shape의 Flag에 대한 Data 정의 */
    SHAPE_DATA ShapeData;
    ShapeData.pShapeDesc = &ShapeDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    ShapeData.eShapeType = SHAPE_TYPE::SPHERE;     // Shape의 형태.
    ShapeData.eMaterial = ACTOR_MATERIAL::NORESTITUTION;  // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    ShapeData.isTrigger = true;                    // Trigger 알림을 받기위한 용도라면 true
    XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixTranslation(0.0f, 0.f, 0.5f)); 

    /* 최종으로 결정 된 ShapeData를 PushBack */
    ActorDesc.ShapeDatas.push_back(ShapeData);

    /* 충돌 필터에 대한 세팅 ()*/
    ActorDesc.tFilterData.MyGroup = OBJECT_GROUP::PLAYER_PROJECTILE;
    ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::MONSTER | OBJECT_GROUP::PLAYER;

    /* Actor Component Finished */
    pDesc->pActorDesc = &ActorDesc;
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;
    //m_pActorCom-> Set_ShapeEnable(0, true);

       /* Test 2D Collider */
    m_p2DColliderComs.resize(1);
    CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
    CircleDesc.pOwner = this;
    CircleDesc.fRadius = 40.f;
    CircleDesc.vScale = { 1.0f, 1.0f };
    CircleDesc.vOffsetPosition = { 0.f, 0.f };
    CircleDesc.isBlock = false;
    CircleDesc.isTrigger = true;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
        TEXT("Com_2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &CircleDesc)))
        return E_FAIL;
	m_pBody2DColliderCom = m_p2DColliderComs[0];
	Safe_AddRef(m_pBody2DColliderCom);
    return S_OK;
}

void CPlayerSword::Update(_float _fTimeDelta)
{
    if (m_eCurrentState != m_ePastState)
        On_StateChange();
    COORDINATE eCoord = Get_CurCoord();
    //if (COORDINATE_2D == eCoord)
    //{
    //    _uint iSectionKey = RG_2D + PR2D_SECTION_START;
    //    if (m_pBody2DColliderCom->Is_Active())
    //        m_pGameInstance->Add_Collider(m_strSectionName, OBJECT_GROUP::PLAYER_PROJECTILE, m_pBody2DColliderCom);
    //}
    switch (m_eCurrentState)
    {
    case Client::CPlayerSword::HANDLING:
        break;
    case Client::CPlayerSword::FLYING:
    {
        _bool bOuting = m_fOutingForce > 0;
        m_fOutingForce -= (COORDINATE_3D == eCoord ? m_fCentripetalForce3D : m_fCentripetalForce2D) * _fTimeDelta;
        _vector vDir = m_vThrowDirection;
        _vector vPosition = Get_ControllerTransform()->Get_State(CTransform::STATE_POSITION);
        _vector vTargetPos = m_pPlayer->Get_CenterPosition();
        if (m_fOutingForce < 0)
        {
            if (bOuting)
            {
                m_AttckedObjects.clear();
                //cout << "m_AttckedObjects clear Update" << endl;
            }
            vDir = XMVector3Normalize(vTargetPos - vPosition);
        }
        if (COORDINATE_3D == eCoord)
        {
            CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);

            _float3 vAngularVelocity = { 0, -m_fRotationForce3D, 0 };
            m_pActorCom->Set_AngularVelocity(vAngularVelocity);
            XMVectorSetY(vTargetPos, XMVectorGetY(vTargetPos) + 0.5f);
            pDynamicActor->Set_LinearVelocity(vDir * abs(m_fOutingForce));
        }
        else
        {
            m_pControllerTransform->Go_Direction(vDir, abs(m_fOutingForce), _fTimeDelta);
        }

        break;
    }
    case Client::CPlayerSword::STUCK:
    {
        if (MOUSE_DOWN(MOUSE_KEY::RB))
        {
            Set_State(FLYING);
        }

        break;
    }
    default:
        break;
    }
    //cout << m_pActorCom->Get_Shapes()[0]->getActor() << endl;

    __super::Update(_fTimeDelta);
}

void CPlayerSword::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);
}

HRESULT CPlayerSword::Render()
{
#ifdef _DEBUG
    if (m_pBody2DColliderCom->Is_Active())
        m_pBody2DColliderCom->Render();
#endif
    if(Is_Active())
        return __super::Render();
    return S_OK;
}

void CPlayerSword::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    if (OBJECT_GROUP::PLAYER == _Other.pActorUserData->iObjectGroup)
    {
        if (Is_Flying() && Is_ComingBack())
        {
            Set_State(HANDLING);
        }
    }
    else
    {
        if (OBJECT_GROUP::MAPOBJECT == _Other.pActorUserData->iObjectGroup)
        {
            if (Is_Flying() && Is_Outing())
            {
                m_fOutingForce = 0;
                m_vStuckDirection = XMVectorSetW(XMVector3Normalize(_Other.pActorUserData->pOwner->Get_FinalPosition() - Get_FinalPosition()), 0);
                Set_State(STUCK);
            }
        }
        else if (OBJECT_GROUP::MONSTER == _Other.pActorUserData->iObjectGroup)
		{
            if (SHAPE_USE::SHAPE_BODY == (SHAPE_USE)_Other.pShapeUserData->iShapeUse)
            {
            }

        }
    }
}

void CPlayerSword::OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    if (OBJECT_GROUP::MONSTER == _Other.pActorUserData->iObjectGroup)
    {
        if (SHAPE_USE::SHAPE_BODY == (SHAPE_USE)_Other.pShapeUserData->iShapeUse)
        {
            if(m_bAttackEnable)
                Attack(_Other.pActorUserData->pOwner);
        }
    }
}

void CPlayerSword::OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    if (OBJECT_GROUP::MONSTER == _Other.pActorUserData->iObjectGroup)
    {
        if (SHAPE_USE::SHAPE_BODY == (SHAPE_USE)_Other.pShapeUserData->iShapeUse)
        {
        }
    }
}

void CPlayerSword::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	OBJECT_GROUP eGroup = (OBJECT_GROUP)_pOtherCollider->Get_CollisionGroupID();
    if (OBJECT_GROUP::PLAYER == eGroup)
    {
        if (Is_ComingBack())
        {
            Set_State(HANDLING);
        }
    }
    else
    {
        if (OBJECT_GROUP::INTERACTION_OBEJCT == eGroup || OBJECT_GROUP::MAPOBJECT == eGroup)
        {
            if ( Is_Outing())
            {
                m_fOutingForce = 0;
                m_vStuckDirection = XMVectorSetW(XMVector3Normalize(_pOtherObject->Get_FinalPosition() - Get_FinalPosition()), 0);
                Set_State(STUCK);
            }
        }
    
    }
}

void CPlayerSword::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    if (OBJECT_GROUP::MONSTER == _pOtherCollider->Get_CollisionGroupID())
    {
        Attack(_pOtherObject);
    }
}

void CPlayerSword::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

HRESULT CPlayerSword::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{
    if (FAILED(__super::Change_Coordinate(_eCoordinate, _pNewPosition)))
        return E_FAIL;

    if (COORDINATE_2D == Get_CurCoord())
    {
        Set_Active(false);
        m_strSectionName = m_pPlayer->Get_Include_Section_Name();
    }
    else
    {
        Set_Active(m_pPlayer->Is_SneakMode());
    }
    return S_OK;
}

void CPlayerSword::Throw(_fvector _vDirection)
{
    COORDINATE eCoord = m_pPlayer->Get_CurCoord();
    m_fOutingForce = COORDINATE_3D == eCoord ? m_fThrowingPower3D : m_fThrowingPower2D;

    m_vThrowDirection = _vDirection;
    Set_State(FLYING);
    if (COORDINATE_2D == eCoord)
    {
        Set_Position(m_pPlayer->Get_CenterPosition());
        Set_Active(true);
		m_pControllerModel->Get_Model(COORDINATE_2D)->Set_Animation(2);
    }
}



void CPlayerSword::Switch_Grip(_bool _bForehand)
{
    if (_bForehand)
        m_pControllerTransform->Rotation(XMConvertToRadians(180.f), _vector{ 0.f,1.f,0.f,0.f });
    else
        m_pControllerTransform->Rotation(XMConvertToRadians(0.f), _vector{ 0.f,1.f,0.f,0.f });
}

void CPlayerSword::Set_State(SWORD_STATE _eNewState)
{
    m_eCurrentState = _eNewState;
}

void CPlayerSword::On_StateChange()
{
    COORDINATE eCoord = Get_CurCoord();
   // cout << "SwrodState : " << m_eCurrentState << endl;
    switch (m_eCurrentState)
    {
    case Client::CPlayerSword::HANDLING:
    {
        m_fOutingForce = 0.f;
        if (COORDINATE_3D == eCoord)
        {
            Set_ParentMatrix(eCoord, m_pPlayer->Get_ControllerTransform()->Get_WorldMatrix_Ptr(COORDINATE_3D));
            C3DModel* p3DModel = static_cast<C3DModel*>(static_cast<CModelObject*>(m_pPlayer->Get_PartObject(CPlayer::PART_BODY))->Get_Model(COORDINATE_3D));
            Set_SocketMatrix(COORDINATE_3D,p3DModel->Get_BoneMatrix("j_glove_hand_attach_r"));
            _matrix matWorld = XMMatrixIdentity() * XMMatrixRotationY(XMConvertToRadians(180));
            m_pControllerTransform->Set_WorldMatrix(matWorld);
            static_cast<CActor_Dynamic*>(m_pActorCom)->Set_Kinematic();
        }
        else
        {
             Set_Active(false);
        }
        Set_AttackEnable(false);
        break;
    }
    case Client::CPlayerSword::FLYING:
    {
        if (COORDINATE_3D == eCoord)
        {
            m_pSocketMatrix [COORDINATE_3D] = nullptr;
            m_pParentMatrices[COORDINATE_3D] = nullptr;
            m_pActorCom->Update(0);
            _vector vLook = XMVectorSetY(m_pControllerTransform->Get_State(CTransform::STATE_LOOK), 0);
            static_cast<CActor_Dynamic*>(m_pActorCom)->Set_Dynamic();
            static_cast<CActor_Dynamic*>(m_pActorCom)->Set_Rotation(vLook);
        }
        else
        {
            Switch_Animation(2);
        }
        Set_AttackEnable(true);
        break;
    }
    case Client::CPlayerSword::STUCK:
    {
        if (COORDINATE_3D == eCoord)
        {
            CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);
            _float3 vSpeed = { 0.f,0.f,0.f };
            pDynamicActor->Set_LinearVelocity({ 0.f,0.f,0.f });
            pDynamicActor->Set_AngularVelocity(vSpeed);
            pDynamicActor->Set_Rotation(m_vStuckDirection);
            static_cast<CActor_Dynamic*>(m_pActorCom)->Late_Update(0);
            static_cast<CActor_Dynamic*>(m_pActorCom)->Set_Kinematic();
        }
        else
        {
            Switch_Animation(0);
            static_cast<CTransform_2D*>(m_pControllerTransform->Get_Transform())->LookDir(m_vStuckDirection);

        }
        Set_AttackEnable(false);
        break;
    }
    default:
        break;
    }
    m_ePastState = m_eCurrentState;
}

void CPlayerSword::Attack(CGameObject* _pVictim)
{
    if (m_AttckedObjects.find(_pVictim) != m_AttckedObjects.end())
        return;
    Event_Hit(this, _pVictim, m_pPlayer->Get_AttackDamg());
    CActorObject* pActor = dynamic_cast<CActorObject*>(_pVictim);
    if (pActor)
    {
        Event_AddImpulse(pActor, Get_LookDirection(), m_f3DKnockBackPower);
    }
    m_AttckedObjects.insert(_pVictim);
}

void CPlayerSword::Set_AttackEnable(_bool _bOn)
{

	if (false == _bOn)
    {
        m_AttckedObjects.clear();
    }
    if (COORDINATE_3D == Get_CurCoord())
        m_pActorCom->Set_ShapeEnable(0, _bOn);
    else
        m_pBody2DColliderCom->Set_Active(_bOn);
    m_bAttackEnable = _bOn;
}

_bool CPlayerSword::Is_AttackEnable()
{
    return    m_bAttackEnable;
}

_vector CPlayerSword::Get_LookDirection()
{

    if(Is_Outing())
        return m_vThrowDirection;
    else if(Is_ComingBack())
    {
        _vector vPosition = Get_ControllerTransform()->Get_State(CTransform::STATE_POSITION);
        _vector vTargetPos = m_pPlayer->Get_CenterPosition();
        return  XMVector3Normalize(vTargetPos - vPosition);
    }
    else
    {
        COORDINATE eCoord = Get_CurCoord();
		return m_pPlayer->Get_LookDirection(eCoord);
    }
}

CPlayerSword* CPlayerSword::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CPlayerSword* pInstance = new CPlayerSword(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : PlayerSword");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayerSword::Clone(void* _pArg)
{
    CPlayerSword* pInstance = new CPlayerSword(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : PlayerSword");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayerSword::Free()
{
	Safe_Release(m_pBody2DColliderCom);
    __super::Free();
}
