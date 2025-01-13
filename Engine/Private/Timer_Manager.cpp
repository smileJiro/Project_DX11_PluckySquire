#include "Timer_Manager.h"
#include "Timer.h"

CTimer_Manager::CTimer_Manager()
{
}

void CTimer_Manager::Update(_float _fTimeDelta)
{
	++m_iCallCount;
	m_fAcc += _fTimeDelta; // DT 누적
	if (m_fAcc >= 1.) // 1초 카운트
	{
		m_iFPS = m_iCallCount;
		m_fAcc = 0.;
		m_iCallCount = 0;
	}
}

_float CTimer_Manager::Get_TimeDelta(const _wstring& strTimerTag)
{
	CTimer* pInstance = Find_Timer(strTimerTag);
	if (nullptr == pInstance)
		return 0.f;

	return pInstance->Get_TimeDelta();
}



HRESULT CTimer_Manager::Add_Timer(const _wstring& strTimerTag)
{
	if (nullptr != Find_Timer(strTimerTag))
	{
		return E_FAIL;
	}
	
	CTimer* pTimer = CTimer::Create();
	m_mapTimer.emplace(make_pair(strTimerTag, pTimer));


    return S_OK;
}

void CTimer_Manager::Update_Timer(const _wstring& strTimerTag)
{
	CTimer* pInstance = Find_Timer(strTimerTag);
	if (nullptr == pInstance)
	{
		return;
	}
	pInstance->Update_Timer(); // dt 계산.

}

HRESULT CTimer_Manager::Set_TimeScale(_float _fTimeScale, const _wstring& _strTimeTag)
{
	CTimer* pTimer = Find_Timer(_strTimeTag);
	if (nullptr == pTimer)
		return E_FAIL;

	pTimer->Set_TimeScale(_fTimeScale);
	return S_OK;
}

HRESULT CTimer_Manager::Reset_TimeScale(const _wstring& _strTimeTag)
{
	CTimer* pTimer = Find_Timer(_strTimeTag);
	if (nullptr == pTimer)
		return E_FAIL;

	pTimer->Reset_TimeScale();
	return S_OK;
}

CTimer* CTimer_Manager::Find_Timer(const _wstring& strTimerTag)
{
	auto iter = m_mapTimer.find(strTimerTag);

	if (iter == m_mapTimer.end())
	{
		return nullptr;
	}

    return iter->second;
}

CTimer_Manager* CTimer_Manager::Create()
{
	return new CTimer_Manager();
}

void CTimer_Manager::Free()
{

	// 범위 기반 for문 맵컨테이너 순회하며, 이때 iter는 페어에 참조.
	for (auto& iter : m_mapTimer)
	{
		Safe_Release(iter.second);
	}
	m_mapTimer.clear();

	__super::Free();

}
