#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	CBase();
	virtual ~CBase() = default;

public:
	_uint AddRef();
	_uint Release();
protected:
	_uint m_iRefCnt = 0;

public:
	_bool Is_Active() const { return m_isActive; }

	void Set_Active(_bool _b) { m_isActive = _b; }
	void Active_On() { m_isActive = true; }
	void Active_Off() { m_isActive = false; }

protected:
	_bool m_isActive = true;
public:
	virtual void Free();
};

END