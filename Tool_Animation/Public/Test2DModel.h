#pragma once
#include "2DModel.h"

BEGIN(AnimTool)
class CTest2DModel :
    public C2DModel
{
protected:
	CTest2DModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTest2DModel(const CTest2DModel& _Prototype);
	virtual ~CTest2DModel() = default;

public:
    HRESULT			Initialize_Prototype_FromModelFile(const _char* _pModelFilePath);

    HRESULT				Export_Model(const wstring& _wstrPath);
public:
    static CTest2DModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _bool _bRawData, const _char* pPath);
    virtual CComponent* Clone(void* _pArg) override;
    virtual void Free() override;
};

END