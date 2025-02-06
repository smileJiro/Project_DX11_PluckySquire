#pragma once

#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CGlobalFunction_Manager final : public CBase
{
private:
	CGlobalFunction_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CGlobalFunction_Manager() = default;

public:
	template<typename T_CONSTANT>
	HRESULT CreateConstBuffer(const T_CONSTANT& _tConstantBufferData, D3D11_USAGE _eUsage, ID3D11Buffer** _ppOutConstantBuffer);
	template<typename T_CONSTANT>
	HRESULT UpdateConstBuffer(const T_CONSTANT& _tConstantBufferData, ID3D11Buffer* _pConstantBuffer);
public:
	_float2				Get_CursorPos(HWND _hWnd);
	_vector				Get_OnClickPos(HWND _hWnd);

	string				WStringToString(const _wstring& _wstr);
	_wstring			StringToWString(const string& _str);
	
	_float3				Get_RotationFromMatrix_Quaternion(const _fmatrix& _WorldMatrix);
	_float				Nomalize_Angle(_float _fAngle);	// 0-360 사이로 각도 변환

	_float				Lerp(_float _fLeft, _float _fRight, _float _fRatio);
	_fvector			Get_BezierCurve(_fvector _vStartPoint, _fvector _vGuidePoint, _fvector _vEndPoint, _float _fRatio);

	_bool				MatrixDecompose(_float3* _vScale, _float4* _vQuaternion, _float3* _vPosition, FXMMATRIX _Matrix);		//원하는 성분만 넣으면 행렬에서 추출

	_float3				Get_ScaleFromMatrix(const _float4x4& _Matrix);
	_float3				Get_ScaleFromMatrix(const _fmatrix _Matrix);

	_float				Get_Angle_Between_Vectors(_fvector _vNormal, _fvector _vVector1, _fvector _vVector2);	//노말벡터 기준으로 방향 벡터 간 각도 구함 (0-360도 간)

	_float				Clamp_Degrees(_float _fDegrees);		//0~360도 사이로 만듦

	_uint					Compare_VectorLength(_fvector _vVector1, _fvector _vVector2);		//같으면 0 / 1번 벡터가 크면 1 / 2번 벡터가 크면 2

private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	CGameInstance* m_pGameInstance = nullptr;

public:
	static CGlobalFunction_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END

template<typename T_CONSTANT>
inline HRESULT CGlobalFunction_Manager::CreateConstBuffer(const T_CONSTANT& _tConstantBufferData, D3D11_USAGE _eUsage, ID3D11Buffer** _ppOutConstantBuffer)
{
	// T_CONSTANT 사이즈 가 16의로 나누어떨어진다면, OK, 만약 나머지가 있다면, 
	if (0 != sizeof(T_CONSTANT) % 16)
	{
		MSG_BOX("Constant buffer Size must be 16-byte aligned");
		return E_FAIL;
	}

	D3D11_BUFFER_DESC Desc;
	ZeroMemory(&Desc, sizeof(Desc));
	Desc.ByteWidth = sizeof(_tConstantBufferData);
	Desc.Usage = _eUsage;
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = _eUsage == D3D11_USAGE_DYNAMIC ? D3D11_CPU_ACCESS_WRITE : 0;
	Desc.MiscFlags = 0;
	Desc.StructureByteStride = 0;
	
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = &_tConstantBufferData;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;


	// 최종 상수 버퍼 리턴.
	if (FAILED(m_pDevice->CreateBuffer(&Desc, &InitData, _ppOutConstantBuffer)))
	{
		MSG_BOX("Failed Create ConstantBuffer");
		return E_FAIL;
	}

	return S_OK;
}

template<typename T_CONSTANT>
inline HRESULT CGlobalFunction_Manager::UpdateConstBuffer(const T_CONSTANT& _tConstantBufferData, ID3D11Buffer* _pConstantBuffer)
{
	if (nullptr == _pConstantBuffer)
		return E_FAIL;

	D3D11_BUFFER_DESC Desc;
	_pConstantBuffer->GetDesc(&Desc);
	if (D3D11_USAGE_DEFAULT == Desc.Usage)
	{
		MSG_BOX("ConstantBuffer Usage 가 Default라 Update ConstBuffer 불가.");
		return E_FAIL;
	}

	D3D11_MAPPED_SUBRESOURCE ms;
	if (FAILED(m_pContext->Map(_pConstantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms)))
		return E_FAIL;

	memcpy(ms.pData, &_tConstantBufferData, sizeof(_tConstantBufferData));
	m_pContext->Unmap(_pConstantBuffer, NULL);

	return S_OK;
}
