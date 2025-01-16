#include "stdafx.h"
#include "Arm_Manager.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CArm_Manager)

CArm_Manager::CArm_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CArm_Manager::Initialize()
{
	return S_OK;
}

void CArm_Manager::Update()
{
}

void CArm_Manager::Render()
{
	for (auto& Arm : m_Arms) 
		Arm.second->Render_Arm();
	
	if (nullptr != m_pCopyArm)
		m_pCopyArm->Render_Arm();

	if (nullptr != m_pCurrentArm)
		m_pCurrentArm->Render_Arm();
}

void CArm_Manager::Copy_Arm()
{
	if (nullptr != m_pCurrentArm) {
		if(nullptr != m_pCopyArm)
			Safe_Release(m_pCopyArm);
		
		m_pCopyArm = m_pCurrentArm->Clone();
	}
}

void CArm_Manager::Add_CopyArm(_wstring _wszArmTag)
{
	if (nullptr == m_pCopyArm)
		return;

	if (nullptr == Find_Arm(_wszArmTag)) {
		m_Arms.emplace(_wszArmTag, m_pCopyArm);
		m_pCopyArm->Set_ArmType(CCameraArm::OTHER);
		m_pCopyArm->Set_ArmTag(_wszArmTag);

		m_pCopyArm = nullptr;
	}
	else
		return;
}

void CArm_Manager::Edit_ArmInfo(_wstring _wszArmTag)
{
}

//_float3 CArm_Manager::Get_ArmRotation()
//{
//	return _float3();
//}
//

_float CArm_Manager::Get_ArmLength(_bool _isCopyArm)
{
	if (true == _isCopyArm) {
		if (nullptr != m_pCopyArm)
			return m_pCopyArm->Get_Length();
	}
	else {
		if (nullptr != m_pCurrentArm)
			return m_pCurrentArm->Get_Length();
	}
		
}

void CArm_Manager::Set_ArmRotation(_vector _vRotation, _bool _isCopyArm)
{
	if (true == _isCopyArm) {
		if(nullptr != m_pCopyArm)
			m_pCopyArm->Set_Rotation(_vRotation);
	}
	else {
		if(nullptr != m_pCurrentArm)
			m_pCurrentArm->Set_Rotation(_vRotation);
	}
}

void CArm_Manager::Set_ArmLength(_float _fLength, _bool _isCopyArm)
{
	if (true == _isCopyArm) {
		if (nullptr != m_pCopyArm)
			m_pCopyArm->Set_Length(_fLength);
	}
	else {
		if (nullptr != m_pCurrentArm)
			m_pCurrentArm->Set_Length(_fLength);
	}
		
}

void CArm_Manager::Set_CurrentArm(CCameraArm* _pCameraArm)
{
	if (nullptr == _pCameraArm)
		return;

	m_pCurrentArm = _pCameraArm;
	Safe_AddRef(m_pCurrentArm);

}

CCameraArm* CArm_Manager::Find_Arm(_wstring _wszArmTag)
{
	auto iter = m_Arms.find(_wszArmTag);

	if (iter == m_Arms.end())
		return nullptr;

	return iter->second;
}

void CArm_Manager::Free()
{
	Safe_Release(m_pGameInstance);

	for (auto& Arm : m_Arms)
		Safe_Release(Arm.second);
	m_Arms.clear();

	Safe_Release(m_pCurrentArm);
	Safe_Release(m_pCopyArm);

	__super::Free();
}
