#pragma once
#include "Base.h"
BEGIN(Engine)
class CGameInstance;
class CD3DUtils final : public CBase
{
private:
	CD3DUtils(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CD3DUtils() = default;

public:
	HRESULT Initialize();


public:
	template<typename T_CONSTANT>
	HRESULT CreateConstBuffer(const T_CONSTANT& _tConstantBufferData, D3D11_USAGE _eUsage, ID3D11Buffer** _ppOutConstantBuffer);
	template<typename T_CONSTANT>
	HRESULT UpdateConstBuffer(const T_CONSTANT& _tConstantBufferData, ID3D11Buffer* _pConstantBuffer);

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;
	CGameInstance*			m_pGameInstance = nullptr;

public:
	static CD3DUtils* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	void Free()override;
};

template<typename T_CONSTANT>
inline HRESULT CD3DUtils::CreateConstBuffer(const T_CONSTANT& _tConstantBufferData, D3D11_USAGE _eUsage, ID3D11Buffer** _ppOutConstantBuffer)
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
inline HRESULT CD3DUtils::UpdateConstBuffer(const T_CONSTANT& _tConstantBufferData, ID3D11Buffer* _pConstantBuffer)
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

END
