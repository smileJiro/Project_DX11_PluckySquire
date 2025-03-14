#include "Trail_Manager.h"

IMPLEMENT_SINGLETON(CTrail_Manager)

_int CTrail_Manager::m_iTrailID = 0;

CTrail_Manager::CTrail_Manager()
{
}

void CTrail_Manager::Update(_float _fTimeDelta)
{
	for (auto& vecTrails : m_TrailInstances)
	{
		for (auto iter = vecTrails.second.begin(); iter != vecTrails.second.end();)
		{
			CTrailInstance* pTrail = *iter;
			if (true == pTrail->Is_Finished())
			{
				/* 트레일이 끝났으면 객체를 해제하고 벡터에서 제거 */
				Safe_Release(pTrail);
				/* erase 후 iter 갱신 */
				iter = vecTrails.second.erase(iter);  
			}
			else
			{
				/*트레일 지속 시간 검사 및 업데이트*/
				pTrail->Update(_fTimeDelta);
				++iter;
			}
		}
	}
}

vector<CTrailInstance*>* CTrail_Manager::Find_Trails(_int _iTrailID)
{
	auto iter = m_TrailInstances.find(_iTrailID);

	if (iter == m_TrailInstances.end())
		return nullptr;

	return &iter->second;
}

HRESULT CTrail_Manager::Add_Trail(_int _iTrailID, CTrailInstance::TRAIL_DESC* _pDesc)
{
	CTrailInstance* pTrailInstance = CTrailInstance::Create(_pDesc);
	if (nullptr == pTrailInstance)
		return E_FAIL;

	vector<CTrailInstance*>* pTrails = Find_Trails(_iTrailID);
	if (nullptr == pTrails)
	{
		/* Trail이 처음 추가 되는 것이라면, */
		vector<CTrailInstance*> Trails;
		Trails.push_back(pTrailInstance);
		m_TrailInstances.emplace(_iTrailID, Trails);
	}
	else
	{
		/* 기존의 트레일이 있다면, */
		pTrails->push_back(pTrailInstance);
	}

	return S_OK;
}

HRESULT CTrail_Manager::Render_Trails(_int _iTrailID, C3DModel* _pModelCom, CShader* _pShaderCom)
{
	vector<CTrailInstance*>* pTrails = Find_Trails(_iTrailID);
	if (nullptr == pTrails)
		return S_OK; /* 트레일이 존재하지 않아도 딱히 문제로 보진 말자. */

	
	for (size_t i = 0; i < pTrails->size(); ++i)
	{
		(*pTrails)[i]->Render(_pShaderCom, _pModelCom);
	}

	return S_OK;
}

void CTrail_Manager::Free()
{
	for (auto& vecTrails : m_TrailInstances)
	{
		for (auto& pTrail : vecTrails.second)
		{
			Safe_Release(pTrail);
		}
		vecTrails.second.clear();
	}
	m_TrailInstances.clear();


	__super::Free();
}
