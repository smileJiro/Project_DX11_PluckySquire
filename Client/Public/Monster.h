#pragma once
#include "Character.h"
#include "AnimEventReceiver.h"

BEGIN(Engine)
class CAnimEventGenerator;
class CDebugDraw_For_Client;
END

BEGIN(Client)
class CFSM;
class CDetectionField;
class CSneak_DetectionField;
class CMonster abstract : public CCharacter, public IAnimEventReceiver
{
public:
	enum MONSTERPART { PART_BODY, PART_EFFECT, PART_WEAPON,  PART_END };

	typedef struct tagMonsterDesc : public CCharacter::CHARACTER_DESC
	{
		_float fAlertRange;
		_float fChaseRange;
		_float fAttackRange;
		_float fAlert2DRange;
		_float fChase2DRange;
		_float fAttack2DRange;
		_float fDelayTime;
		_float fCoolTime;
		_float fHP;
		_float fFOVX;
		_float fFOVY;
		_bool isSneakMode = false;
		WAYPOINTINDEX eWayIndex = WAYPOINTINDEX::LAST;
	}MONSTER_DESC;

protected:
	CMonster(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CMonster(const CMonster& _Prototype);
	virtual ~CMonster() = default;

public:
	virtual void Set_State(_uint _iState)
	{
		m_iState = _iState;
	}
	virtual void Set_PreState(_uint _iState)
	{
		m_iPreState = _iState;
	}

	void Set_AnimChangeable(_bool _isChangeable)
	{
		m_isAnimChangeable = _isChangeable;
	}
	_bool Get_AnimChangeable()
	{
		return m_isAnimChangeable;
	}

	void Set_2D_Direction(F_DIRECTION _eDir);
	F_DIRECTION Get_2DDirection() 
	{ 
		return m_e2DDirection; 
	}

	void Set_SneakMode(_bool _isSneak)
	{
		m_isSneakMode = _isSneak;
	}

	_bool Get_SneakMode()
	{
		return m_isSneakMode;
	}

	_bool IsDelay() 
	{
		return m_isDelay;
	}
	_bool IsCool()
	{
		return m_isCool;
	}
	_bool IsContactToTarget()
	{
		return m_isContactToTarget;
	}

	_float3 Get_RayOffset() const
	{
		return m_vRayOffset;
	}
	_float Get_RayHalfWidth() const
	{
		return m_fRayHalfWidth;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Update(_float _fTimeDelta) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;
	virtual void OnContact_Stay(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;
	virtual void OnContact_Exit(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;

	virtual void OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)override;
	virtual void OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)override;
	virtual void OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)override;

public:
	virtual void	On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void	On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void	On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;

	virtual void	On_Hit(CGameObject* _pHitter, _float _fDamg);


public:
	virtual void Attack();
	virtual void Turn_Animation(_bool _isCW) {};

	//해당 상태의 애니메이션이 존재하는 지 확인(상태 전용 애니메이션이 없는 경우 false)
	virtual _bool Has_StateAnim(MONSTER_STATE _eState = MONSTER_STATE::LAST) { return true; }

public:
	virtual void				Change_Animation() {};
	virtual HRESULT				Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition = nullptr) override;
	void						Change_Dir();
	_bool						IsTarget_In_Detection();
	_bool						IsTarget_In_Sneak_Detection();
	_float					Restrict_2DRangeAttack_Angle(_float _fDegrees);

protected:
	void Delay_On() 
	{ 
		m_fAccTime = 0.f;
		m_isDelay = true; 
	}
	void Delay_Off()
	{
		m_isDelay = false;
		m_fAccTime = 0.f;
	}
	void CoolTime_On()
	{
		m_fAccTime = 0.f;
		m_isCool = true;
	}
	void CoolTime_Off()
	{
		m_isCool = false;
		m_fAccTime = 0.f;
	}

protected:
	_uint		m_iState = {};
	_uint		m_iPreState = {};
	CGameObject* m_pTarget = { nullptr };
	CFSM* m_pFSM = { nullptr };
	CAnimEventGenerator* m_pAnimEventGenerator = { nullptr };
	CDetectionField* m_pDetectionField = { nullptr };
	CSneak_DetectionField* m_pSneak_DetectionField = { nullptr };

#ifdef _DEBUG
	CDebugDraw_For_Client* m_pDraw = { nullptr };
#endif // _DEBUG
	
	_float m_fAlertRange = { 0.f };
	_float m_fChaseRange = { 0.f };
	_float m_fAttackRange = { 0.f };
	_float m_fAlert2DRange = { 0.f };
	_float m_fChase2DRange = { 0.f };
	_float m_fAttack2DRange = { 0.f };
	_bool m_isAnimChangeable = { true };

	_bool m_isContactToTarget = { false };

	_float m_fAccTime = { 0.f };
	_bool m_isDelay = { false };
	_float m_fDelayTime = { 0.f };
	_bool m_isCool = { false };
	_float m_fCoolTime = { 0.f };
	_uint	 m_iAttackCount = { 0 };

	_int m_iHp = { 0 };

	//시야각
	_float m_fFOVX = { 0.f };
	_float m_fFOVY = { 0.f };

	_float3 m_vRayOffset = { 0.f,0.f,0.f };
	_float m_fRayHalfWidth = {};

	//잠입 모드
	_bool m_isSneakMode = { false };

	//장애물 탐지
	WAYPOINTINDEX m_eWayIndex;

	_bool m_isDetect_Block = { false };
	_uint m_iDetect_Block_Count = { 0 };

	F_DIRECTION m_e2DDirection = { F_DIRECTION::F_DIR_LAST };

public:
	HRESULT Cleanup_DeadReferences() override;
	virtual void Active_OnEnable() override;
	virtual void Active_OnDisable() override;

protected:
	virtual HRESULT Ready_ActorDesc(void* _pArg);
	virtual HRESULT Ready_Components() = 0;
	virtual HRESULT Ready_PartObjects() = 0;


public:
	virtual CGameObject* Clone(void* _pArg) = 0;
	virtual void Free() override;
};

END