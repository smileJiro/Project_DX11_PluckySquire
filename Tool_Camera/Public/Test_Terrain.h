#pragma once
#include "ModelObject.h"

BEGIN(Camera_Tool)

class CTest_Terrain final : public CModelObject
{
private:
	CTest_Terrain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTest_Terrain(const CTest_Terrain& _Prototype);
	virtual ~CTest_Terrain() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;

private:
	HRESULT					Ready_Components();

public:
	static CTest_Terrain*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg) override;
	virtual void			Free() override;
};

END