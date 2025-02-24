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
		ACTOR_USERDATA* pTriggerActorUserData = static_cast<ACTOR_USERDATA*>(pairs[i].triggerActor->userData);
		ACTOR_USERDATA* pOtherActorUserData = static_cast<ACTOR_USERDATA*>(pairs[i].otherActor->userData);


		SHAPE_USERDATA* pTriggerShapeUserData = static_cast<SHAPE_USERDATA*>(pairs[i].triggerShape->userData);
		SHAPE_USERDATA* pOtherShapeUserData = static_cast<SHAPE_USERDATA*>(pairs[i].otherShape->userData);


		CActorObject* pTriggerOwner = pTriggerActorUserData->pOwner;
		CActorObject* pOtherOwner = pOtherActorUserData->pOwner;

		TRIGGER_ID ID = {};
		ID.iLeft_ID = pTriggerShapeUserData->iShapeInstanceID;
		ID.iRight_ID = pOtherShapeUserData->iShapeInstanceID;

		COLL_INFO TriggerCollInfo = {};
		TriggerCollInfo.pActorUserData = pTriggerActorUserData;
		TriggerCollInfo.pShapeUserData = pTriggerShapeUserData;

		COLL_INFO OtherCollInfo = {};
		OtherCollInfo.pActorUserData = pOtherActorUserData;
		OtherCollInfo.pShapeUserData = pOtherShapeUserData;

		if ((pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
			|| false == pTriggerOwner->Get_ActorCom()->Is_Active() || false == pOtherOwner->Get_ActorCom()->Is_Active())
		{
			_bool isResult = Erase_StayTrigger(ID.ID);
			if (true == isResult || PxPairFlag::eNOTIFY_TOUCH_LOST == pairs[i].status)
				pTriggerOwner->OnTrigger_Exit(TriggerCollInfo, OtherCollInfo);

			continue;

		}

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
	if (pOwner == nullptr)
		return false;

	if (false == pOwner->Is_Active() || true == pOwner->Is_Dead())
		return false;

	return true;



	//return pOwner != nullptr &&
	//	pOwner->Is_Active() &&
	//	!pOwner->Is_Dead();
}
void CPhysx_EventCallBack::Update()
{
	map<_ulonglong, pair<COLL_INFO, COLL_INFO>>::iterator iter;
	

	for (iter = m_StayTrigger.begin(); iter != m_StayTrigger.end();)
	{
		// Object DeadCheck

		CActorObject* LeftObject = (*iter).second.first.pActorUserData->pOwner;
		CActorObject* RightObject = (*iter).second.second.pActorUserData->pOwner;
		if (false == IsOwnerObjectValid(LeftObject) || false == IsOwnerObjectValid(LeftObject))
		{
			iter = m_StayTrigger.erase(iter);
			if (iter == m_StayTrigger.end())
				break;
			else 
				continue;
		}

		_bool isEmpty = false;
		if (nullptr == iter->second.first.pActorUserData || nullptr == iter->second.second.pActorUserData)
		{
			/* 만약 UserData가 삭제되었다면, 이는 어디선가 객체가 소멸했다는 의미임. */
			Erase_StayTrigger(iter->first, &iter, &isEmpty);
			if (true == isEmpty || iter == m_StayTrigger.end())
				return;

			continue;
		}

		CActorObject* pTriggerObject = iter->second.first.pActorUserData->pOwner;
		CActorObject* pOtherObject = iter->second.second.pActorUserData->pOwner;
		if (false == IsOwnerObjectValid(pTriggerObject) || false == IsOwnerObjectValid(pOtherObject))
		{
 			Erase_StayTrigger(iter->first, &iter, &isEmpty);
			if (true == isEmpty || iter == m_StayTrigger.end())
				return;

			continue;
		}

		if(false == pTriggerObject->Is_Active() || false == pOtherObject->Is_Active())
		{
			Erase_StayTrigger(iter->first, &iter, &isEmpty);
			if (true == isEmpty || iter == m_StayTrigger.end())
				return;

			continue;
		}
		
		/* 예외처리 모두 통과 시 */
		pTriggerObject->OnTrigger_Stay(iter->second.first, iter->second.second);
		++iter;
	}


}
void CPhysx_EventCallBack::Level_Enter()
{
}
void CPhysx_EventCallBack::Level_Exit()
{
	m_StayTrigger.clear();
}
void CPhysx_EventCallBack::Add_StayTrigger(_ulonglong _ID, const COLL_INFO& _TriggerInfo, const COLL_INFO& _OtherInfo)
{
	if (m_StayTrigger.end() != m_StayTrigger.find(_ID))
		return;

	m_StayTrigger.emplace(_ID, make_pair(_TriggerInfo, _OtherInfo));
}
_bool CPhysx_EventCallBack::Erase_StayTrigger(_ulonglong _ID, map<_ulonglong, pair<COLL_INFO, COLL_INFO>>::iterator* _pOutNextiter, _bool* _isEmpty)
{
	auto& iter = m_StayTrigger.find(_ID);
	if (iter == m_StayTrigger.end())
		return false;
	iter = m_StayTrigger.erase(iter);

	if (nullptr != _pOutNextiter)
		*_pOutNextiter = iter;

	if (nullptr != _isEmpty)
		*_isEmpty = m_StayTrigger.empty();

	return true;

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
