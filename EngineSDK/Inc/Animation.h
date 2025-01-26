#pragma once
#include "Base.h"
#include "AnimEventGenerator.h"


BEGIN(Engine)
class CAnimEventGenerator;
class ENGINE_DLL CAnimation abstract:
    public CBase
{
protected:
	CAnimation() = default;
	CAnimation(const CAnimation& _Prototype);
	virtual ~CAnimation() = default;
public:
    virtual _float Get_Progress() abstract;

    virtual void Set_Progress(_float _fProgerss)abstract;
	void Set_SpeedMagnifier(_float _fMag) { m_fSpeedMagnifier = _fMag; }
    void Set_Loop(_bool bIsLoop) { m_bLoop = bIsLoop; }
    virtual void Reset();

    virtual void RegisterAnimEvent(ANIM_EVENT* _pAnimEvent) { m_AnimEvents.push_back(_pAnimEvent); }
protected:
    _float m_fSpeedMagnifier = 1.f;
    _bool m_bLoop = false;
private:
    vector< ANIM_EVENT*> m_AnimEvents;
};

END