#pragma once
#include "ContainerObject.h"

BEGIN(Engine)
class CEffect_System;
END

BEGIN(Client)
class CFresnelModelObject;
class CCyberPlayerBullet :
    public CContainerObject
{
public:
	enum BULLET_PART {BULLET_TRAIL = 1, BULLET_END };
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
	virtual void Active_OnEnable() override;

	virtual void OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)override;
private:
	CFresnelModelObject* m_pModel = nullptr;
	_vector m_vLookDIr = { 0.f,0.f,0.f,0.f };
	_float m_fLifeTime = 5.f;
	_float m_fLifeTimeAcc = 0.f;
	_int m_iDamg = 1;
	_float m_fSpeed = 100.f;

	ID3D11Buffer* m_pFresnelBuffer = { nullptr };
	ID3D11Buffer* m_pColorBuffer = { nullptr };

	class CEffect_System* m_pParticleTrailEffect = { nullptr };


public:
	static CCyberPlayerBullet* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END