#pragma once
#include "Texture.h"
BEGIN(Engine)
class ENGINE_DLL CMipMapTexture final : public CTexture
{
private:
	CMipMapTexture(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CMipMapTexture(const CMipMapTexture& _Prototype);
	virtual ~CMipMapTexture() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	/* 적용할 쉐이더 객체, hlsl 내의 전역변수 이름, 텍스쳐의 인덱스(애니메이션 등등에 대한 인덱스) */
	HRESULT Bind_ShaderResource(class CShader* _pShader, const _char* _pConstantName, _uint _iSRVIndex = 0);

public:
	// Get
	_uint Get_NumSRVs() const { return m_iNumSRVs; }

	ID3D11ShaderResourceView* Get_SRV(const _wstring _strTextureName, _uint* _pIndex = nullptr);
	ID3D11ShaderResourceView* Get_SRV(_uint _iSRVIndex);


	const _wstring* Get_SRVName(_uint _iSRVIndex)
	{
		if (_iSRVIndex >= m_SRVNames.size())
			return nullptr;
		return &m_SRVNames[_iSRVIndex];

	}
	const _float2 Get_Size(_uint _iSRVIndex = 0);

	void	Add_Texture(ID3D11ShaderResourceView* _pSRV, const _wstring strSRVName);

	void	Add_SRVName(const _wstring& _wstrName)
	{
		m_SRVNames.push_back(_wstrName);
		D3D11_SHADER_RESOURCE_VIEW_DESC Desc = {};
		Desc.Format = DXGI_FORMAT_BC7_UNORM;
		Desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		Desc.Texture2D.MostDetailedMip = 2;  // Mip Level 2부터 사용
		Desc.Texture2D.MipLevels = -1;  // 전체 사용

		ID3D11ShaderResourceView* newSRV = nullptr;
		//HRESULT hr = device->CreateShaderResourceView(pTexture, &srvDesc, &newSRV);
		//if (SUCCEEDED(hr)) {
		//	mySRV = newSRV;
		//}

	}



protected:
	vector<ID3D11Texture2D*> m_Textures;

	_uint					 m_iMipLevel;

public:
	static CMipMapTexture* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CComponent* Clone(void* _pArg);
	virtual void Free() override;
};

END