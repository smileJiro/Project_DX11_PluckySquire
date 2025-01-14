#pragma once
#include "Base.h"
BEGIN(Engine)

class CTimer : public CBase
{
private:
	CTimer(void);
	virtual ~CTimer(void) = default;


public:
	HRESULT Ready_Timer(void); // Timer Initialize
	void Update_Timer(void); // DeltaTime Calculator
public:
	_float Get_TimeDelta() const { return m_fTimeDelta; }

	// Set
	void Set_TimeScale(_float _fTimeScale) { m_fTimeScale = _fTimeScale; }
	void Reset_TimeScale() { m_fTimeScale = 1.0f; }
private:
	LARGE_INTEGER			m_FrameTime = {};
	LARGE_INTEGER			m_FixTime = {};
	LARGE_INTEGER			m_LastTime = {};
	LARGE_INTEGER			m_CpuTick = {};

private:
	_float					m_fTimeDelta = {};
	_float					m_fTimeScale = 1.0f;
public:
	static CTimer* Create(void);
	virtual void Free(void) override;
};

END

