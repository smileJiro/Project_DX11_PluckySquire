#pragma once
#include "Component.h"

BEGIN(Engine)
class CMesh;
class CBone;
class CMaterial;

class ENGINE_DLL CEffectMesh : public CComponent
{
private:
	CEffectMesh(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CEffectMesh(const CEffectMesh& _Prototype);
	virtual ~CEffectMesh() = default;

public:
	HRESULT			Initialize_Prototype(const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual HRESULT			Initialize(void* _pArg) override;
private:
	_uint						m_iNumMeshes = 0;
	vector<CMesh*>				m_Meshes;

	_uint						m_iNumMaterials = 0;
	vector<CMaterial*>			m_Materials;
	vector<CBone*>				m_Bones;
	_float4x4					m_PreTransformMatrix = {};

private:
	HRESULT Ready_Bones(ifstream& inFile, _uint iParentBoneIndex);
	HRESULT Ready_Meshes(ifstream& inFile);
	HRESULT Ready_Materials(ifstream& inFile, const _char* pModelFilePath);

public:
	static CEffectMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;

};

END