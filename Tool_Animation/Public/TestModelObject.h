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
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

	HRESULT				Export_Model(ofstream& _outfile, const _char* _szDirPath, _bool _bExportTextures);
	//Set
	void						Set_2DProjMatrix(_fmatrix _vProjMatrix);
	void						Set_Progerss(_float _fTrackPos);
	void						Set_AnimSpeedMagnifier(COORDINATE _eCoord, _uint iAnimIndex, _float _fMagnifier);
	//Get
	void						Get_TextureNames(set<wstring>& _outTextureNames);
	_uint						Get_AnimationCount();	
	_float					Get_Progress();
	_float					Get_AnimSpeedMagnifier(COORDINATE _eCoord, _uint iAnimIndex);
	_bool					Is_LoopAnimation(COORDINATE _eCoord, _uint iAnimIndex);
	void						Get_AnimatinNames(list<string>& _Names);
private:
	HRESULT					Ready_TestComponents(TESTMODELOBJ_DESC* _pDesc);

private:
	_float4x4 m_ViewMatrix{}, m_ProjMatrix{};
	COORDINATE m_eCurCoord = COORDINATE::COORDINATE_LAST;
public:
	static CTestModelObject* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END