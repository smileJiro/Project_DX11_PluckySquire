#pragma once
#include "ModelObject.h"

BEGIN(Client)
class CScrollModelObject :
    public CModelObject
{
public:
	typedef struct tagScrollModelObjectDesc : public CModelObject::MODELOBJECT_DESC
	{
		_float2 vSectionSize = {};
	}SCROLLMODELOBJ_DESC;

protected:
	CScrollModelObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CScrollModelObject(const CScrollModelObject& _Prototype);
	virtual ~CScrollModelObject() = default;
public:
	virtual HRESULT		Initialize(void* _pArg) override;
	virtual void Update(_float _fTimeDelta) override;
	void Set_Direction(T_DIRECTION _eTDir);
private:
	void Scroll(_float _fTimeDelta);
private:
	_float2 m_fSectionSize = {};
};

END