#pragma once
#include "Npc.h"
#include "Interactable.h"



BEGIN(Client)
class CNPC_Companion : public CNPC, public IInteractable
{

public:
	typedef struct NpcCompanionDesc : public tagNPCDesc
	{
		 // 위치
		// 대기 애니메이션
		// 추가?
		// 이름?
		// 넣는 순서?
		


	}COMPANIONDESC;


protected:
	CNPC_Companion(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CNPC_Companion(const CNPC_Companion& _Prototype);
	virtual ~CNPC_Companion() = default;



public:
	virtual HRESULT				Initialize_Prototype();								// 프로토 타입 전용 Initialize
	virtual HRESULT				Initialize(void* _pArg);							// 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
	virtual void				Priority_Update(_float _fTimeDelta);				// 특정개체에 대한 참조가 빈번한 객체들이나, 등등의 우선 업데이트 되어야하는 녀석들.
	virtual void				Update(_float _fTimeDelta);
	virtual void				Late_Update(_float _fTimeDelta);
	virtual HRESULT				Render();

public:
	void						On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject) {};
	void						On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject) {};
	void						On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject) {};
 
public:
	static CNPC_Companion*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*		Clone(void* _pArg) override; // Clone() 프로토 타입이나 객체의 복사시 사용된다.
	virtual void				Free() override;
	virtual HRESULT				Cleanup_DeadReferences() override; // 참조 중인 게임오브젝트들 중 죽은 Dead상태인 오브젝트를 체크해서 참조해제.(액티브 false인 애들때매 만듬)

private:
	HRESULT						Ready_Companion(const _wstring& _strLayerName, void* _pArg);
	void						On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx) {};
	void						Set_NotDisPlaySection();
	void						Suicide();

protected:
	virtual	void				Interact(CPlayer* _pUser) {};
	virtual	_bool				Is_Interactable(CPlayer* _pUser) { return false; };
	virtual	_float				Get_Distance(COORDINATE _eCOord, CPlayer* _pUser) { return 0.f; };
	void						Set_TargetObject(_int _index);
	
	void						Set_PlayerObject(CNPC_Companion* _Desc, CPlayer* _Target) 
	{
		_Desc->m_pTargetObject = reinterpret_cast<CGameObject*>(_Target);
		Safe_AddRef(_Target);
	};
	
	void						Set_NPCObject(CNPC_Companion* _Desc, CNPC_Companion* _Target) 
	{ 
		_Desc->m_pTargetObject = _Target; 
		Safe_AddRef(_Target);
	};
	
	CGameObject*				Get_TargetObject() { return m_pTargetObject; };

	virtual	HRESULT				Ready_ActorDesc(void* _pArg) { return S_OK; };
	virtual	HRESULT				Ready_Components() { return S_OK; };
	virtual	HRESULT				Ready_PartObjects() { return S_OK; };

protected:
	virtual	HRESULT				Child_Initialize(void* _pArg);
	virtual void				Child_Update(_float _fTimeDelta);
	virtual void				Child_LateUpdate(_float _fTimeDelta);
	_float						Cal_PlayerDistance();
	_bool						Trace_Player(_float2 _vPlayerPos, _float2 vNPCPos);
	void						Delay_On();
	void						Delay_Off();
	_bool						is_LookOut() { return m_isLookOut; }
	_bool						is_Trace() { return m_isTrace; }
	vector<_wstring>			m_vecNotDisplaySections;


public:


protected:
	CGameObject*				m_pTargetObject = { nullptr };
	_bool						m_isTrace = { false };
	_bool						m_isDelay = { false };
	_float						m_fAccTime = { 0.f };
	_bool						m_isMove = { false };
	_float						m_f2DSpeed = { 0.f };
	_float						m_f3DSpeed = { 0.f };
	_float						m_fPlayerDistance = { 0.f };
	_float2						m_PrePlayerPos = { 0.f, 0.f };
	_float2						m_CurPlayerPos = { 0.f, 0.f };
	_float2						m_vPreNPCPos = { 0.f ,0.f };
	_bool						m_isLookOut = { false };
	_bool						m_isDeleteObejct = { false };



private:
	vector<CNPC_Companion*>		m_vecCompanionNpc = {};



};

END
