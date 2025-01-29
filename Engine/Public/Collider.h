#pragma once
#include "Component.h"
BEGIN(Engine)

class CCollider abstract:  public CComponent
{
public:
	typedef struct tagColliderDesc
	{
		CActorObject* pOwner = nullptr;

		_float2 vPosition = {};
		_float2 vOffsetPosition = {};
		_float2 vScale = {};
	}COLLIDER_DESC;
protected:
	CCollider(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCollider(const CCollider& _Prototype);
	virtual ~CCollider() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	static _uint			s_iNextColliderID;

private:
	_uint					m_iColliderID = 0;

private:
	CActorObject*			m_pOwner = nullptr;

	_float2					m_vPosition = {};
	_float2					m_vOffsetPosition = {};
	_float2					m_vScale = {};

public:
	virtual void Free() override;

};

END
