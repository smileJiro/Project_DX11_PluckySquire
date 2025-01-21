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
		CHANGE_MONSTERSTATE,
		HR,

		LAST,
	};

	enum class MONSTER_STATE
	{
		IDLE,
		ALERT,
		CHASE,
		ATTACK,

		LAST,
	};
	enum class F_DIRECTION
	{
		LEFT,
		RIGHT,
		UP,
		DOWN,
		F_DIR_LAST
	};
}
