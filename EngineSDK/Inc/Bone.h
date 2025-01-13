#pragma once
#include "Base.h"

BEGIN(Engine)
class CTransform;

class ENGINE_DLL CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;
public:
	const _char* Get_Name() const { return m_szName; }

public:
	HRESULT Initialize(const aiNode* _pAINode, _int _iParentBoneIndex);
	HRESULT Initialize(const FBX_BONE& _pBoneDesc);
	/* 부모의 Combined행렬을 가져와 자기 자신의 Combined행렬을 Update */
	//void Update_CombinedTransformationMatrix(const vector<CBone*>& _vecBones, _fmatrix _PreTransformMatrix, CTransform* _pOwnerTransformCom, _bool* _isBlend);
	void Update_CombinedTransformationMatrix(const vector<CBone*>& _vecBones, _fmatrix _PreTransformMatrix, _float3* _pPreRootBonePos ,_float3* _pOutRootMovement);
	void Update_CombinedTransformationMatrix(const vector<CBone*>& _vecBones, _fmatrix _PreTransformMatrix);

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
	void Reset_TransformationPosition()
	{
		static _float4 fZeroPos = { 0.0f, 1.0f, 0.0f, 1.0f };
		memcpy(m_TransformationMatrix.m[3], &fZeroPos, sizeof(_float4));
	}
	void Reset_CombinedPosition()
	{
		static _float4 fZeroPos = { 0.0f, 1.0f, 0.0f, 1.0f };
		memcpy(m_CombinedTransformationMatrix.m[3], &fZeroPos, sizeof(_float4));
	}

	void Set_PreRootPos(_float3 _vPreRootPos) { m_vPreRootPos = _vPreRootPos; }
	void Set_PreRootPos(_vector _vPreRootPos) { XMStoreFloat3(&m_vPreRootPos, _vPreRootPos); }
private:
	/* 추후 이름을 기반으로 검색하는 경우가 빈번함. */
	_char m_szName[MAX_PATH] = {};

	/* 부모를 기준으로하는 나만의 상태 행렬 (Offset) */
	_float4x4 m_TransformationMatrix = {};
	/* m_TransformationMatrix * Parent's m_CombindTrasnformationMatrix  = My CombindTransformationMatrix (최종 World Space 상의 나의 좌표.) */
	_float4x4 m_CombinedTransformationMatrix = {};
	/* Parent에 대한 정보를 Index로 들고있는다. (부모가 없는 경우 -1로 정의.) */
	_int m_iParentBoneIndex = -1; 

	_float3 m_vPreRootPos = {0.0f, 0.0f, 0.0f};
public:
	static CBone* Create(const aiNode* _pAINode, _int _iParentBoneIndex);
	static CBone* Create(const FBX_BONE& _pBoneDesc);
	virtual CBone* Clone();
	virtual void Free() override;

	/* 바이너리 관련 */
public:
	HRESULT ConvertToBinary(HANDLE _hFile, DWORD* _dwByte);
};

END



/* Bone에 대한 간단 정리 링크 */
// https://www.notion.so/40-Animation-Model_2-1-11fb1e26c8a8802baf83d26c016322c0