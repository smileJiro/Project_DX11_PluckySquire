#pragma once
#include "State_Sneak.h"

BEGIN(Client)

//웨이포인트로 순찰
class CPatrol_WayState final : public CState_Sneak
{
public:
	typedef struct tagPatrol_WayBound {
		_float3 vMin;
		_float3 vMax;
		_float fMinDis;
		_float fMaxDis;
	}SNEAKPATROLBOUND;

private:
	CPatrol_WayState();
	virtual ~CPatrol_WayState() = default;

public:
	void Set_Bound(_float3& _vPosition);
	void Set_CurPatrolIndex(_uint _iIndex)
	{
		m_iCurWayIndex = _iIndex;
	}

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void State_Enter() override;
	virtual void State_Update(_float _fTimeDelta) override;
	virtual void State_Exit() override;

private:
	void Patrol_WayMove(_float _fTimeDelta, _int _iDir);
	void Determine_Direction();

private:
	_float m_fAccTime = { 0.f };
	_float m_fAccDistance = { 0.f };
	_float m_fMoveDistance = {};
	_float m_fPatrol_WayOffset = {};
	_float m_fPatrol_Way2DOffset = {};
	SNEAKPATROLBOUND m_tPatrol_WayBound = {};
	_int m_iPrevDir = {};
	_int m_iDir = {};
	_bool m_isDelay = { false };
	_bool m_isMove = { false };
	_bool m_isTurn = { false };
	_float3 m_vRotate = {};
	_int m_iCurWayIndex = { 0 };
	_int m_iCurDirectionIndex = { 0 };
	_bool m_isBack = { false };
	_bool m_isDirBack = { false };
	_bool m_isToWay = { false };
	_float3 m_vDir = {};

	//정찰시 움직임만
	_bool m_isMoveOnly = { false };
	//정찰시 움직임과 회전
	_bool m_isMoveRotate = { false };
	//정찰시 회전만
	_bool m_isRotateOnly = { false };


public:
	static CPatrol_WayState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END