#pragma once
#include "Friend.h"
BEGIN(Client)
class CExcavator_Switch;
class CFriend_Thrash final : public CFriend
{
public :
	enum ANIM
	{
		Thrash_C06_End01,
		Thrash_C06_End02,
		Thrash_C06_Idle01,				
		Thrash_C06_Idle01Transition,
		Thrash_C06_Idle02,
		Thrash_C06_Jump,
		Thrash_C06_talk01,
		Thrash_C06_talk01transition,
		Thrash_C06_talk02,
		Thrash_C06_talk02FX,
		Thrash_C06_talk02transition,
		Thrash_C06_talk03,
		Thrash_C08V02_End,
		Thrash_C08V02_Idle01,
		Thrash_C08V02_Idle01Transition,
		Thrash_C08V02_Idle01_Into,
		Thrash_C08V02_Idle02,
		Thrash_C08V02_Idle02Transition,
		Thrash_C08V02_Idle03,
		Thrash_C08V02_Jump,
		Thrash_C08V02_Talk01,
		THRASH_C09_JUMPINGOFFPIPE,	   //C09
		THRASH_C09_TAPPING,			   //C09
		Thrash_Elec,
		Thrash_Elec_ow_0001_X,
		Thrash_Elec_ow_loop,
		Thrash_Elec_recovery,
		Thrash_Happy_Into_,
		Thrash_Happy_Loop,
		Thrash_Happy_Out,
		THRASH_HIT_DOWN,
		THRASH_HIT_RIGHT,
		THRASH_HIT_UP,
		Thrash_Idle_Elevator,
		THRASH_IDLE_FLOOR,
		Thrash_Idle_Peer_right,
		Thrash_KnockedBack_Into,
		THRASH_KNOCKEDBACK_LOOP,
		Thrash_KnockedBack_Out,
		Thrash_PullsSwordV02,
		Thrash_Released,
		Thrash_Shocked_up_,
		Thrash_Slipping_Down,
		Thrash_Slipping_Right,
		Thrash_Slipping_Up,
		Thrash_Talk_Generic_right,
		Thrash_Talk_GoodbyeKrong_right,
		Thrash_Talk_MeanVibe_Up,
		Thrash_Talk_NoPass_Down,
		Thrash_Talk_Peer_right,
		Thrash_Talk_SS,
		Thrash_Talk_Serious_Right,
		Thrash_Talk_Surprised_up,
		Thrash_Talk_TotallyUncool_Right,
		Thrash_Talk_Upward_Right,
		Thrash_Talk_YoBozos,
		Thrash_Trapped,
		Thrash_TumbaDance,
		THRASH_ATTACK_DOWN,		// Fight
		THRASH_ATTACK_RIGHT,	// Fight
		THRASH_ATTACK_UP,		// Fight
		Thrash_backflip_right,
		Thrash_emotions_into,
		Thrash_emotions_loop,
		Thrash_emotions_out,
		Thrash_fall_down,
		Thrash_fall_right,
		Thrash_fall_up,
		THRASH_IDLE_DOWN,	   // Idle
		THRASH_IDLE_RIGHT,	   // Idle
		THRASH_IDLE_UP,		   // Idle
		THRASH_MOJAM_INTO,	   //Mojam
		THRASH_MOJAM_MOJAM,	   //Mojam
		Thrash_mojam_mojam_c02_0910,
		Thrash_mojam_mojam_c10_0910,
		THRASH_PULL_LOOP,	// 보스전 당기기 모션
		Thrash_rise_down,
		Thrash_rise_right,
		Thrash_rise_up,
		THRASH_ROLL_INTO,	// 당기고 나서 구르는 모션 시작
		THRASH_ROLL_LOOP,	// 구르기
		THRASH_ROLL_OUT,	// 구르기 끝
		THRASH_RUN_DOWN,	// MOVE
		THRASH_RUN_RIGHT,	// MOVE
		THRASH_RUN_UP,		// MOVE
		Thrash_salute_into,
		Thrash_salute_loop,
		Thrash_salute_out,
		Thrash_salutevillagers_,
		Thrash_salutevillagers_loop,
		Thrash_shocked_right,
		THRASH_TALK01_DOWN,	   // TALK
		THRASH_TALK01_RIGHT,   // TALK
		THRASH_TALK01_UP,	   // TALK
		Thrash_talkvillagers,  
		Thrash_think_right,

		// 아래부턴 안쓴다.
		gnom_backflip_right,
		gnom_idle_down,
		gnom_idle_right,
		gnom_idle_up,
		gnom_jump_fall_down_,
		gnom_jump_fall_right_,
		gnom_jump_fall_up_,
		gnom_jump_rise_down,
		gnom_jump_rise_right,
		gnom_jump_rise_up,
		gnom_run_down,		
		gnom_run_right,		
		gnom_run_up,		
		gnom_shocked_right,
		gnom_think_right,
		thrash_cheer_down_Flipbook,
		thrash_gameover,
		thrash_gameover_loop_,
		thrash_talk_happy_down_Flipbook,

		ANIM_LAST
	};
	typedef struct tagFriendThrashDesc : CFriend::FRIEND_DESC
	{

	}FRIEND_THRASH_DESC;
	enum FRIEND_STATE  {};
protected:
	CFriend_Thrash(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CFriend_Thrash(const CFriend& _Prototype);
	virtual ~CFriend_Thrash() = default;

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
	void					Go_Switch(CExcavator_Switch* _pSwitch);

private:
	CExcavator_Switch*		m_pSwitch = nullptr;
	_bool					m_isGoSwitch = false;

private:
	virtual void			Move_Arrival() override;

public:
	static CFriend_Thrash* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject* Clone(void* _pArg) override;
	void Free() override;



};
END
