#include "stdafx.h"
#include "WordGame_Generator.h"
#include "Player.h"
#include "Section_Manager.h"    

#include "Collider_Circle.h"
#include "Actor_Dynamic.h"
#include "Controller_Model.h"
#include "Model.h"

CWordGame_Generator::CWordGame_Generator(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CBase()
{
}

HRESULT CWordGame_Generator::WordGame_Generate(CSection_2D* _pSection, json _pWordJson)
{
	return E_NOTIMPL;
}

CWordGame_Generator* CWordGame_Generator::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CWordGame_Generator* pInstance = new CWordGame_Generator(_pDevice, _pContext);

	//if (FAILED(pInstance->Initialize_Prototype()))
	//{
	//	MSG_BOX("Failed to Created : CordGame_Generator");
	//	Safe_Release(pInstance);
	//}

	return pInstance;
}

void CWordGame_Generator::Free()
{
	__super::Free();
}
