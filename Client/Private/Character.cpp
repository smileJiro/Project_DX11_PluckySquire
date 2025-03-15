#include "stdafx.h"
#include "Character.h" 
#include "Actor_Dynamic.h"
#include "GameInstance.h"
#include "Section_Manager.h"
#include "ModelObject.h"

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
    m_tStat = pDesc->_tStat;
    m_isIgnoreGround = pDesc->_isIgnoreGround;

    if(false == m_isIgnoreGround)
    {
        m_fStepSlopeThreshold = pDesc->_fStepSlopeThreshold;
        m_fStepHeightThreshold = pDesc->_fStepHeightThreshold;
        m_fGravity = m_pGameInstance->Get_Gravity();
    }

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;
    return S_OK;
}

void CCharacter::Priority_Update(_float _fTimeDelta)
{
    if(false == m_isIgnoreGround)
    {
        COORDINATE eCoord = Get_CurCoord();
        _bool bOldGround = m_bOnGround;
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
            //경사가 너무 급하면 무시
        }
        if (bOldGround == false && m_bOnGround == true)
            On_Land();
    }


    __super::Priority_Update(_fTimeDelta);

}

void CCharacter::Update(_float _fTimeDelta)
{
    if (m_bAutoMoving)
    {
        if (Process_AutoMove(_fTimeDelta))
        {
            if (m_bAutoClearAutoMoves)
            {
                Clear_AutoMove();
            }
            m_bAutoMoving = false;
            On_EndAutoMove();
        }
    }

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
    if (true == m_isIgnoreGround)
        return;

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
            _vector vWallNormal = _ModifiableContacts.Get_Normal(i);
            _float fNormal = abs(XMVectorGetY(vWallNormal));
            _vector vPoint = _ModifiableContacts.Get_Point(i);

            //벽이면? || 
            if (fNormal < m_fStepSlopeThreshold 
                || vPoint.m128_f32[1] > m_fStepHeightThreshold + Get_FinalPosition().m128_f32[1])
            {
                _ModifiableContacts.Set_DynamicFriction(i, 0.0f);
                _ModifiableContacts.Set_StaticFriction(i, 0.0f);
            }
            if(fNormal < m_fStepSlopeThreshold )
                _ModifiableContacts.Set_Normal(i, XMVectorSetY( vWallNormal, 0.f));

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

void CCharacter::Enter_Section(const _wstring _strIncludeSectionName)
{ 
    auto Section = SECTION_MGR->Find_Section(_strIncludeSectionName);
    if (nullptr != Section && static_cast<CSection_2D*>(Section)->Is_Scrolling())
        Set_ScrollingMode(true);

    __super::Enter_Section(_strIncludeSectionName);
}

_bool CCharacter::Is_Dynamic()
{
    if (nullptr == m_pActorCom)
        return false;

    return static_cast<CActor_Dynamic*>(m_pActorCom)->Is_Dynamic();
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
        _vector vAxis = XMVector3Cross(vLook, vDirection);
        //if (XMVector3Equal(vAxis, XMVectorZero()))
        //    vAxis = XMVectorSet(0, 1, 0, 0);
        if (XMVector3NearEqual(vAxis, XMVectorZero(), XMVectorReplicate(0.1f)))
            vAxis = XMVectorSet(0, 1, 0, 0);

        pDynamicActor->Set_AngularVelocity(XMVector3Normalize(vAxis)*_fSpeed);
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



_vector CCharacter::Get_ScrolledPosition()
{
    _float2 fSize = SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName);
    _float fDefaultWitdh = (fSize.x * 0.5f);

    _vector vPos = Get_FinalPosition();
    if (-fDefaultWitdh > vPos.m128_f32[0])
    {
        vPos = XMVectorSetX(vPos, vPos.m128_f32[0] + fSize.x);
    }
    if (fDefaultWitdh < vPos.m128_f32[0])
    {
        vPos =XMVectorSetX(vPos, vPos.m128_f32[0] - fSize.x);
    }
    return vPos;
}

_vector CCharacter::Get_ScrolledPosition(_vector _vPosition)
{
    _float2 fSize = SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName);
    _float fDefaultWitdh = (fSize.x * 0.5f);

    if (-fDefaultWitdh > _vPosition.m128_f32[0])
    {
        _vPosition = XMVectorSetX(_vPosition, _vPosition.m128_f32[0] + fSize.x);
    }
    if (fDefaultWitdh < _vPosition.m128_f32[0])
    {
        _vPosition = XMVectorSetX(_vPosition, _vPosition.m128_f32[0] - fSize.x);
    }
    return _vPosition;
}

void CCharacter::Set_ScrollingMode(_bool _bScrollingMode)
{
    m_bScrollingMode = _bScrollingMode;
}

void CCharacter::Set_2DDirection(E_DIRECTION _eEDir, _bool _isOnChange)
{

    if (m_e2DDirection_E != _eEDir) 
    {
        switch (_eEDir)
        {
        case Client::E_DIRECTION::LEFT:
        case Client::E_DIRECTION::LEFT_UP:
        case Client::E_DIRECTION::LEFT_DOWN:
        {
            _vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
            m_PartObjects[PART_BODY]->Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, -1.f * XMVectorAbs(vRight));
            vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
            break;
        }
        case Client::E_DIRECTION::RIGHT:
        case Client::E_DIRECTION::RIGHT_UP:
        case Client::E_DIRECTION::RIGHT_DOWN:
        {
            _vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
            m_PartObjects[PART_BODY]->Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, XMVectorAbs(vRight));
            break;
        }
        default:
            break;
        }
        m_e2DDirection_E = _eEDir;

        if(true == _isOnChange)
            On_Change2DDirection(m_e2DDirection_E);
    }
}

void CCharacter::Set_2DDirection(F_DIRECTION _eFDir, _bool _isOnChange)
{
	Set_2DDirection( To_EDirection(_eFDir), _isOnChange);
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

        //캡슐의 경우 y축 아니면 z축 회전하므로 y축 회전했을 때를 따로 처리
        if (1.f == m_matQueryShapeOffset._22)
        {
            vOrigin.y += (m_fStepHeightThreshold + pxCapsule.radius);
        }
        //기본을 z축 기준 90도 회전으로 생각
        else
        {
            pxCapsule.radius *= 0.5f;
            vOrigin.y += (m_fStepHeightThreshold + pxCapsule.halfHeight + pxCapsule.radius);
        }
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
                //cout << "iterHitPoint->vNormal.y" << iterHitPoint->vNormal.y << endl;
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

            //캡슐의 경우 y축 아니면 z축 회전하므로 y축 회전했을 때를 따로 처리
            if (1.f == m_matQueryShapeOffset._22)
            {
                vOrigin.y += (vPredictMove.m128_f32[1] + m_fStepHeightThreshold + pxCapsule.radius);
            }
            //기본을 z축 기준 90도 회전으로 생각
            else
            {
                vOrigin.y += (vPredictMove.m128_f32[1] + m_fStepHeightThreshold + pxCapsule.halfHeight + pxCapsule.radius);
            }
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
                        vNewVelocity.m128_f32[1] += m_fGravity * _fTimeDelta;
                    else
                        vNewVelocity.m128_f32[1] -= m_fGravity * _fTimeDelta;
                    
                    return vNewVelocity;
                }
         
            }

        }
    }
	return _vVelocity;
}

void CCharacter::Go_Straight_F_Dir(_float _fTimeDelta)
{
    switch (Get_2DDirection())
    {
    case E_DIRECTION::LEFT:
        Get_ControllerTransform()->Go_Left(_fTimeDelta);
        break;

    case E_DIRECTION::RIGHT:
        Get_ControllerTransform()->Go_Right(_fTimeDelta);
        break;

    case E_DIRECTION::UP:
        Get_ControllerTransform()->Go_Up(_fTimeDelta);
        break;

    case E_DIRECTION::DOWN:
        Get_ControllerTransform()->Go_Down(_fTimeDelta);
        break;

    default:
        break;
    }
}


void CCharacter::Move(_fvector _vVelocity, _float _fTimeDelta)
{
    _vector vVeclocity = XMVectorSetW(_vVelocity, 0.f); /** m_tStat[COORDINATE_3D].fMoveSpeed*/  /** fDot*/;

    if (COORDINATE_3D == Get_CurCoord())
    {
        m_v3DTargetDirection = XMVector4Normalize(_vVelocity);
        CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);

        vVeclocity = XMVectorSetY(vVeclocity, XMVectorGetY(pDynamicActor->Get_LinearVelocity()));

        if (pDynamicActor->Is_Dynamic())
        {
            if (Is_OnGround())
                vVeclocity = StepAssist(vVeclocity, _fTimeDelta);
            pDynamicActor->Set_LinearVelocity(vVeclocity);
        }
        else
        {
            m_pControllerTransform->Go_Direction(_vVelocity,_fTimeDelta);
        }

    }
    else
    {
        m_pControllerTransform->Go_Direction(vVeclocity, XMVectorGetX(XMVector3Length(vVeclocity)), _fTimeDelta);

        if (m_bScrollingMode)
        {
            Set_Position(Get_ScrolledPosition());
        }
    }
}

_bool CCharacter::Move_To_3D(_fvector _vPosition, _float _fEpsilon, _bool _FreezeY)
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
//이전 프레임의 위치는 언제 업데이트 해야할까?
// 오븨젝트이 Updaet ->충돌처리(피직스,콜라이더)->lateUpdate 
// 
//이전 프레임의 위치를 저장해 놓고, 
//이전 프레임 위치와 이번 프레임의 위치를 비교하여
//목표 지점을 넘어갔으면 멈춤.
//목표 지점을 못 넘어갔으면?
// -> 이동
//만약 첫 번째 프레임이면? 이전 프레임의 위치가 없을 것.
// ->일단 이동하고, 
_bool CCharacter::Move_To(_fvector _vPosition, _float _fTimeDelta, _float fInterval)
{
    COORDINATE eCoord = Get_CurCoord();
	_vector vCurrentPos = Get_FinalPosition();
	_float fEpsilon = COORDINATE_2D == eCoord ? 10.f : 0.3f;

    _float fMoveSpeed = m_pControllerTransform->Get_SpeedPerSec();
    _vector vDir = XMVector3Normalize(XMVectorSetW(_vPosition - vCurrentPos, 0.f));
	if (Check_Arrival(_vPosition, fEpsilon + fInterval))
	{
        if(Is_Dynamic())
        {
			_float3 vPos; XMStoreFloat3(&vPos, _vPosition + (-vDir * fInterval));
            m_pActorCom->Set_GlobalPose(vPos);
        }
		Set_Position(_vPosition + (-vDir * fInterval));
		return true;
	}

    Move(vDir * fMoveSpeed, _fTimeDelta);
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

_bool CCharacter::Check_Arrival(_fvector _vPrevPosition, _fvector _vNextPosition, _fvector _vTargetPosition)
{

    return _bool();
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
    pDynamicActor->Set_Rotation(vDir);
}


void CCharacter::Free()
{
	__super::Free();
}




void CCharacter::Add_AutoMoveCommand(AUTOMOVE_COMMAND _pCommand)
{
	m_AutoMoveQue.push(_pCommand);
}

void CCharacter::Start_AutoMove(_bool _bAutoClear)
{
	if (m_AutoMoveQue.empty())
    {
        m_bAutoMoving = false;
        return;
    }
    m_bAutoMoving = true;
    On_StartAutoMove();

}

void CCharacter::Stop_AutoMove()
{
    m_bAutoMoving = false;
}

void CCharacter::Clear_AutoMove()
{
    m_bAutoMoving = false;
	while (false == m_AutoMoveQue.empty())
		m_AutoMoveQue.pop();
}


_bool CCharacter::Process_AutoMove(_float _fTimeDelta)
{
    assert(false == m_AutoMoveQue.empty());
    assert(m_bAutoMoving);

    AUTOMOVE_COMMAND& tCommand = m_AutoMoveQue.front();

    _bool bRuntimeBefore = tCommand.Is_RunTime();
    tCommand.Update(_fTimeDelta);
    //선딜레이
    if (tCommand.Is_PreDelayTime())
    {

	}
    //실행중
    else if (tCommand.Is_RunTime())
    {
        if(false == bRuntimeBefore)
            static_cast<CModelObject*>(m_PartObjects[0])->Switch_Animation(tCommand.iAnimIndex);
        switch (tCommand.eType)
        {
        case AUTOMOVE_TYPE::MOVE_TO:
        {
            if(Process_AutoMove_MoveTo(tCommand, _fTimeDelta))
				tCommand.Complete_Command();
            break;
        }
        case AUTOMOVE_TYPE::MOVE_TOWARD:
        {
            if(Process_AutoMove_MoveToward(tCommand, _fTimeDelta))
                tCommand.Complete_Command();
            break;
        }
        case AUTOMOVE_TYPE::LOOK_DIRECTION:
        {
            if(Process_AutoMove_LookDirection(tCommand, _fTimeDelta))
                tCommand.Complete_Command();
            break;
        }
        case AUTOMOVE_TYPE::CHANGE_ANIMATION:
        {
            if (Process_AutoMove_ChangeAnimation(tCommand, _fTimeDelta))
                tCommand.Complete_Command();
            break;
        }
        case AUTOMOVE_TYPE::WAIT:
        {
            if(Process_AutoMove_Wait(tCommand, _fTimeDelta))
                tCommand.Complete_Command();
            break;
        }
        }
    }
    //후딜레이
	else if (tCommand.Is_PostDelayTime())
    {

    }
    if (tCommand.Is_End())
    {
        m_AutoMoveQue.pop();
    }
   
    return m_AutoMoveQue.empty();
}

_bool CCharacter::Process_AutoMove_MoveTo(const AUTOMOVE_COMMAND& _pCommand, _float _fTimeDelta)
{
    _vector vPosition = Get_FinalPosition();
    _vector vDir = XMVector3Normalize(XMVectorSetW( _pCommand.vTarget - vPosition,0.f));

    COORDINATE eCoord = Get_CurCoord();
    if (COORDINATE_2D == eCoord)
    {
        Set_2DDirection(To_EDirection(vDir));
    }
    else
    {
        Rotate_To_Radians(vDir, m_pControllerTransform->Get_RotationPerSec());
    }
    _bool _bResult = Move_To(XMVectorSetW(_pCommand.vTarget,1.f), _fTimeDelta);
    if (_bResult)
        Stop_Move();
    return _bResult;
}

_bool CCharacter::Process_AutoMove_MoveToward(const AUTOMOVE_COMMAND& _pCommand, _float _fTimeDelta)
{
    return _bool();
}

_bool CCharacter::Process_AutoMove_LookDirection(const AUTOMOVE_COMMAND& _pCommand, _float _fTimeDelta)
{
    COORDINATE eCoord = Get_CurCoord();
    _vector vDir = XMVector3Normalize(_pCommand.vTarget);
    if (COORDINATE_2D == eCoord)
    {
        Set_2DDirection(To_EDirection(vDir));
        return true;
    }
    else
    {
        return Rotate_To_Radians(vDir, m_pControllerTransform->Get_RotationPerSec());
    }
}

_bool CCharacter::Process_AutoMove_ChangeAnimation(const AUTOMOVE_COMMAND& _pCommand, _float _fTimeDelta)
{
    return false;
}

_bool CCharacter::Process_AutoMove_Wait(const AUTOMOVE_COMMAND& _pCommand, _float _fTimeDelta)
{
    return _bool();
}

