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
	_bool						Set_NextCutScene(_wstring _wszCutSceneName, CUTSCENE_INITIAL_DATA* _pTargetPos = nullptr);
	void						Add_CutScene(_wstring _wszCutSceneTag, pair<_float2, vector<CUTSCENE_DATA>> _CutSceneData);

private:
	map<_wstring, pair<_float2, vector<CUTSCENE_DATA>>>	m_CutSceneDatas;	// Sector -> CUTSCENE_DATA
	pair<_float2, vector<CUTSCENE_DATA>>*				m_pCurCutScene = { nullptr };

	_uint										m_iCurCutSceneIndex = {};

	_bool										m_isStartCutScene = { false };
	_bool										m_isFinishCutScene = { false };

	// Target
	_float3										m_vTargetPos = {};
	CUTSCENE_INITIAL_DATA						m_tInitialData = {};
	_bool										m_isInitialData = { false };
	_float2										m_InitialTime = { 0.3f, 0.f };

	// Finish

private:
	void						Play_CutScene(_float _fTimeDelta);
	void						Change_Sector();

	pair<_float2, vector<CUTSCENE_DATA>>*		Find_CutScene(_wstring _wszCutSceneName);

	void						Before_CutScene(_float _fTimeDelta);
	void						After_CutScene(_float _fTimeDelta);

	void						Initialize_CameraInfo(CUTSCENE_INITIAL_DATA* _pTargetPos);

public:
	static CCamera_CutScene*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*		Clone(void* _pArg);
	virtual void				Free() override;
};

END