#pragma once
#include "Base.h"

BEGIN(Engine)

class CAnimation abstract:
    public CBase
{
public:
    virtual _float Get_Progress() abstract;
};

END