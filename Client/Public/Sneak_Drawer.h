#pragma once
#include "Sneak_DefaultObject.h"
BEGIN(Client)
class CSneak_Drawer : public CSneak_DefaultObject
{
public:
	typedef struct tagDrawerDesc : public CModelObject::MODELOBJECT_DESC
	{
		_bool isBright = true;
	} DRAWER_DESC ;
	
private:
	CSneak_Drawer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CSneak_Drawer(const CSneak_Drawer& _Prototype);
	virtual ~CSneak_Drawer() = default;

	enum ANIM {B_CLOSE, B_CLOSED, B_OPENED, B_OPEN, D_CLOSE, D_CLOSED, D_OPENED, D_OPEN};

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual void Move() override;

private:
	_bool		m_isBright = true;
	_bool		m_isClosed = true;

public:
	static CSneak_Drawer* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END