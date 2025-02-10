#pragma once
#include "3DMapObject.h"


BEGIN(Client)

class C3DMapSpskObject final : public C3DMapObject
{
private:
	C3DMapSpskObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	C3DMapSpskObject(const C3DMapSpskObject& _Prototype);
	virtual ~C3DMapSpskObject() = default;

public:
	static C3DMapSpskObject* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};
END
