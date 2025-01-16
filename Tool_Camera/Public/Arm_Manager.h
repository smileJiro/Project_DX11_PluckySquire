#pragma once

#include "Camera_Tool_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CCameraArm;
END

BEGIN(Camera_Tool)
class CArm_Manager : public CBase
{
	DECLARE_SINGLETON(CArm_Manager)

public:
	typedef struct tagArmDataDesc
	{
		_float3				vArm = { 0.f, 0.f, -1.f };
		_float				fLength = 1.f;
		_float				fTurnTime = {};
		_float				fLengthTime = {};
	} ARM_DATA;

private:
	CArm_Manager();
	virtual ~CArm_Manager() = default;

public:
	HRESULT				Initialize();
	void				Update();
	void				Render();

public:
	void				Copy_Arm();
	void				Add_CopyArm(_wstring _wszArmTag);
	void				Edit_ArmInfo(_wstring _wszArmTag);			// Copy Arm에 넣어서 초록색인 상태로 수정

	//_float3			Get_ArmRotation();
	_float				Get_ArmLength(_bool _isCopyArm);

	void				Set_ArmRotation(_vector _vRotation, _bool _isCopyArm);
	void				Set_ArmLength(_float _fLength, _bool _isCopyArm);
	void				Set_CurrentArm(CCameraArm* _pCameraArm);	// Target Camera에 넣어 주는 애 받아서 함 Tool에서 Copy하기 위한 목적

private:
	CGameInstance*							m_pGameInstance = { nullptr };

private:
	map<_wstring, class CCameraArm*>		m_Arms;
	class CCameraArm*						m_pCurrentArm = { nullptr };
	CCameraArm*								m_pCopyArm = { nullptr };

	map<_wstring, ARM_DATA*>				m_ArmDatas;				// Data 저장

private:
	CCameraArm*			Find_Arm(_wstring _wszArmTag);

public:
	virtual void		Free() override;
};

END