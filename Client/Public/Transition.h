#pragma once
#include "Condition.h"

BEGIN(Client)
class  CTransition : public CBase
{
protected:
	CTransition(_uint iNextState);
	CTransition(const CTransition& Prototype) ;
public:
	bool CheckConditions();

	_uint Get_ConditionCount() { return (_uint)m_vecConditions.size(); }
	_uint Get_NextState() { return m_iNextState; }
	void Add_Condition(Condition* pCond){m_vecConditions.push_back(pCond);}
private:
	vector<Condition*> m_vecConditions;
	_uint m_iNextState;
public:
	static CTransition* Create(_uint iNextState);
	virtual CBase* Clone();
	virtual void Free() override;
};

END