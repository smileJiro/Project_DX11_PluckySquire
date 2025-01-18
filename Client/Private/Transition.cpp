#include "stdafx.h"
#include "Transition.h"

CTransition::CTransition(_uint iNextState)
	:CBase(), m_iNextState(iNextState)
{
}

CTransition::CTransition(const CTransition& Prototype)
	: CBase(Prototype), m_iNextState(Prototype.m_iNextState)
{
	for (auto& cond : Prototype.m_vecConditions)
	{
		m_vecConditions.push_back((Condition*)cond->Clone());
	}
}

bool CTransition::CheckConditions()
{
	for (auto& cond : m_vecConditions)
	{
		if (cond->CheckCondition() == false)
			return false;
	}
	return true;
}

CTransition* CTransition::Create(_uint iNextState)
{
		CTransition* pInstance = new CTransition(iNextState);
		return pInstance;
}

CBase* CTransition::Clone()
{
		return new CTransition(*this);
}

void CTransition::Free()
{
	__super::Free();
	for (auto& cond : m_vecConditions)
	{
		Safe_Release(cond);
	}
	m_vecConditions.clear();
}
