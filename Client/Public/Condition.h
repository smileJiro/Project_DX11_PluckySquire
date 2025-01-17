#pragma once
#include "Base.h"

BEGIN(Client)

enum class  CONDITION_TYPE
{
	EQUAL,
	NOT_EQUAL,
	GREATER,
	LESS,
	EQUAL_GREATER,
	EQUAL_LESS,
	LAST
};
enum class  CONDITION_VAR_TYPE
{
	INT,
	FLOAT,
	BOOL,
	TRIGGER,
	LAST
};

struct ConditionVariable
{
};
struct IntConditionVariable : public ConditionVariable
{
	int* pIntValue;
};
struct FloatConditionVariable : public ConditionVariable
{
	float* pFloatValue;
};
struct BoolConditionVariable : public ConditionVariable
{
	bool* pBoolValue;
};
struct TriggerConditionVariable:public ConditionVariable
{
	bool bValue = false;
};
class Condition : public CBase
{
protected :
	Condition(CONDITION_TYPE eCondType, CONDITION_VAR_TYPE eVarType, ConditionVariable* pConditionVar);
	Condition(const Condition& Prototype);
	virtual ~Condition() = default;
public:
	virtual bool CheckCondition() abstract;

protected:
	ConditionVariable* m_pConditionVar;
	CONDITION_TYPE eCondType;
	CONDITION_VAR_TYPE eVarType;
public:
	virtual CBase* Clone() abstract;
};
class IntCondition : public Condition
{
protected:
	IntCondition(CONDITION_TYPE eCondType, int iCompareValue, ConditionVariable* pConditionVar);
	IntCondition(const IntCondition& Prototype);
public:
	bool CheckCondition() override;

	int m_iCompareValue;

public:
	static IntCondition* Create(CONDITION_TYPE eCondType, int iCompareValue, ConditionVariable* pConditionVar);
	virtual CBase* Clone();
};
class FloatCondition : public Condition
{
protected:
	FloatCondition(CONDITION_TYPE eCondType, float fCompareValue, ConditionVariable* pConditionVar);

	FloatCondition(const FloatCondition& Prototype);
public:
	bool CheckCondition() override;

	float m_fCompareValue;

public:
	static FloatCondition* Create(CONDITION_TYPE eCondType, float fCompareValue, ConditionVariable* pConditionVar);

	virtual CBase* Clone();
};
class BoolCondition : public Condition
{
protected:
	BoolCondition(CONDITION_TYPE eCondType, bool bCompareValue, ConditionVariable* pConditionVar);
	BoolCondition(const BoolCondition& Prototype);
public:
	bool CheckCondition() override;

	bool m_bCompareValue;

public:
	static BoolCondition* Create(CONDITION_TYPE eCondType, bool bCompareValue, ConditionVariable* pConditionVar);
	virtual CBase* Clone();
};
class TriggerCondition : public Condition
{
protected:
	TriggerCondition(ConditionVariable* pConditionVar);
	TriggerCondition(const TriggerCondition& Prototype);
public:
	bool CheckCondition() override;

public:
	static TriggerCondition* Create(ConditionVariable* pConditionVar);
	virtual CBase* Clone();
};
END