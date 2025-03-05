#pragma once
#include "ModelObject.h"

BEGIN(Client)
class CDefenderPlayerProjectile :
    public CModelObject
{
public:
	typedef struct tagDefenderPlayerProjectileDesc : public CModelObject::MODELOBJECT_DESC
	{
		T_DIRECTION _eTDirection = T_DIRECTION::RIGHT;
	}DEFENDERPLAYER_PROJECTILE_DESC;

private:
	CDefenderPlayerProjectile(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CDefenderPlayerProjectile(const CDefenderPlayerProjectile& _Prototype);
	virtual ~CDefenderPlayerProjectile() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* _pArg) override;
	virtual void		Priority_Update(_float _fTimeDelta) override;
	virtual void		Update(_float _fTimeDelta) override;
	virtual void		Late_Update(_float _fTimeDelta) override;
	virtual HRESULT		Render() override;

private:
	T_DIRECTION m_eTDirection = T_DIRECTION::RIGHT;
public:
	static CDefenderPlayerProjectile* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END