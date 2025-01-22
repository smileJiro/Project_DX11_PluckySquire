#pragma once
#include "Component.h"
BEGIN(Engine)
class ENGINE_DLL CTexture final : public CComponent
{
private:
	CTexture(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTexture(const CTexture& _Prototype);
	virtual ~CTexture() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* _pTextureFilePath, _uint _iNumTextures);
	virtual HRESULT Initialize_Prototype(const _char* _szTextureFilePath, _uint _iNumTextures);
	virtual HRESULT Initialize(void* _pArg) override;

public:
	/* 적용할 쉐이더 객체, hlsl 내의 전역변수 이름, 텍스쳐의 인덱스(애니메이션 등등에 대한 인덱스) */
	HRESULT Bind_ShaderResource(class CShader* _pShader, const _char* _pConstantName, _uint _iSRVIndex = 0);

public:
	// Get
	_uint Get_NumSRVs() const { return m_iNumSRVs; }

	ID3D11ShaderResourceView* Get_SRV(const _wstring _strTextureName, _uint* _pIndex = nullptr);
	ID3D11ShaderResourceView* Get_SRV(_uint _iSRVIndex)
	{
		if (_iSRVIndex >= m_SRVNames.size())
			return nullptr;
		if (nullptr == m_SRVs[_iSRVIndex])
			return nullptr;
		return m_SRVs[_iSRVIndex];
	}

	const _wstring* Get_SRVName(_uint _iSRVIndex) 
	{
			if (_iSRVIndex >= m_SRVNames.size())
				return nullptr;
		return &m_SRVNames[_iSRVIndex];

	}

	void	Add_Texture(ID3D11ShaderResourceView* _pSRV, const _wstring strSRVName)
	{
		m_SRVs.push_back(_pSRV);
		m_SRVNames.push_back(strSRVName);
		m_iNumSRVs++;
	}
private:
	_uint m_iNumSRVs = 0;
	vector<ID3D11ShaderResourceView*> m_SRVs;
	vector<_wstring> m_SRVNames;

public:
	static CTexture* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _char* _szTextureFilePath, _uint _iNumTextures = 1);
	static CTexture* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pTextureFilePath, _uint _iNumTextures = 1);
	static CTexture* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CComponent* Clone(void* _pArg);
	virtual void Free() override;
};

END