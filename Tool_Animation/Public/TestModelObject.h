#pragma once
#include "ModelObject.h"
BEGIN(Engine)

END
BEGIN(AnimTool)
class CTestModelObject :
    public CModelObject
{
public:
	typedef struct tagTestModelObjDesc : public CModelObject::MODELOBJECT_DESC
	{
		CModel* pModel = nullptr;
	}TESTMODELOBJ_DESC;
protected:
	CTestModelObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTestModelObject(const CTestModelObject& _Prototype);
	virtual ~CTestModelObject() = default;

public:
	virtual HRESULT Initialize(void* _pArg); 

private:
	HRESULT					Ready_TestComponents(TESTMODELOBJ_DESC* _pDesc);

public:
	static CTestModelObject* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END