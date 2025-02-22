#pragma once
#include "Base.h"

BEGIN(Engine)
class CThreadPool final : public CBase
{
private:
	CThreadPool(size_t _iNumThreads);
	virtual ~CThreadPool() = default;

public:
	HRESULT Initialize();

public: /* 작업 큐에 작업을 추가하는 함수. */
	void EnqueueJob(function<void()> job);
	//template<class F, class... Args>
	//future<typename invoke_result<F, Args...>::type> EnqueueJob(F&& f, Args&&... args);

private:
	size_t						m_iNumThreads = 0;
	vector<thread>				m_WorkerThreads; // 작업을 수행할 WorkerThread들을 보관하는 컨테이너
	queue<function<void()>>		m_Jobs; // 할 일들을 보관하는 작업 큐

	condition_variable			m_cvJobqueue;   // Job Queue와 관련된 condition_variable
	mutex						m_mJobqueue;				 // Job Queue와 관련된 mutex

private:
	_bool						m_isStopAll = false;

private:
	void WorkerThread();

public:
	static CThreadPool* Create(size_t _iNumThreads);
	void Free() override;
};
//
//template<class F, class ...Args>
//inline std::future<typename invoke_result<F, Args...>::type> CThreadPool::EnqueueJob(F&& f, Args&&... args)
//{
//	if (true == m_isStopAll)
//	{
//		/* m_isStopAll == true 라면, Job을 추가하지않는다. */
//		throw runtime_error("ThreadPool 사용 중지됨");
//	}
//
//	/* 1. F(Args...) >>> F 함수의 매개변수의 조합으로부터 반환타입을 추론한다. */
//	using return_type = typename invoke_result<F, Args...>::type;
//
//	/* 2. package_task<return_type()>는 호출 가능한 객체를 비동기 작업으로 실행가능하게 래핑한다. 
//	내부적으로 std::promise() 를 사용하여 작업 결과를 저장하고, 이를 std::future로 반환한다. */
//	auto pJob = make_shared<packaged_task<return_type()>>(bind(forward<F>(f), forward<Args>(args)...));
//
//	/* 3. future<> : 특정 비동기 작업의 결과를 나타낸다, 작업이 끝이나면 결과를 가져올 수 있다. */
//	future<return_type> Job_Result_future = pJob->get_future();
//
//	{
//		lock_guard<mutex> lock(m_mJobqueue);
//		m_Jobs.push([pJob]()
//			{
//				(*pJob)();
//			});
//	}
//
//	m_cvJobqueue.notify_one();
//
//	return Job_Result_future;
//}

END
