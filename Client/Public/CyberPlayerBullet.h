#pragma once
#include "ModelObject.h"

BEGIN(Client)
class CCyberPlayerBullet :
    public CModelObject
{
public:
	typedef struct tagCyberPlayerProjectileDesc : public CModelObject::MODELOBJECT_DESC
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
	virtual void OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;

private:
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