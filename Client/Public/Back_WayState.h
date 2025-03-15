#pragma once
#include "State_Sneak.h"

BEGIN(Client)

class CBack_WayState final : public CState_Sneak
{
public:
	typedef struct tagSneak_PatrolBound {
		_float3 vMin;
		_float3 vMax;
		_float fMinDis;
		_float fMaxDis;
	}SNEAKPATROLBOUND;

private:
	CBack_WayState();
	virtual ~CBack_WayState() = default;

public:
	void Set_Bound(_float3& _vPosition);
	void Set_WayPoint(SNEAKWAYPOINTINDEX _eWayIndex);

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void State_Enter() override;
	virtual void State_Update(_float _fTimeDelta) override;
	virtual void State_Exit() override;

private:
	void Back_WayMove(_float _fTimeDelta, _int _iDir);
	void Determine_Direction();
	_vector Set_Sneak_PatrolDirection(_int _iDir);
	void Determine_BackDirection(_float3* _vDirection);

private:
	_float m_fAccTime = { 0.f };
	_float m_fAccDistance = { 0.f };
	_float m_fMoveDistance = {};
	_int m_iPrevDir = {};
	_int m_iDir = {};
	_bool m_isDelay = { false };
	_bool m_isMove = { false };
	_bool m_isTurn = { false };
	_float3 m_vRotate = {};
	_int m_iCurWayIndex = { 0 };
	_int m_iDestPatrolIndex = { 0 };
	_bool m_isBack = { false };
	_bool m_isToWay = { false };
	_float3 m_vDir = {};

public:
	static CBack_WayState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END