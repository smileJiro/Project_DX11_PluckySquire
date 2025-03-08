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
		F_DIRECTION _eCurDirection = F_DIRECTION::F_DIR_LAST;

	} SNEAK_TROOP_DESC;
	enum TROOP_ANIM 
	{
		ALERT_DOWN = 0, ALERT_RIGHT, ALERT_UP,
		FALL_DOWN = 6,
		FLIP_DOWN = 7, FLIP_RIGHT, FLIP_UP,
		IDLE_DOWN = 10, IDLE_RIGHT, IDLE_UP,
		MOVE_DOWN = 13, MOVE_RIGHT, MOVE_UP
	};
	enum TROOP_ACTION
	{
		IDLE, MOVE, TURN, CATCH, FALL
	};
	enum TROOP_PART { TROOP_BODY, TROOP_LAST };

private:
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

	void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx);
	void Switch_Animation_ByState();
private:
	class CMinigame_Sneak* m_pSneakGameManager = { nullptr };

	CModelObject* m_pBody = nullptr;

private:
	F_DIRECTION	  m_eCurDirection = { F_DIRECTION::F_DIR_LAST };
	TROOP_ACTION  m_eCurAction = IDLE;

private:
	HRESULT Ready_PartObjects();

public:
	static CSneak_Troop* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END