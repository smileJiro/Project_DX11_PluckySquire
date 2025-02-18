#pragma once
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END


BEGIN(Client)
class CWordPrinter : public CBase
{
private :
	CWordPrinter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContex);
	virtual ~CWordPrinter() = default;

public :
	HRESULT Initialize();

public :
	ID3D11ShaderResourceView* Print_Word(const _wstring& _strWordText, _float2 _fSize);



	

private :
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;
	CGameInstance*			m_pGameInstance = nullptr;

	D3D11_TEXTURE2D_DESC				m_tSampleRenderTargetDesc;
	ID3D11RenderTargetView*				m_pBackRTV = nullptr;
	ID3D11DepthStencilView*				m_pOriginalDSV = nullptr;
public :
	static CWordPrinter* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free();
};
END

