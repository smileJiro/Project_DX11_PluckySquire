#pragma once
#include "Engine_Defines.h"
namespace Client
{
	enum RENDERGROUP
	{
		RG_2D = 1000,
		RG_3D = 2000,

		RG_LAST,
	};

	enum PRIORITY_3D
	{
		PR3D_BOOK2D =		0,
		PR3D_PRIORITY =		10,
		PR3D_SHADOW =		20,
		PR3D_NONBLEND =		30,
		PR3D_LIGHTS =		40,
		PR3D_FINAL =		50,
		PR3D_BLEND =		60,
		PR3D_EFFECT =		70,
		PR3D_AFTEREFFECT =	80,
		PR3D_UI =			90,

		PR3D_LAST = 999
	};

	
	enum PRIORITY_2D
	{
		// 전처리 그룹
		PR2D_PRIORITY =			0,
		PR2D_SECTION_START =	10,
		// 10 ~ 970 
		//-> 섹션 유동생성 발급
		
		PR2D_SECTION_END =		970,
		PR2D_UI =				980,
		// 후처리 그룹
		PR2D_AFTER =			990,
		PR2D_LAST =				999
	};



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
		TRIGGER_OBJECT =		    0x80,
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
		CHANGE_COORDINATE,
		SET_KINEMATIC,
		CAMERATRIGGER_EVENT,
		

		LAST,
	};

	enum class MONSTER_STATE
	{
		IDLE,
		PATROL,
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
		PURPLEBALL,

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
	enum class E_DIRECTION
	{
		LEFT,
		RIGHT,
		UP,
		DOWN,
		LEFT_UP,
		RIGHT_UP,
		RIGHT_DOWN,
		LEFT_DOWN,
		E_DIR_LAST
	};

	enum class TRIGGER_TYPE
	{
		CAMERA_TRIGGER,
		LAST
	};
}
