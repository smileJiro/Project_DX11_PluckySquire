#pragma once
#include "Level.h"

BEGIN(Engine)
class CGameObject;
class CCutScene_Sector;
END

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
	HRESULT				Ready_CubeMap(const _wstring& _strLayerTag);
	HRESULT				Ready_Layer_Camera(const _wstring& _strLayerTag, CGameObject* _pTarget);
	HRESULT				Ready_Layer_Player(const _wstring& _strLayerTag, CGameObject** _ppOut);
	HRESULT				Ready_Layer_TestTerrain(const _wstring& _strLayerTag);
	HRESULT				Ready_DataFiles();

private:
	_float				m_fRotationValue = { 0.5f };
	_float				m_fLengthValue = { 0.5f };

	_char				m_szCopyArmName[MAX_PATH] = { "" };
	_char				m_szSelectedArmName[MAX_PATH] = { "" };
	vector<_wstring>	m_ArmNames;
	_uint				m_iSelectedArmNum = {};
	_wstring			m_wszSelectedArm = {};

	ARM_DATA			m_tArmData = {};
	SUB_DATA			m_tSubData = {};
	_bool				m_isUseSubData = {};

	_float3				m_vResetArm = {};
	_float				m_fResetLength = {};
	_float3				m_vDesireArm = {};
	_float				m_fDesireLength = {};
	_float3				m_vFirstResetArm = {};

	// Zoom
	_int				m_iZoomLevel = {};
	_float				m_fFovys[10] = {};
	_uint				m_iRatioType = {};
	_float				m_fZoomTime = {};

	// Change AtOffset
	_float3				m_vNextAtOffset = {};
	_float				m_fAtOffsetTime = {};
	_uint				m_iAtOffsetRatioType = {};

	// Shake
	_float				m_fShakeForce = {};
	_float				m_fShakeTime = {};
	_float				m_fShakeCycleTime = { 0.05f };
	_float				m_fShakeDelayTime = { 0.f };
	_uint				m_iShakeType = {};
	_int				m_iShakeCount = {};

	_uint				m_iCycleType = {};

	// ======================== CutScene
	_bool								m_isCreatePoint = { false };
	_bool								m_isEditPoint = { false };
	_bool								m_isCreateSector = { false };
	_bool								m_isEditSector = {};

	list<pair<CUTSCENE_KEYFRAME, CGameObject*>>		m_KeyFrames;
	pair<CUTSCENE_KEYFRAME, CGameObject*>*			m_pCurKeyFrame = { nullptr };

	map<_wstring, vector<CCutScene_Sector*>>		m_CutScenes;
	vector<CUTSCENE_KEYFRAME>				m_SelectedKeyFrame;
	_uint									m_iSectorType = {};
	_float									m_fSectorDuration = {};
	_int									m_iDeleteSectorNum = {};

	// Sector와 CutScene에 들어간 것들을 수정하기 위한 변수
	_int									m_iEditSectorNum = {};
	_int									m_iEditFrameNum = {};

	_char									m_szCutSceneTag[MAX_PATH] = { "" };
	vector<_wstring>						m_CutSceneTags;
	_uint									m_iSelectedCutSceneNum = {};

	// Frame Info
	CUTSCENE_KEYFRAME						m_tKeyFrameInfo = {};

	// Simulation
	CGameObject*							m_pSimulationCube = { nullptr };
	_bool									m_isSimulation = {};

	//
	_bool									m_isMaintainOriginPos = { false };

	// SaveLoad
	_bool									m_isCutSceneData = { false };
	_bool									m_isArmData = { false };
	_bool									m_isForClient = { false };
	_bool									m_isShowPopUp = { false };
	vector<_string>							m_JsonFilePaths;
	_int									m_iCurrentJsonFileIndex = { 0 };
	_char									m_szSaveName[MAX_PATH] = { "" };

	// Initial Data
	INITIAL_DATA							m_tInitialData = {};


private:
	void				Show_CameraTool();
	void				Show_CutSceneTool(_float fTimeDelta);
	void				Show_ArmInfo();
	void				Show_CutSceneInfo();

	void				Create_Arms();
	void				Show_ComboBox();
	void				Show_SelectedArmData();
	void				Show_CameraZoomInfo();

	//CutScene
	void				Show_KeyFrameInfo();
	void				Show_CutSceneComboBox();

	// Save
	void				Show_SaveLoadFileWindow();

private:
	// Tool
	void				Change_ArmLength();
	void				Input_NextArm_Info();
	void				Edit_CopyArm();
	void				Set_MovementInfo();
	void				Set_Zoom();
	void				Set_AtOffsetInfo();
	void				Set_ShakeInfo();
	void				Set_CurInfo();

	// CutScene
	void				Set_KeyFrameInfo();
	void				Set_InitialData();
	void				Create_KeyFrame();
	CGameObject*		Create_Cube();
	void				Edit_KeyFrame();
	void				Delete_KeyFrame();
	void				Set_CurrentKeyFrame();

	void				Create_Sector();
	void				Edit_Sector();
	
	void				Picking();
	void				Get_RayInfo(_vector* _pRayPos, _vector* _pRayDir);

	void				Play_CutScene(_float fTimeDelta);

	// Save
	void				Save_Data_CutScene();
	void				Save_Data_Arm();
	void				Load_Data_CutScene();
	void				Load_Data_Arm();

public:
	static CLevel_Camera_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END