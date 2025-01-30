#pragma once
#include "Character.h"

BEGIN(Client)
class CFSM;
class CNPC abstract : public CCharacter
{
public:
	enum NPCPART { PART_BODY, PART_EFFECT, PART_WEAPON,  PART_END };

	typedef struct tagNPCDesc : public CCharacter::CHARACTER_DESC
	{
		_uint iMainIndex;
		_uint iSubIndex = 0;
		
	}NPC_DESC;

protected:
	CNPC(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CNPC(const CNPC& _Prototype);
	virtual ~CNPC() = default;

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

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Update(_float _fTimeDelta) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Attack(_float _fTimeDelta);

public:
	virtual void				Change_Animation() {};
	virtual HRESULT				Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition = nullptr) override;
	void						Change_Dir();

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
	_uint			m_iState = {};
	_uint			m_iPreState = {};
	CGameObject*	m_pTarget = { nullptr };
	CFSM*			m_pFSM = { nullptr };
	_float			m_fAlertRange = { 0.f };
	_float			m_fChaseRange = { 0.f };
	_float			m_fAttackRange = { 0.f };
	_bool			m_isAnimChangeable = { true };

	_float			m_fAccTime = { 0.f };
	_bool			m_isDelay = { false };
	_float			m_fDelayTime = { 0.f };
	_bool			m_isCool = { false };
	_float			m_fCoolTime = { 0.f };
	_uint			m_iAttackCount = { 0 };

	F_DIRECTION		m_e2DDirection = { F_DIRECTION::F_DIR_LAST };

	_uint			m_iMainIndex = { 0 };
	_uint			m_iSubIndex = { 0 };

public:
	HRESULT Cleanup_DeadReferences() override;
	virtual void Active_OnEnable() override;
	virtual void Active_OnDisable() override;


protected:
	virtual HRESULT Ready_ActorDesc(void* _pArg) = 0;
	virtual HRESULT Ready_Components() = 0;
	virtual HRESULT Ready_PartObjects() = 0;


public:
	virtual CGameObject* Clone(void* _pArg) = 0;
	virtual void Free() override;
};

END