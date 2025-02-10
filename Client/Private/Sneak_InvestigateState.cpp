#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Sneak_InvestigateState.h"
#include "Monster.h"
#include "FSM.h"

CSneak_InvestigateState::CSneak_InvestigateState()
{
}

HRESULT CSneak_InvestigateState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);
	m_fAlertRange = pDesc->fAlertRange;
	m_fChaseRange = pDesc->fChaseRange;
	m_fAttackRange = pDesc->fAttackRange;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;
		
	return S_OK;
}


void CSneak_InvestigateState::State_Enter()
{
}

void CSneak_InvestigateState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;

	_vector vDir = XMLoadFloat3(&m_vSneakPos) - m_pOwner->Get_FinalPosition();
	_float fDis = XMVectorGetX(XMVector3Length((vDir)));	//3D상에서 y값도 더해서 거리 계산하는거 주의
	//XMVectorSetY(vDir, XMVectorGetY(m_pOwner->Get_FinalPosition()));
	XMVectorSetW(vDir, 0.f);
	vDir = XMVector3Normalize(XMVectorSetY(vDir, 0.f));
	
	//플레이어 시야 들어오면 인식 전환
	Check_Target3D(true);

	//이동하다 소리가 나면 범위 내에서 가장 최근 위치로 다음 위치를 갱신
	if (m_pOwner->IsTarget_In_Sneak_Detection())
	{
		Set_Sneak_InvestigatePos(m_pTarget->Get_FinalPosition());
	}

	//위치에 도착했는데 안 보이면 경계상태로 전환
	if (m_pOwner->Check_Arrival(XMLoadFloat3(&m_vSneakPos)))
	{
		Event_ChangeMonsterState(MONSTER_STATE::SNEAK_AWARE, m_pFSM);
		return;
	}
	else
	{
		//소리 난 위치로 장애물 피해서 추적
		//m_pOwner->Move_To(XMLoadFloat3(&m_vSneakPos)-m_pOwner->Get_FinalPosition());
		//m_pOwner->Rotate_To_Radians(vDir, m_pOwner->Get_ControllerTransform()->Get_RotationPerSec());
		if (false == m_isTurn)
		{
			Determine_AvoidDirection(XMLoadFloat3(&m_vSneakPos), &m_vDir);
			m_isTurn = true;
		}
		else if(m_pOwner->Rotate_To_Radians(XMLoadFloat3(&m_vDir), m_pOwner->Get_ControllerTransform()->Get_RotationPerSec()))
		{
			
			static_cast<CActor_Dynamic*>(m_pOwner->Get_ActorCom())->Set_LinearVelocity(XMLoadFloat3(&m_vDir), m_pOwner->Get_ControllerTransform()->Get_SpeedPerSec());
			m_isTurn = false;
		}
	}
}

void CSneak_InvestigateState::State_Exit()
{
	m_isTurn = false;
}

void CSneak_InvestigateState::Determine_Direction()
{
	//이동할 때마다 타겟 방향쪽으로 꺾어서 막혔나 체크하고 안 막혔으면 회전

	_float3 vPos; XMStoreFloat3(&vPos, XMVector3Transform(XMLoadFloat3(&m_pOwner->Get_RayOffset()), m_pOwner->Get_FinalWorldMatrix()));
	_vector vRayDir = XMVector3Normalize(XMVectorSetY(XMLoadFloat3(&m_vSneakPos) - XMLoadFloat3(&vPos), 0.f));
	_float3 vRayDirection; XMStoreFloat3(&vRayDirection, vRayDir);
	_vector vLook = XMVector3Normalize(m_pOwner->Get_ControllerTransform()->Get_State(CTransform::STATE_LOOK));
	_vector vResult = XMVectorZero();

	//레이 사거리 임시
	// 
	//가운데 레이
	if (m_pGameInstance->RayCast_Nearest(vPos, vRayDirection, m_fRayDis))
	{
		_vector AxisY = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		//룩 벡터에서 타겟 방향과 가까운 방향으로 꺾은 방향이 막혔나 체크
		
		//룩 기준 왼쪽 대각선
		if (0 >= XMVectorGetY(XMVector3Cross(XMVectorSetY(vLook, 0.f), vRayDir)))
		{
			_float3 vDirLeftLookDiag; XMStoreFloat3(&vDirLeftLookDiag, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, -45.f)));
			if (m_pGameInstance->RayCast_Nearest(vPos, vDirLeftLookDiag, m_fRayDis))
			{
				_float3 vLookDir; XMStoreFloat3(&vLookDir, vLook);
				//현재 룩 벡터 막혔나 체크
				if (m_pGameInstance->RayCast_Nearest(vPos, vLookDir, m_fRayDis))
				{
					//타겟과 가까운 방향, 룩 방향 모두 막혀있으면 우선순위대로 체크
					_float3 vDirLeftLook; XMStoreFloat3(&vDirLeftLook, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, -90.f)));
					if (m_pGameInstance->RayCast_Nearest(vPos, vDirLeftLook, m_fRayDis))
					{
						_float3 vDirRightLookDiag; XMStoreFloat3(&vDirRightLookDiag, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, 45.f)));
						if (m_pGameInstance->RayCast_Nearest(vPos, vDirRightLookDiag, m_fRayDis))
						{
							_float3 vDirRightLook; XMStoreFloat3(&vDirRightLook, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, 90.f)));
							if (m_pGameInstance->RayCast_Nearest(vPos, vDirRightLook, m_fRayDis))
							{
								//뒤 쪽은 일단 처리 안함
							}
							else
							{
								vResult = XMLoadFloat3(&vDirRightLook);
								cout<< " 룩 오른쪽" << endl;
							}
						}
						else
						{
							vResult = XMLoadFloat3(&vDirRightLookDiag);
							cout << " 룩 오른쪽 대각선" << endl;
						}
					}
					else
					{
						vResult = XMLoadFloat3(&vDirLeftLook);
						cout << " 룩 왼쪽" << endl;
					}
				}
				else
				{
					//현재 룩벡터라서 바꿀 필요없을거 같긴 한데 일단 헷갈리니까 명시적으로
					vResult = vLook;
					cout << " 룩" << endl;
				}
			}
			else
			{
				vResult = XMLoadFloat3(&vDirLeftLookDiag);
				cout << " 룩 왼쪽 대각선" << endl;
			}
		}

		//룩 기준 오른쪽 대각선
		else
		{
			_float3 vDirRightLookDiag; XMStoreFloat3(&vDirRightLookDiag, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, -45.f)));
			if (m_pGameInstance->RayCast_Nearest(vPos, vDirRightLookDiag, m_fRayDis))
			{
				_float3 vLookDir; XMStoreFloat3(&vLookDir, vLook);
				//현재 룩 벡터 막혔나 체크
				if (m_pGameInstance->RayCast_Nearest(vPos, vLookDir, m_fRayDis))
				{
					//타겟과 가까운 방향, 룩 방향 모두 막혀있으면 우선순위대로 체크
					_float3 vDirRightLook; XMStoreFloat3(&vDirRightLook, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, -90.f)));
					if (m_pGameInstance->RayCast_Nearest(vPos, vDirRightLook, m_fRayDis))
					{
						_float3 vDirLeftLookDiag; XMStoreFloat3(&vDirLeftLookDiag, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, 45.f)));
						if (m_pGameInstance->RayCast_Nearest(vPos, vDirLeftLookDiag, m_fRayDis))
						{
							_float3 vDirLeftLook; XMStoreFloat3(&vDirLeftLook, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, 90.f)));
							if (m_pGameInstance->RayCast_Nearest(vPos, vDirLeftLook, m_fRayDis))
							{
								//뒤 쪽은 일단 처리 안함
							}
							else
							{
								vResult = XMLoadFloat3(&vDirLeftLook);
								cout << " 룩 왼쪽" << endl;
							}
						}
						else
						{
							vResult = XMLoadFloat3(&vDirLeftLookDiag);
							cout << " 룩 왼쪽 대각선" << endl;
						}
					}
					else
					{
						vResult = XMLoadFloat3(&vDirRightLook);
						cout << " 룩 오른쪽" << endl;
					}
				}
				else
				{
					//현재 룩벡터라서 바꿀 필요없을거 같긴 한데 일단 헷갈리니까 명시적으로
					vResult = vLook;
					cout << " 룩" << endl;
				}
			}
			else
			{
				vResult = XMLoadFloat3(&vDirRightLookDiag);
				cout << " 룩 오른쪽 대각선" << endl;
			}
		}






		/*

		//레이에 막혔으면 왼쪽, 오른쪽 대각선 순으로 레이 쏨
		_float3 vDirLeftDiag; XMStoreFloat3(&vDirLeftDiag, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vRayDir, -45.f)));
		_float3 vDirRightDiag; XMStoreFloat3(&vDirRightDiag, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vRayDir, 45.f)));
		if (m_pGameInstance->RayCast_Nearest(vPos, vDirLeftDiag, m_fRayDis))
		{
			if (m_pGameInstance->RayCast_Nearest(vPos, vDirRightDiag, m_fRayDis))
			{
				//왼쪽, 오른쪽으로 쏘는데 더하지 않고 우선순위대로 하나만 씀
				_float3 vDirLeft; XMStoreFloat3(&vDirLeft, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vRayDir, -90.f)));
				_float3 vDirRight; XMStoreFloat3(&vDirRight, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vRayDir, 90.f)));

				if (false == m_pGameInstance->RayCast_Nearest(vPos, vDirLeft, m_fRayDis))
				{
					vResult += XMLoadFloat3(&vDirLeft);
					cout << "왼쪽" << endl;
				}
				else if (false == m_pGameInstance->RayCast_Nearest(vPos, vDirRight, m_fRayDis))
				{
					vResult += XMLoadFloat3(&vDirRight);
					cout << "오른쪽" << endl;
				}

				//전부 다 막혔을 때는 뒤로감 (임시처리)
				else
				{
					vResult = -1.f * vRayDir;
					cout << "뒤" << endl;
				}

			}
			else
			{
				vResult += XMLoadFloat3(&vDirRightDiag);
				cout << "오른쪽 대각선" << endl;
			}
		}
		else
		{
			vResult += XMLoadFloat3(&vDirLeftDiag);
			cout << "왼쪽 대각선" << endl;
		}

		*/
	}
	//막히지 않은 쪽의 방향 벡터 더함
	else
	{
		vResult += vRayDir;
		cout << "가운데" << endl;
	}

	XMStoreFloat3(&m_vDir, vResult);
	m_isTurn = true;
}

CSneak_InvestigateState* CSneak_InvestigateState::Create(void* _pArg)
{
	CSneak_InvestigateState* pInstance = new CSneak_InvestigateState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CSneak_InvestigateState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSneak_InvestigateState::Free()
{
	__super::Free();
}
