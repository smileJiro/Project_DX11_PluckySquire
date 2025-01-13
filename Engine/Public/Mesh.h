#pragma once
#include "VIBuffer.h"
#include "Model.h"

/* 1. fbx 파일에서 읽어들인 데이터를 기반으로 각 부위 별 메쉬에 대한 VB, IB를 저장하는 객체. */
/* 2. VIBuffer의 VB, IB, 및 Render 관련 함수들을 사용하기 위해 상속 받았다. */

BEGIN(Engine)
class ENGINE_DLL CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const { return m_iMaterialIndex; }
public:
	virtual HRESULT Initialize_Prototype(CModel::TYPE _eModelType, class CModel* _pModel, const aiMesh* _pAIMesh, _matrix _PreTransformMatrix);
	virtual HRESULT Initialize_Prototype(CModel::TYPE _eModelType, FBX_MESH& _tMeshDesc, _matrix _PreTransformMatrix);
	virtual HRESULT Initialize(void* _pArg);

public:
	HRESULT Bind_BoneMatrices(class CShader* _pShader, const _char* _pConstantName, const vector<CBone*>& _vecBones);

	HRESULT Copy_BoneMatrices(array<_float4x4, 256>* _pOutBoneMatrices);
public:
	// Get
	_string Get_Name() const { return _string(m_szName); }
	vector<_uint>& Get_BoneIndices() { return m_BoneIndices; }
	// Set
	void Set_Name(string _strName) { strcpy_s(m_szName, _strName.c_str()); }
private:
	/* Mesh Data */
	_char m_szName[MAX_PATH] = ""; // Mesh의 이름은 추후 중요하게 사용 된다.
	_uint m_iMaterialIndex = 0; // 내가 렌더링에 사용하고자하는 Material Index;
	_uint m_iNumBones = 0; // 나에게 영향을 주는 뼈의 개수.

	/* 나에게 영향을 주는 본들의 Model 클래스에 정의된 m_Bones 벡터에서의 인덱스를 저장하는 벡터.. */
	vector<_uint>				m_BoneIndices;
	vector<_float4x4>			m_BoneOffsetMatrices;
	_float4x4					m_BoneMatrices[256];
private:
	HRESULT Ready_VertexBuffer_For_NonAnim(const aiMesh* _pAIMesh, _fmatrix _PreTransformMatrix);
	HRESULT Ready_VertexBuffer_For_Anim(const aiMesh* _pAIMesh, class CModel* _pModel);

	HRESULT Ready_VertexBuffer_For_NonAnim(const FBX_MESH& _tMeshDesc, _fmatrix _PreTransformMatrix);
	HRESULT Ready_VertexBuffer_For_Anim(const FBX_MESH& _tMeshDesc);
public:
	/* Modeltype : Anim인지, NonAnim인지에 따라 각기 다른 처리를 하기위함. pModel* :*/
	static CMesh* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CModel::TYPE _eModelType, class CModel* _pModel, const aiMesh* _pAIMesh, _matrix _PreTransformMatrix);
	static CMesh* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CModel::TYPE _eModelType, FBX_MESH& _tMeshDesc, _matrix _PreTransformMatrix);
	virtual CComponent* Clone(void* _pArg) override; // Clone은 순수가상함수라 만들었다. 실제로 Mesh 는 얕은 복사하지 않고 깊은 복사할 것이다.
	virtual void Free() override;


public:
	/* 바이너리 관련 */
	HRESULT ConvertToBinary(HANDLE _hFile, DWORD* _dwByte, const aiMesh* _pAIMesh, CModel* _pModel);
};

END