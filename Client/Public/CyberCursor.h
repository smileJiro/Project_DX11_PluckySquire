#pragma once
#include "ModelObject.h"

BEGIN(Client)
class CCyberCursor :
    public CModelObject
{
public:
	explicit CCyberCursor(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CCyberCursor(const CCyberCursor& _Prototype);
	virtual ~CCyberCursor() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT	Render() override;

	void Set_CursorPos(_fvector _v3DPos);
protected:
	virtual HRESULT			Bind_ShaderResources_WVP() override;
	void Update_CursorPos();
private:
private:
	_float4x4	m_ViewMatrix{}, m_ProjMatrix{};
	_vector m_vWorldPos = { 0.f,0.f,0.f };
	
public:
	static CCyberCursor* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END