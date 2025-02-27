#pragma once
#include "Base.h"

BEGIN(Client)
class IBombable
{
public:
	virtual void Detonate() abstract;
};

END