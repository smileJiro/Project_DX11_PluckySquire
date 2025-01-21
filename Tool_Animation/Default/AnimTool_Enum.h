#pragma once

namespace AnimTool
{
	enum LEVEL_ID
	{
		LEVEL_LOADING,
		LEVEL_STATIC,
		LEVEL_ANIMTOOL,

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

	enum class F_DIRECTION
	{
		LEFT,
		RIGHT,
		UP,
		DOWN,
		F_DIR_LAST
	};
}