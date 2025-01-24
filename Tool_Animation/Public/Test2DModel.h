#pragma once
#include "2DModel.h"

BEGIN(AnimTool)
// C2DModel에서 JsonRawData를 읽어서 생성하는 기능과 
// //model2D 파일로 Export하는 기능 추가됨.
class CTest2DModel :
    public C2DModel
{
protected:
	CTest2DModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTest2DModel(const CTest2DModel& _Prototype);
	virtual ~CTest2DModel() = default;

public:
    HRESULT	 Initialize_Prototype_FromJsonFile(const _char* _szRawDataDirPath);
    virtual HRESULT	Initialize_Prototype(const _char* _szModel2DFilePath);

    HRESULT				Export_Model(ofstream& _outfile);

    //Set
    void						Set_Progerss(_float _fTrackPos);
    //Get
    void						Get_TextureNames(set<wstring>& _outTextureNames);
    _float					Get_Progerss();
private:

public:
    static CTest2DModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _bool _bRawData, const _char* pPath);
    virtual CComponent* Clone(void* _pArg) override;
    virtual void Free() override;
};

END