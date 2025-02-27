#include "stdafx.h"
#include "Character.h" 
#include "Actor_Dynamic.h"
#include "GameInstance.h"

CCharacter::CCharacter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CContainerObject(_pDevice, _pContext)
{
	
}

CCharacter::CCharacter(const CCharacter& _Prototype)
	: CContainerObject(_Prototype)
{
}

HRESULT CCharacter::Initialize(void* _pArg)
{
	CHARACTER_DESC* pDesc = static_cast<CHARACTER_DESC*>(_pArg);
	m_fStepSlopeThreshold = pDesc->_fStepSlopeThreshold;
	m_fStepHeightThreshold = pDesc->_fStepHeightThreshold;

    XMStoreFloat4x4(&m_matQueryShapeOffset, XMMatrixRotationZ(XMConvertToRadians(90.f)));
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;
    return S_OK;
}

void CCharacter::Priority_Update(_float _fTimeDelta)
{

    __super::Priority_Update(_fTimeDelta);
    COORDINATE eCoord = Get_CurCoord();
    if (COORDINATE_2D == eCoord)
    {
        if (m_bPlatformerMode)
        {
            Move(_vector{ 0.f,1.f,0.f } *m_f2DUpForce, _fTimeDelta);
        }
        else
        {
            m_f2DUpForce -= 9.8f * _fTimeDelta * 180;

            m_f2DFloorDistance += m_f2DUpForce * _fTimeDelta;
            if (0 > m_f2DFloorDistance)
            {
                m_f2DFloorDistance = 0;
                m_bOnGround = true;
                m_f2DUpForce = 0;
            }
            else if (0 == m_f2DFloorDistance)
                m_bOnGround = true;
            else
                m_bOnGround = false;

        }

    }
    else
    {
        Measure_FloorDistance();

        if (m_f3DFloorDistance > -1)
        {
            if (m_f3DFloorDistance < m_fStepHeightThreshold)
                m_bOnGround = true;
            else
                m_bOnGround = false;
        }
        else
        {
            m_f3DFloorDistance = -1;
            m_bOnGround = false;
        }
        //경사가 너ㅏ무 급하면 무시
    }
}

void CCharacter::Update(_float _fTimeDelta)
{
    if (COORDINATE_3D == Get_CurCoord() || false == Is_Stopped())
    {
        __super::Update(_fTimeDelta);
    }
    

    //여기는 파트 오브젝트까지 전부 업데이트 끝난 상황.
    m_vLookBefore = XMVector3Normalize(m_pControllerTransform->Get_State(CTransform::STATE_LOOK));

}

void CCharacter::Late_Update(_float _fTimeDelta)
{
	if (COORDINATE_2D == Get_CurCoord())
    {
        if (true == m_isKnockBack)
        {
            _vector vKnockBack = XMLoadFloat3(&m_vKnockBackDirection);
            vKnockBack *= m_fKnockBackForce * _fTimeDelta;
            m_fKnockBackAccTime += _fTimeDelta;
            m_fKnockBackForce = (1 - m_fKnockBackAccTime) * m_fKnockBackForce;
            Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, Get_FinalPosition() + vKnockBack);
            if (0.1f >= m_fKnockBackForce)
            {
                m_fKnockBackForce = 0.f;
                m_fKnockBackAccTime = 0.f;
                m_isKnockBack = false;
            }
        }
    }

   
	__super::Late_Update(_fTimeDelta);
}

void CCharacter::OnContact_Modify(const COLL_INFO& _0, const COLL_INFO& _1, CModifiableContacts& _ModifiableContacts, _bool _bIm0)
{
	if (_bIm0)
		_ModifiableContacts.Set_InvInertiaScale0(0.f);
	else
		_ModifiableContacts.Set_InvInertiaScale1(0.f);
    SHAPE_USE eMyShapeUse = (SHAPE_USE)_0.pShapeUserData->iShapeUse;
    switch (eMyShapeUse)
    {
    case Client::SHAPE_USE::SHAPE_BODY:
    {
        _uint iContactCount = _ModifiableContacts.Get_ContactCount();
        for (_uint i = 0; i < iContactCount; i++)
        {
            //벽이면?
            _float fNormal = abs(XMVectorGetY(_ModifiableContacts.Get_Normal(i)));
            _vector vPoint = _ModifiableContacts.Get_Point(i);

            if (fNormal < m_fStepSlopeThreshold
                || vPoint.m128_f32[1] > m_fStepHeightThreshold + Get_FinalPosition().m128_f32[1])
            {
                _ModifiableContacts.Set_DynamicFriction(i, 0.0f);
                _ModifiableContacts.Set_StaticFriction(i, 0.0f);
                continue;
            }
            _ModifiableContacts.Set_Restitution(i, 0);
        }
        break;
    }
    default:
        break;
    }
}

void CCharacter::OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
}

void CCharacter::OnContact_Stay(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
    //SHAPE_USE eShapeUse = (SHAPE_USE)_My.pShapeUserData->iShapeUse;
    //switch (eShapeUse)
    //{
    //case Client::SHAPE_USE::SHAPE_BODY:
    //    for (auto& pxPairData : _ContactPointDatas)
    //    {
    //        //경사가 너ㅏ무 급하면 무시
    //        if (abs(pxPairData.normal.y) < m_fStepSlopeThreshold)
    //            continue;

    //        return;
    //    }
    //    break;
    //case Client::SHAPE_USE::SHAPE_FOOT:

    //    break;
    //case Client::SHAPE_USE::SHAPE_TRIGER:

    //    break;
    //default:
    //    break;
    //}
}

void CCharacter::OnContact_Exit(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
}

_bool CCharacter::Is_OnGround()
{
    if (Is_PlatformerMode())
    {
        return CGravity::STATE::STATE_FLOOR == m_pGravityCom->Get_CurState();
    }
    else
    {
        return m_bOnGround;
    }
}
void CCharacter::Stop_Rotate()
{
    ACTOR_TYPE eActorType = Get_ActorType();
    if (ACTOR_TYPE::DYNAMIC == eActorType)
    {
        if (COORDINATE_3D == Get_CurCoord())
        {
            CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);
            pDynamicActor->Set_AngularVelocity(_vector{ 0,0,0,0 });
        }
    }
}

void CCharacter::Stop_Move()
{
    ACTOR_TYPE eActorType = Get_ActorType();
    if (ACTOR_TYPE::DYNAMIC == eActorType)
    {
        if (COORDINATE_3D == Get_CurCoord())
        {
            CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);
  
            _float fYVelocity = XMVectorGetY(pDynamicActor->Get_LinearVelocity());
            pDynamicActor->Set_LinearVelocity(_vector{ 0,fYVelocity,0 });
        }
        else
        {

        }
    }
}

void CCharacter::Stop_MoveXZ()
{
    ACTOR_TYPE eActorType = Get_ActorType();
    if (ACTOR_TYPE::DYNAMIC == eActorType)
    {
        if (COORDINATE_3D == Get_CurCoord())
        {
            CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);
            pDynamicActor->Set_LinearVelocity(_vector{ 0,0,0,0 });
        }
        else
        {

        }
    }
}


_bool CCharacter::Rotate_To(_fvector _vDirection, _float _fSpeed)
{
    CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);
	_vector vDirection = XMVector3Normalize(_vDirection);
    _vector vLook = XMVector3Normalize(m_pControllerTransform->Get_State(CTransform::STATE_LOOK));
    _float3 vLookDiff; XMStoreFloat3(&vLookDiff, vDirection - vLook);
    _float3 vLookDiffBefore; XMStoreFloat3(&vLookDiffBefore, vDirection - m_vLookBefore);
    if (XMVector3Equal(vDirection, XMVectorZero()))
    {
        return true;
    }
    if (XMVector3NearEqual(vDirection, vLook, XMVectorReplicate(0.001f)))
    {
        pDynamicActor->Set_Rotation(vDirection);
        pDynamicActor->Set_AngularVelocity(_vector{ 0,0,0,0 });
        return true;
    }
    else if ((vLookDiff.x * vLookDiffBefore.x) < 0
        || (vLookDiff.z * vLookDiffBefore.z) < 0)
    {
        pDynamicActor->Set_Rotation(vDirection);
        pDynamicActor->Set_AngularVelocity(_vector{ 0,0,0,0 });
        return true;
    }
    else
    {
        _vector vAxis = XMVector3Normalize(XMVector3Cross(vLook, vDirection));
        if (XMVector3Equal(vAxis, XMVectorZero()))
            vAxis = XMVectorSet(0, 1, 0, 0);
        pDynamicActor->Set_AngularVelocity(vAxis * XMConvertToRadians(_fSpeed));
        return false;
    }
}

_bool CCharacter::Rotate_To_Radians(_fvector _vDirection, _float _fSpeed)
{
    CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);

	_vector vDirection = XMVector3Normalize(_vDirection);
    _vector vLook = XMVector3Normalize(m_pControllerTransform->Get_State(CTransform::STATE_LOOK));
    _float3 vLookDiff; XMStoreFloat3(&vLookDiff, vDirection - vLook);
    _float3 vLookDiffBefore; XMStoreFloat3(&vLookDiffBefore, vDirection - m_vLookBefore);
    if (XMVector3Equal(vDirection, XMVectorZero()))
    {
        return true;
    }

    if (XMVector3NearEqual(vDirection, vLook, XMVectorReplicate(0.1f)))
    {
        pDynamicActor->Set_Rotation(vDirection);
        pDynamicActor->Set_AngularVelocity(_vector{ 0,0,0,0 });
        return true;
    }
   /* else if ((vLookDiff.x * vLookDiffBefore.x) < 0
        || (vLookDiff.z * vLookDiffBefore.z) < 0)
    {
        pDynamicActor->Set_Rotation(vDirection);
        pDynamicActor->Set_AngularVelocity(_vector{ 0,0,0,0 });
        return true;
    }*/
    else if (XMVectorGetY(XMVector3Cross(vLook, vDirection)) * XMVectorGetY(XMVector3Cross(m_vLookBefore, vDirection)) < 0)
    {
        pDynamicActor->Set_Rotation(vDirection);
        pDynamicActor->Set_AngularVelocity(_vector{ 0,0,0,0 });
        return true;
    }
    else
    {
        _vector vAxis = XMVector3Normalize(XMVector3Cross(vLook, vDirection));
        if (XMVector3Equal(vAxis, XMVectorZero()))
            vAxis = XMVectorSet(0, 1, 0, 0);
        pDynamicActor->Set_AngularVelocity(vAxis * _fSpeed);
        return false;
    }
}

void CCharacter::KnockBack(_fvector _vForce)
{
    if (true == XMVector3Equal(_vForce, XMVectorZero()))
        return;

    if (COORDINATE_3D == Get_CurCoord())
    {
        //속도 0으로 만들고 넉백
        Get_ActorCom()->Set_LinearVelocity(_vForce, 0.f);
        Add_Impuls(_vForce);
    }
    else if (COORDINATE_2D == Get_CurCoord())
    {
        if(false == m_isKnockBack)
        {
            m_fKnockBackForce=XMVectorGetX(XMVector3Length(_vForce));
            XMStoreFloat3(&m_vKnockBackDirection, XMVector3Normalize(_vForce));
            m_isKnockBack = true;
        }
    }
}

_float CCharacter::Measure_FloorDistance()
{
    _float fFloorHeihgt = -1;
     _vector vCharacterPos = Get_FinalPosition();
    _float3 vOrigin;
    XMStoreFloat3(&vOrigin, vCharacterPos);

    _float3 vRayDir = { 0,-1,0 };
    list<CActorObject*> hitActors;
    list<RAYCASTHIT> raycasthits;

    PxGeometryHolder pxGeomHolder = m_pActorCom->Get_Shapes()[(_uint)SHAPE_USE::SHAPE_BODY]->getGeometry().any();
    PxGeometryType::Enum eGeomType = pxGeomHolder.getType();
    if (PxGeometryType::eCAPSULE == eGeomType)
    {
        PxCapsuleGeometry& pxCapsule = pxGeomHolder.capsule();
        vOrigin.y += (m_fStepHeightThreshold + pxCapsule.halfHeight + pxCapsule.radius);
    }
    else if (PxGeometryType::eBOX == eGeomType)
    {
        PxBoxGeometry& pxBox = pxGeomHolder.box();
        vOrigin.y += (m_fStepHeightThreshold + pxBox.halfExtents.y);
    }
    else if (PxGeometryType::eSPHERE == eGeomType)
    {
        PxSphereGeometry& pxSphere = pxGeomHolder.sphere();
        vOrigin.y += (m_fStepHeightThreshold + pxSphere.radius);
    }

    _bool bResult =m_pGameInstance->MultiSweep(&pxGeomHolder.any(), m_matQueryShapeOffset, vOrigin, vRayDir, 10.f, hitActors, raycasthits);
    if (bResult)
    {
        //닿은 것들 중에서 가장 높은 것을 찾기
        auto& iterHitPoint = raycasthits.begin();
        for (auto& pActor : hitActors)
        {
            if (pActor != this)//맵과 닿음.
            {
                if (iterHitPoint->vNormal.y > m_fStepSlopeThreshold)//닿은 곳의 경사가 너무 급하지 않으면
                {
                    fFloorHeihgt = max(fFloorHeihgt, iterHitPoint->vPosition.y);
                    m_vFloorNormal = { iterHitPoint->vNormal.x, iterHitPoint->vNormal.y, iterHitPoint->vNormal.z };
                }
            }
            iterHitPoint++;
        }
    }
    if(fFloorHeihgt > 0)
        m_f3DFloorDistance = XMVectorGetY(vCharacterPos) - fFloorHeihgt;
    else
        m_f3DFloorDistance = fFloorHeihgt;
    return m_f3DFloorDistance;
}

_vector CCharacter::StepAssist(_fvector _vVelocity,_float _fTimeDelta)
{ 
    if (COORDINATE_3D == Get_CurCoord())
    {
        CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);
        _float3 vMyPos;// = pDynamicActor->Get_GlobalPose();
        XMStoreFloat3(&vMyPos, Get_FinalPosition());
		_vector vPredictMove = _vVelocity * _fTimeDelta;
		vPredictMove = XMVectorSetY(vPredictMove, 0.f);
        _float3 vOrigin = vMyPos;
		vOrigin.x += vPredictMove.m128_f32[0];
		vOrigin.z += vPredictMove.m128_f32[2];

        _float3 vDir = { 0.f,-1.f, 0.f };
        _float fDistance = m_fStepHeightThreshold*2.f;
        list<CActorObject*> hitActors;
        list<RAYCASTHIT> raycastHits;
        PxGeometryHolder pxGeomHolder= m_pActorCom->Get_Shapes()[(_uint)SHAPE_USE::SHAPE_BODY]->getGeometry().any();
        PxGeometryType::Enum eGeomType = pxGeomHolder.getType();
        if (PxGeometryType::eCAPSULE == eGeomType)
        {
			 PxCapsuleGeometry& pxCapsule = pxGeomHolder.capsule();
            vOrigin.y += (vPredictMove.m128_f32[1] + m_fStepHeightThreshold + pxCapsule.halfHeight + pxCapsule.radius);
		}
		else if (PxGeometryType::eBOX == eGeomType)
		{
			 PxBoxGeometry& pxBox = pxGeomHolder.box();
			vOrigin.y += (vPredictMove.m128_f32[1] + m_fStepHeightThreshold + pxBox.halfExtents.y);
		}
		else if (PxGeometryType::eSPHERE == eGeomType)
		{
             PxSphereGeometry& pxSphere = pxGeomHolder.sphere();
			vOrigin.y += (vPredictMove.m128_f32[1] + m_fStepHeightThreshold + pxSphere.radius);
        }

        _bool bResult = m_pGameInstance->MultiSweep(&pxGeomHolder.any(), m_matQueryShapeOffset, vOrigin, vDir, fDistance, hitActors, raycastHits);

        if (bResult)
        {
            _bool bFloorChecked = false;
            _float fMaxDiffY = -999.f;
			_float3 vMaxDiffYPos = { 0.f,0.f,0.f };
            auto& iterHitPoint = raycastHits.begin();
            for (auto& pActor : hitActors)
            {
                OBJECT_GROUP eOtherGroup = (OBJECT_GROUP)pActor->Get_ObjectGroupID();
                if (OBJECT_GROUP::MAPOBJECT & eOtherGroup
                    || OBJECT_GROUP::DYNAMIC_OBJECT & eOtherGroup)
                {
                    //맵이나 다이나믹 오브젝트와 충돌됐을 경우
                    //오브젝트의 높이를 판단함.
					//높이가 m_fStepHeightThreshold이하일 경우, 높이만큼 위로 이동시킴.
					_float fDiffY = iterHitPoint->vPosition.y - vMyPos.y;
                    //cout << "DiffY: " << fDiffY << endl;
                    if (fDiffY <= m_fStepHeightThreshold
                        && fDiffY > 0.f
                        && fMaxDiffY < fDiffY
                        && iterHitPoint->vNormal.y > m_fStepSlopeThreshold)
                    {
                        fMaxDiffY = fDiffY;
						vMaxDiffYPos = iterHitPoint->vPosition;
                        bFloorChecked = true;
                    }
                }
                iterHitPoint++;
            }
            if (bFloorChecked)
            {

               // vMyPos.y += fMaxDiffY;
               // pDynamicActor->Set_GlobalPose(vMyPos);

                _float fVelocitySlope = abs(XMVector3Normalize(_vVelocity).m128_f32[1]);
                if (fVelocitySlope < m_fStepSlopeThreshold)
                {
                    _float fDiffXZ = sqrtf(powf(vMaxDiffYPos.x - vMyPos.x, 2.f) + powf(vMaxDiffYPos.z - vMyPos.z, 2));
                    _vector vNewVelocity = _vVelocity;
                    vNewVelocity = XMVectorSetY(vNewVelocity, 0.f);
                    vNewVelocity.m128_f32[1] += fMaxDiffY / fDiffXZ * XMVector3Length(vNewVelocity).m128_f32[0];
      
                    if (fMaxDiffY > 0)
                        vNewVelocity.m128_f32[1] += 9.8f * 3.f * _fTimeDelta;
                    else
                        vNewVelocity.m128_f32[1] -= 9.8f * 3.f * _fTimeDelta;
                    
                    return vNewVelocity;
                }
         
            }

        }
    }
	return _vVelocity;
}


void CCharacter::Move(_fvector _vForce, _float _fTimeDelta)
{
    ACTOR_TYPE eActorType = Get_ActorType();

    if (COORDINATE_3D == Get_CurCoord())
    {
        m_v3DTargetDirection = XMVector4Normalize(_vForce);
        CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);
        _vector vVeclocity = _vForce /** m_tStat[COORDINATE_3D].fMoveSpeed*/  /** fDot*/;

        vVeclocity = XMVectorSetY(vVeclocity, XMVectorGetY(pDynamicActor->Get_LinearVelocity()));

        if (pDynamicActor->Is_Dynamic())
        {
            if (Is_OnGround())
                vVeclocity = StepAssist(vVeclocity, _fTimeDelta);
            pDynamicActor->Set_LinearVelocity(vVeclocity);
        }
        else
        {
            m_pControllerTransform->Go_Direction(_vForce,_fTimeDelta);
        }


    }
    else
    {
        m_pControllerTransform->Go_Direction(_vForce, XMVectorGetX(XMVector3Length(_vForce)), _fTimeDelta);
    }
}

_bool CCharacter::Move_To(_fvector _vPosition, _float _fEpsilon, _bool _FreezeY)
{
    CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);
    _vector vDir = _vPosition - Get_FinalPosition();
    if(true == _FreezeY)
        vDir.m128_f32[1] = 0.f;
    vDir.m128_f32[3] = 0.f;
    if (Check_Arrival(_vPosition, _fEpsilon))
    {
        pDynamicActor->Set_LinearVelocity(_vector{ 0,0,0,0 });
        return true;
    }

    pDynamicActor->Set_LinearVelocity(XMVector3Normalize(vDir), m_pControllerTransform->Get_SpeedPerSec());

    return false;
}

_bool CCharacter::Check_Arrival(_fvector _vPosition, _float _fEpsilon)
{
    _vector vDir = _vPosition - Get_FinalPosition();
	if (COORDINATE_3D == Get_CurCoord())
	{
        vDir.m128_f32[1] = 0.f;
        vDir.m128_f32[3] = 0.f;
	}
    else
    {
        vDir.m128_f32[2] = 0.f;
        vDir.m128_f32[3] = 0.f;
    }

    _float fLength = XMVectorGetX(XMVector3Length(vDir));
    if (_fEpsilon >= fLength)
    {
        return true;
    }
    return false;
}

void CCharacter::LookDirectionXZ_Kinematic(_fvector _vDir)
{
	_vector vDir = XMVector3Normalize(_vDir);
	_vector vPos = m_pControllerTransform->Get_State(CTransform::STATE_POSITION);
    _vector vAt = XMVectorSetY(vPos + vDir, XMVectorGetY(vPos));
	m_pControllerTransform->LookAt_3D(vAt);
}

void CCharacter::LookDirectionXZ_Dynamic(_fvector _vDir)
{
    _vector vDir = XMVector3Normalize(_vDir);
    vDir = XMVectorSetY(vDir, 0);
    CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);
    pDynamicActor->Set_Rotation(_vDir);
}


void CCharacter::Free()
{
	__super::Free();
}

