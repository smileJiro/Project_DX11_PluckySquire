#include "stdafx.h"
#include "CriticalSectionGuard.h"

CCriticalSectionGuard::CCriticalSectionGuard(CRITICAL_SECTION* _cs)
	:m_cs(_cs)
{
	if (nullptr == m_cs)
		MSG_BOX("Failed CriticalSectionCreate");

	EnterCriticalSection(m_cs);
}

CCriticalSectionGuard::~CCriticalSectionGuard()
{
	if (m_cs)
		LeaveCriticalSection(m_cs);
}
