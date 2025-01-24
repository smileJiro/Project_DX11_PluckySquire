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
	// 비트연산자를 통해 SimulationFilter를 설정해야해서 enum으로 하겠음. enum class는 비트연산자 쓰려면 형변환많이하고 번거로워서.
	enum OBJECT_GROUP // 아래 GROUP은 예시임. 편한대로 사용하시면 됨.
	{
		NONE =						0x00, 
		PLAYER =					0x02,
		MONSTER =					0x04,
		MAPOBJECT =					0x08,
		INTERACTION_OBEJCT =		0x10,
		PLAYER_PROJECTILE =			0x20,
		MONSTER_PROJECTILE =		0x40,
								 // 0x80
								 // 0x100
								 // 0x200
								 // 0x400 

		LAST
	};


	enum class EVENT_TYPE 
	{
		CREATE_OBJECT,
		DELETE_OBJECT,
		LEVEL_CHANGE,
		SET_ACTIVE,
		SETUP_SIMULATION_FILTER,
		CHANGE_MONSTERSTATE,
		CHANGE_BOSSSTATE,
		

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

	enum class BOSS_STATE
	{
		SCENE,
		IDLE,
		ATTACK,
		ENERGYBALL,
		HOMINGBALL,
		YELLOWBALL,

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
