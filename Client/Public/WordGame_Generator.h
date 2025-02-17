#pragma once
#include "Base.h"
#include "Section_2D.h"
BEGIN(Client)
class CWordGame_Generator final :
	public CBase
{
protected:
	explicit CWordGame_Generator(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CWordGame_Generator() = default;

public:
	HRESULT WordGame_Generate(CSection_2D* _pSection, json _pWordJson);

public:
	static CWordGame_Generator* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void			Free() override;

};

END