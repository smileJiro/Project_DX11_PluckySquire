#include "stdafx.h"
#include "PlayerSword.h"
#include "Actor_Dynamic.h"
#include "Player.h"
#include "3DModel.h"
#include "GameInstance.h"
#include "Section_Manager.h"     
#include "Camera_Manager.h"
#include "Effect_Manager.h"
#include "Effect_Trail.h"
#include "Effect_Beam.h"

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

HRESULT CPlayerSword::Initialize_Prototype()
{


    return S_OK;
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
    pDesc->iObjectGroupID = OBJECT_GROUP::PLAYER_PROJECTILE;

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


    //일반 공격용 트리거
    SHAPE_SPHERE_DESC ShapeDesc = {};
    ShapeDesc.fRadius = m_f3DNormalAttackRange;
    SHAPE_DATA ShapeData;
    ShapeData.pShapeDesc = &ShapeDesc;       
    ShapeData.eShapeType = SHAPE_TYPE::SPHERE;  
    ShapeData.eMaterial = ACTOR_MATERIAL::NORESTITUTION;  
	ShapeData.iShapeUse = (_uint)SHAPE_USE::SHAPE_BODY;
    ShapeData.isTrigger = true;                    
    XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixTranslation(0.0f, 0.f, m_f3DNormalAttackZOffset));
    ActorDesc.ShapeDatas.push_back(ShapeData);

    //점프 공격용 트리거
    SHAPE_SPHERE_DESC ShapeDesc2 = {};
    ShapeDesc2.fRadius = m_f3DJumpAttackRange;
    ShapeData.pShapeDesc = &ShapeDesc2;
    ShapeData.eShapeType = SHAPE_TYPE::SPHERE;
    ShapeData.eMaterial = ACTOR_MATERIAL::NORESTITUTION;
    ShapeData.iShapeUse = (_uint)SHAPE_USE::SHAPE_BODY;
    ShapeData.isTrigger = true;
    XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixTranslation(0.0f, 0.f, m_f3DJumpAttackZOffset));
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
    CircleDesc.iCollisionGroupID = OBJECT_GROUP::PLAYER_PROJECTILE;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
        TEXT("Com_2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &CircleDesc)))
        return E_FAIL;
	m_pBody2DColliderCom = m_p2DColliderComs[0];
	Safe_AddRef(m_pBody2DColliderCom);

    // Trail Effect 생성
    CEffect_Trail::EFFECTTRAIL_DESC SwordTrailDesc = {};
    SwordTrailDesc.eStartCoord = COORDINATE_3D;
    SwordTrailDesc.iCurLevelID = m_iCurLevelID;
    SwordTrailDesc.isCoordChangeEnable = false;
    SwordTrailDesc.pParentMatrices[COORDINATE_3D] = &m_WorldMatrices[COORDINATE_3D];
    SwordTrailDesc.fLength = 0.8f;
    SwordTrailDesc.vAddPoint = _float3(0.f, 0.f, -0.85f);
    SwordTrailDesc.vColor = _float4(0.62f, 0.82f, 1.33f, 0.85f);
    SwordTrailDesc.szTextureTag = L"Prototype_Component_Texture_Trail";
    SwordTrailDesc.szBufferTag = L"Prototype_Component_VIBuffer_Trail32";
    SwordTrailDesc.fAddTime = 0.015f;
    SwordTrailDesc.fTrailLifeTime = 0.25f;

    m_pTrailEffect = static_cast<CEffect_Trail*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_EffectTrail"), &SwordTrailDesc));

    if (nullptr == m_pTrailEffect)
        return E_FAIL;

    // Throw Trail
    CEffect_System::EFFECT_SYSTEM_DESC EffectSystemDesc = {};
    EffectSystemDesc.iCurLevelID = m_iCurLevelID;
    EffectSystemDesc.isCoordChangeEnable = false;
    EffectSystemDesc.iSpriteShaderLevel = LEVEL_STATIC;
    EffectSystemDesc.szSpriteShaderTags = L"Prototype_Component_Shader_VtxPointInstance";
    
    EffectSystemDesc.iModelShaderLevel = LEVEL_STATIC;
    EffectSystemDesc.szModelShaderTags = L"Prototype_Component_Shader_VtxMeshInstance";
    
    EffectSystemDesc.iEffectShaderLevel = LEVEL_STATIC;
    EffectSystemDesc.szEffectShaderTags = L"Prototype_Component_Shader_VtxMeshEffect";
    
    EffectSystemDesc.iSingleSpriteShaderLevel = LEVEL_STATIC;
    EffectSystemDesc.szSingleSpriteShaderTags = L"Prototype_Component_Shader_VtxPoint";
    EffectSystemDesc.iSingleSpriteBufferLevel = LEVEL_STATIC;
    EffectSystemDesc.szSingleSpriteBufferTags = L"Prototype_Component_VIBuffer_Point";
    
    EffectSystemDesc.szSpriteComputeShaderTag = L"Prototype_Component_Compute_Shader_SpriteInstance";
    EffectSystemDesc.szMeshComputeShaderTag = L"Prototype_Component_Compute_Shader_MeshInstance";

    m_pThrowTrailEffect = static_cast<CEffect_System*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("SwordThrowTrail2.json"), &EffectSystemDesc));
    if (nullptr == m_pThrowTrailEffect)
        return E_FAIL;
    m_pThrowTrailEffect->Set_ParentMatrix(COORDINATE_3D, &m_WorldMatrices[COORDINATE_3D]);

    // Pullback Beam
    CEffect_Beam::EFFECTBEAM_DESC EffectBeamDesc = {};
    EffectBeamDesc.iCurLevelID = m_iCurLevelID;
    EffectBeamDesc.isCoordChangeEnable = false;
    EffectBeamDesc.eStartCoord = COORDINATE_3D;
    EffectBeamDesc.szBufferTag = L"Prototype_Component_VIBuffer_Beam16";
    EffectBeamDesc.szTextureTag = L"Prototype_Component_Texture_Grad04";
    EffectBeamDesc.vRandomMin = _float3(-0.5f, -0.5f, 0.f);
    EffectBeamDesc.vRandomMax = _float3(0.5f, 0.5f, 0.f);
    EffectBeamDesc.fWidth = 0.08f;
    EffectBeamDesc.vColor = _float4(0.67f, 0.789f, 0.88f, 5.f);
    EffectBeamDesc.isConverge = true;
    EffectBeamDesc.isRenderPoint = false;
    EffectBeamDesc.fConvergeSpeed = 4.f;
    //EffectBeamDesc.fPointSize = 0.01f;
    //EffectBeamDesc.vPointColor = _float4(0.1f, 1.f, 0.6f, 3.f);
    //EffectBeamDesc.szPointTextureTag = L"Prototype_Component_Texture_Glow01";
    m_pBeamEffect = static_cast<CEffect_Beam*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_EffectBeam"), &EffectBeamDesc));
    if (nullptr == m_pBeamEffect)
        return E_FAIL;
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
    case Client::CPlayerSword::OUTING:
    {
        m_fFlyingTimeAcc += _fTimeDelta;
        if (m_fFlyingTimeAcc > m_fOutingTime) // Outing 끝.
        {
            Set_AttackEnable(false);
            Set_AttackEnable(true);
            Set_State(RETRIEVING);
            break;
        }
        if (COORDINATE_3D == eCoord)
        {
            CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);

            _float3 vAngularVelocity = { 0, -m_fRotationForce3D, 0 };
            m_pActorCom->Set_AngularVelocity(vAngularVelocity);
            pDynamicActor->Set_LinearVelocity(m_vThrowDirection * m_fFlyingSpeed3D);
            m_pThrowTrailEffect->Update(_fTimeDelta);
        }
        else
        {
            m_pControllerTransform->Go_Direction(m_vThrowDirection, abs(m_fFlyingSpeed2D), _fTimeDelta);
        }
        break;
    }
    case Client::CPlayerSword::RETRIEVING:
    {
        m_fFlyingTimeAcc += _fTimeDelta;

		_vector vMyPos = Get_FinalPosition();
		_vector vTargetPos = m_pPlayer->Get_CenterPosition();
        _vector vDiff = vTargetPos - vMyPos;
		_vector vDir = XMVector3Normalize(vDiff);
		_float fDistance = XMVectorGetX(XMVector3Length(vDiff));
        if (COORDINATE_3D == eCoord)
        {
            CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);

            _float3 vAngularVelocity = { 0, -m_fRotationForce3D, 0 };
            m_pActorCom->Set_AngularVelocity(vAngularVelocity);
            pDynamicActor->Set_LinearVelocity(vDir * m_fFlyingSpeed3D);
            m_pThrowTrailEffect->Update(_fTimeDelta);

            if (0.15f < m_fFlyingTimeAcc)
            {
                m_pBeamEffect->Set_Active(false);
            }
        }
        else
        {
            m_pControllerTransform->Go_Direction(vDir, abs(m_fFlyingSpeed2D), _fTimeDelta);
        }

		_float fRetriveRange = (COORDINATE_3D == eCoord) ? m_fRetriveRange3D : m_fRetriveRange2D;
		if (fDistance < fRetriveRange)
		{
			Set_State(HANDLING);
            if (COORDINATE_3D == Get_CurCoord())
            {
                m_pThrowTrailEffect->Stop_Spawn(0.5f);
                m_pBeamEffect->Set_Active(false);
            }
		}

        break;
    }
    case Client::CPlayerSword::STUCK:
    {
        if (MOUSE_DOWN(MOUSE_KEY::RB))
        {
            m_pThrowTrailEffect->Active_Effect();
            Set_State(RETRIEVING);

            m_pBeamEffect->Set_StartPosition(m_pPlayer->Get_CenterPosition());
            m_pBeamEffect->Set_EndPosition(XMLoadFloat3((_float3*)&m_WorldMatrices[COORDINATE_3D].m[3]));
            m_pBeamEffect->Set_Active(true);
        }

        break;
    }
    default:
        break;
    }
    //cout << m_pActorCom->Get_Shapes()[0]->getActor() << endl;

    __super::Update(_fTimeDelta);

    // Trail Effect 정점 갱신
    if (COORDINATE_3D == Get_CurCoord())
    {
        m_pTrailEffect->Update(_fTimeDelta);
        m_pBeamEffect->Update(_fTimeDelta);
    }
}

void CPlayerSword::Late_Update(_float _fTimeDelta)
{
    if (COORDINATE_3D == Get_CurCoord())
    {
        if (false == m_isFrustumCulling)
            m_pGameInstance->Add_RenderObject_New(m_iRenderGroupID_3D, PR3D_PLAYERDEPTH, this);
    }

    __super::Late_Update(_fTimeDelta);

    // Effect Update
    if (COORDINATE_3D == Get_CurCoord())
    {
        m_pTrailEffect->Late_Update(_fTimeDelta);
        m_pThrowTrailEffect->Late_Update(_fTimeDelta);
        m_pBeamEffect->Late_Update(_fTimeDelta);
    }
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
        //if (Is_ComingBack())
        //{
        //    Set_State(HANDLING);
        //}
    }
    else
    {
        if (OBJECT_GROUP::MAPOBJECT == _Other.pActorUserData->iObjectGroup || OBJECT_GROUP::BLOCKER == _Other.pActorUserData->iObjectGroup)
        {
            if (Is_Outing())
            {
                m_fFlyingTimeAcc = 0.f;
                m_vStuckDirection = XMVectorSetW(XMVector3Normalize(_Other.pActorUserData->pOwner->Get_FinalPosition() - Get_FinalPosition()), 0);
                Set_State(STUCK);

                m_pGameInstance->Start_SFX(_wstring(L"A_sfx_sword_stick-") + to_wstring(rand() % 3), 30.f);
                CEffect_Manager::GetInstance()->Active_Effect(TEXT("SwordThrowBlock"), true, &m_WorldMatrices[COORDINATE_3D]);

                if (COORDINATE_3D == Get_CurCoord())
                    m_pThrowTrailEffect->Stop_Spawn(0.5f);
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
        //if (Is_ComingBack())
        //{
        //    Set_State(HANDLING);
        //}
    }
    else
    {
    }
}

void CPlayerSword::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    if (OBJECT_GROUP::MONSTER == _pOtherCollider->Get_CollisionGroupID())
    {
        m_pGameInstance->Start_SFX(_wstring(L"A_Sfx_Sword_Impact_Body_") + to_wstring(rand() % 3), 50.f);
        Attack(_pOtherObject);
    }
    
    OBJECT_GROUP eGroup = (OBJECT_GROUP)_pOtherCollider->Get_CollisionGroupID();
    if (OBJECT_GROUP::MAPOBJECT == eGroup || OBJECT_GROUP::BLOCKER == eGroup)
    {
        if (Is_Outing())
        {
            m_fFlyingTimeAcc = 0;
            _float2 vMyPos = (_pMyCollider->Get_Position());
            _float2 vOtherPos = (_pOtherCollider->Get_Position());
            if (_pOtherCollider->Is_ContainsPoint(vMyPos))
            {
                CCollider::TYPE eType = _pOtherCollider->Get_Type();
                if (CCollider::TYPE::AABB_2D == eType)
                {
                    _float2 vLT = static_cast<CCollider_AABB*>(_pOtherCollider)->Get_LT();
                    _float2 vRB = static_cast<CCollider_AABB*>(_pOtherCollider)->Get_RB();
                    _float2 vMin = { vLT.x, vRB.y }, vMax = { vRB.x, vLT.y };
                    _float fDiagSlope1 = (vMax.y - vMin.y) / (vMax.x - vMin.x);
                    _float fDiagSlope2 = -fDiagSlope1;
                    _float fCollisionSlope = (vOtherPos.y - vMyPos.y) / (vOtherPos.x - vMyPos.x);
                    m_vStuckPosition = XMVectorClamp(XMLoadFloat2(&vMyPos), XMLoadFloat2(&vMin), XMLoadFloat2(&vMax));
                    m_vStuckPosition = XMVectorSetW(m_vStuckPosition, 1);
                    m_vStuckPosition = XMVectorSetZ(m_vStuckPosition, 0);
                    //좌우충돌
                    if (abs(fCollisionSlope) < abs(fDiagSlope1))
                    {
                        //좌
                        if (vMyPos.x < vOtherPos.x)
                            m_vStuckDirection = _vector{ 1.f,0.f,0.f,0.f };
                        //우
                        else
                            m_vStuckDirection = _vector{ -1.f,0.f,0.f,0.f };
                    }
                    //상하 충돌
                    else
                    {
                        //상
                        if (vMyPos.y > vOtherPos.y)
                            m_vStuckDirection = _vector{ 0.f,-1.f,0.f,0.f };
                        //우
                        else
                            m_vStuckDirection = _vector{ 0.f,1.f,0.f,0.f };
                    }
                }
                else if (CCollider::TYPE::CIRCLE_2D == eType)
                {
                    _float fRadius = static_cast<CCollider_Circle*>(_pOtherCollider)->Get_FinalRadius();
                    m_vStuckDirection = XMVectorSetW(XMVector3Normalize(XMLoadFloat2(&vOtherPos) - XMLoadFloat2(&vMyPos)), 0);
                    m_vStuckPosition = XMLoadFloat2(&vOtherPos) - m_vStuckDirection * fRadius;
                    m_vStuckPosition = XMVectorSetW(m_vStuckPosition, 1);
                    m_vStuckPosition = XMVectorSetZ(m_vStuckPosition, 0);
                }
                Set_State(STUCK);
            }

        }
    }

}

void CPlayerSword::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}


void CPlayerSword::Active_OnEnable()
{
    COORDINATE eCoord = Get_CurCoord();
    if (COORDINATE_2D == eCoord)
    {
		m_pBody2DColliderCom->Set_Active(true);
    }
    else
    {
		m_pBody2DColliderCom->Set_Active(false);
    }
}

void CPlayerSword::Active_OnDisable()
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
        Set_Active(m_pPlayer->Is_SwordMode());
    }
    return S_OK;
}

void CPlayerSword::Throw(_fvector _vDirection)
{
    COORDINATE eCoord = m_pPlayer->Get_CurCoord();
    m_fFlyingTimeAcc = 0;

    m_vThrowDirection = _vDirection;
    Set_State(OUTING);
    if (COORDINATE_2D == eCoord)
    {
        Set_Position(m_pPlayer->Get_CenterPosition());
        Set_Active(true);
		m_pControllerModel->Get_Model(COORDINATE_2D)->Set_Animation(2);  
    }
    if (COORDINATE_3D == Get_CurCoord())
    {
        m_pThrowTrailEffect->Active_Effect();
        m_pTrailEffect->Delete_Effect();
    }
}



void CPlayerSword::Switch_Grip(_bool _bForehand)
{
    if (_bForehand)
        m_pControllerTransform->Rotation(XMConvertToRadians(0.f), _vector{ 0.f,1.f,0.f,0.f });
    else
        m_pControllerTransform->Rotation(XMConvertToRadians(180.f), _vector{ 0.f,1.f,0.f,0.f });
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
        m_fFlyingTimeAcc = 0.f;
        if (COORDINATE_3D == eCoord)
        {
            Set_ParentMatrix(eCoord, m_pPlayer->Get_ControllerTransform()->Get_WorldMatrix_Ptr(COORDINATE_3D));
            C3DModel* p3DModel = static_cast<C3DModel*>(static_cast<CModelObject*>(m_pPlayer->Get_PartObject(CPlayer::PART_BODY))->Get_Model(COORDINATE_3D));
            Set_SocketMatrix(COORDINATE_3D,p3DModel->Get_BoneMatrix("j_glove_hand_attach_r"));
            _matrix matWorld = XMMatrixIdentity() * XMMatrixRotationY(XMConvertToRadians(180));
            m_pControllerTransform->Set_WorldMatrix(matWorld);
            static_cast<CActor_Dynamic*>(m_pActorCom)->Set_Kinematic();
            Switch_Grip(true);
        }
        else
        {
             Set_Active(false);
        }

        m_pGameInstance->Start_SFX(_wstring(L"A_sfx_sword_catch-") + to_wstring(rand() % 2), 50.f);

        Set_AttackEnable(false);
        break;
    }
    case Client::CPlayerSword::OUTING:
    case Client::CPlayerSword::RETRIEVING:
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
            m_pBody2DColliderCom->Set_Active(true);
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
            pDynamicActor->Set_Rotation(-m_vStuckDirection);
            static_cast<CActor_Dynamic*>(m_pActorCom)->Late_Update(0);
            static_cast<CActor_Dynamic*>(m_pActorCom)->Set_Kinematic();
        }
        else
        {
            Switch_Animation(0);
            static_cast<CTransform_2D*>(m_pControllerTransform->Get_Transform())->LookDir(m_vStuckDirection);
            Set_Position(m_vStuckPosition);

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
    CCamera_Manager::CAMERA_TYPE eCameraType = (COORDINATE_2D == Get_CurCoord()) ? CCamera_Manager::TARGET_2D : CCamera_Manager::TARGET;
    CCamera_Manager::GetInstance()->Start_Shake_ByCount(eCameraType, 0.15f, 0.2f, 20, CCamera::SHAKE_XY);
    _vector vKnockBackForce = Get_LookDirection() * (COORDINATE_2D == Get_CurCoord() ? m_f2DKnockBackPower : m_f3DKnockBackPower);
    Event_Hit(this, static_cast<CCharacter*>(_pVictim), m_pPlayer->Get_AttackDamg(), vKnockBackForce);

    m_AttckedObjects.insert(_pVictim);
	Safe_AddRef(_pVictim);
}

void CPlayerSword::Set_AttackEnable(_bool _bOn, CPlayer::ATTACK_TYPE _eAttackType)
{
	COORDINATE eCoord = Get_CurCoord();
    m_bAttackEnable = _bOn;
    if (COORDINATE_3D == eCoord)
    {
        _uint iShapeCount = (_uint)m_pActorCom->Get_Shapes().size();
        for (_uint i = 0; i < iShapeCount; i++)
            m_pActorCom->Set_ShapeEnable(i, false);
        
        if (HANDLING == m_eCurrentState)
            m_pTrailEffect->Set_AddUpdate(_bOn);
        if (_bOn)
        {
            if (CPlayer::ATTACK_TYPE::ATTACK_TYPE_NORMAL3 == _eAttackType)
                CEffect_Manager::GetInstance()->Active_Effect(TEXT("SwordCombo"), true, &m_WorldMatrices[COORDINATE_3D]);
            if (CPlayer::ATTACK_TYPE::ATTACK_TYPE_JUMPATTACK == _eAttackType)
                CEffect_Manager::GetInstance()->Active_EffectPosition(TEXT("SwordJumpAttack"), true, XMLoadFloat4((_float4*)&m_WorldMatrices[COORDINATE_3D].m[3]));
        }

        else 
        {
            m_pTrailEffect->Delete_Delay(0.5f);
        }
    }
    if (false == _bOn)
    {
        for (CGameObject* pObj : m_AttckedObjects)
			Safe_Release(pObj);
        m_AttckedObjects.clear();
        if (COORDINATE_2D == eCoord)
            m_pBody2DColliderCom->Set_Active(false);
    }
    else
    {
        if (COORDINATE_3D == eCoord)
            m_pActorCom->Set_ShapeEnable(CPlayer::ATTACK_TYPE::ATTACK_TYPE_JUMPATTACK == _eAttackType ? 1 : 0, true);
        else
            m_pBody2DColliderCom->Set_Active(true);
    }

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
    Safe_Release(m_pTrailEffect);
    Safe_Release(m_pThrowTrailEffect);
    Safe_Release(m_pBeamEffect);

	Safe_Release(m_pBody2DColliderCom);
    for (CGameObject* pObj : m_AttckedObjects)
    {
        Safe_Release(pObj);
    }
    __super::Free();
}
