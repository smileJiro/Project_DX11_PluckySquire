#include "stdafx.h"
#include "Section_2D_PlayMap_Book.h"
#include "GameInstance.h"
#include "2DMapObject.h"
#include "Map_2D.h"
#include "Section_Manager.h"
#include "Engine_Macro.h"
#include "MapObjectFactory.h"
#include "Trigger_Manager.h"

CSection_2D_PlayMap_Book::CSection_2D_PlayMap_Book(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CSection_2D_PlayMap(_pDevice, _pContext, PLAYMAP, SECTION_2D_BOOK)
{
}

HRESULT CSection_2D_PlayMap_Book::Section_Enter(const _wstring& _strPreSectionTag)
{
	if (TEXT("Stop") == m_strBGMTag)
		m_pGameInstance->End_BGM();
	else if (TEXT("") != m_strBGMTag)
		m_pGameInstance->Transition_BGM(m_strBGMTag, 20.f);

	return S_OK;
}

CSection_2D_PlayMap_Book* CSection_2D_PlayMap_Book::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pDesc)
{
	CSection_2D_PlayMap_Book* pInstance = new CSection_2D_PlayMap_Book(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_pDesc)))
	{
		MSG_BOX("Failed Create CSection_2D_PlayMap_Book");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CSection_2D_PlayMap_Book::Free()
{
	__super::Free();
}
