#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_CutScene final : public CCamera
{
private:
	CCamera_CutScene(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCamera_CutScene(const CCamera_CutScene& _Prototype);
	virtual ~CCamera_CutScene() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* _pArg) override;
	virtual void				Priority_Update(_float _fTimeDelta) override;
	virtual void				Update(_float _fTimeDelta) override;
	virtual void				Late_Update(_float _fTimeDelta) override;

public:
	_bool						Set_NextCutScene(_wstring _wszCutSceneName);
	void						Add_CutScene(_wstring _wszCutSceneTag, pair<CUTSCENE_SUB_DATA, vector<CUTSCENE_DATA*>> _CutSceneData);
	
	virtual void				Switch_CameraView(INITIAL_DATA* _pInitialData = nullptr) override;
	virtual INITIAL_DATA		Get_InitialData() override;

public:
	void						Set_Pause_After_CutScene(_bool _isPause);
	_bool						Is_Finish_CutScene() { return m_isFinishCutScene; }

private:
	map<_wstring, pair<CUTSCENE_SUB_DATA, vector<CUTSCENE_DATA*>>>	m_CutSceneDatas;	// Sector -> CUTSCENE_DATA
	pair<CUTSCENE_SUB_DATA, vector<CUTSCENE_DATA*>>*		m_pCurCutScene = { nullptr };

	_uint										m_iCurCutSceneIndex = {};

	_bool										m_isStartCutScene = { false };
	_bool										m_isFinishCutScene = { false };

	_float3										m_vAt = {};
	_wstring									m_szCurCutSceneName = {};

	_int										m_iFrameCount = {};

private:
	void						Play_CutScene(_float _fTimeDelta);
	void						Change_Sector();

	pair<CUTSCENE_SUB_DATA, vector<CUTSCENE_DATA*>>*		Find_CutScene(_wstring _wszCutSceneName);

	void						Before_CutScene(_float _fTimeDelta);
	void						After_CutScene(_float _fTimeDelta);

private:
	virtual	void				Switching(_float _fTimeDelta) override;

public:
	static CCamera_CutScene*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*		Clone(void* _pArg);
	virtual void				Free() override;
};

END