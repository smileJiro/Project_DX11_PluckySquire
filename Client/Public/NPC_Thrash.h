#pragma once
#include "Npc_Companion.h"




BEGIN(Client)
class CNPC_Thrash final : public CNPC_Companion
{

public:
	enum RENDER
	{
		THRASH_RENDER,
		THRASH_NOTRENDER,
		THRASHRENDER_END
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

	enum MOVING
	{
		MOVING_PRE,
		MOVING_CUR,
		MOVING_DIA,
		MOVING_END
	};


	enum ANIM_2D
	{
		Thrash_C06_End01,
		Thrash_C06_End02,
		Thrash_C06_Idle01,				// 아이들
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
		Thrash_C09_JumpingOffPipe,
		Thrash_C09_Tapping,
		Thrash_Elec,
		Thrash_Elec_ow_0001_X,
		Thrash_Elec_ow_loop,
		Thrash_Elec_recovery,
		Thrash_Happy_Into_,
		Thrash_Happy_Loop,
		Thrash_Happy_Out,
		Thrash_Hit_down,
		Thrash_Hit_right,
		Thrash_Hit_up,
		Thrash_Idle_Elevator,
		Thrash_Idle_Floor,
		Thrash_Idle_Peer_right,
		Thrash_KnockedBack_Into,
		Thrash_KnockedBack_Loop,
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
		Thrash_attack_down,
		Thrash_attack_right,
		Thrash_attack_up,
		Thrash_backflip_right,
		Thrash_emotions_into,
		Thrash_emotions_loop,
		Thrash_emotions_out,
		Thrash_fall_down,
		Thrash_fall_right,
		Thrash_fall_up,
		Thrash_idle_down,
		Thrash_idle_right,
		Thrash_idle_up,
		Thrash_mojam_into,
		Thrash_mojam_mojam,
		Thrash_mojam_mojam_c02_0910,
		Thrash_mojam_mojam_c10_0910,
		Thrash_pull_loop,
		Thrash_rise_down,
		Thrash_rise_right,
		Thrash_rise_up,
		Thrash_roll_into,
		Thrash_roll_loop,
		Thrash_roll_out,
		Thrash_run_down,
		Thrash_run_right,
		Thrash_run_up,
		Thrash_salute_into,
		Thrash_salute_loop,
		Thrash_salute_out,
		Thrash_salutevillagers_,
		Thrash_salutevillagers_loop,
		Thrash_shocked_right,
		Thrash_talk01_down,
		Thrash_talk01_right,
		Thrash_talk01_up,
		Thrash_talkvillagers,
		Thrash_think_right,
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

		Thrash_End
	};


private:
	CNPC_Thrash(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CNPC_Thrash(const CNPC_Thrash& _Prototype);
	virtual ~CNPC_Thrash() = default;



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
	virtual _float				Get_Distance(COORDINATE _eCOord, CPlayer* _pUser);

	virtual void				Child_Update(_float _fTimeDelta);
	virtual void				Child_LateUpdate(_float _fTimeDelta);

public:
	static CNPC_Thrash*			Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*		Clone(void* _pArg) override; // Clone() 프로토 타입이나 객체의 복사시 사용된다.
	virtual void				Free() override;
	virtual HRESULT				Cleanup_DeadReferences() override; // 참조 중인 게임오브젝트들 중 죽은 Dead상태인 오브젝트를 체크해서 참조해제.(액티브 false인 애들때매 만듬)

private:
	void						Trace(_float _fTimeDelta);
	void						On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx);
	void						ChangeState_Panel(_float _fTimeDelta, _float2 _vNpcPos);
	void						For_MoveAnimationChange(_float _fTimeDelta, _float2 _vNpcPos);
	void						Welcome_Jot(_float _fTimeDelta);
	void						Rock_Dialogue(_float _fTimeDelta);

private:
	CNPC_Thrash::ACTION			m_eActionType = { ACTION_END };
	CNPC_Thrash::ANIMATION		m_eAnimationType = { ANIM_END };
	CNPC_Thrash::MOVING			m_eMoving = { MOVING_END };
	CNPC_Thrash::RENDER			m_eRenderType = { THRASHRENDER_END };
	_float						m_fIdleWaitTime = { 0.f };
	_float						m_fWelcomeWaitTime = { 0.f };
	_bool						m_isDialoging = { false };

	_float						m_fNextDialogWait = { 0.f };
	_bool						m_isNextDialog = { false };


	_bool						m_isInteraction = { false };
	_bool						m_isDisplayDialogue = { false };

};

END
