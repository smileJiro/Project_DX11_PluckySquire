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

	void Set_Active(_bool _isActive) {
		m_isActive = _isActive;

		if (true == m_isActive)
			Active_OnEnable();
		else
			Active_OnDisable();
	}

	/* Active 변경시 호출되는 함수 추가. */
	virtual void Active_OnEnable() { return; };
	virtual void Active_OnDisable() { return; };
protected:
	_bool m_isActive = true;
public:
	virtual void Free();
};

END