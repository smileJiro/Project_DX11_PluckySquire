#pragma once
#include "Level.h"

BEGIN(Client)

class CLevel_Static final : public CLevel
{
private:
	CLevel_Static(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_Static() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	static CLevel_Static* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;


};

END

