#pragma once
#include "Base.h"

BEGIN(Engine)
class CTransform;

class ENGINE_DLL CBone : public CBase
{
protected:
	CBone();
	virtual ~CBone() = default;
public:
	const _char* Get_Name() const { return m_szName; }

public:
	virtual HRESULT Initialize(ifstream& _inFile, _int iParentBoneIndex);
	virtual void Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix);

public:
	// Get
	_matrix Get_TransformationMatrix() const { return XMLoadFloat4x4(&m_TransformationMatrix);}
	_vector Get_TransformationPosition() const { return XMLoadFloat4x4(&m_TransformationMatrix).r[3]; }
	_vector Get_CombinedPosition() { return XMLoadFloat4x4(&m_CombinedTransformationMatrix).r[3]; }
	_matrix Get_CombinedTransformationMatrix() const { return XMLoadFloat4x4(&m_CombinedTransformationMatrix); }
	const _float4x4* Get_CombinedTransformationFloat4x4() const { return &m_CombinedTransformationMatrix; }
	// Set
	void Set_TransformationMatrix(_matrix _TransformationMatrix)
	{
		XMStoreFloat4x4(&m_TransformationMatrix, _TransformationMatrix); 
	}

protected:
	/* 추후 이름을 기반으로 검색하는 경우가 빈번함. */
	_char m_szName[MAX_PATH] = {};

	/* 부모를 기준으로하는 나만의 상태 행렬 (Offset) */
	_float4x4 m_TransformationMatrix = {};
	/* m_TransformationMatrix * Parent's m_CombindTrasnformationMatrix  = My CombindTransformationMatrix (최종 World Space 상의 나의 좌표.) */
	_float4x4 m_CombinedTransformationMatrix = {};
	/* Parent에 대한 정보를 Index로 들고있는다. (부모가 없는 경우 -1로 정의.) */
	_int m_iParentBoneIndex = -1; 

public:
	static CBone* Create(ifstream& _inFile, _int iParentBoneIndex);
	virtual CBone* Clone();
	virtual void Free() override;

};

END



/* Bone에 대한 간단 정리 링크 */
// https://www.notion.so/40-Animation-Model_2-1-11fb1e26c8a8802baf83d26c016322c0