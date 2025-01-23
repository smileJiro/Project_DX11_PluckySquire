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

	// CutScene
	_bool								m_isCreatePoint = { false };
	_bool								m_isEditPoint = { false };
	_bool								m_isCreateSector = { false };
	_bool								m_isEditSector = {};

	list<pair<CUTSCENE_KEYFRAME, CGameObject*>>		m_KeyFrames;
	pair<CUTSCENE_KEYFRAME, CGameObject*>*			m_pCurKeyFrame = { nullptr };

	map<_wstring, vector<CCutScene_Sector*>>		m_CutScenes;
	vector<CUTSCENE_KEYFRAME>				m_SelectedKeyFrame;
	_uint									m_iSectorType = {};
	_int									m_iDeleteSectorNum = {};

	// Sector와 CutScene에 들어간 것들을 수정하기 위한 변수
	_int									m_iEditSectorNum = {};
	_int									m_iEditFrameNum = {};

	_char									m_szCutSceneTag[MAX_PATH] = { "" };
	vector<_wstring>						m_CutSceneTags;
	_uint									m_iSelectedCutSceneNum = {};

	// Frame Info
	CUTSCENE_KEYFRAME						m_tKeyFrameInfo = {};

private:
	void				Show_CameraTool();
	void				Show_CutSceneTool();
	void				Show_ArmInfo();
	void				Show_CutSceneInfo();

	void				Create_Arms();
	void				Show_ComboBox();
	void				Show_SelectedArmData();
	void				Show_CameraZoomInfo();

	//CutScene

	void				Show_KeyFrameInfo();
	void				Show_CutSceneComboBox();
private:
	// Tool
	void				Rotate_Arm();
	void				Change_ArmLength();
	void				Input_NextArm_Info();
	void				Edit_CopyArm();
	void				Set_MovementInfo();
	void				Play_Movement();
	void				Reset_CurrentArmPos();
	void				Set_Zoom();
	void				Set_AtOffsetInfo();
	void				Set_ShakeInfo();

	// CutScene
	void				Set_KeyFrameInfo();
	void				Create_KeyFrame();
	CGameObject*		Create_Cube(CUTSCENE_KEYFRAME* _tKeyFrame);
	void				Edit_KeyFrame();
	void				Delete_KeyFrame();
	void				Set_CurrentKeyFrame();

	void				Create_Sector();
	void				Edit_Sector();
	
	void				Picking();
	void				Get_RayInfo(_vector* _pRayPos, _vector* _pRayDir);

private:
	void				Key_Input();

public:
	static CLevel_Camera_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END