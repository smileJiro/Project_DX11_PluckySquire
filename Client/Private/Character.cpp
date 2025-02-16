#include "stdafx.h"
#include "Character.h" 
#include "Actor_Dynamic.h"

CCharacter::CCharacter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CContainerObject(_pDevice, _pContext)
{
	
}

CCharacter::CCharacter(const CCharacter& _Prototype)
	: CContainerObject(_Prototype)
{
}

void CCharacter::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
  
}

void CCharacter::Late_Update(_float _fTimeDelta)
{
    cout << m_fKnockBackForce << endl;
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

	__super::Late_Update(_fTimeDelta);
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
    if (COORDINATE_3D == Get_CurCoord())
    {
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


_bool CCharacter::Move_To(_fvector _vPosition, _float _fEpsilon)
{
    CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);
    _vector vDir = _vPosition - Get_FinalPosition();
    vDir.m128_f32[1] = 0.f;
    vDir.m128_f32[3] = 0.f;
    _float fLength = XMVectorGetX(XMVector3Length(vDir));
    if (_fEpsilon >= fLength)
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
    vDir.m128_f32[1] = 0.f;
    vDir.m128_f32[3] = 0.f;
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
