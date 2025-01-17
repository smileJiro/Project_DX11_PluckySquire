#include "stdafx.h"
#include "Condition.h"

Condition::Condition(CONDITION_TYPE eCondType, CONDITION_VAR_TYPE eVarType, ConditionVariable* pConditionVar)
	: eCondType(eCondType), eVarType(eVarType), m_pConditionVar(pConditionVar)
{
}

Condition::Condition(const Condition& Prototype)
	: CBase(Prototype), eCondType(Prototype.eCondType), eVarType(Prototype.eVarType)
{
}

#pragma region Int
//INT CONDITION
IntCondition::IntCondition(CONDITION_TYPE eCondType, int iCompareValue, ConditionVariable* pConditionVar)
	: Condition(eCondType, CONDITION_VAR_TYPE::INT, pConditionVar), m_iCompareValue(iCompareValue)
{
}

IntCondition::IntCondition(const IntCondition& Prototype)
	: Condition(Prototype), m_iCompareValue(Prototype.m_iCompareValue)
{
}

bool IntCondition::CheckCondition()
{
	IntConditionVariable* pIntConditionVar = static_cast<IntConditionVariable*>(m_pConditionVar);

	switch (eCondType)
	{
	case Client::CONDITION_TYPE::EQUAL:
		return *pIntConditionVar->pIntValue == m_iCompareValue;
	case Client::CONDITION_TYPE::NOT_EQUAL:
		return *pIntConditionVar->pIntValue != m_iCompareValue;
	case Client::CONDITION_TYPE::GREATER:
		return *pIntConditionVar->pIntValue > m_iCompareValue;
	case Client::CONDITION_TYPE::LESS:
		return *pIntConditionVar->pIntValue < m_iCompareValue;
	case Client::CONDITION_TYPE::EQUAL_GREATER:
		return *pIntConditionVar->pIntValue >= m_iCompareValue;
	case Client::CONDITION_TYPE::EQUAL_LESS:
		return *pIntConditionVar->pIntValue <= m_iCompareValue;
	case Client::CONDITION_TYPE::LAST:
	default:
		return false;
	}
}

IntCondition* IntCondition::Create(CONDITION_TYPE eCondType, int iCompareValue, ConditionVariable* pConditionVar)
{
	IntCondition* pInstance = new IntCondition(eCondType, iCompareValue, pConditionVar);
	return pInstance;
}

CBase* IntCondition::Clone()
{
	return new IntCondition(*this);
}
#pragma endregion

#pragma region Float

//FLOAT CONDITION
FloatCondition::FloatCondition(CONDITION_TYPE eCondType, float fCompareValue, ConditionVariable* pConditionVar)
	: Condition(eCondType, CONDITION_VAR_TYPE::FLOAT, pConditionVar), m_fCompareValue(fCompareValue)
{
}

FloatCondition::FloatCondition(const FloatCondition& Prototype)
	: Condition(Prototype), m_fCompareValue(Prototype.m_fCompareValue)
{
}

bool FloatCondition::CheckCondition()
{
	FloatConditionVariable* pFloatConditionVar = static_cast<FloatConditionVariable*>(m_pConditionVar);

	switch (eCondType)
	{
	case Client::CONDITION_TYPE::EQUAL:
		return *pFloatConditionVar->pFloatValue == m_fCompareValue;
	case Client::CONDITION_TYPE::NOT_EQUAL:
		return *pFloatConditionVar->pFloatValue != m_fCompareValue;
	case Client::CONDITION_TYPE::GREATER:
		return *pFloatConditionVar->pFloatValue > m_fCompareValue;
	case Client::CONDITION_TYPE::LESS:
		return *pFloatConditionVar->pFloatValue < m_fCompareValue;
	case Client::CONDITION_TYPE::EQUAL_GREATER:
		return *pFloatConditionVar->pFloatValue >= m_fCompareValue;
	case Client::CONDITION_TYPE::EQUAL_LESS:
		return *pFloatConditionVar->pFloatValue <= m_fCompareValue;
	case Client::CONDITION_TYPE::LAST:
	default:
		return false;
	}

}

FloatCondition* FloatCondition::Create(CONDITION_TYPE eCondType, float fCompareValue, ConditionVariable* pConditionVar)
{
	FloatCondition* pInstance = new FloatCondition(eCondType, fCompareValue, pConditionVar);
	return pInstance;
}

CBase* FloatCondition::Clone()
{
	return new FloatCondition(*this);
}
#pragma endregion

#pragma region Bool

BoolCondition::BoolCondition(CONDITION_TYPE eCondType, bool bCompareValue, ConditionVariable* pConditionVar)
	: Condition(eCondType, CONDITION_VAR_TYPE::BOOL, pConditionVar), m_bCompareValue(bCompareValue)
{
}

BoolCondition::BoolCondition(const BoolCondition& Prototype)
	: Condition(Prototype), m_bCompareValue(Prototype.m_bCompareValue)
{
}

bool BoolCondition::CheckCondition()

{
	BoolConditionVariable* pBoolConditionVar = static_cast<BoolConditionVariable*>(m_pConditionVar);

	switch (eCondType)
	{
	case Client::CONDITION_TYPE::EQUAL:
		return *pBoolConditionVar->pBoolValue == m_bCompareValue;
	case Client::CONDITION_TYPE::NOT_EQUAL:
		return *pBoolConditionVar->pBoolValue != m_bCompareValue;
	case Client::CONDITION_TYPE::LAST:
	default:
		return false;
	}
}

BoolCondition* BoolCondition::Create(CONDITION_TYPE eCondType, bool bCompareValue, ConditionVariable* pConditionVar)
{
	BoolCondition* pInstance = new BoolCondition(eCondType, bCompareValue, pConditionVar);
	return pInstance;
}

CBase* BoolCondition::Clone()
{
	return new BoolCondition(*this);
}
#pragma endregion

TriggerCondition::TriggerCondition(ConditionVariable* pConditionVar)
	: Condition(CONDITION_TYPE::EQUAL, CONDITION_VAR_TYPE::TRIGGER, pConditionVar)
{
}

TriggerCondition::TriggerCondition(const TriggerCondition& Prototype)
	: Condition(Prototype)
{
}

bool TriggerCondition::CheckCondition()
{
	TriggerConditionVariable* pTriggerConditionVar = static_cast<TriggerConditionVariable*>(m_pConditionVar);

	return pTriggerConditionVar->bValue == true;
}

TriggerCondition* TriggerCondition::Create(ConditionVariable* pConditionVar)
{
	TriggerCondition* pInstance = new TriggerCondition(pConditionVar);
	return pInstance;
}

CBase* TriggerCondition::Clone()
{
	return new TriggerCondition(*this);
}
