#pragma once
#include "VIBuffer.h"
#include "3DModel.h"

/* 1. fbx 파일에서 읽어들인 데이터를 기반으로 각 부위 별 메쉬에 대한 VB, IB를 저장하는 객체. */
/* 2. VIBuffer의 VB, IB, 및 Render 관련 함수들을 사용하기 위해 상속 받았다. */

BEGIN(Engine)
class ENGINE_DLL CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CMesh() = default;

public:
	virtual HRESULT Initialize_Prototype(C3DModel::TYPE _eModelType, C3DModel* _pModel, ifstream& inFile, _fmatrix _PreTransformMatrix);
	virtual HRESULT Initialize(void* _pArg);

public:
	HRESULT Bind_BoneMatrices(class CShader* _pShader, const _char* _pConstantName, const vector<CBone*>& _Bones);
	void ReSet_OffsetMarix();
public:
	HRESULT Ready_VertexBuffer_For_NonAnim(ifstream& _inFile, _fmatrix _PreTransformMatrix);
	HRESULT Ready_VertexBuffer_For_Anim(ifstream& _inFile,  C3DModel* _pModel);
public:
	// Get
	_char* Get_Name()  { return m_szName; }
	_uint Get_MaterialIndex() const { return m_iMaterialIndex; }
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
public:
public:	
	static CMesh* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, C3DModel::TYPE _eModelType, C3DModel* pModel, ifstream& inFile, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END