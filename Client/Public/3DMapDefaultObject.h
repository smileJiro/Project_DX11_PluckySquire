#pragma once
#include "3DMapObject.h"


BEGIN(Client)

class C3DMapDefaultObject final : public C3DMapObject
{
private:
	C3DMapDefaultObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	C3DMapDefaultObject(const C3DMapDefaultObject& _Prototype);
	virtual ~C3DMapDefaultObject() = default;

public:
	static C3DMapDefaultObject* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};
END
