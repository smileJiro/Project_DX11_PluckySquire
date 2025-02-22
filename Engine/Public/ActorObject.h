#pragma once
#include "GameObject.h"
#include "Actor.h"
BEGIN(Engine)
class ENGINE_DLL CActorObject abstract : public CGameObject
{
public:
	typedef struct tagActorObjectDesc : public CGameObject::GAMEOBJECT_DESC
	{
		ACTOR_TYPE			eActorType = ACTOR_TYPE::LAST;
		_bool				isAddActorToScene = true;
		CActor::ACTOR_DESC* pActorDesc = nullptr;
	}ACTOROBJECT_DESC;

protected:
	CActorObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CActorObject(const CActorObject& _Prototype);
	virtual ~CActorObject() = default;

public:
	virtual HRESULT				Initialize_Prototype();								// 프로토 타입 전용 Initialize
	virtual HRESULT				Initialize(void* _pArg);							// 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
public:/* Actor를 Scene에 추가 혹은 제거 */
	void Add_ActorToScene();
	void Remove_ActorFromScene();
	_bool Is_ActorInScene();
public:
	virtual HRESULT				Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition = nullptr) override;
	
public:
	virtual void OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) { return; }
	virtual void OnContact_Stay(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) { return; }
	virtual void OnContact_Exit(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) { return; }

	virtual void OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other) { return; }
	virtual void OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other) { return; }
	virtual void OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other) { return; }


	void Add_Impuls(_fvector _vForce);
	void Add_Force(_fvector _vForce);
public:
	void		 Set_Dead() override;
public:
	// Get 
	ACTOR_TYPE					Get_ActorType() const { return m_pActorCom->Get_ActorType(); }
	CActor*							Get_ActorCom() const { return m_pActorCom; }
	// Set

protected:
	CActor*						m_pActorCom = nullptr;

protected:
	/* Active 변경시 호출되는 함수 추가. */
	virtual void Active_OnEnable() override;
	virtual void Active_OnDisable() override;
protected:
	HRESULT						Ready_Components(ACTOROBJECT_DESC* _pDesc);

public:
	void						Free() override;
	HRESULT						Cleanup_DeadReferences() override;
};

END