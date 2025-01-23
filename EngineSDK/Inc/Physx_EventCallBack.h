#pragma once
#include "Base.h"
BEGIN(Engine)
#pragma region TRIGGER_ID(Union)
union TRIGGER_ID
{
	struct
	{
		_uint iLeft_ID;
		_uint iRight_ID;
	};
	_ulonglong ID;
};
#pragma endregion // TRIGGER_ID(Union)

class CPhysx_EventCallBack final : public PxSimulationEventCallback, public CBase
{
private:
	CPhysx_EventCallBack();
	virtual ~CPhysx_EventCallBack() = default;

public:
	void Update(); // 물리시뮬이 다 끝난경우 호출.
private:
	// 충돌 이벤트가 발생했을 때 호출된다.
	void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override;
	// 트리거 이벤트(진입, 유지, 퇴출)가 발생했을 때 호출된다.
	void onTrigger(PxTriggerPair* pairs, PxU32 count) override;

private:
	// 파괴 가능한 제약조건이 파괴되었을 때 호출된다.(무슨 소리인지 아직 잘 모르겠음.)
	void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override;
	// 물리 시뮬레이션 공간에서의 수면 상태였던 객체가 깨어난 경우 호출된다.
	void onWake(PxActor** actors, PxU32 count) override;
	// 물리 시뮬레이션 공간에서의 객체가 수면상태에 진입한 경우 호출된다.
	void onSleep(PxActor** actors, PxU32 count) override;
	// 시뮬레이션 도중 강체의 위치를 미리 확인할 수 있는 콜백함수이다.(디버깅용으로도 사용가능하겠지.)
	void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) override;

private:
	_bool IsOwnerObjectValid(CActorObject* pOwner);

private:

	void Add_StayTrigger(_ulonglong _ID, const COLL_INFO& _TriggerInfo, const COLL_INFO& _OtherInfo);
	void Erase_StayTrigger(_ulonglong _ID, _bool* _isEmpty = nullptr);
private:
	map<_ulonglong, pair<COLL_INFO, COLL_INFO>> m_StayTrigger;
public:
	static CPhysx_EventCallBack* Create();
	void Free() override;
};
END
