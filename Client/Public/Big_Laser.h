#pragma once
#include "ModelObject.h"

BEGIN(Client)

class CBig_Laser : public CModelObject
{
public :
	enum BIG_LASER_STATE
	{ 
		LASER_START,
		LASER_STOP,
		LASER_LOOP,
		LASER_END
	};
private:
	explicit CBig_Laser(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CBig_Laser(const CBig_Laser& _Prototype);
	virtual ~CBig_Laser() = default;

public:
	virtual HRESULT			Initialize(void* _pArg) override;
	void					Update(_float _fTimeDelta) override;
	void					Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;
	void					Anim_End(COORDINATE _eCoord, _uint iAnimIdx);

	void					Move_Start(_float _fMovePosX, _float _fSpeed);
	void					Set_Beam_Collider(_bool _isBeamCollider);
	_bool					Is_Move() { return m_isMove; }
	virtual void					On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject);

private:
	_bool	m_isMove = false;
	_float2 m_fTargetPos = {};
	
	HRESULT		Ready_Collider();
public:
	static CModelObject*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg) override;
	virtual void			Free() override;
};

END