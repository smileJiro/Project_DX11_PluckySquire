#include "Font_Manager.h"
#include "GameInstance.h"

CFont_Manager::CFont_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : m_pDevice(_pDevice)
    , m_pContext(_pContext)
{
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pDevice);
}

HRESULT CFont_Manager::Add_Font(const _wstring& _strFontTag, const _tchar* _pFontFilePath)
{
    if (nullptr != Find_Font(_strFontTag))
        return E_FAIL;

    CCustomFont* pFont = CCustomFont::Create(m_pDevice, m_pContext, _pFontFilePath);
    if (nullptr == pFont)
        return E_FAIL;

    m_Fonts.emplace(_strFontTag, pFont);

    return S_OK;
}

HRESULT CFont_Manager::Render_Font(const _wstring& _strFontTag, const _tchar* _pText, const _float2& _vPosition, _fvector _vColor, _float _fRotation, const _float2& _vOrigin)
{
    CCustomFont* pFont = Find_Font(_strFontTag);
    if (nullptr == pFont)
        return E_FAIL;


    return pFont->Render(_pText, _vPosition, _vColor, _fRotation, _vOrigin);
}

HRESULT CFont_Manager::Render_Scaling_Font(const _wstring& _strFontTag, const _tchar* _pText, const _float2& _vPosition, _fvector _vColor, _float _fRotation, const _float2& _vOrigin, _float _fScale)
{
	CCustomFont* pFont = Find_Font(_strFontTag);
	if (nullptr == pFont)
		return E_FAIL;


	return pFont->Render_Scaling(_pText, _vPosition, _vColor, _fRotation, _vOrigin, _fScale);
}

CCustomFont* CFont_Manager::Find_Font(const _wstring& _strFontTag)
{
    auto	iter = m_Fonts.find(_strFontTag);

    if (iter == m_Fonts.end())
        return nullptr;

    return iter->second;
}

CFont_Manager* CFont_Manager::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    return new CFont_Manager(_pDevice, _pContext);
}

void CFont_Manager::Free()
{
    __super::Free();

    for (auto& iter : m_Fonts)
        Safe_Release(iter.second);

    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
}
