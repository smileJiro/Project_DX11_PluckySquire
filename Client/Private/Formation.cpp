#include "stdafx.h"
#include "Formation.h"
#include "GameInstance.h"
#include "PlayerData_Manager.h"
#include "Event_Manager.h"
#include "Monster.h"

CFormation::CFormation(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CFormation::CFormation(const CGameObject& Prototype)
	: CGameObject(Prototype)
{
}

_bool CFormation::Has_EmptySlot()
{
	if (false == m_EmptySlots.empty())
		return true;

	return false;
}

HRESULT CFormation::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFormation::Initialize(void* _pArg)
{
	CFormation::FORMATIONDESC* pDesc = static_cast<FORMATIONDESC*>(_pArg);

	if (0 < pDesc->iRow)
		m_iRow = pDesc->iRow;
	if (0 < pDesc->iColumn)
		m_iColumn = pDesc->iColumn;
	if (0.f < pDesc->fRow_Interval)
		m_fRow_Interval = pDesc->fRow_Interval;
	if (0.f < pDesc->fColumn_Interval)
		m_fColumn_Interval = pDesc->fColumn_Interval;

	m_fDelayTime = pDesc->fDelayTime;

	m_OffSets.resize(m_iRow * m_iColumn);

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;
	if (FAILED(Initialize_OffSets()))
		return E_FAIL;
	/*if (FAILED(Initialize_Members(_pArg)))
		return E_FAIL;*/


	return S_OK;
}

void CFormation::Update(_float _fTimeDelta)
{
	if (true == m_isDelay)
	{
		m_fAccTime += _fTimeDelta;
		if (m_fDelayTime <= m_fAccTime)
		{
			m_fAccTime = 0.f;
			m_isDelay = false;

			if (true == m_isBack)
				--m_iPatrolIndex;
			else
				++m_iPatrolIndex;

			if (m_PatrolPoints.size()-1 <= m_iPatrolIndex)
			{
				m_isBack = true;
				m_iPatrolIndex = m_PatrolPoints.size() - 1;
			}
			if (0 >= m_iPatrolIndex)
			{
				m_isBack = false;
				m_iPatrolIndex = 0;
			}
		}
	}
	else
	{
		//포인트로 이동 (회전하면서 이동하는거라 이상한지 체크해봐야함)
		if (true == Get_ControllerTransform()->MoveTo(XMLoadFloat3(&m_PatrolPoints[m_iPatrolIndex]), _fTimeDelta))
		{
			//도착하면 딜레이 동안 대기 하다가 다시 이동
			m_isDelay = true;
		}
	}
}

HRESULT CFormation::Initialize_Members(void* _pArg)
{
	CFormation::FORMATIONDESC* pDesc = static_cast<FORMATIONDESC*>(_pArg);

	CGameObject* pObject = nullptr;

	CMonster::MONSTER_DESC Monster_Desc;
	Monster_Desc.iCurLevelID = m_iCurLevelID;
	Monster_Desc.eStartCoord = COORDINATE_3D;
	Monster_Desc.isCoordChangeEnable = false;
	Monster_Desc.tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	Monster_Desc.isSneakMode = true;

	//+z 기준으로 행,열 세팅
	//초기 위치는 포메이션의 위치의 오프셋을 적용한채로 생성 (포메이션의 위치는 중심)
	_float3 vPos;

	for (_uint i = 0; i < m_iRow; ++i)
	{
		for (_uint j = 0; j < m_iColumn; ++j)
		{
			pObject = nullptr;
			XMStoreFloat3(&vPos, Get_FinalPosition() + XMLoadFloat3(&m_OffSets[i * m_iColumn + j]));
			
			Monster_Desc.tTransform3DDesc.vInitialPosition = vPos;
			//Monster_Desc.eWayIndex = SNEAKWAYPOINTINDEX::CHAPTER8_1;
			Monster_Desc.isFormationMode = true;
			Monster_Desc.pFormation = this;

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, pDesc->strMemberPrototypeTag, m_iCurLevelID, pDesc->strMemberLayerTag, &pObject, &Monster_Desc)))
				return E_FAIL;

			if (pObject != nullptr)
			{
				CMonster* pMonster = static_cast<CMonster*>(pObject);
				m_Members.push_back(pMonster);
				Safe_AddRef(pObject);
				Event_Set_Kinematic(static_cast<CActor_Dynamic*>(pMonster->Get_ActorCom()), true);
				return S_OK;
			}
		}
	}
	return S_OK;
}

HRESULT CFormation::Initialize_OffSets()
{
	_float3 vPos = { 0.f, 0.f, 0.f };

	//포메이션의 각 위치마다 중심으로부터의 오프셋을 저장
	for (_uint i = 0; i < m_iRow; ++i)
	{
		vPos.z = 0.f;

		_int iCenterZ = m_iRow / 2;

		//행이 짝수 일 때
		if (0 == m_iRow % 2)
		{
			if (iCenterZ > i)
			{
				vPos.z += m_fRow_Interval * 0.5f;
				vPos.z += (iCenterZ - 1 - i) * m_fRow_Interval;
			}
			else
			{
				vPos.z -= m_fRow_Interval * 0.5f;
				vPos.z -= (i - iCenterZ) * m_fRow_Interval;
			}
		}
		//행이 홀수 일 때
		else
		{
			if (iCenterZ > i)
			{
				vPos.z += (iCenterZ - i) * m_fRow_Interval;
			}
			else
			{
				vPos.z -= (i - iCenterZ) * m_fRow_Interval;
			}
		}


		for (_uint j = 0; j < m_iColumn; ++j)
		{
			vPos.x = 0.f;

			_int iCenterX = m_iColumn / 2;

			//열이 짝수 일 때
			if (0 == m_iColumn % 2)
			{
				if (iCenterX > j)
				{
					vPos.x -= m_fColumn_Interval * 0.5f;
					vPos.x -= (iCenterX - 1 - j) * m_fColumn_Interval;
				}
				else
				{
					vPos.x += m_fColumn_Interval * 0.5f;
					vPos.x += (j - iCenterX) * m_fColumn_Interval;
				}
			}
			//열이 홀수 일 때
			else
			{
				if (iCenterX > j)
				{
					vPos.x -= (iCenterX - j) * m_fColumn_Interval;
				}
				else
				{
					vPos.x += (j - iCenterX) * m_fColumn_Interval;
				}
			}

			m_OffSets[i * m_iColumn + j] = vPos;
		}
	}

	return S_OK;
}

void CFormation::Add_Formation_PatrolPoints(_float3& _vPatrolPoint)
{
	m_PatrolPoints.push_back(_vPatrolPoint);
}

_bool CFormation::Add_To_Formation(CMonster* _pMember, CFormation** _pFormation)
{
	if (nullptr == _pMember || false == _pMember->Is_ValidGameObject())
		return false;

	if (true == m_EmptySlots.empty())
		return false;

	_uint iIndex = *(m_EmptySlots.begin());

	if (nullptr != m_Members[iIndex])
	{
		m_EmptySlots.erase(iIndex);
		return Add_To_Formation(_pMember, _pFormation);
	}
	else
	{
		m_Members[iIndex] = _pMember;
		Safe_AddRef(_pMember);
		*_pFormation = this;
		return true;
	}

	return false;
}

_bool CFormation::Remove_From_Formation(CMonster* _pMember)
{
	if (nullptr == _pMember || false == _pMember->Is_ValidGameObject())
		return false;

	for (_uint iIndex = 0; iIndex < m_Members.size(); ++iIndex)
	{
		if (_pMember == m_Members[iIndex])
		{
			if (false == m_EmptySlots.insert(iIndex).second)
			{
				Safe_Release(m_Members[iIndex]);
				m_Members[iIndex] = nullptr;
				continue;
			}
			else
				return true;
		}
	}

	return false;
}

_bool CFormation::Get_Formation_Position(CMonster* _pMember, _float3* _vPosition)
{
	_float3 vPos;
	XMStoreFloat3(&vPos, Get_FinalPosition());
	for (_uint i = 0; i < m_Members.size(); ++i)
	{
		if (_pMember == m_Members[i])
		{
			XMStoreFloat3(&vPos, Get_FinalPosition() + XMLoadFloat3(&m_OffSets[i]));
			*_vPosition = vPos;
			return true;
		}
	}

	return false;
}


HRESULT CFormation::Cleanup_DeadReferences()
{
	return S_OK;
}

CFormation* CFormation::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CFormation* pInstance = new CFormation(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFormation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFormation::Clone(void* _pArg)
{
	CFormation* pInstance = new CFormation(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CFormation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFormation::Free()
{
	for (_uint i = 0; i < m_Members.size(); ++i)
	{
		if (nullptr != m_Members[i])
		{
			Safe_Release(m_Members[i]);
		}
	}
	m_Members.clear();

	__super::Free();
}