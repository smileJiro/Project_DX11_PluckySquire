#include "Bone.h"
#include "Transform.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(const aiNode* _pAINode, _int _iParentBoneIndex)
{
	// 본에서 본의 이름을 지정. >>> 메쉬클래스에서는 이 이름을 기준으로 탐색해서 본인에게 영향받는 뼈를 찾을 것임.
	strcpy_s(m_szName, _pAINode->mName.data);

	// 초기 Transformation 행렬자체는 Assimp가 제공한다. Combined는 제공하지 않음. 우리가 만드는 것임.
	memcpy(&m_TransformationMatrix, &_pAINode->mTransformation, sizeof(_float4x4));

	// Assimp는 행렬을 Column major 방식으로 저장하고있음. 전치 시켜줘야한다.
	XMStoreFloat4x4(&m_TransformationMatrix,XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	m_iParentBoneIndex = _iParentBoneIndex;

	return S_OK;
}

HRESULT CBone::Initialize(const FBX_BONE& _pBoneDesc)
{
	strcpy_s(m_szName, _pBoneDesc.szName);
	m_TransformationMatrix = _pBoneDesc.TransformationMatrix;
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());
	m_iParentBoneIndex = _pBoneDesc.m_iParentBoneIndex;

	return S_OK;
}
//
//void CBone::Update_CombinedTransformationMatrix(const vector<CBone*>& _vecBones, _fmatrix _PreTransformMatrix, _bool* _isBlend)
//{
//	//if (false == *_isBlend)
//	//{
//	//	/* 1. 매 프레임 호출하며, 부모의 Combined 행렬을 가져와, 자기자신의 Combined 행렬을 업데이트. */
//	//	if (-1 == m_iParentBoneIndex)
//	//	{
//	//		// 부모가 없다면, 최상위 부모노드라는 의미. PreTransformMatrix를 최상위 부모노드에게만 곱해준다.
//	//		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * _PreTransformMatrix);
//	//	}
//	//	else if (!strcmp(m_szName, "Bip001"))
//	//	{
//	//		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&_vecBones[m_iParentBoneIndex]->m_CombinedTransformationMatrix));
//	//		_vector vRootPos = XMVectorSet(m_CombinedTransformationMatrix._41, m_CombinedTransformationMatrix._42, m_CombinedTransformationMatrix._43, 1.0f);
//	//		_vector vLocalMovement = vRootPos - XMLoadFloat3(&m_vPreRootPos);
//	//		
//	//		_pOwnerTransformCom->Set_RootBoneMovement(vLocalMovement);
//	//		XMStoreFloat3(&m_vPreRootPos, vRootPos);
//	//		m_CombinedTransformationMatrix._41 = 0.0f;
//	//		m_CombinedTransformationMatrix._42 = 0.0f;
//	//		m_CombinedTransformationMatrix._43 = 0.0f;
//	//		m_CombinedTransformationMatrix._44 = 1.0f;
//	//	}
//	//	else
//	//	{
//	//		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&_vecBones[m_iParentBoneIndex]->m_CombinedTransformationMatrix));
//	//	}
//
//	//}
//	//else
//	//{
//	//	/* 1. 매 프레임 호출하며, 부모의 Combined 행렬을 가져와, 자기자신의 Combined 행렬을 업데이트. */
//	//	if (-1 == m_iParentBoneIndex)
//	//	{
//	//		// 부모가 없다면, 최상위 부모노드라는 의미. PreTransformMatrix를 최상위 부모노드에게만 곱해준다.
//	//		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * _PreTransformMatrix);
//	//	}
//	//	else if (!strcmp(m_szName, "Bip001"))
//	//	{
//	//		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&_vecBones[m_iParentBoneIndex]->m_CombinedTransformationMatrix));
//	//		_vector vRootPos = XMVectorSet(m_CombinedTransformationMatrix._41, m_CombinedTransformationMatrix._42, m_CombinedTransformationMatrix._43, 1.0f);
//	//		XMStoreFloat3(&m_vPreRootPos, vRootPos);
//	//		//
//	//	}
//	//	else
//	//	{
//	//		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&_vecBones[m_iParentBoneIndex]->m_CombinedTransformationMatrix));
//	//	}
//
//
//	//}
//}

void CBone::Update_CombinedTransformationMatrix(const vector<CBone*>& _vecBones, _fmatrix _PreTransformMatrix, _float3* _pPreRootBonePos, _float3* _pOutRootMovement)
{
	/* 1. 매 프레임 호출하며, 부모의 Combined 행렬을 가져와, 자기자신의 Combined 행렬을 업데이트. */
	if (-1 == m_iParentBoneIndex)
	{
		// 부모가 없다면, 최상위 부모노드라는 의미. PreTransformMatrix를 최상위 부모노드에게만 곱해준다.
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * _PreTransformMatrix);
	}
	else if (!strcmp(m_szName, "Bip001"))
	{
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&_vecBones[m_iParentBoneIndex]->m_CombinedTransformationMatrix));
		_vector vRootPos = XMVectorSet(m_CombinedTransformationMatrix._41, m_CombinedTransformationMatrix._42, m_CombinedTransformationMatrix._43, 1.0f);
		// 이동량 계산
		_vector vLocalMovement = vRootPos - XMLoadFloat3(_pPreRootBonePos);
		// 이동량 저장
		XMStoreFloat3(_pOutRootMovement, vLocalMovement);
		// 이전 루트본 위치 저장.
		XMStoreFloat3(_pPreRootBonePos, vRootPos);
		m_CombinedTransformationMatrix._41 = 0.0f;
		//m_CombinedTransformationMatrix._42 = 0.0f;
		m_CombinedTransformationMatrix._43 = 0.0f;

	}
	else
	{
		// 부모가 있다면, 부모의 Combined 행렬과 자기자신의 Transformation 행렬을 곱하여 자기자신의 Combined 행렬을 결정.
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&_vecBones[m_iParentBoneIndex]->m_CombinedTransformationMatrix));
	}
}

void CBone::Update_CombinedTransformationMatrix(const vector<CBone*>& _vecBones, _fmatrix _PreTransformMatrix)
{	/* 1. 매 프레임 호출하며, 부모의 Combined 행렬을 가져와, 자기자신의 Combined 행렬을 업데이트. */
	if (-1 == m_iParentBoneIndex)
	{
		// 부모가 없다면, 최상위 부모노드라는 의미. PreTransformMatrix를 최상위 부모노드에게만 곱해준다.
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * _PreTransformMatrix);
	}
	else
	{
		// 부모가 있다면, 부모의 Combined 행렬과 자기자신의 Transformation 행렬을 곱하여 자기자신의 Combined 행렬을 결정.
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&_vecBones[m_iParentBoneIndex]->m_CombinedTransformationMatrix));
	}
}

CBone* CBone::Create(const aiNode* _pAINode, _int _iParentBoneIndex)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(_pAINode, _iParentBoneIndex)))
	{
		MSG_BOX("Failed to Created : CBone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CBone* CBone::Create(const FBX_BONE& _pBoneDesc)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(_pBoneDesc)))
	{
		MSG_BOX("Failed to Created : CBone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CBone* CBone::Clone()
{
	return new CBone(*this);
}

void CBone::Free()
{
	__super::Free();
}

HRESULT CBone::ConvertToBinary(HANDLE _hFile, DWORD* _dwByte)
{

	if (!WriteFile(_hFile, m_szName, sizeof(m_szName), _dwByte, nullptr))
		return E_FAIL;

	if (!WriteFile(_hFile, &m_TransformationMatrix, sizeof(_float4x4), _dwByte, nullptr))
		return E_FAIL;

	if (!WriteFile(_hFile, &m_iParentBoneIndex, sizeof(_int), _dwByte, nullptr))
		return E_FAIL;


	return S_OK;
}
