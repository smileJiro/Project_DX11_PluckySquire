#pragma once
#include "Level.h"

BEGIN(Camera_Tool)

class CLevel_Camera_Tool final : public CLevel
{
private:
	CLevel_Camera_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_Camera_Tool() = default;

public:
	virtual HRESULT		Initialize() override;
	virtual void		Update(_float _fTimeDelta) override;
	virtual HRESULT		Render() override;

public:
	HRESULT				Ready_Lights();
	HRESULT				Ready_Layer_Camera(const _wstring& _strLayerTag, CGameObject* _pTarget);
	HRESULT				Ready_Layer_Player(const _wstring& _strLayerTag, CGameObject** _ppOut);
	HRESULT				Ready_Layer_TestTerrain(const _wstring& _strLayerTag);

private:
	_float				m_fRotationValue = { 0.5f };
	_float				m_fLengthValue = { 0.5f };

	_char				m_szCopyArmName[MAX_PATH] = { "" };
	_char				m_szSelectedArmName[MAX_PATH] = { "" };
	vector<_wstring>	m_ArmNames;
	_uint				m_iSelectedArmNum = {};
	_wstring			m_wszSelectedArm = {};

	// 
	_float				m_fMoveTimeAxisY = {};
	_float				m_fMoveTimeAxisRight = {};
	_float				m_fLengthTime = {};
	_float2				m_fRotationPerSecAxisY{};
	_float2				m_fRotationPerSecAxisRight{};
	_float				m_fLength = {};
	

	_float3				m_vResetArmPos = {};

private:
	void				Show_CameraTool();
	void				Show_ArmInfo();

	void				Create_Arms();
	void				Show_ComboBox();
	void				Show_SelectedArmData();

private:
	// Tool
	void				Rotate_Arm();
	void				Change_ArmLength();
	void				Input_NextArm_Info();
	void				Edit_CopyArm();
	void				Set_MovementInfo();
	void				Play_Movement();
	void				Reset_CurrentArmPos();

public:
	static CLevel_Camera_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END