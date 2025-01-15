#pragma once
#include "ModelObject.h"

BEGIN(Engine)
class CTestTerrain final : public CModelObject
{
private:
	CTestTerrain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTestTerrain(const CTestTerrain& _Prototype);
	virtual ~CTestTerrain() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render_Shadow() override;

private:
	HRESULT					Ready_Components();

public:
	static CTestTerrain* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};
END
