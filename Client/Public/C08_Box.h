#pragma once
#include "CarriableObject.h"

BEGIN(Client)
class CC08_Box :
	public CCarriableObject
{
protected:
	explicit CC08_Box(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CC08_Box(const CC08_Box& _Prototype);
	virtual ~CC08_Box() = default;
public:
	HRESULT Initialize(void* _pArg);
	virtual void		Update(_float _fTimeDelta) override;
	virtual void		Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void On_PickUpStart(CPlayer* _pPalyer, _fmatrix _matPlayerOffset) override;
	virtual void On_Land() override;
	virtual void On_LayDownEnd(_fmatrix _matWorld) override;

public:
	static CC08_Box* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END