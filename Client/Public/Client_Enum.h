#pragma once
#include "Engine_Defines.h"
namespace Client
{
	enum LEVEL_ID 
	{
		LEVEL_LOADING,
		LEVEL_STATIC,
		LEVEL_LOGO,
		LEVEL_GAMEPLAY,

		LEVEL_END
	};

	enum class EVENT_TYPE 
	{
		CREATE_OBJECT,
		DELETE_OBJECT,
		LEVEL_CHANGE,
		SET_ACTIVE,
		HR,

		LAST,
	};

}
