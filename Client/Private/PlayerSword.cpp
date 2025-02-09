#include "stdafx.h"
#include "PlayerSword.h"
#include "Actor_Dynamic.h"
#include "Player.h"
#include "3DModel.h"
#include "GameInstance.h"

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
    Set_SocketMatrix(p3DModel->Get_BoneMatrix("j_glove_hand_attach_r"));

    pDesc->isCoordChangeEnable = false;
    pDesc->strModelPrototypeTag_3D = TEXT("latch_sword");
    pDesc->strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
    pDesc->iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;
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
    ActorDesc.FreezePosition_XYZ[2]  = false;

    /* 사용하려는 Shape의 형태를 정의 */
    SHAPE_SPHERE_DESC ShapeDesc = {};
    ShapeDesc.fRadius = 0.5f;

    /* 해당 Shape의 Flag에 대한 Data 정의 */
    SHAPE_DATA ShapeData;
    ShapeData.pShapeDesc = &ShapeDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    ShapeData.eShapeType = SHAPE_TYPE::SPHERE;     // Shape의 형태.
    ShapeData.eMaterial = ACTOR_MATERIAL::DEFAULT;  // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    ShapeData.isTrigger = true;                    // Trigger 알림을 받기위한 용도라면 true
    XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixIdentity()); // Shape의 LocalOffset을 행렬정보로 저장.

    /* 최종으로 결정 된 ShapeData를 PushBack */
    ActorDesc.ShapeDatas.push_back(ShapeData);

    /* 충돌 필터에 대한 세팅 ()*/
    ActorDesc.tFilterData.MyGroup = OBJECT_GROUP::PLAYER_PROJECTILE;
    ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::MONSTER | OBJECT_GROUP::PLAYER;

    /* Actor Component Finished */
    pDesc->pActorDesc = &ActorDesc;
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;
    m_pActorCom-> Set_ShapeEnable(0, true);
    return S_OK;
}

void CPlayerSword::Update(_float _fTimeDelta)
{
	if (m_eCurrentState != m_ePastState)
		On_StateChange();

    switch (m_eCurrentState)
    {
    case Client::CPlayerSword::HANDLING:
        break;
    case Client::CPlayerSword::FLYING:
    {
        m_fOutingForce -= m_fCentripetalForce * _fTimeDelta;
        _vector vDir = m_vThrowDirection;
        CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);
        if (m_fOutingForce < 0)
        {
            _vector vPosition = Get_ControllerTransform()->Get_State(CTransform::STATE_POSITION);
            _vector vTargetPos = m_pPlayer->Get_CenterPosition();
            XMVectorSetY(vTargetPos, XMVectorGetY(vTargetPos) + 0.5f);
            vDir = XMVector3Normalize(vTargetPos - vPosition);
        }
        _float3 vAngularVelocity = { 0, -m_fRotationForce, 0 };
        m_pActorCom->Set_AngularVelocity(vAngularVelocity);
        pDynamicActor->Set_LinearVelocity(vDir * abs(m_fOutingForce));
        break;
    }
    case Client::CPlayerSword::STUCK:
    {
        CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);
        if (MOUSE_DOWN(MOUSE_KEY::RB))
        {
			Set_State(FLYING);
        }
        break;
    }
    default:
        break;
    }


	__super::Update(_fTimeDelta);
}

void CPlayerSword::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

void CPlayerSword::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    if (OBJECT_GROUP::PLAYER == _Other.pActorUserData->iObjectGroup)
    {
        if (Is_Flying()&& Is_ComingBack())
        {
            Set_State(HANDLING);
            Set_AttackEnable(false);
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
        cout << "_Other.pActorUserData->pOwner" << _Other.pActorUserData->pOwner->Get_GameObjectInstanceID() << endl;
		Event_Hit(m_pPlayer,_Other.pActorUserData->pOwner, m_pPlayer->Get_AttackDamg());
    }
}

void CPlayerSword::OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    int a = 0;
}

void CPlayerSword::OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    int a = 0;
}

void CPlayerSword::Throw( _fvector _vDirection)
{
    m_fOutingForce = m_fThrowingPower;
    m_vThrowDirection = _vDirection;
	Set_State(FLYING);
    Set_AttackEnable(true);
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
    switch (m_eCurrentState)
    {
    case Client::CPlayerSword::HANDLING:
    {
        m_fOutingForce = 0.f;
        COORDINATE eCoord = Get_CurCoord();
        if (COORDINATE_3D == eCoord)
        {
            Set_ParentMatrix(eCoord, m_pPlayer->Get_ControllerTransform()->Get_WorldMatrix_Ptr(COORDINATE_3D));
            C3DModel* p3DModel = static_cast<C3DModel*>(static_cast<CModelObject*>(m_pPlayer->Get_PartObject(CPlayer::PART_BODY))->Get_Model(COORDINATE_3D));
            Set_SocketMatrix(p3DModel->Get_BoneMatrix("j_glove_hand_attach_r"));
            _matrix matWorld = XMMatrixIdentity() * XMMatrixRotationY(XMConvertToRadians(180));
            m_pControllerTransform->Set_WorldMatrix(matWorld);
            //_float3 vAngularVelocity = { 0, -m_fRotationForce, 0 };
            //m_pActorCom->Set_AngularVelocity(vAngularVelocity);

            static_cast<CActor_Dynamic*>(m_pActorCom)->Set_Kinematic();
           // m_pActorCom->Update(0);
        }

        break;
    }
    case Client::CPlayerSword::FLYING:
    {

        m_pSocketMatrix = nullptr;
        m_pParentMatrices[COORDINATE_3D] = nullptr;
        _matrix matWorld = XMMatrixIdentity(); 
        matWorld.r[3] = XMLoadFloat4x4(&m_WorldMatrices[COORDINATE_3D]).r[3];
        XMStoreFloat4x4(&m_WorldMatrices[COORDINATE_3D], matWorld);
        m_pActorCom->Update(0.0f);
        static_cast<CActor_Dynamic*>(m_pActorCom)->Set_Dynamic();
        break;
    }
    case Client::CPlayerSword::STUCK:
    {
        COORDINATE eCoord = Get_CurCoord();
        if (COORDINATE_3D == eCoord)
        {
            CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);
            _float3 vSpeed = { 0.f,0.f,0.f };
            pDynamicActor->Set_LinearVelocity({ 0.f,0.f,0.f });
            pDynamicActor->Set_AngularVelocity(vSpeed);
            pDynamicActor->Set_Rotation(m_vStuckDirection);
        }

        break;
    }

    default:
        break;
    }
    m_ePastState = m_eCurrentState;
}

void CPlayerSword::Set_AttackEnable(_bool _bOn)
{
    //m_pActorCom->Set_ShapeEnable(0, _bOn);
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
    __super::Free();
}
