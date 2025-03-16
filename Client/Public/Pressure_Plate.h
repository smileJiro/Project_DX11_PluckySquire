#pragma once
#include "ModelObject.h"

BEGIN(Client)

class CPressure_Plate : public CModelObject
{
public:
	enum PRESSURE_PLATE_STATE {DOWN, DOWN_IDLE, IDLE, UP, PRESSURE_PLATE_STATE_LAST};

private:
	CPressure_Plate(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPressure_Plate(const CPressure_Plate& _Prototype);
	virtual ~CPressure_Plate() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual HRESULT Render() override;

public:
	_bool			Is_Down() { return (0 < m_iCollisionObjects); }

public:
	virtual void	On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject) override;
	virtual void	On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject) override;
	
	void			On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx);


private:
	_uint					m_iCollisionObjects = {0};
	PRESSURE_PLATE_STATE	m_eState = {IDLE};
public:
	static CPressure_Plate* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;

};

END