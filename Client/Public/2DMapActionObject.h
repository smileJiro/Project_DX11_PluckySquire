#pragma once
#include "2DMapObject.h"


BEGIN(Client)

class C2DMapActionObject final : public C2DMapObject
{
public :
	enum MAPOBJ_2D_ACTION_TYPE
	{
		ACTIVE_TYPE_BREAKABLE,
		ACTIVE_TYPE_PATROL,
		ACTIVE_TYPE_ATTACKABLE,
		ACTIVE_TYPE_DIALOG,
		ACTIVE_TYPE_MODEL_CLOSE,
		ACTIVE_TYPE_DROPBLOCK,
		ACTIVE_TYPE_DYNAMIC_BACKGROUND,
		ACTIVE_TYPE_DAMEGED,
		ACTIVE_TYPE_ACTIONANIM,
		ACTIVE_TYPE_SKSP_HUMGRUMP,
		ACTIVE_TYPE_MODEL_CLOSE_DELETE,
		ACTIVE_TYPE_SPIKE,
		ACTIVE_TYPE_ACTION_TRIGGER,
		ACTIVETYPE_MOVING_BRIDGE,
		ACTIVE_TYPE_LAST
	};

private :
	enum MAPOBJ_2D_ACTIONOBJ_SPKIE_PATTERN
	{
		SPKIE_IDLE_DOWN,
		SPKIE_DOWN,
		SPKIE_UP,
		SPKIE_IDLE_UP,
		SPKIE_PATTERN_END
	};

private:
	C2DMapActionObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	C2DMapActionObject(const C2DMapActionObject& _Prototype);
	virtual ~C2DMapActionObject() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* _pArg) override;
	virtual void					Priority_Update(_float _fTimeDelta) override;
	virtual void					Update(_float _fTimeDelta) override;
	virtual void					Late_Update(_float _fTimeDelta) override;
	virtual HRESULT					Render() override;


	MAPOBJ_2D_ACTION_TYPE			Get_ActionType() { return m_eType; }

	void							Ready_Action();
	void							MapActionObject_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx);

public:
	virtual void					On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void					On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject) override;
	virtual void					On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject) override;



private :
	void							Spike_Pattern_UpdateProcess(_float _fTimeDelta);
	void							Spike_Pattern_EndProcess();

private:
	MAPOBJ_2D_ACTION_TYPE			m_eType;

	_bool							m_isFadeOut = false;
	_bool							m_isAction = false;
	_float							m_fAlpha = 1.f;
	_float							m_fFadeOutSecond = 0.3f;
	_float							m_fAccTime = 0.f;
	_uint							m_eCurPattern;
public:
	static C2DMapActionObject*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*		Clone(void* _pArg) override;
	virtual void				Free() override;
};
END
