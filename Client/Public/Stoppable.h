#pragma once
#include "Base.h"

BEGIN(Client)
class IStoppable
{
public:
	void Stop()
	{
		m_bStopoped = true; 
		On_Stop();
	};
	void UnStop() 
	{ 
		m_bStopoped = false; 
		On_UnStop();
	};

	virtual void On_Stop() {};
	virtual void On_UnStop() {};
	_bool Is_Stopped() { return m_bStopoped; };
private:
	_bool m_bStopoped = false;
};

END