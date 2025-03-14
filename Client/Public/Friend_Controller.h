#pragma once
#include "Base.h"
// 1. 친구를 등록한다. 
// 2. 친구의 상태를 조작한다. 
// 3. 친구들끼리 뭉쳐다니기 기능에 필요한 데이터를 저장하고 제공한다. 
BEGIN(Client)
class CFriend_Controller final : public CBase
{
	DECLARE_SINGLETON(CFriend_Controller)

public:

public:
	virtual void Free() override;
};

END