#pragma once
#include "Component.h"

BEGIN(Engine)
class CMesh;
class CMaterial;
class CBone;
class CAnimation;
class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { NONANIM, ANIM, LAST, };
private:
	CModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CModel(const CModel& _Prototype);
	virtual ~CModel() = default;

public:
	virtual HRESULT			Initialize_Prototype(const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual HRESULT			Render(_uint _iMeshIndex);
	
public:
	HRESULT					Bind_Matrices(class CShader* _pShader, const _char* _pConstantName, _uint _iMeshIndex);
	HRESULT					Bind_Material(class CShader* _pShader, const _char* _pConstantName, _uint _iMeshIndex, aiTextureType _eTextureType, _uint _iTextureIndex = 0);
	_uint					Find_BoneIndex(const _char* _pBoneName) const;
	const _float4x4*		Find_BoneMatrix(const _char* _pBoneName) const;
	_bool					Swap_AnimIndex(_uint iSrc, _uint _iDst)
	{
		if (iSrc >= m_iNumAnimations || _iDst >= m_iNumAnimations)
			return false;

		std::swap(m_Animations[iSrc], m_Animations[_iDst]);
		return true;
	}

public:
	HRESULT Copy_BoneMatrices(_int _iNumMeshIndex, array<_float4x4, 256>* _pOutBoneMatrices);
public:
	// Get
	_uint					Get_NumMeshes() const { return m_iNumMeshes; } // Client 쪽에서 Render를 위한 데이터들을 조작하기위함.
	const vector<CMesh*>&	Get_Meshes() const { return m_Meshes; }
	_string					Get_MeshName(_uint _iMeshIndex);
	TYPE					Get_ModelType()const { return m_eModelType; }
	_matrix					Get_PreTransformationMatrix() const { return XMLoadFloat4x4(&m_PreTransformMatrix); }
	const _float4x4&		Get_PreTransformationMatrix_Float4x4() const { return m_PreTransformMatrix; }
	_uint					Get_NumAnimations() const { return m_iNumAnimations; }
	_uint					Get_NumBones() const { return m_iNumBones; }
	vector<CAnimation*>*	Get_AnimationsAdress() { return &m_Animations; }
	CAnimation*				Get_CurrentAnimation() { return m_Animations[m_iCurAnimIndex]; }
	CAnimation*				Get_Animation(_uint _iAnimIndex) { return m_Animations[_iAnimIndex]; }
	vector<CBone*>*			Get_BonesAdress() { return &m_Bones; }
	_float					Get_RootBonePositionY();
	_uint Get_BoneIndex(const _char* _pBoneName) const;
	// Set

private: /* for. File Load */
	Assimp::Importer	m_Importer;
	const aiScene*		m_pAIScene = nullptr;
	TYPE				m_eModelType = TYPE::LAST;

private: /* for. Bone Data */
	_uint				m_iNumBones = 0;
	vector<CBone*>		m_Bones;

private:/* for. Mesh Data */
	_uint				m_iNumMeshes = 0;
	vector<CMesh*>		m_Meshes;

private:/* for. Material Texture Data */
	_uint				m_iNumMaterials = 0;
	vector<CMaterial*>	m_Materials;

private: /* for. PreTransformMatrix */
	_float4x4			m_PreTransformMatrix = {};

private: /* for. Animation */
	_uint				m_iNumAnimations = 0;
	vector<CAnimation*> m_Animations;

private:/* 툴용 */
	_int m_iCurAnimIndex = 0;
	_bool m_isFinished = false;
	_bool m_isLoop = true;
public:
	void SetUp_ToolAnimation(_int _iAnimIndex, _bool _isLoop) { m_iCurAnimIndex = _iAnimIndex; m_isLoop = _isLoop; }

protected:
	HRESULT Ready_Bones(ifstream& inFile, _uint iParentBoneIndex);
	HRESULT Ready_Meshes(ifstream& inFile);
	HRESULT Ready_Materials(ifstream& inFile, const _char* pModelFilePath);
	HRESULT Ready_Animations(ifstream& inFile);
public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
public:

};

END
