#include "Model.h"
#include "AnimEventGenerator.h"
#include "Animation.h"
CModel::CModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CComponent(_pDevice, _pContext)
{
}

CModel::CModel(const CModel& _Prototype)
	:CComponent(_Prototype)
	, m_eAnimType{ _Prototype.m_eAnimType }
{
}


void CModel::Free()
{
	__super::Free();
}

void CModel::Set_AnimEventGenerator(CAnimEventGenerator* _pAnimEventGenerator)
{
	auto& AnimEvents = _pAnimEventGenerator->Get_AnimEvents();
	for (auto& pairAnimEvents : AnimEvents)
	{
		CAnimation* pAnim =  Get_Animation(pairAnimEvents.first);
		for (auto& AnimEvent : pairAnimEvents.second)
		{
			pAnim->RegisterAnimEvent(&AnimEvent);
		}
	}
}
