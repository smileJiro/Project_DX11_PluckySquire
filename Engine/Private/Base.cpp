#include "Base.h"



Engine::CBase::CBase()
{
}

/* _uint : 증가한 결과물을 리턴한다 */
_uint CBase::AddRef()
{
	return ++m_iRefCnt;
}
/* _uint : 감소하기 전의 결과물을 리턴한다.*/
_uint CBase::Release()
{
	if (0 == m_iRefCnt)
	{
		Free(); // 해당 객체의 재정의 된 Free()를 호출 한 뒤,Free()가 끝이나면 객체를 Delete
		delete this;
		
		return 0;
	}
	/* 감소한다 */
	else
	return m_iRefCnt--;

	// 여기서 감소하기 전의 카운트를 리턴하는 이유가 뭘까?
	// 만약 RefCnt가 1일 때를 생각해보자, 1인 경우 RefCnt를 감소 시킨 후, Return 하게 되면, 0을 리턴한다.
	// 우리는 현재 return 값이 0이면, 해당 객체는 올바르게 삭제된 것으로 가정하고있다.
	// 하지만 RefCnt가 1인경우 --RefCnt를 하여도 해당 객체는 삭제되지 않는다, 결국 이러한 착오를 피하기위해
	// 후위 연산을 통해 감소하기 전의 RefCnt를 리턴한다.
}

void CBase::Free()
{
}

