#pragma once
#include "Level.h"

BEGIN(Engine)
class CGameObject;
class CTriggerObject;
END

BEGIN(Camera_Tool)

class CLevel_Trigger_Tool final : public CLevel
{
	enum TRIGGER_TYPE
	{
		ARM_TRIGGER,
		CUTSCENE_TRIGGER,
		FREEZE_X_TRIGGER,
		FREEZE_Y_TRIGGER,
		TELEPORT_TRIGGER,
		EVENT_TRIGGER,
		TRIGGER_TYPE_END
	};

	enum EXIT_RETURN_MASK
	{
		NONE = 0x00,
		RIGHT = 0x01,
		LEFT = 0x02,
		UP = 0x04,
		DOWN = 0x08,
		RETURN_MASK_END
	};

	typedef struct tagTriggerObjectData
	{
		_uint				iShapeType = {};
		_float3				vHalfExtents = {};
		_float				fRadius = {};

		_uint				iFillterMyGroup = {};
		_uint				iFillterOtherGroupMask = {};

		_uint				iTriggerType = {};
		_wstring			szEventTag = {};
	} TRIGGEROBJECT_DATA;

private:
	CLevel_Trigger_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_Trigger_Tool() = default;

public:
	virtual HRESULT		Initialize() override;
	virtual void		Update(_float _fTimeDelta) override;
	virtual HRESULT		Render() override;

public:
	HRESULT				Ready_Lights();
	HRESULT				Ready_Layer_Camera(const _wstring& _strLayerTag, CGameObject* _pTarget);
	HRESULT				Ready_Layer_Player(const _wstring& _strLayerTag, CGameObject** _ppOut);
	HRESULT				Ready_Layer_Map(const _wstring& _strLayerTag);
	HRESULT				Ready_DataFiles();

private:
	list<pair<TRIGGEROBJECT_DATA, CTriggerObject*>>		m_Triggers;
	pair<TRIGGEROBJECT_DATA, CTriggerObject*>*			m_pCurTrigger = { nullptr };

	_float3				m_vPosition = {};
	_float3				m_vRotation = {};

	_float				m_fRadius = { 0.5f };			// Scale 대신 사용
	_float3				m_vHalfExtents = { 0.5f, 0.5f, 0.5f };	// Scale 대신 사용

	SHAPE_TYPE			m_eShapeType = {};

	_uint				m_iFillterMyGroup = {};				// 현재 선택한 곳, 만들 때 m_szObjectGroupTag[m_iFillterMyGroup].first로 넣으면 됨
	_uint				m_iFillterOtherGroup = {};			// 현재 선택한 곳, 만들 때 m_szObjectGroupTag[m_iFillterMyGroup].first로 넣으면 됨
	_uint				m_iTotalOtherGroupMask = {};		// || 연산 후 Total

	_uint				m_iTriggerType = {};
	_char				m_szEventTag[MAX_PATH] = { "" };

	_bool				m_isCreate = { false };
	_bool				m_isEdit = { false };

	// ========== ListBox
	vector<_wstring>	m_TriggerTags;
	vector<_wstring>	m_ShapeTags;
	vector<pair<_uint, _wstring>>	m_ObjectGroupTags;
	vector<pair<_uint, wstring>>	m_ExitReturnTags;

	// ========== Camera Trigger
	_char				m_szEventTemp[MAX_PATH] = { "" };
	_uint				m_iExitReturnMask = {};
	_uint				m_iExitReturnIndex = {};		// List Box 선택한 Index

	// Save Load
	vector<_string>		m_JsonFilePaths;
	_int				m_iCurrentJsonFileIndex = { 0 };
	_char				m_szSaveName[MAX_PATH] = { "" };
	_bool				m_isShowPopUp = { false };

private:
	void				Show_TriggerTool();
	void				Show_Info();
	void				Show_CurTriggerInfo();
	void				Show_SaveLoadFileWindow();

	// ========== ListBox
	void				Show_TriggerTypeListBox();
	void				Show_ShapeTypeListBox();
	void				Show_MyObjectGroup();
	void				Show_OtherGroup();
	void				Show_ExitReturnMaskListBox();			// Camera Trigger

	void				Set_TriggerBasicInfo();
	void				Set_TriggerInfoByType();

	HRESULT				Create_Trigger();
	void				Delete_Trigger();
	void				Edit_Trigger();
	void				Set_CurTrigger();

private:
	void				Initialize_ListBoxName();
	void				Picking();
	void				Get_RayInfo(_vector* _pRayPos, _vector* _pRayDir);
	pair<CLevel_Trigger_Tool::TRIGGEROBJECT_DATA, CTriggerObject*>* Get_SelectedTrigger();

	void				Save_TriggerData();
	void				Load_TriggerData();

private:
	_float3				Quaternion_ToEuler(const _float4 _q);

public:
	static CLevel_Trigger_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void		Free() override;
};
END