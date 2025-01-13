#pragma once
#include "Base.h"
BEGIN(Engine)
class CTimer;

// Engine_dll 을 붙이지 않는 이유 : Engine에 있는 모든 기능은 GameInstance() 클래스 만을 통해 접근할 예정.
// Export가 필요한 것은 GameInstance 객체와 부모타입이 될 CGameObject, CLevel과 같은 클래스들 뿐.
class CTimer_Manager final : public CBase
{
private:
	CTimer_Manager();
	virtual ~CTimer_Manager() = default; // 소멸자를 막아두는 이유, 우리는 Free() 함수를 통해 좀 더 자유도 있는 소멸 과정을 설계한다.

public:
	void Update(_float _fTimeDelta);

public:
	_float Get_TimeDelta(const _wstring& strTimerTag);
	_uint Get_FPS() const { return m_iFPS; }

public:
	HRESULT Add_Timer(const _wstring& strTimerTag);
	void Update_Timer(const _wstring& strTimerTag);

public: /* Timer Controll */
	HRESULT Set_TimeScale(_float _fTimeScale, const _wstring& _strTimeTag);
	HRESULT Reset_TimeScale(const _wstring& _strTimeTag);
private:
	map<const _wstring, CTimer*> m_mapTimer;
private:
	CTimer* Find_Timer(const _wstring& strTimerTag);
private:
	// Check Fps
	_float						m_fAcc = 0.0f;
	_uint						m_iCallCount = 0;
	_uint						m_iFPS = 0;
public:
	static CTimer_Manager* Create();
	virtual void Free() override;
};

END

