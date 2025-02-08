#pragma once
#include "ModelObject.h"

BEGIN(Engine)
class CModelObject;
END

BEGIN(Client)
class CItem abstract : public CModelObject
{
public:

	typedef struct tagItemDesc
	{

	}ITEM_DESC;

private:
	CItem(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CItem(const CItem& _Prototype);
	virtual ~CItem() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* _pArg) override;
	virtual void				Priority_Update(_float _fTimeDelta) override;
	virtual void				Update(_float _fTimeDelta) override;
	virtual void				Late_Update(_float _fTimeDelta) override;
	
public:
	virtual void				OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other) override;
	virtual void				OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)override;
	virtual void				OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)override;

private:
	_uint						m_iItemType = {};

public:
	virtual CGameObject*		Clone(void* _pArg) = 0;
	virtual void				Free() override;
};
END