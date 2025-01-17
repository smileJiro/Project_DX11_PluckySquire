#pragma once
#include "ModelObject.h"
#include "Client_Defines.h"

BEGIN(Client)
class CProjectile_BarfBug : public CModelObject
{
protected:
	CProjectile_BarfBug(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CProjectile_BarfBug(const CProjectile_BarfBug& _Prototype);
	virtual ~CProjectile_BarfBug() = default;

public:
	virtual HRESULT Initialize_Prototype(); // 프로토 타입 전용 Initialize
	virtual HRESULT Initialize(void* _pArg); // 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render_Shadow() { return S_OK; }
	virtual HRESULT			Render() override;
	virtual void Update(_float _fTimeDelta) override;

protected:
	//HRESULT					Ready_Components(MODELOBJECT_DESC* _pDesc);


public:
	static CProjectile_BarfBug* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};
END