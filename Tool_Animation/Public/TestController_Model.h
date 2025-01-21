#pragma once
#include "Controller_Model.h"

BEGIN(AnimTool)
class CTestController_Model :
    public CController_Model
{
public:
	typedef struct tagTestConModelDesc : public CController_Model::CON_MODEL_DESC
	{
		CModel* pModel = nullptr;
	}TESTCONTMODEL_DESC;
private:
	CTestController_Model(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CTestController_Model() = default;

public:
	HRESULT			Initialize(TESTCONTMODEL_DESC* _pDesc);

public:
	static CTestController_Model* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, TESTCONTMODEL_DESC* _pDesc);
	virtual void Free() override;
};

END