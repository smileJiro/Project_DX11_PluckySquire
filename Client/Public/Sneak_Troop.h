#pragma once
#include "Character.h"

BEGIN(Engine)
class CModelObject;
END

BEGIN(Client)

class CSneak_Troop : public CCharacter
{
public:
	typedef struct tagSneakTroop : public CCharacter::CHARACTER_DESC
	{
		_bool		_isMoveable = { false };
		_int		_iTileIndex = { -1 };
		F_DIRECTION _eCurDirection = F_DIRECTION::F_DIR_LAST;
		F_DIRECTION _eTurnDirection = F_DIRECTION::F_DIR_LAST;
		F_DIRECTION _eSecondTurnDirection = F_DIRECTION::F_DIR_LAST;

		_int		_iDetectCount = { 3 };
		_bool		_isRedDetect = { false };

	} SNEAK_TROOP_DESC;
	enum TROOP_ANIM 
	{
		ALERT_DOWN = 0, ALERT_RIGHT, ALERT_UP,
		FALL_DOWN = 6,
		FLIP_UD = 7, FLIP_LR, FLIP_DU,
		IDLE_DOWN = 10, IDLE_RIGHT, IDLE_UP,
		MOVE_DOWN = 13, MOVE_RIGHT, MOVE_UP,
		FLIP_LU = 19, FLIP_RD, FLIP_UR,
		FLIP_LD = 22, FLIP_DR, FLIP_RU
	};
	enum TROOP_ACTION
	{
		IDLE, MOVE, TURN, CATCH, FALL, SCOUT
	};
	enum TROOP_PART { TROOP_BODY, TROOP_LAST };

protected:
	CSneak_Troop(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CSneak_Troop(const CSneak_Troop& _Prototype);
	virtual ~CSneak_Troop() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void	Priority_Update(_float _fTimeDelta) override;
	virtual void	Update(_float _fTimeDelta) override;
	virtual void	Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_bool			Is_Moveable() const { return m_isMoveable; }
	_int			Predict_Route() const;
	F_DIRECTION		Get_CurrentDirection() const { return m_eCurDirection; }
	_int			Get_CurTile() const { return m_iTargetTileIndex; }
	TROOP_ACTION	Get_TroopState() const { return m_eCurAction; }


public:
	void Update_Detection();
	void Action_Move(_int _iTileIndex, _float2 _vPosition);
	void Action_Turn();
	virtual void Action_Fall();
	void Action_Catch();
	virtual void Action_Scout();
	virtual void GameStart();

	void FadeIn(_float _fTimeDelta);
	void FadeOut(_float _fTimeDelta);

	void Switch_TurnAnimation(F_DIRECTION _eTargetDirection);
	void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx);
	virtual void Switch_Animation_ByState();




protected:
	class CMinigame_Sneak* m_pSneakGameManager = { nullptr };
	CModelObject* m_pBody = nullptr;

protected:
	// 이동 가능 여부, Turn Direction
	_bool		  m_isMoveable = { false };
	F_DIRECTION	  m_eCurTurnDirection = { F_DIRECTION::F_DIR_LAST };
	F_DIRECTION	  m_eTurnDirection = { F_DIRECTION::F_DIR_LAST };
	F_DIRECTION	  m_eSecondTurnDirection = { F_DIRECTION::F_DIR_LAST };

	// 시작 타일 위치, 시작 방향
	_int		  m_iInitTileIndex = { -1 };
	F_DIRECTION	  m_eInitDirection = { F_DIRECTION::F_DIR_LAST };

	// 현재 타일 위치, 현재 방향
	_int		  m_iCurTileIndex = { -1 };
	F_DIRECTION	  m_eCurDirection = { F_DIRECTION::F_DIR_LAST };

	// 타겟 타일 위치, 타겟 위치
	_int				m_iTargetTileIndex = { 0 };
	_float2				m_vTargetPosition = { 0.f, 0.f };
	TROOP_ACTION		m_eCurAction = IDLE;

	// Detect 타일 개수, 타일 위치, Detect Type.
	_bool				m_isRedDetect = { false };
	_int				m_iDetectCount = { 3 };
	_int*				m_pDetected = { nullptr };

protected:
	void Do_Action(_float _fTimeDelta);
	void Dir_Move(_float _fTimeDelta);

	virtual HRESULT Ready_PartObjects();

public:
	static CSneak_Troop* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END