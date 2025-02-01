#include "CustomFont.h"

CCustomFont::CCustomFont(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : m_pDevice(_pDevice)
    , m_pContext(_pContext)
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CCustomFont::Initialize(const _tchar* _pFontFilePath)
{
    /* Create Font */
    m_pFont = new SpriteFont(m_pDevice, _pFontFilePath);

    /* Font Render Batch */
    m_pBatch = new SpriteBatch(m_pContext);
    return S_OK;
}

HRESULT CCustomFont::Render(const _tchar* _pText, const _float2& _vPosition, _fvector _vColor, _float _fRotation, const _float2& _vOrigin)
{
    m_pBatch->Begin();

    /* Rotation : Origin을 기준으로 z축 회전 각 */
    m_pFont->DrawString(m_pBatch, _pText, _vPosition, _vColor, _fRotation, _vOrigin);

    m_pBatch->End();

    return S_OK;
}

HRESULT CCustomFont::Render_Scaling(const _tchar* _pText, const _float2& _vPosition, _fvector _vColor, _float _fRotation, const _float2& _vOrigin, float _fScale)
{
	m_pBatch->Begin();

	/* Rotation : Origin을 기준으로 z축 회전 각 */
	m_pFont->DrawString(m_pBatch, _pText, _vPosition, _vColor, _fRotation, _vOrigin, _fScale);

	m_pBatch->End();

	return S_OK;
}

CCustomFont* CCustomFont::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pFontFilePath)
{
    CCustomFont* pInstance = new CCustomFont(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(_pFontFilePath)))
    {
        MSG_BOX("Failed to Created : CCustomFont");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCustomFont::Free()
{
    Safe_Delete(m_pFont);
    Safe_Delete(m_pBatch);

    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);

    __super::Free();
}
