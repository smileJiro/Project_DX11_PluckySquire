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
	_bool				m_isCopyArm = { false };

	_char				m_szCopyArmName[MAX_PATH] = { "" };

private:
	void				Show_CameraTool();
	void				Create_Arms();

private:
	// Tool
	void				Rotate_Arm(_bool _isCopyArm);
	void				Change_ArmLength(_bool _isCopyArm);
	void				Add_CopyArm();

public:
	static CLevel_Camera_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END