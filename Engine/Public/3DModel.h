#pragma once
#include "Model.h"

BEGIN(Engine)
class CMesh;
class CMaterial;
class CBone;
class CAnimation3D;
class CShader;
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

public:
	typedef struct tag3DModelDesc
	{
		// 모델의 머티리얼 클래스가 얕은복사라 같은모델이지만 별도의 머티리얼 컬러를 가지는 경우에 대한 처리가 어려움이있어 생성된 desc
		_bool isDeepCopyConstBuffer = false;
	}MODEL3D_DESC;
protected:
	C3DModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	C3DModel(const C3DModel& _Prototype);
	virtual ~C3DModel() = default;

public:
	virtual HRESULT			Initialize_Prototype(const _char* pModelFilePath, _fmatrix PreTransformMatrix, _bool _isCollider);
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual HRESULT			Render(CShader* _pShader, _uint _iShaderPass) override;
	virtual HRESULT			Render_Shadow(CShader* _pShader, _uint _iShaderPass) override;
	virtual HRESULT			Render_Default(CShader* _pShader, _uint _iShaderPass) override;
public:
	HRESULT					Bind_Material(CShader* _pShader, const _char* _pConstantName, _uint _iMeshIndex, aiTextureType _eTextureType, _uint _iTextureIndex = 0);
	HRESULT					Bind_Matrices(CShader* _pShader, const _char* _pConstantName, _uint _iMeshIndex);
	virtual _bool			Play_Animation(_float fTimeDelta, _bool bReverse = false) override;
	HRESULT					Bind_Material_PixelConstBuffer(_uint _iMaterialIndex,  CShader* _pShader);
public:
	// Get
	_uint					Get_NumMeshes() const { return m_iNumMeshes; }
	CMesh*					Get_Mesh(_uint iMeshIndex) const { return m_Meshes[iMeshIndex]; }
	_uint					Get_MeshIndex(const _char* _szName) const;
	_uint					Get_BoneIndex(const _char* pBoneName) const;
	float					Get_AnimTime();
	_uint					Get_AnimIndex();
	virtual CAnimation* Get_Animation(_uint iAnimIndex) override;
	_float					Get_AnimationProgress(_uint iAnimIdx);
	virtual _float			Get_AnimationTime(_uint iAnimIndex) override;
	virtual _float			Get_AnimationTime() override;

	const _float4x4*		Get_BoneMatrix(const _char* pBoneName) const;
	class CBone*			Get_Bone(const _char* pBoneName) const;
	const vector<CMesh*>&	Get_Meshes() { return m_Meshes; }

	//Get
	virtual _uint			Get_AnimCount() override;
	virtual _uint			Get_CurrentAnimIndex() override { return m_iCurrentAnimIndex; }
	virtual _float			Get_CurrentAnimProgeress() override;
	// Set
	virtual void			Set_AnimationLoop(_uint iIdx, _bool bIsLoop)override;
	virtual void			Set_Animation(_uint iIdx, _bool _bReverse = false)override;
	void						Set_AnimationTransitionTime(_uint iIdx, _float _fTime);
	virtual void			Switch_Animation(_uint iIdx, _bool _bReverse = false)override;
	virtual void			To_NextAnimation() override;
	virtual void			Set_AnimSpeedMagnifier(_uint iAnimIndex, _float _fMag) override;

	// Material Get, Set 

	// Get
	const CONST_PS			Get_MaterialConstBuffer_PixelConstBuffer(_uint _iMaterialIndex) const;
	const _float4&			Get_MaterialConstBuffer_Albedo(_uint _iMaterialIndex) const;
	float					Get_MaterialConstBuffer_Roughness(_uint _iMaterialIndex) const;
	float					Get_MaterialConstBuffer_Metallic(_uint _iMaterialIndex) const;
	float					Get_MaterialConstBuffer_AO(_uint _iMaterialIndex) const;
	const _float4&			Get_MaterialConstBuffer_MultipleAlbedo(_uint _iMaterialIndex) const;

	void					Set_MaterialConstBuffer_PixelConstBuffer(_uint _iMaterialIndex, const CONST_PS& _tPixelConstData, _bool _isUpdate = false);
	void					Set_MaterialConstBuffer_UseAlbedoMap(_uint _iMaterialIndex, _bool _useAlbedoMap, _bool _isUpdate = false);
	void					Set_MaterialConstBuffer_Albedo(_uint _iMaterialIndex, const _float4& _vAlbedoColor, _bool _isUpdate = false);
	void					Set_MaterialConstBuffer_MultipleAlbedo(_uint _iMaterialIndex, const _float4& _vMultipleAlbedoColor, _bool _isUpdate = false);
	void					Set_MaterialConstBuffer_Roughness(_uint _iMaterialIndex, _float _fRoughness, _bool _isUpdate = false);
	void					Set_MaterialConstBuffer_Metallic(_uint _iMaterialIndex, _float _fMetallic, _bool _isUpdate = false);
	void					Set_MaterialConstBuffer_AO(_uint _iMaterialIndex, _float _fAO, _bool _isUpdate = false);

#ifdef _DEBUG
public:
	vector<CMesh*>&			Get_Meshs() { return m_Meshes; }
	vector<CMaterial*>&		Get_Materials() { return m_Materials; }

	//vector<CAnimation*>&	Get_Animations() { return m_Animations; }
#endif // _DEBUG
public :
	_float4x4					Get_PreTransformMatrix() { return m_PreTransformMatrix; }
	vector<CBone*>&				Get_Bones() { return m_Bones; }
	void						Binding_TextureIndex(_uint _iIndex, _uint _eTextureType, _uint _iMaterialIndex) { m_arrTextureBindingIndex[_iMaterialIndex][_eTextureType] = _iIndex; }
	_uint						Get_TextureIndex(_uint _eTextureType, _uint _iMaterialIndex) { return m_arrTextureBindingIndex[_iMaterialIndex][_eTextureType]; }


	_char*						Get_CookingColliderData() { return m_arrCookingColliderData; }
	_uint						Get_CookingColliderDataLength() { return m_iCookingColliderDataLength; }
	_bool						Has_CookingCollider() { return m_isCookingCollider; }
	_uint						Get_CookingColliderType() { return m_iCoockingMeshType; }
	_string						Get_CookingColliderPath() { return m_strColliderPath; }

protected:
	_uint						m_arrTextureBindingIndex[aiTextureType_UNKNOWN][aiTextureType_UNKNOWN];

	_uint						m_iNumMeshes = 0;
	vector<CMesh*>				m_Meshes;

	_uint						m_iNumMaterials = 0;
	vector<CMaterial*>			m_Materials;
	vector<CBone*>				m_Bones;
	_float4x4					m_PreTransformMatrix = {};

	_uint						m_iCurrentAnimIndex = {};
	_uint						m_iPrevAnimIndex = {};
	_uint						m_iNumAnimations = 0;
	vector<CAnimation3D*>		m_Animations;
	

	map<_uint, KEYFRAME>		m_mapAnimTransLeftFrame;

	_bool						m_isCookingCollider = false;
	_uint						m_iCoockingMeshType = 0;
	_uint						m_iCookingColliderDataLength = 0;
	_string						m_strColliderPath = "";	
	_char*						m_arrCookingColliderData = nullptr;


protected:
	virtual HRESULT Ready_Bones(ifstream& inFile, _uint iParentBoneIndex);
	virtual HRESULT Ready_Meshes(ifstream& inFile);
	virtual HRESULT Ready_Materials(ifstream& inFile, const _char* pModelFilePath);
	virtual HRESULT Ready_Animations(ifstream& inFile);

public:
	static C3DModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, _fmatrix PreTransformMatrix, _bool _isCookingCollider = false);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;



};

END
