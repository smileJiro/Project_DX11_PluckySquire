#pragma once
#include "Actor.h"
BEGIN(Engine)
class ENGINE_DLL CActor_Static final : public CActor
{
private:
	CActor_Static(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CActor_Static(const CActor_Static& _Prototype);
	virtual ~CActor_Static() = default;

public:
	HRESULT						Initialize_Prototype();
	HRESULT						Initialize(void* _pArg);
	void						Priority_Update(_float _fTimeDelta) override;
	void						Update(_float _fTimeDelta)  override;
	void						Late_Update(_float _fTimeDelta)  override;

public:
	static CActor_Static*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CComponent*					Clone(void* _pArg) override;
	void						Free() override;
};

END