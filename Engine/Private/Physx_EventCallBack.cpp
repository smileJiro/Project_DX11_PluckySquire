#include "Physx_EventCallBack.h"
#include "ActorObject.h"

CPhysx_EventCallBack::CPhysx_EventCallBack()
{
}

void CPhysx_EventCallBack::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{

	ACTOR_USERDATA* pActorUserData_0 = static_cast<ACTOR_USERDATA*>(pairHeader.actors[0]->userData);
	ACTOR_USERDATA* pActorUserData_1 = static_cast<ACTOR_USERDATA*>(pairHeader.actors[1]->userData);
	if (nullptr == pActorUserData_0 || nullptr == pActorUserData_1)
		return;

	CActorObject* pOwner_0 = pActorUserData_0->pOwner;
	CActorObject* pOwner_1 = pActorUserData_1->pOwner;
	
	if (false == IsOwnerObjectValid(pOwner_0) || false == IsOwnerObjectValid(pOwner_1))
		return;

	if (false == pOwner_0->Get_ActorCom()->Is_Active() || false == pOwner_1->Get_ActorCom()->Is_Active())
		return;

	for(_uint i = 0; i < nbPairs; ++i)
	{
		const PxContactPair& ContactPair = pairs[i];
		SHAPE_USERDATA* pShapeUserData_0 = static_cast<SHAPE_USERDATA*>(pairs[i].shapes[0]->userData);
		SHAPE_USERDATA* pShapeUserData_1 = static_cast<SHAPE_USERDATA*>(pairs[i].shapes[1]->userData);

		if (nullptr == pShapeUserData_0 || nullptr == pShapeUserData_1)
			continue;
		
		COLL_INFO CollInfo_0;
		CollInfo_0.pActorUserData = pActorUserData_0;
		CollInfo_0.pShapeUserData = pShapeUserData_0;

		COLL_INFO CollInfo_1;
		CollInfo_1.pActorUserData = pActorUserData_1;
		CollInfo_1.pShapeUserData = pShapeUserData_1;

		/* 충돌에 대한공통 정보 */
		vector<PxContactPairPoint> ContactPoints;
		ContactPoints.resize(pairs[i].contactCount);
		pairs[i].extractContacts(ContactPoints.data(), pairs[i].contactCount);

		if (ContactPair.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			/* Enter 상태 */
			pOwner_0->OnContact_Enter(CollInfo_0, CollInfo_1, ContactPoints);
			pOwner_1->OnContact_Enter(CollInfo_1, CollInfo_0, ContactPoints);
			continue;
		}

		if (ContactPair.events & PxPairFlag::eNOTIFY_TOUCH_PERSISTS) 
		{
			// Stay 상태 처리
			pOwner_0->OnContact_Stay(CollInfo_0, CollInfo_1, ContactPoints);
			pOwner_1->OnContact_Stay(CollInfo_1, CollInfo_0, ContactPoints);
			continue;
		}
		
		if (ContactPair.events & PxPairFlag::eNOTIFY_TOUCH_LOST) 
		{
			// Exit 상태 처리
			pOwner_0->OnContact_Exit(CollInfo_0, CollInfo_1, ContactPoints);
			pOwner_1->OnContact_Exit(CollInfo_1, CollInfo_0, ContactPoints);
			continue;
		}
	}

}

void CPhysx_EventCallBack::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	for (_uint i = 0; i < count; ++i)
	{
		if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
			continue;

		ACTOR_USERDATA* pTriggerActorUserData = static_cast<ACTOR_USERDATA*>(pairs[i].triggerActor->userData);
		ACTOR_USERDATA* pOtherActorUserData = static_cast<ACTOR_USERDATA*>(pairs[i].otherActor->userData);
		if (nullptr == pTriggerActorUserData || nullptr == pOtherActorUserData)
			continue;

		SHAPE_USERDATA* pTriggerShapeUserData = static_cast<SHAPE_USERDATA*>(pairs[i].triggerShape->userData);
		SHAPE_USERDATA* pOtherShapeUserData = static_cast<SHAPE_USERDATA*>(pairs[i].otherShape->userData);
		if (nullptr == pTriggerShapeUserData || nullptr == pOtherShapeUserData)
			continue;

		CActorObject* pTriggerOwner = pTriggerActorUserData->pOwner;
		CActorObject* pOtherOwner = pOtherActorUserData->pOwner;
		if (false == IsOwnerObjectValid(pTriggerOwner) || false == IsOwnerObjectValid(pOtherOwner))
			continue;

		if (false == pTriggerOwner->Get_ActorCom()->Is_Active() || false == pOtherOwner->Get_ActorCom()->Is_Active())
			continue;

		TRIGGER_ID ID = {};
		ID.iLeft_ID = pTriggerShapeUserData->iShapeInstanceID;
		ID.iRight_ID = pOtherShapeUserData->iShapeInstanceID;

		COLL_INFO TriggerCollInfo = {};
		TriggerCollInfo.pActorUserData = pTriggerActorUserData;
		TriggerCollInfo.pShapeUserData = pTriggerShapeUserData;

		COLL_INFO OtherCollInfo = {};
		OtherCollInfo.pActorUserData = pOtherActorUserData;
		OtherCollInfo.pShapeUserData = pOtherShapeUserData;

		
		if (PxPairFlag::eNOTIFY_TOUCH_FOUND == pairs[i].status)
		{
			pTriggerOwner->OnTrigger_Enter(TriggerCollInfo, OtherCollInfo);
			/* Enter 상태에 들어온 경우 해당 충돌 대상(Shape)들을 기준으로하는 고유 ID 값을 부여하고, 해당 ID 값으로 충돌데이터 */
			Add_StayTrigger(ID.ID, TriggerCollInfo, OtherCollInfo);

		}
		else if (PxPairFlag::eNOTIFY_TOUCH_LOST == pairs[i].status)
		{
			pTriggerOwner->OnTrigger_Exit(TriggerCollInfo, OtherCollInfo);
			Erase_StayTrigger(ID.ID);
		}
	}
}

bool CPhysx_EventCallBack::IsOwnerObjectValid(CActorObject* pOwner)
{
	return pOwner != nullptr &&
		pOwner->Is_Active() &&
		!pOwner->Is_Dead();

}
void CPhysx_EventCallBack::Update()
{
	
	for (auto& Pair : m_StayTrigger)
	{
		_bool isEmpty = false;
		if (nullptr == Pair.second.first.pActorUserData || nullptr == Pair.second.second.pActorUserData)
		{
			/* 만약 UserData가 삭제되었다면, 이는 어디선가 객체가 소멸했다는 의미임. */
			Erase_StayTrigger(Pair.first, &isEmpty);
			if (true == isEmpty)
				return;

			continue;
		}

		CActorObject* pTriggerObject = Pair.second.first.pActorUserData->pOwner;
		CActorObject* pOtherObject = Pair.second.second.pActorUserData->pOwner;
		if (false == IsOwnerObjectValid(pTriggerObject) || false == IsOwnerObjectValid(pOtherObject))
		{
			Erase_StayTrigger(Pair.first, &isEmpty);
			if (true == isEmpty)
				return;

			continue;
		}

		if(false == pTriggerObject->Is_Active() || false == pOtherObject->Is_Active())
		{
			Erase_StayTrigger(Pair.first, &isEmpty);
			if (true == isEmpty)
				return;

			continue;
		}
		
		/* 예외처리 모두 통과 시 */
		pTriggerObject->OnTrigger_Stay(Pair.second.first, Pair.second.second);
	}

}
void CPhysx_EventCallBack::Add_StayTrigger(_ulonglong _ID, const COLL_INFO& _TriggerInfo, const COLL_INFO& _OtherInfo)
{
	if (m_StayTrigger.end() != m_StayTrigger.find(_ID))
		return;

	m_StayTrigger.emplace(_ID, make_pair(_TriggerInfo, _OtherInfo));
}
void CPhysx_EventCallBack::Erase_StayTrigger(_ulonglong _ID, _bool* _isEmpty)
{
	auto& iter = m_StayTrigger.find(_ID);
	if (iter == m_StayTrigger.end())
		return;

	iter = m_StayTrigger.erase(iter);


	if (nullptr != _isEmpty)
		*_isEmpty = m_StayTrigger.empty();

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
