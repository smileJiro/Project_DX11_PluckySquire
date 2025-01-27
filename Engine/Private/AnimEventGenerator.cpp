#include "AnimEventGenerator.h"
#include "Animation.h"
#include "Model.h"
#include "AnimEventReceiver.h"

CAnimEventGenerator::CAnimEventGenerator(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
{
}

CAnimEventGenerator::CAnimEventGenerator(const CAnimEventGenerator& _Prototype)
	: CComponent(_Prototype)
	,m_AnimEvents(_Prototype.m_AnimEvents)
{

}


HRESULT CAnimEventGenerator::Initialize_Prototype(const _char* _pAnimEventFilePath)
{
	std::ifstream inFile(_pAnimEventFilePath, std::ios::binary);
	if (!inFile) {
		string str = "파일을 열 수 없습니다.";
		str += _pAnimEventFilePath;
		MessageBoxA(NULL, str.c_str(), "에러", MB_OK);
		return E_FAIL;
	}
	_uint iAnimIndexCount = 0;
	inFile.read(reinterpret_cast<char*>(&iAnimIndexCount), sizeof(_uint));
	for (_uint i = 0; i < iAnimIndexCount; i++)
	{
		_uint iAnimIndex = 0;
		_uint iEventCount = 0;
		inFile.read(reinterpret_cast<char*>(&iAnimIndex), sizeof(_uint));
		inFile.read(reinterpret_cast<char*>(&iEventCount), sizeof(_uint));
		m_AnimEvents.emplace(iAnimIndex, vector<ANIM_EVENT>());
		m_AnimEvents[iAnimIndex].resize(iEventCount);
		for (_uint j = 0; j < iEventCount; j++)
		{
			m_AnimEvents[iAnimIndex][j].ReadFile(inFile);
		}
	}

	inFile.close();
	return S_OK;
}

HRESULT CAnimEventGenerator::Initialize(void* _pArg)
{
	ANIMEVTGENERATOR_DESC* pDesc = (ANIMEVTGENERATOR_DESC*)_pArg;
	m_pSenderModel = pDesc->pSenderModel;
	m_pReceiver = pDesc->pReceiver;
	m_pSenderModel->Set_AnimEventGenerator(this);
	__super::Initialize(_pArg);
	return S_OK;
}

void CAnimEventGenerator::Update(_float fTimeDelta)
{
	_uint iCurAnimIdx = m_pSenderModel->Get_CurrentAnimIndex();
	if (m_AnimEvents.find(iCurAnimIdx) == m_AnimEvents.end())
		return;
	vector<ANIM_EVENT>& Events =  m_AnimEvents[iCurAnimIdx];
	for (ANIM_EVENT& tEvnt : Events)
	{
		if (tEvnt.bIsTriggered)
			continue;
		if (m_pSenderModel->Get_CurrentAnimProgeress() >= tEvnt.fProgress)
		{
			m_pReceiver->Invoke(tEvnt.strFuncName);
			tEvnt.bIsTriggered = true;
		}
	}
	
}

CAnimEventGenerator* CAnimEventGenerator::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _char* pModelFilePath)
{
	CAnimEventGenerator* pInstance = new CAnimEventGenerator(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath)))
	{
		MSG_BOX("Failed to Created : AnimEventGenerator");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CAnimEventGenerator::Clone(void* _pArg)
{
	CAnimEventGenerator* pInstance = new CAnimEventGenerator(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : AnimEventGenerator");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimEventGenerator::Free()
{
	__super::Free();
}


#ifdef _TOOL
HRESULT CAnimEventGenerator::Export_AnimEvents(ofstream& _OutFile)
{
	if (m_AnimEvents.empty())
	{
		MSG_BOX("이벤트가 없는데요?");
		return E_FAIL;
	}

	_uint iAnimIndexCount = m_AnimEvents.size();
	_OutFile.write(reinterpret_cast<char*>(&iAnimIndexCount), sizeof(_uint));
	for (auto& pairAnimEvt : m_AnimEvents)
	{
		_uint iAnimIndex = pairAnimEvt.first;
		_uint iEventCount = pairAnimEvt.second.size();
		_OutFile.write(reinterpret_cast<char*>(&iAnimIndex), sizeof(_uint));
		_OutFile.write(reinterpret_cast<char*>(&iEventCount), sizeof(_uint));
		for (auto& tEvent : pairAnimEvt.second)
		{
			tEvent.WriteFile(_OutFile);
		}
	}
}
#endif // _TOOL