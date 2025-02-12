#pragma once
#include "Base.h"

BEGIN(Engine)

class CCustomFont final : public CBase
{
private:
	CCustomFont(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CCustomFont() = default;

public:
	HRESULT Initialize(const _tchar* _pFontFilePath);
	HRESULT Render(const _tchar* _pText, const _float2& _vPosition, _fvector _vColor, _float _fRotation = 0.f, const _float2& _vOrigin = _float2(0.f, 0.f));
	HRESULT Render_Scaling(const _tchar* _pText, const _float2& _vPosition, _fvector _vColor, _float _fRotation, const _float2& _vOrigin, float _fScale);
	
	// TODO :: 테스트용 코드 - 박상욱
	_vector	Measuring(_wstring _text);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	SpriteFont* m_pFont = { nullptr };
	SpriteBatch* m_pBatch = { nullptr };

public:

	static CCustomFont* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pFontFilePath);
	virtual void Free() override;
};

END