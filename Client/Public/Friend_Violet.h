#pragma once
#include "Friend.h"
BEGIN(Client)
class CFriend_Violet final : public CFriend
{
public :
	enum ANIM
	{
		Jump_Fall_violet_jump_fall_down,
		Jump_Fall_violet_jump_fall_right,
		Jump_Fall_violet_jump_fall_up,
		Jump_Rise_violet_jump_rise_down,
		Jump_Rise_violet_jump_rise_right,
		Jump_Rise_violet_jump_rise_up,
		Violet_C07_Idle01,
		Violet_C07_Idle02,
		Violet_C07_Idle03,
		Violet_C07_Idle04,
		Violet_C07_Idle05,
		Violet_C07_Idle06,
		Violet_C07_Talk01,
		Violet_C07_Talk02,
		Violet_C07_Talk02_Into,
		Violet_C07_Talk03,
		Violet_C07_Talk03_Into,
		Violet_C07_Talk04,
		Violet_C07_Talk05,
		Violet_C07_Talk06,
		Violet_C07_Talk07,
		Violet_C07_Talk08,
		Violet_C07_Turn,
		Violet_C07_Turn02,
		Violet_C07_Wave01,
		Violet_C08V02_End,
		Violet_C08V02_Idle01,
		Violet_C08V02_Idle01Transition,
		Violet_C08V02_Idle02,
		Violet_C08V02_Jump,
		Violet_C08V02_Talk01,
		Violet_C08V02_Talk01Transition,
		Violet_C08V02_Talk02,
		Violet_C08V02_Talk02Transition,
		Violet_C08V02_Talk03,
		Violet_C08V02_Talk03Transition,
		Violet_C08V02_Talk04,
		Violet_C08V02_Turn,
		Violet_C09_Hat,
		Violet_C09_JumpingOffBed,
		Violet_C09_LyingDespondent,
		Violet_C09_LyingToSittingUp,
		Violet_C09_Nod,
		Violet_C09_ReceivingWand,
		Violet_C09_ReturnToIdle,
		Violet_C09_SittingIdle,
		Violet_C09_SittingTalkingA,
		Violet_C09_SittingTalkingAIdle,
		Violet_C09_SittingTalkingB,
		Violet_C09_SittingTalkingBIdle,
		Violet_C09_StandingTalking,
		Violet_CliffCrumble,
		Violet_CliffCrumble_airloop,
		Violet_Cry_Into,
		Violet_Cry_Loop,
		Violet_Cry_Out,
		Violet_Floor_Talk,
		Violet_HappyCh09,
		Violet_Idle_Dismayed,
		Violet_Idle_Elevator,
		Violet_Idle_Floor,
		Violet_Idle_Peer,
		Violet_Idle_SurprisedMe_right,
		Violet_Idle_ThisIsHeavy_right,
		Violet_KnockedBack_Into,
		Violet_KnockedBack_Loop,
		Violet_KnockedBack_out,
		Violet_Magic01_Up,
		Violet_Magic01_Up_Loop_,
		Violet_Magic01_Up_Outro,
		VIOLET_MAGIC02_INTO_RIGHT,
		VIOLET_MAGIC02_LOOP_RIGHT,
		Violet_PullSwordV02,
		Violet_RubHead_Up,
		VIOLET_RUN_DOWN,	 // Move
		VIOLET_RUN_RIGHT,	 // Move
		VIOLET_RUN_UP,		 // Move
		Violet_Slipping_Down,
		Violet_Slipping_Right,
		Violet_Slipping_Up,
		Violet_Surprise_up,
		Violet_Surprise_up_loop,
		Violet_Surprise_up_out,
		VIOLET_TALK01_DOWN,	   // Talk
		VIOLET_TALK01_RIGHT,   // Talk
		VIOLET_TALK01_UP,	   // Talk
		Violet_Talk02,
		Violet_Talk02_right,
		Violet_Talk02_up,
		Violet_TalkGeneric_Right,
		Violet_Talk_Determined_Down,
		Violet_Talk_Dismayed,
		Violet_Talk_MyTown_up,
		Violet_Talk_Peer,
		Violet_Talk_Sheepish_right,
		Violet_Talk_Shocked_Right,
		Violet_Talk_SurprisedMe_right,
		Violet_Talk_ThisIsHeavy_right,
		Violet_Talk_Woo_right,
		Violet_Think_Right,
		Violet_Trapped,
		Violet_TumbaDanceV02,
		Violet_Whoa_Down,
		VIOLET_ATTACK_DOWN,	 // Attack
		VIOLET_ATTACK_RIGHT, // Attack
		VIOLET_ATTACK_UP,	 // Attack
		VIOLET_HIT_DOWN,	 // Hit
		VIOLET_HIT_RIGHT,	 // Hit
		VIOLET_HIT_UP,		 // Hit
		VIOLET_IDLE_DOWN, 	 // Idle
		VIOLET_IDLE_RIGHT,	 // Idle
		VIOLET_IDLE_UP,		 // Idle
		Violet_released,
		artia_violet_up_getApple,
		artia_violet_up_giveApple,
		c07_violet_fx_leaves,
		c07_violet_roll_loop,
		c07_violet_roll_start,
		c07_violet_roll_stop,
		c07_violet_talk_soCute,
		violet_gameover,
		violet_gameover_loop,
		VIOLET_MOJAM_INTO,	 // Mojam
		VIOLET_MOJAM_MOJAM,	 // Mojam
		violet_mojam_mojam_c02_0910,
		violet_mojam_mojam_c10_0910,
		violet_talk_holyguacamole_into_,
		violet_talk_holyguacamole_loop,
		violet_talk_tadaa_,

		ANIM_LAST
	};
	typedef struct tagFriendVioletDesc : CFriend::FRIEND_DESC
	{

	}FRIEND_VIOLET_DESC;

protected:
	CFriend_Violet(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CFriend_Violet(const CFriend& _Prototype);
	virtual ~CFriend_Violet() = default;

public:
	HRESULT					Initialize_Prototype() override;
	HRESULT					Initialize(void* _pArg) override;

private: /* Abstract Func */
	void					Change_AnimIndex_CurDirection() override;
	void					Finished_DialogueAction() override;
	void					On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx) override;
	void					Switch_AnimIndex_State() override;

public:
	void					Switch_PartAnim(_uint _iPartIndex, _uint _iAnimIndex, _bool _isLoop = false) override;

public:
	ANIM					Get_CurAnimIndex() const { return m_eCurAnimIndex; }

private:
	ANIM					m_eCurAnimIndex = ANIM::ANIM_LAST;

public:
	static CFriend_Violet* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject* Clone(void* _pArg) override;
	void Free() override;



};
END
