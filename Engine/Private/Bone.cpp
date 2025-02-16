#include "Bone.h"
#include "Transform.h"

CBone::CBone()
{
}


HRESULT CBone::Initialize(ifstream& inFile, _int iParentBoneIndex)
{

	_uint iNameLength = 0;
	inFile.read((char*)&iNameLength, sizeof(_uint));
	//cout << iNameLength << endl;
	inFile.read((char*)&m_szName, iNameLength);
	m_szName[iNameLength] = '\0';
	//cout << m_szName << endl;
	//inFile.read((char*)&m_bBillboard, 1);
	inFile.read(reinterpret_cast<char*>(&m_TransformationMatrix), sizeof(_float4x4));
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	m_iParentBoneIndex = iParentBoneIndex;

	return S_OK;
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
CBone* CBone::Create(ifstream& inFile, _int iParentBoneIndex)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(inFile, iParentBoneIndex)))
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
