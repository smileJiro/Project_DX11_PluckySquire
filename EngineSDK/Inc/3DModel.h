#pragma once
#include "Model.h"

BEGIN(Engine)
class CMesh;
class CMaterial;
class CBone;
class CAnimation;
class ENGINE_DLL C3DModel : public CModel
{
public:
	// 20250122 박예슬 : 셰이더에서 디퓨즈 색상값을 결정하는 enum
	enum COLOR_SHADER_MODE {
		// 디퓨즈 색상 그대로 렌더
		COLOR_NONE,
		// 주어진 색상 그대로 렌더
		COLOR_DEFAULT,
		// 디퓨즈와 주어진 색상 섞어서 렌더
		MIX_DIFFUSE,
		COLOR_SHADER_MODE_LAST
	};

protected:
	C3DModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	C3DModel(const C3DModel& _Prototype);
	virtual ~C3DModel() = default;

public:
	virtual HRESULT			Initialize_Prototype(const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual HRESULT			Render(CShader* _Shader, _uint _iShaderPass) override;

public:
	HRESULT					Bind_Material(class CShader* _pShader, const _char* _pConstantName, _uint _iMeshIndex, aiTextureType _eTextureType, _uint _iTextureIndex = 0);
	HRESULT					Bind_Matrices(class CShader* _pShader, const _char* _pConstantName, _uint _iMeshIndex);
	virtual _bool			Play_Animation(_float fTimeDelta) override;


public:
	// Get
	_uint					Get_NumMeshes() const { return m_iNumMeshes; }
	CMesh*					Get_Mesh(_uint iMeshIndex) const { return m_Meshes[iMeshIndex]; }
	_uint					Get_MeshIndex(const _char* _szName) const;
	_uint					Get_BoneIndex(const _char* pBoneName) const;
	float					Get_AnimTime();
	_uint					Get_AnimIndex();
	_float					Get_AnimationProgress(_uint iAnimIdx);

	const _float4x4*		Get_BoneMatrix(const _char* pBoneName) const;
	class CBone*			Get_Bone(const _char* pBoneName) const;
	const vector<CMesh*>&	Get_Meshes() { return m_Meshes; }

	//Get
	virtual _uint			Get_AnimCount() override;
	virtual _uint			Get_CurrentAnimIndex() override { return m_iCurrentAnimIndex; }
	// Set
	virtual void			Set_AnimationLoop(_uint iIdx, _bool bIsLoop)override;
	virtual void			Set_Animation(_uint iIdx)override;
	virtual void			Switch_Animation(_uint iIdx)override;
	virtual void			To_NextAnimation() override;


#ifdef _DEBUG
public:
	//vector<CMesh*>&			Get_Meshs() { return m_Meshes; }
	vector<CMaterial*>&		Get_Materials() { return m_Materials; }
	vector<CBone*>&			Get_Bones() { return m_Bones; }
	//vector<CAnimation*>&	Get_Animations() { return m_Animations; }
#endif // _DEBUG
public :

	void						Binding_TextureIndex(_uint _iIndex, _uint _eTextureType, _uint _iMaterialIndex) { m_arrTextureBindingIndex[_iMaterialIndex][_eTextureType] = _iIndex; }
	_uint						Get_TextureIndex(_uint _eTextureType, _uint _iMaterialIndex) { return m_arrTextureBindingIndex[_iMaterialIndex][_eTextureType]; }

protected:
	_uint						m_arrTextureBindingIndex[AI_TEXTURE_TYPE_MAX][AI_TEXTURE_TYPE_MAX];

	_uint						m_iNumMeshes = 0;
	vector<CMesh*>				m_Meshes;

	_uint						m_iNumMaterials = 0;
	vector<CMaterial*>			m_Materials;
	vector<CBone*>				m_Bones;
	_float4x4					m_PreTransformMatrix = {};

	_uint						m_iCurrentAnimIndex = {};
	_uint						m_iPrevAnimIndex = {};
	_uint						m_iNumAnimations = 0;
	vector<CAnimation*>			m_Animations;

	map<_uint, KEYFRAME>		m_mapAnimTransLeftFrame;

protected:
	HRESULT Ready_Bones(ifstream& inFile, _uint iParentBoneIndex);
	HRESULT Ready_Meshes(ifstream& inFile);
	HRESULT Ready_Materials(ifstream& inFile, const _char* pModelFilePath);
	HRESULT Ready_Animations(ifstream& inFile);



	

public:
	static C3DModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;

};

END
