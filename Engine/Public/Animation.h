#pragma once
#include "Base.h"
#include "AnimEventGenerator.h"


BEGIN(Engine)
class CAnimEventGenerator;
class CAnimation abstract:
    public CBase
{
public:
    virtual _float Get_Progress() abstract;
    virtual void Set_Progress(_float _fProgerss)abstract;
    virtual void Reset();

    virtual void RegisterAnimEvent(ANIM_EVENT* _pAnimEvent) { m_AnimEvents.push_back(_pAnimEvent); }
private:
    vector< ANIM_EVENT*> m_AnimEvents;
};

END