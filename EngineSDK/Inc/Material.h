#pragma once
#include "Component.h"

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
		if (m_MaterialTexture[_eTextureType].empty())
			return nullptr;

		if (nullptr == m_MaterialTexture[_eTextureType][_iIndex])
			return nullptr;

		return m_MaterialTexture[_eTextureType][_iIndex];
	}
	HRESULT Add_Texture(aiTextureType _eTextureType, ID3D11ShaderResourceView* _pSRV)
	{
		m_MaterialTexture[_eTextureType].push_back(_pSRV);
		// 일반적으로 SRV는 이곳이 원본일 것임. AddRef() 안한다.
		return S_OK;
	}

private:
	// 텍스쳐를 담는 벡터를 보관하는 배열.
	vector<ID3D11ShaderResourceView*> m_MaterialTexture[AI_TEXTURE_TYPE_MAX];
public:
	static CMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* szDirPath, ifstream& inFile);
	virtual void Free() override;

	// CComponent을(를) 통해 상속됨
	CComponent* Clone(void* pArg) override;
};

END