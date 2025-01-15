#pragma once
#include "Component.h"

BEGIN(Engine)
class CMesh;
class CMaterial;
class CBone;
class CAnimation;
class ENGINE_DLL C3DModel final : public CComponent
{
public:
	enum TYPE { NONANIM, ANIM, LAST, };
private:
	C3DModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	C3DModel(const C3DModel& _Prototype);
	virtual ~C3DModel() = default;

public:
	virtual HRESULT			Initialize_Prototype(const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual HRESULT			Render(_uint _iMeshIndex);

public:
	HRESULT					Bind_Material(class CShader* _pShader, const _char* _pConstantName, _uint _iMeshIndex, aiTextureType _eTextureType, _uint _iTextureIndex = 0);
	HRESULT					Bind_Matrices(class CShader* _pShader, const _char* _pConstantName, _uint _iMeshIndex);
	_bool Play_Animation(_float fTimeDelta);


public:
	// Get
	_uint Get_NumMeshes() const { return m_iNumMeshes; }
	CMesh* Get_Mesh(_uint iMeshIndex) const { return m_Meshes[iMeshIndex]; }
	_uint Get_MeshIndex(const _char* _szName) const;
	_uint Get_BoneIndex(const _char* pBoneName) const;
	float Get_AnimTime();
	_uint Get_AnimIndex();
	_float Get_AnimationProgress(_uint iAnimIdx);
	TYPE Get_Type() { return m_eModelType; }
	const _float4x4* Get_BoneMatrix(const _char* pBoneName) const;
	class CBone* Get_Bone(const _char* pBoneName) const;
	const vector<CMesh*>& Get_Meshes() { return m_Meshes; }
	_bool Is_AnimModel() { return m_eModelType == TYPE::ANIM; };
	// Set
	void Set_AnimationLoop(_uint iIdx, _bool bIsLoop);
	void Set_Animation(_uint iIdx);
	void Switch_Animation(_uint iIdx);
protected:
	TYPE				m_eModelType = TYPE::LAST;

	_uint				m_iNumMeshes = 0;
	vector<CMesh*>		m_Meshes;

	_uint				m_iNumMaterials = 0;
	vector<CMaterial*>	m_Materials;

	vector<CBone*>		m_Bones;
	_float4x4			m_PreTransformMatrix = {};

	_uint						m_iCurrentAnimIndex = {};
	_uint						m_iPrevAnimIndex = {};
	_uint				m_iNumAnimations = 0;
	vector<CAnimation*> m_Animations;

	map<_uint, KEYFRAME> m_mapAnimTransLeftFrame;

protected:
	HRESULT Ready_Bones(ifstream& inFile, _uint iParentBoneIndex);
	HRESULT Ready_Meshes(ifstream& inFile);
	HRESULT Ready_Materials(ifstream& inFile, const _char* pModelFilePath);
	HRESULT Ready_Animations(ifstream& inFile);
public:
	static C3DModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
public:

};

END
