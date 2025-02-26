#pragma once
#include "Component.h"
#include "Texture.h"
/* Material 에 대한 텍스쳐를 저장하기위한 데이터 저장용 Class */
/* 클라이언트에서 접근할 일이... */
BEGIN(Engine)
class CShader;
class ENGINE_DLL CMaterial  : public CComponent
{
protected:
	CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMaterial(const CMaterial& _Rhs); // 프로토타입용 복사생성자 아님. (모두 얕은복사하지만, constBuffer 만 깊은복사 생성하는 복사생성자.)
	virtual ~CMaterial() = default;

public:
	virtual HRESULT Initialize(const _char* szDirPath, ifstream& inFile);

public:
	ID3D11ShaderResourceView* Find_Texture(aiTextureType _eTextureType, const _wstring _strTextureName, _uint* _pIndex = nullptr)
	{
		ID3D11ShaderResourceView* pReturnTexture = nullptr;
		if (nullptr == m_MaterialTextures[_eTextureType])
			return nullptr;
		pReturnTexture = m_MaterialTextures[_eTextureType]->Get_SRV(_strTextureName, _pIndex);
		return pReturnTexture;
	}
	ID3D11ShaderResourceView* Find_Texture(aiTextureType _eTextureType, _uint _iIndex)
	{
		if (nullptr == m_MaterialTextures[_eTextureType] || 
			_iIndex >= m_MaterialTextures[_eTextureType]->Get_NumSRVs())
			return nullptr;
		ID3D11ShaderResourceView* pSRV = nullptr;
		pSRV = m_MaterialTextures[_eTextureType]->Get_SRV(_iIndex);
		if (nullptr == pSRV)
			return nullptr;

		return pSRV;
	}
	const wstring* Find_Name(aiTextureType _eTextureType, _uint _iIndex)
	{
		if (nullptr == m_MaterialTextures[_eTextureType] ||
			_iIndex >= m_MaterialTextures[_eTextureType]->Get_NumSRVs())
			return nullptr;
		return m_MaterialTextures[_eTextureType]->Get_SRVName(_iIndex);
	}
	HRESULT Add_Texture(aiTextureType _eTextureType, ID3D11ShaderResourceView* _pSRV, const _wstring _strSRVName)
	{
		if (nullptr == m_MaterialTextures[_eTextureType])
			m_MaterialTextures[_eTextureType] = CTexture::Create(m_pDevice,m_pContext);
		m_MaterialTextures[_eTextureType]->Add_Texture(_pSRV, _strSRVName);
		// 일반적으로 SRV는 이곳이 원본일 것임. AddRef() 안한다.
		return S_OK;
	}
#ifdef _DEBUG
	HRESULT Add_Texture(aiTextureType _eTextureType)
	{
		if (nullptr != m_MaterialTextures[_eTextureType])
			return E_FAIL;
		m_MaterialTextures[_eTextureType] = CTexture::Create(m_pDevice, m_pContext);

		return S_OK;
	}

	HRESULT Delete_Texture(aiTextureType _eTextureType, _uint _iIndex)
	{
		if (nullptr == m_MaterialTextures[_eTextureType])
			m_MaterialTextures[_eTextureType] = CTexture::Create(m_pDevice, m_pContext);
		m_MaterialTextures[_eTextureType]->Delete_Texture(_iIndex);
		return S_OK;
	}

#endif // _DEBUG

	HRESULT	Update_PixelConstBuffer();
	HRESULT	Ready_PixelConstBuffer();

	HRESULT Bind_PixelConstBuffer(CShader* _pShader);

public:
	// Get
	const CONST_PS Get_PixelConstBuffer() const { return m_tPixelConstData; }

	const _float4& Get_Albedo() const { return m_tPixelConstData.Material.Albedo; }
	float Get_Roughness() const { return m_tPixelConstData.Material.Roughness; }
	float Get_Metallic() const { return m_tPixelConstData.Material.Metallic; }
	float Get_AO() const { return m_tPixelConstData.Material.AO; }

	const _float4& Get_MultipleAlbedo() const { return m_tPixelConstData.Material.MultipleAlbedo; }

	// Set
	void Use_AlbedoMap(_bool _useAlbedoMap, _bool _isUpdate = false);
	void Set_PixelConstBuffer(const CONST_PS& _tPixelConstData, _bool _isUpdate = false);

	void Set_Albedo(const _float4& _vAlbedo, _bool _isUpdate = false);
	void Set_Roughness(_float _fRoughness, _bool _isUpdate = false);
	void Set_Metallic(_float _fMetallic, _bool _isUpdate = false);
	void Set_AO(_float _fAO, _bool _isUpdate = false);

	void Set_MultipleAlbedo(const _float4& _vMutipleAlbedo, _bool _isUpdate = false);

protected:
	// 텍스쳐를 담는 벡터를 보관하는 배열.
	CTexture*					m_MaterialTextures[aiTextureType_UNKNOWN];

protected: /* Shader에 바인딩하는 ConstBuffer */
	CONST_PS					m_tPixelConstData = {};
	ID3D11Buffer*				m_pPixeConstBuffer = nullptr;


public:
	static CMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* szDirPath, ifstream& inFile);
	virtual void Free() override;

	// CComponent을(를) 통해 상속됨
	CMaterial* Clone_DeepCopyConstBuffer(); // constbuffer만 새로생성하고, 나머지는 얕은복사하는
	CComponent* Clone(void* pArg) override;
};

END