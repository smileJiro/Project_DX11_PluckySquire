#pragma once
#include "Friend.h"
BEGIN(Client)
class CFriend_Pip final : public CFriend
{
	enum ANIM
	{
		GameOver_Pip_Idle,
		GameOver_Pip_Recover,
		Pip_ButtonPush,
		PIP_C09_MOJAM,					//Mojam
		Pip_C09_Mojam_09_5152,
		Pip_C09_Mojam_10_0910,
		Pip_Jump_Fall_right_,
		PIP_JUMP_RISE_TALK01_DOWN,		  // TALK
		PIP_JUMP_RISE_TALK01_RIGHT,		  // TALK
		PIP_JUMP_RISE_TALK01_UP,		  // TALK
		Pip_Jump_Rise_right,
		PIP_RUN_DOWN,					  // MOVE
		PIP_RUN_RIGHT,					  // MOVE
		PIP_RUN_UP,						  // MOVE
		PIP_ATTACK_DOWN,				  // ATTACK
		PIP_ATTACK_RIGHT,				  // ATTACK
		PIP_ATTACK_UP,					  // ATTACK
		Pip_excited_down,
		Pip_excited_into,
		PIP_HAPPY,						  // HAPPY
		PIP_HAPPY_DOWN,					  // HAPPY
		PIP_HIT_DOWN,					  // HIT
		PIP_HIT_RIGHT,					  // HIT
		PIP_HIT_UP,						  // HIT
		PIP_IDLE_DOWN,					  // IDLE
		PIP_IDLE_RIGHT,					  // IDLE
		PIP_IDLE_UP,					  // IDLE
		Pip_jumpup_,
		Pip_jumpup_idle,
		Pip_knockedback_End,
		Pip_knockedback_Loop,
		Pip_released,
		Pip_trapped,
		pip_talk_happy_up_Flipbook,

		ANIM_LAST
	};
	typedef struct tagFriendPipDesc : CFriend::FRIEND_DESC
	{

	}FRIEND_PIP_DESC;

protected:
	CFriend_Pip(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CFriend_Pip(const CFriend& _Prototype);
	virtual ~CFriend_Pip() = default;

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
	static CFriend_Pip* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject* Clone(void* _pArg) override;
	void Free() override;



};
END
