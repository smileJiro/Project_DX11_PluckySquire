#pragma once
#include "Npc_Companion.h"




BEGIN(Client)
class CNPC_Violet final : public CNPC_Companion
{

public:
	enum RENDER
	{
		VIOLET_RENDER,
		VIOLET_NOTRENDER,
		VIOLETRENDER_END
	};

	enum ACTION
	{
		ACTION_WAIT,
		ACTION_IDLE,
		ACTION_INTERACTION,
		ACTION_DIALOG,
		ACTION_TRACE,
		ACTION_END
	};

	enum ANIMATION
	{
		ANIM_UP,
		ANIM_DOWN,
		ANIM_LEFT,
		ANIM_RIGHT,

		ANIM_IDLE,
		ANIM_END
	};


	enum ANIM_2D
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
		Violet_Magic02_into_right,
		Violet_Magic02_loop_right,
		Violet_PullSwordV02,
		Violet_RubHead_Up,
		Violet_Run_Down,
		Violet_Run_Right,
		Violet_Run_Up,
		Violet_Slipping_Down,
		Violet_Slipping_Right,
		Violet_Slipping_Up,
		Violet_Surprise_up,
		Violet_Surprise_up_loop,
		Violet_Surprise_up_out,
		Violet_Talk01,
		Violet_Talk01_Right,
		Violet_Talk01_Up,
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
		Violet_attack_down,
		Violet_attack_right,
		Violet_attack_up,
		Violet_hit_down,
		Violet_hit_right,
		Violet_hit_up,
		Violet_idle_down, // 기본 아이들
		Violet_idle_right,
		Violet_idle_up,
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
		violet_mojam_into,
		violet_mojam_mojam,
		violet_mojam_mojam_c02_0910,
		violet_mojam_mojam_c10_0910,
		violet_talk_holyguacamole_into_,
		violet_talk_holyguacamole_loop,
		violet_talk_tadaa_,

		Violet_End
	};


private:
	CNPC_Violet(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CNPC_Violet(const CNPC_Violet& _Prototype);
	virtual ~CNPC_Violet() = default;



public:
	virtual HRESULT				Initialize_Prototype();								// 프로토 타입 전용 Initialize
	virtual HRESULT				Initialize(void* _pArg);							// 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
	virtual void				Priority_Update(_float _fTimeDelta);				// 특정개체에 대한 참조가 빈번한 객체들이나, 등등의 우선 업데이트 되어야하는 녀석들.
	virtual void				Update(_float _fTimeDelta) { return; };
	virtual void				Late_Update(_float _fTimeDelta) { return; };
	virtual HRESULT				Render();

public:
	void						On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject);
	void						On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject);
	void						On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject);
	

protected:
	virtual HRESULT				Ready_ActorDesc(void* _pArg) override;
	virtual HRESULT				Ready_Components() override;
	virtual HRESULT				Ready_PartObjects() override;

	virtual void				Interact(CPlayer* _pUser);
	virtual _bool				Is_Interactable(CPlayer* _pUser);
	virtual _float				Get_Distance(COORDINATE _eCOord, CPlayer* _pUser) override;

	virtual void				Child_Update(_float _fTimeDelta);
	virtual void				Child_LateUpdate(_float _fTimeDelta);

public:
	static CNPC_Violet*			Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*		Clone(void* _pArg) override; // Clone() 프로토 타입이나 객체의 복사시 사용된다.
	virtual void				Free() override;
	virtual HRESULT				Cleanup_DeadReferences() override; // 참조 중인 게임오브젝트들 중 죽은 Dead상태인 오브젝트를 체크해서 참조해제.(액티브 false인 애들때매 만듬)
	void						Trace(_float _fTimeDelta);
	CNPC_Violet::RENDER			Get_RenderType() { return m_eRenderType; }

private:
	void						On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx);
	void						ChangeState_Panel();
	void						For_MoveAnimationChange(_float _fTimeDelta, _float2 _vNpcPos);
	void						Welcome_Jot(_float _fTimeDelta);
	void						Rock_Dialog(_float _fTimeDelta);

private:
	CNPC_Violet::ACTION			m_eActionType = { ACTION_END };
	CNPC_Violet::ANIMATION		m_eAnimationType = { ANIM_END };
	CNPC_Violet::RENDER			m_eRenderType = { VIOLETRENDER_END };
	_float						m_fIdleWaitTime = { 0.f };
	_float						m_fWelcomeWaitTime = { 0.f };
	_bool						m_isDialoging = { false };
	_bool						m_isDialogRock = { false };

	_bool						m_DisplayWelcomeDialog = { false };
	_bool						m_isWelcomeDialog = { false };
	_bool						m_isWelcomeDisplayDialog = { false };

	_bool						m_isRockDialogue = { false };

	_bool						m_isInteraction = { false };

};

END
