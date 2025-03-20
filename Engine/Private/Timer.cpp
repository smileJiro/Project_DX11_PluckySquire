#include "Timer.h"

CTimer::CTimer(void)
	: m_fTimeDelta(0.0f)
{
	ZeroMemory(&m_FixTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_LastTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_FrameTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_CpuTick, sizeof(LARGE_INTEGER));
}

HRESULT CTimer::Ready_Timer(void)
{
	QueryPerformanceCounter(&m_FrameTime);			// 1077
	QueryPerformanceCounter(&m_LastTime);			// 1085
	QueryPerformanceCounter(&m_FixTime);			// 1090 Prev Count

	QueryPerformanceFrequency(&m_CpuTick);
    return S_OK;
}

void CTimer::Update_Timer(void)
{
	QueryPerformanceCounter(&m_FrameTime); // Current Countnmnnmmnnaaaaaaaaaaadddd

	/* 아래 코드는 요즘 같은 경우 불필요하다. QuadPart가 늘 고정값이기 때문,.*/
	if (m_FrameTime.QuadPart - m_FixTime.QuadPart >= m_CpuTick.QuadPart)
	{
		// 현재 프레임 - 기준 프레임이 1000만을 넘었을때,
		// CpuTick 다시계산 후 
		QueryPerformanceFrequency(&m_CpuTick);
		// FixTime 수정.
		m_FixTime = m_FrameTime;
	}

	m_fTimeDelta = (_float)(m_FrameTime.QuadPart - m_LastTime.QuadPart) / (_float)m_CpuTick.QuadPart * m_fTimeScale;

#ifdef NDEBUG // 프레임 정상화 되면 다시 걸기.
	//if (m_fTimeDelta > (1.f / 60.f))
	//{
	//	m_fTimeDelta = (1.f / 60.f);
	//}
#endif

	m_LastTime = m_FrameTime;
	Compute_FPS();
}

void CTimer::Render_FPS(HWND _hWnd)
{
	++m_iCallCount;
	m_fFPSAcc += m_fTimeDelta; // DT 누적
	if (m_fFPSAcc >= 1.0f) // 1초 카운트
	{
		m_iFPS = m_iCallCount;
		m_fFPSAcc = 0.;
		m_iCallCount = 0;

		wchar_t szBuffer[255] = {};
		swprintf_s(szBuffer, L"FPS : %d, DT : %f", m_iFPS, m_fTimeDelta);
		SetWindowText(_hWnd, szBuffer);
	}

}

void CTimer::Compute_FPS()
{
	++m_iCallCount;
	m_fFPSAcc += m_fTimeDelta; // DT 누적
	if (m_fFPSAcc >= 1.0f) // 1초 카운트
	{
		m_iFPS = m_iCallCount;
		m_fFPSAcc = 0.;
		m_iCallCount = 0;
	}
;
}

CTimer* CTimer::Create(void)
{
	CTimer* pInstance = new CTimer;

	if (FAILED(pInstance->Ready_Timer()))
	{
		MSG_BOX("Failed To Created : CTimer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTimer::Free(void)
{
	__super::Free();
}
