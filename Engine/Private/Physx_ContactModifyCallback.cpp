#include "Physx_ContactModifyCallback.h"
#include "ActorObject.h"

CPhysx_ContactModifyCallback::CPhysx_ContactModifyCallback()
{
}

void CPhysx_ContactModifyCallback::onContactModify(PxContactModifyPair* pairs, PxU32 count)
{

    for (PxU32 i = 0; i < count; i++)
    {
        PxContactModifyPair& pair = pairs[i];

        const PxRigidActor* actor0 = pair.actor[0];
        const PxRigidActor* actor1 = pair.actor[1];

        ACTOR_USERDATA* pActorUserData_0 = static_cast<ACTOR_USERDATA*>(actor0->userData);
        ACTOR_USERDATA* pActorUserData_1 = static_cast<ACTOR_USERDATA*>(actor1->userData);
        if (nullptr == pActorUserData_0 || nullptr == pActorUserData_1)
            return;
        CActorObject* pOwner_0 = pActorUserData_0->pOwner;
        CActorObject* pOwner_1 = pActorUserData_1->pOwner;

        if (false == pOwner_0->Get_ActorCom()->Is_Active() || true == pOwner_0->Is_Dead()
            ||false == pOwner_1->Get_ActorCom()->Is_Active() || true == pOwner_1->Is_Dead())
            return;

        SHAPE_USERDATA* pShapeUserData_0 = static_cast<SHAPE_USERDATA*>(pairs[i].shape[0]->userData);
        SHAPE_USERDATA* pShapeUserData_1 = static_cast<SHAPE_USERDATA*>(pairs[i].shape[1]->userData);
        if (nullptr == pShapeUserData_0 || nullptr == pShapeUserData_1)
            continue;
        COLL_INFO CollInfo_0;
        CollInfo_0.pActorUserData = pActorUserData_0;
        CollInfo_0.pShapeUserData = pShapeUserData_0;

        COLL_INFO CollInfo_1;
        CollInfo_1.pActorUserData = pActorUserData_1;
        CollInfo_1.pShapeUserData = pShapeUserData_1;

        pOwner_0->OnContact_Modify(CollInfo_0, CollInfo_1, CModifiableContacts(pair.contacts),true);
        pOwner_1->OnContact_Modify(CollInfo_0, CollInfo_1, CModifiableContacts(pair.contacts), false);
    }
}

CPhysx_ContactModifyCallback* CPhysx_ContactModifyCallback::Create()
{
    return new CPhysx_ContactModifyCallback();
}

void CPhysx_ContactModifyCallback::Free()
{
    __super::Free();
}

CModifiableContacts::CModifiableContacts(PxContactSet& contactSet)
	: m_ContactSet(contactSet)
{
}

_float CModifiableContacts::Get_DynamicFriction(_uint _iIndex)
{
    return m_ContactSet.getDynamicFriction(_iIndex);
}

_float CModifiableContacts::Get_StaticFriction(_uint _iIndex)
{
	return m_ContactSet.getStaticFriction(_iIndex);
}

_float CModifiableContacts::Get_Restitution(_uint _iIndex)
{
	return m_ContactSet.getRestitution(_iIndex);
}

_vector CModifiableContacts::Get_Normal(_uint _iIndex)
{
    PxVec3 pxNormal = m_ContactSet.getNormal(_iIndex);
    return  { pxNormal.x,pxNormal.y, pxNormal.z };
}

_vector CModifiableContacts::Get_Point(_uint _iIndex)
{
    PxVec3 pxPoint = m_ContactSet.getPoint(_iIndex);
    return { pxPoint.x,pxPoint.y, pxPoint.z };
}

_vector CModifiableContacts::Get_Velocity(_uint _iIndex)
{
	PxVec3 pxVelocity = m_ContactSet.getTargetVelocity(_iIndex);
	return { pxVelocity.x,pxVelocity.y, pxVelocity.z };
}

_float CModifiableContacts::Get_MaxImpulse(_uint _iIndex)
{
    return m_ContactSet.getMaxImpulse(_iIndex);
}

_uint CModifiableContacts::Get_ContactCount()
{
    return m_ContactSet.size();
}

void CModifiableContacts::Set_DynamicFriction(_uint _iIndex, _float _fFriction)
{
	m_ContactSet.setDynamicFriction(_iIndex, _fFriction);
}

void CModifiableContacts::Set_StaticFriction(_uint _iIndex, _float _fFriction)
{
	m_ContactSet.setStaticFriction(_iIndex, _fFriction);
}

void CModifiableContacts::Set_Restitution(_uint _iIndex, _float _fRestitution)
{
	m_ContactSet.setRestitution(_iIndex, _fRestitution);
}

void CModifiableContacts::Set_Normal(_uint _iIndex, _fvector _vNormal)
{
	m_ContactSet.setNormal(_iIndex, PxVec3(_vNormal.m128_f32[0], _vNormal.m128_f32[1], _vNormal.m128_f32[2]));
}

void CModifiableContacts::Set_Normal(_uint _iIndex, _float3 _vNormal)
{
	m_ContactSet.setNormal(_iIndex, PxVec3(_vNormal.x, _vNormal.y, _vNormal.z));
}

void CModifiableContacts::Set_Point(_uint _iIndex, _fvector _vPoint)
{
	m_ContactSet.setPoint(_iIndex, PxVec3(_vPoint.m128_f32[0], _vPoint.m128_f32[1], _vPoint.m128_f32[2]));
}

void CModifiableContacts::Set_Point(_uint _iIndex, _float3 _vPoint)
{
	m_ContactSet.setPoint(_iIndex, PxVec3(_vPoint.x, _vPoint.y, _vPoint.z));
}

void CModifiableContacts::Set_Velocity(_uint _iIndex, _fvector _vVelocity)
{
	m_ContactSet.setTargetVelocity(_iIndex, PxVec3(_vVelocity.m128_f32[0], _vVelocity.m128_f32[1], _vVelocity.m128_f32[2]));
}

void CModifiableContacts::Set_Velocity(_uint _iIndex, _float3 _vVelocity)
{
	m_ContactSet.setTargetVelocity(_iIndex, PxVec3(_vVelocity.x, _vVelocity.y, _vVelocity.z));
}

void CModifiableContacts::Set_MaxImpulse(_uint _iIndex, _float _fImpulse)
{
    m_ContactSet.setMaxImpulse(_iIndex, _fImpulse);
}

void CModifiableContacts::Set_InvInertiaScale0(_float _fScale)
{
    m_ContactSet.setInvInertiaScale0(_fScale);
}

void CModifiableContacts::Set_InvInertiaScale1(_float _fScale)
{
    m_ContactSet.setInvInertiaScale1(_fScale);
}

void CModifiableContacts::Set_InvMassScale0(_float _fScale)
{
    m_ContactSet.setInvMassScale0(_fScale);
}

void CModifiableContacts::Set_InvMassScale1(_float _fScale)
{
    m_ContactSet.setInvMassScale1(_fScale);
}




void CModifiableContacts::Ignore(_uint _iIndex)
{
	m_ContactSet.ignore(_iIndex);
}
