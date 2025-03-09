#pragma once
#include "Sneak_Troop.h"


BEGIN(Client)
class CSneak_SentryTroop : public CSneak_Troop
{
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
};

END