#pragma once
#include "3DModel.h"

BEGIN(AnimTool)
class CTest3DModel :
    public C3DModel
{
protected:
	CTest3DModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTest3DModel(const CTest3DModel& _Prototype);
	virtual ~CTest3DModel() = default;

public:

	HRESULT				Export_Model(ofstream& _outfile);

	//Get
	void						Get_TextureNames(set<wstring>& _outTextureNames);
public:
	static CTest3DModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END