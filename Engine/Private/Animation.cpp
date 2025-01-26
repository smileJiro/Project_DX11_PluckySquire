
#include "Animation.h"

void CAnimation::Reset()
{
	Set_Progress(0.f);
	for (auto& pAnimEvent : m_AnimEvents)
	{
		pAnimEvent->bIsTriggered = false;
	}
}
