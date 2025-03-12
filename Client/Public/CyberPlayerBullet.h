#pragma once
#include "ContainerObject.h"
BEGIN(Engine)
class CModelObject;
END
BEGIN(Client)
class CCyberPlayerBullet :
    public CContainerObject
{
public:
	typedef struct tagCyberPlayerProjectileDesc : public CContainerObject::CONTAINEROBJ_DESC
	{

	}CYBERPLAYER_PROJECTILE_DESC;

private:
	CCyberPlayerBullet(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCyberPlayerBullet(const CCyberPlayerBullet& _Prototype);
	virtual ~CCyberPlayerBullet() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* _pArg) override;
	virtual void		Priority_Update(_float _fTimeDelta) override;
	virtual void		Update(_float _fTimeDelta) override;


public:
	virtual void OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)override;
private:
	CModelObject* m_pModel = nullptr;
	_vector m_vLookDIr = { 0.f,0.f,0.f,0.f };
	_float m_fLifeTime = 10.f;
	_float m_fLifeTimeAcc = 0.f;
	_int m_iDamg = 1;
	_float m_fSpeed = 30.f;
public:
	static CCyberPlayerBullet* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END