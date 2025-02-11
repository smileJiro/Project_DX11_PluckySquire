#pragma once
#include "ModelObject.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Camera_Tool)
class CBulb final : public CModelObject
{
public:
	typedef struct tagBulbDesc : public CModelObject::MODELOBJECT_DESC
	{

	}BULB_DESC;
private:
	explicit CBulb(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CBulb(const CBulb& _Prototype);
	virtual ~CBulb() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	static CBulb*			Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg) override;
	virtual void			Free() override;
};
END