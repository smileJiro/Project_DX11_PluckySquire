
#include "Animation.h"

CAnimation::CAnimation(const CAnimation& _Prototype)
	:m_bLoop(_Prototype.m_bLoop)
	, m_fSpeedMagnifier(_Prototype.m_fSpeedMagnifier)
{
}

void CAnimation::Reset(_bool _bReverse)
{
	Set_Progress(_bReverse ? 1.f : 0.f);
	for (auto& pAnimEvent : m_AnimEvents)
	{
		pAnimEvent->bIsTriggered = false;
	}
}

