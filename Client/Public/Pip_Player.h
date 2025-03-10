#pragma once

#include "Playable.h"
#include "AnimEventReceiver.h"

BEGIN(Engine)
class CModelObject;
END

BEGIN(Client)

class CPip_Player : public CPlayable
{

public:
	typedef struct tagPipDesc : public CCharacter::CHARACTER_DESC
	{
		
	} PIP_DESC;

	enum PIP_PART {PIP_BODY, PIP_LAST};
	enum PIP_ANIM {
		FLIP_DOWN, FLIP_RIGHT, FLIP_UP, 
		CAUGHT_DOWN, CAUGHT_RIGHT, CAUGHT_UP,
		IDLE_RIGHT, IDLE_DOWN, IDLE_UP, DANCE_DOWN,
		MOVE_DOWN, MOVE_RIGHT, MOVE_UP, ANIM_VICTORY,
	};
	enum PIP_ACTION { IDLE, MOVE, TURN, CAUGHT, FLIP, VICTORY };
private:
	CPip_Player(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPip_Player(const CPip_Player& _Prototype);
	virtual ~CPip_Player() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void	Priority_Update(_float _fTimeDelta) override;
	virtual void	Update(_float _fTimeDelta) override;
	virtual void	Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	F_DIRECTION Get_InputDirection() const { return m_eInputDirection; }
	_int		Get_CurTile() const { return m_iTargetTileIndex; }

public:
	void Start_Stage(_float2 _vPosition);
	void FadeIn(_float _fTime);

	void Action_Move(_int _iTileIndex, _float2 _vPosition);
	void Action_None();
	void Action_Flip();
	void Action_Caught();
	void Action_Victory();

	void Switch_Animation_ByState();

public:



public:
	void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx);


private:
	class CMinigame_Sneak*	m_pSneakGameManager = { nullptr };
	_int					m_iCurTileIndex = { 0 };
	_int					m_iTargetTileIndex = { 0 };
	_float2					m_vTargetPosition = {0.f, 0.f};

private:
	CModelObject* m_pBody = nullptr;
	F_DIRECTION	  m_eCurDirection = F_DIRECTION::RIGHT;
	F_DIRECTION	  m_eInputDirection = F_DIRECTION::F_DIR_LAST;
	PIP_ACTION	  m_eCurAction = IDLE;

private:
	void Do_Action(_float _fTimeDelta);
	void Dir_Move(_float _fTimeDelta);

	void	Key_Input(_float _fTimeDelta);


	HRESULT	Ready_Components();
	HRESULT	Ready_PartObjects();

public:
	static CPip_Player* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END