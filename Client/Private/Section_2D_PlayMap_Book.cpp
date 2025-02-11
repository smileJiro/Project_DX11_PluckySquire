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

CSection_2D_PlayMap_Book* CSection_2D_PlayMap_Book::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iPriorityKey, json _SectionJson)
{
	CSection_2D_PlayMap_Book* pInstance = new CSection_2D_PlayMap_Book(_pDevice, _pContext);

	if (FAILED(pInstance->Import(_SectionJson, _iPriorityKey)))
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
