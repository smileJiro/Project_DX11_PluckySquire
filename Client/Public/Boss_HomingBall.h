#pragma once
#include "Monster.h"
#include "Client_Defines.h"

BEGIN(Client)
class CBoss_HomingBall final : public CContainerObject
{
public:
	typedef struct tagBoss_HomingBall_Desc : public CContainerObject::CONTAINEROBJ_DESC
	{
		_float fLifeTime;
	}BOSS_HOMINGBALL_DESC;

protected:
	CBoss_HomingBall(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CBoss_HomingBall(const CBoss_HomingBall& _Prototype);
	virtual ~CBoss_HomingBall() = default;

public:
	virtual HRESULT Initialize_Prototype(); // 프로토 타입 전용 Initialize
	virtual HRESULT Initialize(void* _pArg); // 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	virtual void Active_OnEnable() override;
	virtual void Active_OnDisable() override;

private:
	_float m_fLifeTime = { 5.f };
	_float m_fAccTime = { 0.f };

private:
	virtual HRESULT					Ready_Components();
	virtual HRESULT					Ready_PartObjects();

public:
	static CBoss_HomingBall* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};
END