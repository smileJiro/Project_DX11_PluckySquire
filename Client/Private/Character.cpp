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
    m_vLookBefore = XMVector3Normalize(m_pControllerTransform->Get_State(CTransform::STATE_LOOK));
}

void CCharacter::Late_Update(_float _fTimeDelta)
{
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
            pDynamicActor->Set_LinearVelocity(_vector{ 0,0,0,0 });
        }
        else
        {

        }
    }
}

void CCharacter::Add_Impuls(_vector _vForce)
{
    _float3 f3Force;
    XMStoreFloat3(&f3Force, _vForce);
    m_pActorCom->Add_Impulse(f3Force);
}

void CCharacter::Add_Force(_vector _vForce)
{
    _float3 f3Force;
    XMStoreFloat3(&f3Force, _vForce);
    m_pActorCom->Add_Force(f3Force);
}

void CCharacter::Rotate_To(_vector _vDirection)
{
    CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);

    _vector vLook = XMVector3Normalize(m_pControllerTransform->Get_State(CTransform::STATE_LOOK));
    _float3 vLookDiff; XMStoreFloat3(&vLookDiff, _vDirection - vLook);
    _float3 vLookDiffBefore; XMStoreFloat3(&vLookDiffBefore, _vDirection - m_vLookBefore);
    if (XMVector3Equal(_vDirection, XMVectorZero()))
    {
        _vDirection = vLook;
    }
    if (XMVector3NearEqual(_vDirection, vLook, XMVectorReplicate(1e-6f)))
    {
        pDynamicActor->Set_AngularVelocity(_vector{ 0,0,0,0 });
    }
    else if ((vLookDiff.x * vLookDiffBefore.x) < 0
        || (vLookDiff.z * vLookDiffBefore.z) < 0)
    {
        pDynamicActor->Set_Rotation(_vDirection);
        pDynamicActor->Set_AngularVelocity(_vector{ 0,0,0,0 });
    }
    else
    {
        _vector vAxis = XMVector3Normalize(XMVector3Cross(vLook, _vDirection));
        if (XMVector3Equal(vAxis, XMVectorZero()))
            vAxis = XMVectorSet(0, 1, 0, 0);
        pDynamicActor->Set_AngularVelocity(vAxis * XMConvertToRadians(1080));

    }
}

_bool CCharacter::Move_To(_fvector _vPosition)
{
    static _float fEpsilon = 0.5f;
    CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);
    _vector vDir = _vPosition - Get_FinalPosition();
    _float fLength = XMVectorGetX(XMVector3Length(vDir));
    if (fEpsilon >= fLength)
    {
        pDynamicActor->Set_LinearVelocity(_vector{ 0,0,0,0 });
        return true;
    }

    pDynamicActor->Set_LinearVelocity(XMVector3Normalize(vDir), m_pControllerTransform->Get_SpeedPerSec());

    return false;
}


void CCharacter::Free()
{
	__super::Free();
}
