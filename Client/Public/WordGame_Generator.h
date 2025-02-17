#pragma once
#include "Base.h"
#include "Section_2D.h"

BEGIN(Client)

class CWord;
class CWordPrinter;

class CWordGame_Generator final :
	public CBase
{
protected:
	explicit CWordGame_Generator(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CWordGame_Generator() = default;
public :
	HRESULT Initialize();

public:
	HRESULT	Load_WordTexture(const _wstring& _strJsonPath);
	HRESULT	Clear_WordTexture();
	HRESULT WordGame_Generate(CSection_2D* _pSection,const _wstring& _strJsonPath);

	CWord* Find_Word(_uint _iWordIndex);

private:
	ID3D11Device*						m_pDevice = nullptr;
	CWordPrinter*						m_pWordPrinter = nullptr;
	ID3D11DeviceContext*				m_pContext = nullptr;
	CGameInstance*						m_pGameInstance = nullptr;
	vector<CWord*>						m_Words;

public:
	static CWordGame_Generator* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void			Free() override;

};

END