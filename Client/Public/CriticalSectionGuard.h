#pragma once
#include "Engine_Defines.h"

BEGIN(Client)
class CCriticalSectionGuard
{
public:
	explicit CCriticalSectionGuard(CRITICAL_SECTION* _cs);
	virtual ~CCriticalSectionGuard();

	CCriticalSectionGuard() = delete;
	CCriticalSectionGuard(const CCriticalSectionGuard&) = delete;
	CCriticalSectionGuard& operator=(const CCriticalSectionGuard&) = delete;
	CCriticalSectionGuard(CCriticalSectionGuard&&) = delete;
	CCriticalSectionGuard& operator=(CCriticalSectionGuard&&) = delete;

private:
	CRITICAL_SECTION* m_cs = nullptr;
};
END
