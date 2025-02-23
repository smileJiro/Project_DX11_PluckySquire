//#pragma once
//
//#include "Camera.h"
//
//BEGIN(Engine)
//class CCutScene_Sector;
//END
//
//BEGIN(Client)
//class CCamera_CutScene_Save final : public CCamera
//{
//private:
//	CCamera_CutScene_Save(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
//	CCamera_CutScene_Save(const CCamera_CutScene_Save& _Prototype);
//	virtual ~CCamera_CutScene_Save() = default;
//
//public:
//	virtual HRESULT				Initialize_Prototype() override;
//	virtual HRESULT				Initialize(void* _pArg) override;
//	virtual void				Priority_Update(_float _fTimeDelta) override;
//	virtual void				Update(_float _fTimeDelta) override;
//	virtual void				Late_Update(_float _fTimeDelta) override;
//
//public:
//#ifdef _DEBUG
//	_float3						Get_SimulationPos() { return m_vSimulationPos; }
//	_bool						Get_IsFinish() { return m_isFinishCutScene; }
//	_bool						Get_IsSimulation() { return m_isSimulation; }
//	map<_wstring, pair<_float2, vector<CUTSCENE_DATA>>>* Get_CutSceneDatas() { return &m_CutSceneDatas; }
//
//	void						Set_IsFinish(_bool _isFinish) { m_isFinishCutScene = _isFinish; }
//	void						Set_IsSimulation(_bool _isSimulation) { m_isSimulation = _isSimulation; }
//	void						CutScene_Clear();
//#endif
//
//public:
//	void						Set_NextCutScene(_wstring _wszCutSceneName, INITIAL_DATA* _pTargetPos = nullptr);
//	void						Add_CutScene(_wstring _wszCutSceneTag, vector<CCutScene_Sector*> _vecCutScene);
//
//private:
//	map<_wstring, vector<CCutScene_Sector*>>	m_CutScenes;
//	map<_wstring, pair<_float2, vector<CUTSCENE_DATA>>>		m_CutSceneDatas;
//	vector<CCutScene_Sector*>* m_pCurCutScene = { nullptr };
//	_wstring									m_wszCurCutSceneTag = {};
//
//	_uint										m_iSectorNum = {};
//	_uint										m_iCurSectorIndex = {};
//
//	_bool										m_isStartCutScene = { false };
//	_bool										m_isFinishCutScene = { false };
//
//	// Target
//	_float3										m_vTargetPos = {};
//	//INITIAL_DATA								m_tInitialData = {};
//	//_bool										m_isInitialData = { false };
//	//_float2										m_InitialTime = { 0.3f, 0.f };
//
//#ifdef _DEBUG
//	_float3										m_vSimulationPos = {};
//	_bool										m_isSimulation = { false };
//	_float2										m_fSimulationTime = { 0.5f, 0.f };
//#endif
//
//private:
//	void						Play_CutScene(_float _fTimeDelta);
//	void						Change_Sector();
//
//	vector<CCutScene_Sector*>* Find_CutScene(_wstring _wszCutSceneName);
//	pair < _float2, vector<CUTSCENE_DATA>>* Find_CutSceneData(_wstring _wszCutSceneName);
//
//	void						Before_CutScene(_float _fTimeDelta);
//	void						After_CutScene(_float _fTimeDelta);
//
//	void						Process_Movement(_float _fTimeDelta);
//	void						Initialize_CameraInfo(INITIAL_DATA* _pTargetPos);
//
//	void						Save_Data();
//
//public:
//	static CCamera_CutScene_Save* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
//	virtual CGameObject*		Clone(void* _pArg);
//	virtual void				Free() override;
//};
//END
