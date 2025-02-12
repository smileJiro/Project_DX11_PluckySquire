#pragma once
#include "Base.h"
BEGIN(Engine)

class CFont_Manager final : public CBase
{
private:
	CFont_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CFont_Manager() = default;

public:
	HRESULT Add_Font(const _wstring& _strFontTag, const _tchar* _pFontFilePath);
	HRESULT Render_Font(const _wstring& _strFontTag, const _tchar* _pText, const _float2& _vPosition, _fvector _vColor, _float _fRotation = 0.f, const _float2& _vOrigin = _float2(0.f, 0.f));
	HRESULT Render_Scaling_Font(const _wstring& _strFontTag, const _tchar* _pText, const _float2& _vPosition, _fvector _vColor, _float _fRotation = 0.f, const _float2& _vOrigin = _float2(0.f, 0.f), _float _fScale = 1.f);

	// TODO ::  테스트용도 - 박상욱
	_vector	Measuring(const _wstring& _strFontTag, _wstring _text);


private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	map<const _wstring, class CCustomFont*>			m_Fonts;


private:
	class CCustomFont* Find_Font(const _wstring& _strFontTag);



public:
	static CFont_Manager* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END

