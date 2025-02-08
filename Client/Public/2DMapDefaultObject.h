#pragma once
#include "2DMapObject.h"


BEGIN(Engine)
class CCollider;
END


BEGIN(Client)

class C2DMapDefaultObject final : public C2DMapObject
{
protected:
	C2DMapDefaultObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	C2DMapDefaultObject(const C2DMapDefaultObject& _Prototype);
	virtual ~C2DMapDefaultObject() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* _pArg) override;
	virtual void					Priority_Update(_float _fTimeDelta) override;
	virtual void					Update(_float _fTimeDelta) override;
	virtual void					Late_Update(_float _fTimeDelta) override;
	virtual HRESULT					Render() override;
	virtual HRESULT					Render_Shadow() override;


public:
	static C2DMapDefaultObject* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};
END
