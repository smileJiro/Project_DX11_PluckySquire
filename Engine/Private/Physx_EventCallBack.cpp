#include "Physx_EventCallBack.h"

CPhysx_EventCallBack::CPhysx_EventCallBack()
{
}

void CPhysx_EventCallBack::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
	int a = 0;
}

void CPhysx_EventCallBack::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	int a = 0;
}

void CPhysx_EventCallBack::onConstraintBreak(PxConstraintInfo* constraints, PxU32 count)
{
}

void CPhysx_EventCallBack::onWake(PxActor** actors, PxU32 count)
{
}

void CPhysx_EventCallBack::onSleep(PxActor** actors, PxU32 count)
{
}

void CPhysx_EventCallBack::onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count)
{
}

CPhysx_EventCallBack* CPhysx_EventCallBack::Create()
{
	return new CPhysx_EventCallBack();
}

void CPhysx_EventCallBack::Free()
{
	__super::Free();
}
