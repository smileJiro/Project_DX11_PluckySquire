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
	HRESULT				Copy_Textures(const filesystem::path& _szDestPath);
	//Set
	void						Set_Progress(_float _fTrackPos);
	//Get
	void						Get_TextureNames(set<wstring>& _outTextureNames);
	_float					Get_Progress();
	_float	                Get_AnimSpeedMagnifier(_uint iAnimIndex);
	void						Get_AnimationNames(list<string>& _Names);
	_bool	                Is_LoopAnimation(_uint iAnimIndex);
protected:
	virtual HRESULT Ready_Bones(ifstream& inFile, _uint iParentBoneIndex);
	virtual HRESULT Ready_Meshes(ifstream& inFile);
	virtual HRESULT Ready_Materials(ifstream& inFile, const _char* pModelFilePath) override;
	virtual HRESULT Ready_Animations(ifstream& inFile) override;
private:
	HRESULT Export_Bone(ofstream& _outFile);
	HRESULT Export_Meshes(ofstream& _outFile, _bool _bAnim);
	HRESULT Export_Materials(ofstream& _outFile);
	HRESULT Export_Animations(ofstream& _outFile);

public:
	static CTest3DModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END