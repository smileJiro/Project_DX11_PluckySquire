#pragma once
#include "Mesh.h"
BEGIN(AnimTool)

typedef	struct tagPackagingAnimBone
{
	_uint						iNameSize;
	_char						szName[MAX_PATH];
	_float4x4					matOffset;
	_uint						iNumVertices;

	//iVertexIdx, fWeight
	vector<pair<_uint, _float>> WeightInfos;
} ANIMBONE;

class CToolMesh :
    public CMesh
{
protected:
	CToolMesh(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CToolMesh() = default;

public:
	virtual HRESULT Ready_VertexBuffer_For_NonAnim(ifstream& _inFile, _fmatrix _PreTransformMatrix) override;
	virtual HRESULT Ready_VertexBuffer_For_Anim(ifstream& _inFile, C3DModel* _pModel) override;

	void						Export(ofstream& _outfile, _bool _bAnim);

	//Get
	_uint& Get_MaterialIndex() { return m_iMaterialIndex; }
private:
	vector<VTXMESH>		m_Vertices;
	vector<ANIMBONE>	m_BoneInfos;
public:
	static CToolMesh* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, C3DModel::ANIM_TYPE _eModelType, C3DModel* pModel, ifstream& inFile, _fmatrix PreTransformMatrix);
	virtual void Free() override;
};

END