#include "ThreadPool.h"

CThreadPool::CThreadPool(size_t _iNumThreads)
	:m_iNumThreads(_iNumThreads)
	,m_isStopAll(false)
{
	m_WorkerThreads.reserve(_iNumThreads);
}

HRESULT CThreadPool::Initialize()
{
	// 요청된 쓰레드 개수만큼, 쓰레드를 추가하며, 각각의 워커 스레드들에게 WorkerThread() 함수를 실행시킨다.
	for (size_t i = 0; i < m_iNumThreads; ++i)
	{
		m_WorkerThreads.emplace_back([this]() 
			{
				// 람다를 통해 this를 캡처하고 this->WorkerThread(); 를 실행.
				this->WorkerThread();
			});
	}

	return S_OK;
}

void CThreadPool::EnqueueJob(function<void()> job)
{
	if (true == m_isStopAll)
	{
		/* m_isStopAll == true 라면, Job을 추가하지않는다. */
		throw runtime_error("ThreadPool 사용 중지됨");
	}

	{
		lock_guard<mutex> lock(m_mJobqueue);
		m_Jobs.push(move(job));
	}

	m_cvJobqueue.notify_one();

}

void CThreadPool::WorkerThread()
{
	while (true)
	{
		/* Mutex Lock */
		unique_lock<mutex> lock(m_mJobqueue);

		m_cvJobqueue.wait(lock, [this]() 
			{
				/* m_Jobs가 비어있지 않거나, m_isStopAll이 true인 경우, wait를 해제한다. (true == 해제 , false == 대기)*/
				return false == this->m_Jobs.empty() || true == m_isStopAll;
			});

		/* 1. wait 상태에 벗어났을때, m_isStopAll이 True 이고, Jobs 목록이 비어있다면, 해당 함수를 리턴하고 종료한다. */
		if (true == m_isStopAll && true == this->m_Jobs.empty())
			return;

		/* 2. wait 상태에 벗어났을때, Jobs 목록중 맨 앞에 위치한 잡을 빼고, 뮤텍스를 해제한다. */
		function<void()> Job = move(m_Jobs.front());
		m_Jobs.pop();
		lock.unlock();
		
		Job();
	}


}

CThreadPool* CThreadPool::Create(size_t _iNumThreads)
{
	CThreadPool* pInstance = new CThreadPool(_iNumThreads);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed Create : CThreadPool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CThreadPool::Free()
{
	m_isStopAll = true;

	m_cvJobqueue.notify_all();

	for (auto& Thread : m_WorkerThreads)
	{
		if(true == Thread.joinable())
		{
			Thread.join();
		}
	}


	__super::Free();
}
