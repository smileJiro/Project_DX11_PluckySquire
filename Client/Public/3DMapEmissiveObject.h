#pragma once
#include "3DMapObject.h"

BEGIN(Client)
class C3DMapEmissiveObject : public C3DMapObject
{
	typedef struct tagEmissiveObjectDesc: public C3DMapObject::MAPOBJ_3D_DESC
	{
		_float4			m_vColor = { 0.7f, 0.7f, 0.2f, 1.f };
		_float4			m_vBloomColor = { 0.8f, 0.8f, 0.6f, 1.f };
	} EMISSIVEOBJECT_DESC;

private:
	C3DMapEmissiveObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	C3DMapEmissiveObject(const C3DMapEmissiveObject& _Prototype);
	virtual ~C3DMapEmissiveObject() = default;


public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual HRESULT Render() override;

private:
	_float4		m_vColor = {0.7f, 0.7f, 0.2f, 1.f};
	_float4		m_vBloomColor = {0.8f, 0.8f, 0.6f, 1.f};

private:
	HRESULT			Bind_ShaderResources();

public:
	static C3DMapEmissiveObject* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END