#include "stdafx.h"
#include "Word_Controller.h"
#include "Word.h"
#include "FloorWord.h"

CWord_Controller::CWord_Controller(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CContainerObject(_pDevice,_pContext)
{
}

CWord_Controller::CWord_Controller(const CWord_Controller& _Prototype)
	: CContainerObject(_Prototype)
{
}

HRESULT CWord_Controller::Initialize_Prototype()
{
	return __super::Initialize_Prototype();
}

HRESULT CWord_Controller::Initialize(void* _pArg)
{
	return __super::Initialize(_pArg);
}

HRESULT CWord_Controller::Import(json _ControllerJson)
{
	_uint iContainerIndex = _ControllerJson["Container_Index"];

	_string strText = _ControllerJson["Text"];

	CWord::WORD_TYPE eType = _ControllerJson["Init_Word_Type"];

	_float2 fPos = { _ControllerJson["Position"][0].get<_float>(),  
		_ControllerJson["Position"][1].get<_float>() };

	_float2 fScale = { _ControllerJson["Scale"][0].get<_float>(),  
		_ControllerJson["Scale"][1].get<_float>() };

	



	if (_ControllerJson.contains("Init_Word_Type"))
	{
		for (auto& iIndex : _ControllerJson["Init_Word_Type"])
		{
			if (iIndex.is_number())
			{
				_uint _iIndex = iIndex;
			}
		}
	}


	return S_OK;
#pragma region 워드 확인

#pragma endregion

}

void CWord_Controller::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CWord_Controller::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CWord_Controller::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CWord_Controller::Render()
{
	return __super::Render();;
}

CWord_Controller* CWord_Controller::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, json _ControllerJson)
{
	CWord_Controller* pInstance = new CWord_Controller(_pDevice, _pContext);

	if (FAILED(pInstance->Import(_ControllerJson)))
	{
		MSG_BOX("Failed to Load : CWord_Controller");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWord_Controller::Clone(void* _pArg)
{
	return nullptr;
}

void CWord_Controller::Free()
{
	__super::Free();
}
