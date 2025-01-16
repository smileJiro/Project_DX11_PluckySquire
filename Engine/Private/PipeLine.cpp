#include "PipeLine.h"

CPipeLine::CPipeLine()
{
}

HRESULT CPipeLine::Initialize()
{

    return S_OK;
}

void CPipeLine::Update()
{
    // 입력 받아온 Matrix들의 역 행렬을 미리 구해서 저장해둔다. 
    Setting_PipeLineData();
}

_float2 CPipeLine::Transform_WorldToScreen(_fvector _vWorldPos, _float2 _fScreenSize)
{
    _vector vWorldPos = _vWorldPos;

    _matrix matVP = XMLoadFloat4x4(&m_TransformMatrices[D3DTS_VIEW])* XMLoadFloat4x4(&m_TransformMatrices[D3DTS_PROJ]);
    _vector vClipPos = XMVector3TransformCoord(vWorldPos, matVP);
    _float2 vNDCPos = {};
    XMStoreFloat2(&vNDCPos, vClipPos / XMVectorGetW(vWorldPos));

    /* Screen 좌표로 변환 . */
    _float2 vScreenPos = { (vNDCPos.x + 1.0f) * _fScreenSize.x * 0.5f ,
                           (1.0f - vNDCPos.y) * _fScreenSize.y * 0.5f };

    return vScreenPos;
}

void CPipeLine::Setting_PipeLineData()
{
    // Inverse Matrix
    for (_uint i = 0; i < D3DTS_END; ++i)
    {
        XMStoreFloat4x4(&m_TransformInverseMatrices[i], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformMatrices[i])));
    }

    // Camera Position
    XMStoreFloat4(&m_vCamPosition, XMLoadFloat4((_float4*)&m_TransformInverseMatrices[D3DTS_VIEW].m[3]));
}

CPipeLine* CPipeLine::Create()
{
    CPipeLine* pInstance = new CPipeLine();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CPipeLine");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPipeLine::Free()
{
    __super::Free();

}