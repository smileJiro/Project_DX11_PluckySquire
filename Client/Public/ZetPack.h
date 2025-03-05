#pragma once
#include "ModelObject.h"

BEGIN(Engine)
class CEffect_System;
class CLight_Target;
END

BEGIN(Client)
class CZetPack :
    public CModelObject
{
public:
	enum ANIMATION_2D
	{
		ANIM_IDLE,
		ANIM_JUMP_RISE,
		ANIM_JUMP_FALL,
		ANIM_ASCEND,
		ANIM_ATTACK,
		ANIM_DESCEND,
		ANIM_FLOAT,
		ANIM_RUN,
		ANIM_SWORDTHROW_INTO,
		ANIM_SWORDTHROW_LOOP,
		ANIM_SWORDTHROW_OUT,
		ANIM_DIE,
		ANIMATION_2D_LAST

	};
	enum ZET_STATE
	{
		STATE_IDLE,
		STATE_ASCEND,
		STATE_DESCEND,
		ZET_STATE_LAST
	};
public:
	typedef struct tagZetPackDesc : public CModelObject::MODELOBJECT_DESC
	{
		CPlayer* pPlayer = nullptr;
	}ZETPACK_DESC;
private:
	explicit CZetPack(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CZetPack(const CZetPack& _Prototype);
	virtual ~CZetPack() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	void Priority_Update(_float _fTimeDelta) override;
	void Update(_float _fTimeDelta) override;
	void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

	void ReFuel();
	_bool Is_FuelEmpty() { return m_fFuel <= 0.f; }
	//추진
	void Propel(_float _fTimeDelta);

	void Switch_State(ZET_STATE _eState);

public:
	// Get
	ZET_STATE Get_State() const { return m_eState; }
	// Set
private:
	CPlayer* m_pPlayer = nullptr;
	_bool m_bPropel = false;

	_float m_fFuel = 1.;
	_float m_fMaxFuel = 1.f;
	_float m_fFuelConsumption3D = 0.4f;
	_float m_fFuelConsumption2D = 0.4f;

	//추진력.
	_float m_fPropelForce3D = 1900.f;
	_float m_fPropelForce2D = 5000.f;

	//최대 연료일 때 [추진력 * m_fMaxForeceRatio] 만큼의 추진력을 냄.
	_float m_fMaxForeceRatio = 1.0f;
	//최저 추진력 비율. 연료가 남아있는 동안엔 최소 이정도임.
	_float m_fMinForeceRatio3D = 0.5f;
	_float m_fMinForeceRatio2D = 0.2f;

	// MinForceFuelRatio 만큼의 연료가 남았을 때 [추진력 * m_fMinForceFuelRatio] 만큼의 추진력을 냄.
	_float m_fMinForceFuelRatio3D = 0.0f;
	_float m_fMinForceFuelRatio2D = 0.725f;


	ZET_STATE m_eState = ZET_STATE::STATE_IDLE;

private: /* 태웅 추가 Target Light */
	HRESULT Ready_TargetLight();
private:
	CLight_Target* m_pTargetLight = nullptr;
public:
	static CZetPack* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END