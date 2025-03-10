#pragma once
#include "Sneak_Troop.h"


BEGIN(Client)
class CSneak_SentryTroop : public CSneak_Troop
{
public:
	enum SENTRYTROOP_ANIM {S_ALERT_RIGHT, S_FALL, S_IDLE_RIGHT };
private:
	CSneak_SentryTroop(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CSneak_SentryTroop(const CSneak_SentryTroop& _Prototype);
	virtual ~CSneak_SentryTroop() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void	Priority_Update(_float _fTimeDelta) override;
	virtual void	Update(_float _fTimeDelta) override;
	virtual void	Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	virtual void Action_Scout() override;
	virtual void GameStart() override;

	virtual void Switch_Animation_ByState() override;


private:
	virtual HRESULT Ready_PartObjects();

private:
	_int			m_iMaxDetectCount = 15;
	
public:
	static CSneak_SentryTroop* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END