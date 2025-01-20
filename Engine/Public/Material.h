#pragma once
#include "Component.h"
#include "Texture.h"
/* Material 에 대한 텍스쳐를 저장하기위한 데이터 저장용 Class */
/* 클라이언트에서 접근할 일이... */
BEGIN(Engine)

class CMaterial final : public CComponent
{
protected:
	CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMaterial() = default;

public:
	HRESULT Initialize( const _char* szDirPath, ifstream& inFile);

public:
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
	HRESULT Add_Texture(aiTextureType _eTextureType, ID3D11ShaderResourceView* _pSRV, const _wstring strSRVName)
	{
		m_MaterialTextures[_eTextureType]->Add_Texture(_pSRV, strSRVName);
		// 일반적으로 SRV는 이곳이 원본일 것임. AddRef() 안한다.
		return S_OK;
	}

private:
	// 텍스쳐를 담는 벡터를 보관하는 배열.
	CTexture* m_MaterialTextures[AI_TEXTURE_TYPE_MAX];

public:
	static CMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* szDirPath, ifstream& inFile);
	virtual void Free() override;

	// CComponent을(를) 통해 상속됨
	CComponent* Clone(void* pArg) override;
};

END