#pragma once
#include "ModelObject.h"
#include "Interactable.h"
BEGIN(Client)
class CPalmMarker :
	public CModelObject
{
private:
	explicit CPalmMarker(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CPalmMarker(const CPalmMarker& _Prototype);
	virtual ~CPalmMarker() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	void Update(_float _fTimeDelta) override;
	void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
private:

public:
	static CPalmMarker* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;

};

END