#pragma once
#include "Camera_Tool_Defines.h"
#include "ActorObject.h"

BEGIN(Engin)
class CCollider;
END

BEGIN(Camera_Tool)

class CTriggerObject : public CActorObject
{
public:
	enum TRIGGER_CONDITION
	{
		TRIGGER_ENTER,
		TRIGGER_STAY,
		TRIGGER_EXIT,

		CONDITION_END
	};

	typedef struct tagTriggerObjectDesc : public CActorObject::ACTOROBJECT_DESC
	{
		SHAPE_TYPE					eShapeType = { SHAPE_TYPE::LAST };
		_float3						vHalfExtents = { 0.5f, 0.5f, 0.5f };	// Box
		_float						fRadius = { 0.5f };						// Sphere
		_float3						vRotation = {};	// Box
	
		_uint						iFillterMyGroup = {};
		_uint						iFillterOtherGroupMask = {};

		_uint						iTriggerType = {};
		_wstring					szEventTag = {};
		TRIGGER_CONDITION			eConditionType = { CONDITION_END };
	}TRIGGEROBJECT_DESC;

protected:
	CTriggerObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTriggerObject(const CTriggerObject& _Prototype);
	virtual ~CTriggerObject() = default;

public:
	virtual HRESULT				Initialize_Prototype();								// 프로토 타입 전용 Initialize
	virtual HRESULT				Initialize(void* _pArg);							// 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
	
	virtual HRESULT				Render() override;

	virtual HRESULT				Initialize_3D_Trigger(CActor::ACTOR_DESC** _pActorDesc, TRIGGEROBJECT_DESC* _pDesc);							// 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
	virtual HRESULT				Initialize_2D_Trigger(TRIGGEROBJECT_DESC* _pDesc);							// 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.


	virtual void				Late_Update(_float _fTimeDelta);

public:
	_uint						Get_TriggerType() { return m_iTriggerType; }
	_wstring					Get_TriggerEventTag() { return m_szEventTag; }
	_uint						Get_ConditionType() { return m_eConditionType; }

#ifdef _DEBUG
	void						Set_TriggerType(_uint _iTriggerType) { m_iTriggerType = _iTriggerType; }
	void						Set_TriggerEventTag(_wstring _szEventTag) { m_szEventTag = _szEventTag; }
#endif

public:
	void						Set_CustomData(_wstring _Key, any _pValue);
	any							Get_CustomData(_wstring _Key);

	void						Resister_EnterHandler(function<void(_uint, _int, _wstring)> _Handler);
	void						Resister_StayHandler(function<void(_uint, _int, _wstring)> _Handler);
	void						Resister_ExitHandler(function<void(_uint, _int, _wstring)> _Handler);
	void						Resister_ExitHandler_ByCollision(function<void(_uint, _int, const COLL_INFO&, const COLL_INFO&)> _Handler);

public:
	virtual void				OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other) override;
	virtual void				OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other) override;
	virtual void				OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other) override;
public:
	virtual void				On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject);
	virtual void				On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject);
	virtual void				On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject);

protected:
	class CCollider*			m_pColliderCom = { nullptr };
	SHAPE_TYPE					m_eShapeType = { SHAPE_TYPE::LAST };
	_uint						m_iTriggerType;

	_int						m_iMyColliderGroup = {};
	_int						m_iTriggerID = {};
	_wstring					m_szEventTag = {};
	TRIGGER_CONDITION			m_eConditionType = { CONDITION_END };

	// CustumData
	unordered_map<_wstring, any>	m_CustomDatas;

	// Function
	function<void(_uint, _int, _wstring)>	m_EnterHandler;
	function<void(_uint, _int, _wstring)>	m_StayHandler;
	function<void(_uint, _int, _wstring)>	m_ExitHandler;
	function<void(_uint, _int, const COLL_INFO&, const COLL_INFO&)> m_CollisionExitHandler;


private:
	static _int					g_iNextID;

public:
	static CTriggerObject*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*		Clone(void* _pArg);
	void						Free() override;
};

END