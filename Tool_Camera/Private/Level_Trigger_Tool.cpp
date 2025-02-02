#include "stdafx.h"
#include "Level_Trigger_Tool.h"
#include "Level_Loading.h"
#include "GameInstance.h"

#include "Camera_Free.h"
#include "Camera_Manager_Tool.h"

#include "Camera_Trigger.h"

#include "ModelObject.h"
#include "Layer.h"

CLevel_Trigger_Tool::CLevel_Trigger_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_Trigger_Tool::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"), nullptr)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Map(TEXT("Layer_Terrain"))))
		return E_FAIL;

	Ready_DataFiles();
	Initialize_ListBoxName();

	return S_OK;
}

void CLevel_Trigger_Tool::Update(_float _fTimeDelta)
{
	Picking();
	Show_TriggerTool();
	Show_Info();
	Show_SaveLoadFileWindow();
}

HRESULT CLevel_Trigger_Tool::Render()
{
#ifdef _DEBUG
	//SetWindowText(g_hWnd, TEXT("Camera Tool Level"));
	m_pGameInstance->Render_FPS(TEXT("Timer_Default"));
#endif

	return S_OK;
}

HRESULT CLevel_Trigger_Tool::Ready_Lights()
{
	LIGHT_DESC LightDesc{};

	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTOINAL;
	LightDesc.vDirection = _float4(-1.f, -1.f, 0.5f, 0.f);
	LightDesc.vDiffuse = _float4(1.0f, 1.0f, 1.0f, 1.f);
	LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Trigger_Tool::Ready_Layer_Camera(const _wstring& _strLayerTag, CGameObject* _pTarget)
{
	CGameObject* pCamera = nullptr;

	// Free Camera
	CCamera_Free::CAMERA_FREE_DESC Desc{};

	Desc.fMouseSensor = 0.1f;

	Desc.fFovy = XMConvertToRadians(30.f);
	Desc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	Desc.vEye = _float3(0.f, 10.f, -7.f);
	Desc.vAt = _float3(0.f, 0.f, 0.f);
	Desc.eZoomLevel = CCamera::LEVEL_6;
	Desc.iCameraType = CCamera_Manager_Tool::FREE;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_TRIGGER_TOOL, TEXT("Prototype_GameObject_Camera_Free"),
		LEVEL_TRIGGER_TOOL, _strLayerTag, &pCamera, &Desc)))
		return E_FAIL;

	CCamera_Manager_Tool::GetInstance()->Add_Camera(CCamera_Manager_Tool::FREE, dynamic_cast<CCamera*>(pCamera));
	//CCamera_Manager_Tool::GetInstance()->Change_CameraType(CCamera_Manager_Tool::FREE);

	return S_OK;
}

HRESULT CLevel_Trigger_Tool::Ready_Layer_Player(const _wstring& _strLayerTag, CGameObject** _ppOut)
{
	return S_OK;
}

HRESULT CLevel_Trigger_Tool::Ready_Layer_Map(const _wstring& _strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Trigger_Tool::Ready_DataFiles()
{
	std::filesystem::path path;
	path = "../Bin/Resources/DataFiles/TriggerData/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
		if (entry.path().extension() == ".json") {
			m_JsonFilePaths.push_back(entry.path().string());
		}
	}

	return S_OK;
}

void CLevel_Trigger_Tool::Show_TriggerTool()
{
	ImGui::Begin("Trigger Tool");

	Set_TriggerBasicInfo();
	Set_TriggerInfoByType();

	ImGui::NewLine();
	ImGui::Checkbox("Create Trigger", &m_isCreate);
	ImGui::SameLine();
	ImGui::Checkbox("Edit Trigger", &m_isEdit);

	Set_CurTrigger();
	Delete_Trigger();

	if (true == m_isCreate && false == m_isEdit) {
		Create_Trigger();
	
	}

	if (false == m_isCreate && true == m_isEdit) {
		Edit_Trigger();
	}

	ImGui::End();
}

void CLevel_Trigger_Tool::Show_Info()
{
	ImGui::Begin("Cur Trigger Info");

	Show_CurTriggerInfo();
	
	ImGui::End();
}

void CLevel_Trigger_Tool::Show_CurTriggerInfo()
{
	ImGui::NewLine();
	ImGui::Dummy(ImVec2((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Centered Text").x) * 0.5f, 0.0f));
	ImGui::SameLine();
	ImGui::Text("Cur Trigger Info");
	ImGui::Separator();

	if (nullptr == m_pCurTrigger)
		return;

	_string TriggerName = m_pGameInstance->WStringToString(m_TriggerTags[m_pCurTrigger->second->Get_TriggerType()]);
	ImGui::Text("%s ", TriggerName.c_str());

	_float3 vPosition = m_pCurTrigger->second->Get_ActorCom()->Get_GlobalPose();
	ImGui::Text("Position: %.2f, %.2f, %.2f", vPosition.x, vPosition.y, vPosition.z);

	const vector<PxShape*>& Shapes = m_pCurTrigger->second->Get_ActorCom()->Get_Shapes();
	PxTransform ShapeTransform = Shapes[0]->getLocalPose();
	ShapeTransform.q;
	_float3 vRotation = Quaternion_ToEuler({ ShapeTransform.q.x, ShapeTransform.q.y, ShapeTransform.q.z, ShapeTransform.q.w });
	
	ImGui::Text("Rotation: %.2f, %.2f, %.2f", XMConvertToDegrees(vRotation.x), XMConvertToDegrees(vRotation.y), XMConvertToDegrees(vRotation.z));
	ImGui::SameLine();

	// Shape Info
	ImGui::NewLine();
	ImGui::Text("Shape Tag:");
	ImGui::SameLine();
	_string Name = m_pGameInstance->WStringToString(m_ShapeTags[(_uint)m_pCurTrigger->first.iShapeType]);
	ImGui::Text("%s               ", Name.c_str());

	switch ((_uint)m_pCurTrigger->first.iShapeType) {
	case (_uint)SHAPE_TYPE::BOX:
		ImGui::Text("HalfExtents: %.2f, %.2f, %.2f", m_pCurTrigger->first.vHalfExtents.x, m_pCurTrigger->first.vHalfExtents.y, m_pCurTrigger->first.vHalfExtents.z);
		break;
	case (_uint)SHAPE_TYPE::SPHERE:
		ImGui::Text("Radius: %.2f                 ", m_pCurTrigger->first.fRadius);
		break;
	}

	ImGui::Text("My Object Group:");
	ImGui::SameLine();
	for (auto& ObjectGroupTag : m_ObjectGroupTags) {
		if (ObjectGroupTag.first == (m_pCurTrigger->first.iFillterMyGroup & ObjectGroupTag.first)) {
			if (0x00 == ObjectGroupTag.first)
				continue;

			_string Name = m_pGameInstance->WStringToString(ObjectGroupTag.second);
			ImGui::Text("%s", Name.c_str());
		}
	}

	// Fitter들은 넣을 때 frist로 넣어야 함
	ImGui::Text("Other Group:");
	for (auto& ObjectGroupTag : m_ObjectGroupTags) {
		if (ObjectGroupTag.first == (m_pCurTrigger->first.iFillterOtherGroupMask & ObjectGroupTag.first)) {
			if (0x00 == ObjectGroupTag.first)
				continue;

			_string Name = m_pGameInstance->WStringToString(ObjectGroupTag.second);
			ImGui::Text("%s | ", Name.c_str());
			ImGui::SameLine();
		}
	}

	switch (m_pCurTrigger->second->Get_TriggerType()) {
	case CAMERA_TRIGGER:
	{
		ImGui::Text("Camera Trigger Tag");
		ImGui::SameLine();
		_string Name = m_pGameInstance->WStringToString(m_CameraTriggerTags[dynamic_cast<CCamera_Trigger*>(m_pCurTrigger->second)->Get_CameraTriggerType()]);
		ImGui::Text("%s     ", Name.c_str());

		ImGui::Text("Event Tag");
		ImGui::SameLine();
		_string EventTag = m_pGameInstance->WStringToString(dynamic_cast<CCamera_Trigger*>(m_pCurTrigger->second)->Get_CameraTriggerEventTag());
		ImGui::Text("%s     ", EventTag.c_str());

		// Exit Return
		ImGui::Text("Exit Enable Return Type:");
		for (auto& ReturnTag : m_ExitReturnTags) {
			if (ReturnTag.first == (dynamic_cast<CCamera_Trigger*>(m_pCurTrigger->second)->Get_ReturnMask() & ReturnTag.first)) {
				if (0x00 == ReturnTag.first)
					continue;

				_string Name = m_pGameInstance->WStringToString(ReturnTag.second);
				ImGui::Text("%s | ", Name.c_str());
				ImGui::SameLine();
			}
		}
	}
		break;
	}
}

void CLevel_Trigger_Tool::Show_SaveLoadFileWindow()
{
	ImGui::Begin("Save Load File");

	ImGui::BeginChild("Files", ImVec2(400, 150), true);  // true는 border 표시
	for (int i = 0; i < m_JsonFilePaths.size(); ++i)
	{
		if (ImGui::Selectable(m_JsonFilePaths[i].c_str(), m_iCurrentJsonFileIndex == i))
		{
			m_iCurrentJsonFileIndex = i;
		}
	}
	ImGui::EndChild();

	if (ImGui::Button("Save")) {
		ImGui::OpenPopup("Save File");
		m_isShowPopUp = true;
	}

	if (ImGui::BeginPopupModal("Save File", &m_isShowPopUp)) {
		ImGui::Text("Enter File Name");

		if (ImGui::InputText("File Name", m_szSaveName, sizeof(_char) * MAX_PATH,
			ImGuiInputTextFlags_EnterReturnsTrue)) {
			ImGui::CloseCurrentPopup();
			m_isShowPopUp = false;
		}

		if (ImGui::Button("Save")) {
			ImGui::CloseCurrentPopup();
			m_isShowPopUp = false;

			Save_TriggerData();
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel")) {
			ImGui::CloseCurrentPopup();
			m_isShowPopUp = false;
		}

		ImGui::EndPopup();
	}

	ImGui::SameLine();

	if (ImGui::Button("Load")) {
		Load_TriggerData();
	}

	ImGui::End();
}

void CLevel_Trigger_Tool::Show_TriggerTypeListBox()
{
	ImGui::SameLine();
	ImGui::SetNextItemWidth(120.0f);

	if (m_TriggerTags.size() <= 0)
		return;

	_string Name = m_pGameInstance->WStringToString(m_TriggerTags[m_iTriggerType]);

	if (ImGui::BeginCombo("##TriggerTypeLabel", Name.c_str())) {
		for (_int i = 0; i < m_TriggerTags.size(); ++i) {
			_bool bSelected = (m_iTriggerType == i);

			if (ImGui::Selectable(m_pGameInstance->WStringToString(m_TriggerTags[i]).c_str(), bSelected))
				m_iTriggerType = i;

			if (bSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}
}

void CLevel_Trigger_Tool::Show_ShapeTypeListBox()
{
	ImGui::SameLine();
	ImGui::SetNextItemWidth(120.0f);

	if (m_ShapeTags.size() <= 0)
		return;

	_string Name = m_pGameInstance->WStringToString(m_ShapeTags[(_uint)m_eShapeType]);

	if (ImGui::BeginCombo("##ShapeTypeLabel", Name.c_str())) {
		for (_int i = 0; i < m_ShapeTags.size(); ++i) {
			_bool bSelected = ((_uint)m_eShapeType == i);

			if (ImGui::Selectable(m_pGameInstance->WStringToString(m_ShapeTags[i]).c_str(), bSelected))
				m_eShapeType = (SHAPE_TYPE)i;

			if (bSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}
}

void CLevel_Trigger_Tool::Show_MyObjectGroup()
{
	ImGui::SameLine();
	ImGui::SetNextItemWidth(120.0f);

	if (m_ObjectGroupTags.size() <= 0)
		return;

	_string Name = m_pGameInstance->WStringToString(m_ObjectGroupTags[m_iFillterMyGroup].second);

	if (ImGui::BeginCombo("##ObjectGroupLabel", Name.c_str())) {
		for (_int i = 0; i < m_ObjectGroupTags.size(); ++i) {
			_bool bSelected = ((_uint)m_iFillterMyGroup == i);

			if (ImGui::Selectable(m_pGameInstance->WStringToString(m_ObjectGroupTags[i].second).c_str(), bSelected))
				m_iFillterMyGroup = i;

			if (bSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}
}

void CLevel_Trigger_Tool::Show_OtherGroup()
{
	//ImGui::SameLine();
	ImGui::NewLine();
	ImGui::SetNextItemWidth(120.0f);

	if (m_ObjectGroupTags.size() <= 0)
		return;

	_string Name = m_pGameInstance->WStringToString(m_ObjectGroupTags[m_iFillterOtherGroup].second);

	if (ImGui::BeginCombo("##ObjectOtherGroupLabel", Name.c_str())) {
		for (_int i = 0; i < m_ObjectGroupTags.size(); ++i) {
			_bool bSelected = (m_iFillterOtherGroup == i);

			if (ImGui::Selectable(m_pGameInstance->WStringToString(m_ObjectGroupTags[i].second).c_str(), bSelected)) {
				m_iFillterOtherGroup = i;

				m_iTotalOtherGroupMask |= m_ObjectGroupTags[m_iFillterOtherGroup].first;
			}

			if (bSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}
}

void CLevel_Trigger_Tool::Show_CameraTriggerListBox()
{
	ImGui::SameLine();
	ImGui::SetNextItemWidth(120.0f);

	if (m_CameraTriggerTags.size() <= 0)
		return;

	_string Name = m_pGameInstance->WStringToString(m_CameraTriggerTags[m_iCameraTriggerType]);

	if (ImGui::BeginCombo("##CameraTriggerTag", Name.c_str())) {
		for (_int i = 0; i < m_CameraTriggerTags.size(); ++i) {
			_bool bSelected = (m_iCameraTriggerType == i);

			if (ImGui::Selectable(m_pGameInstance->WStringToString(m_CameraTriggerTags[i]).c_str(), bSelected))
				m_iCameraTriggerType = i;

			if (bSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}
}

void CLevel_Trigger_Tool::Show_ExitReturnMaskListBox()
{
	ImGui::NewLine();
	ImGui::SetNextItemWidth(120.0f);

	if (m_ExitReturnTags.size() <= 0)
		return;

	_string Name = m_pGameInstance->WStringToString(m_ExitReturnTags[m_iExitReturnIndex].second);

	if (ImGui::BeginCombo("##ExitReturn", Name.c_str())) {
		for (_int i = 0; i < m_ExitReturnTags.size(); ++i) {
			_bool bSelected = (m_iExitReturnIndex == i);

			if (ImGui::Selectable(m_pGameInstance->WStringToString(m_ExitReturnTags[i].second).c_str(), bSelected)) {
				m_iExitReturnIndex = i;

				m_iExitReturnMask |= m_ExitReturnTags[m_iExitReturnIndex].first;
			}

			if (bSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}
}

void CLevel_Trigger_Tool::Set_TriggerBasicInfo()
{
	ImGui::NewLine();
	ImGui::Dummy(ImVec2((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Centered Text").x) * 0.5f, 0.0f));
	ImGui::SameLine();
	ImGui::Text("Trigger Info");
	ImGui::Separator();

	ImGui::Text("Trigger Tag:");
	if (m_iTriggerType <= m_TriggerTags.size() - 1) {
		ImGui::SameLine();
		_string Name = m_pGameInstance->WStringToString(m_TriggerTags[m_iTriggerType]);
		ImGui::Text("%s ", Name.c_str());
	}
	Show_TriggerTypeListBox();

	ImGui::Text("Position: %.2f, %.2f, %.2f", m_vPosition.x, m_vPosition.y, m_vPosition.z);
	ImGui::SameLine();

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##X", &m_vPosition.x, 0.1f);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##Y", &m_vPosition.y, 0.1f);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##Z", &m_vPosition.z, 0.1f);

	ImGui::Text("Rotation: %.2f, %.2f, %.2f", m_vRotation.x, m_vRotation.y, m_vRotation.z);
	ImGui::SameLine();

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##RoataionX", &m_vRotation.x, 0.1f);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##RoataionY", &m_vRotation.y, 0.1f);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##RoataionZ", &m_vRotation.z, 0.1f);

	// Shape Info
	ImGui::NewLine();
	ImGui::Dummy(ImVec2((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Centered Text").x) * 0.5f, 0.0f));
	ImGui::SameLine();
	ImGui::Text("Shape Info");
	ImGui::Separator();

	ImGui::Text("Shape Tag:");
	if ((_uint)m_eShapeType <= m_ShapeTags.size() - 1) {
		ImGui::SameLine();
		_string Name = m_pGameInstance->WStringToString(m_ShapeTags[(_uint)m_eShapeType]);
		ImGui::Text("%s               ", Name.c_str());
	}
	Show_ShapeTypeListBox();

	switch ((_uint)m_eShapeType) {
	case (_uint)SHAPE_TYPE::BOX:
		ImGui::Text("HalfExtents: %.2f, %.2f, %.2f", m_vHalfExtents.x, m_vHalfExtents.y, m_vHalfExtents.z);
		ImGui::SameLine();

		ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
		ImGui::DragFloat("##HalfExtentX", &m_vHalfExtents.x, 0.1f, 0.5f, 20.f);
		ImGui::SameLine(0, 10.0f);

		ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
		ImGui::DragFloat("##HalfExtentY", &m_vHalfExtents.y, 0.1f, 0.5f, 20.f);
		ImGui::SameLine(0, 10.0f);

		ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
		ImGui::DragFloat("##HalfExtentZ", &m_vHalfExtents.z, 0.1f, 0.5f, 20.f);
		break;
	case (_uint)SHAPE_TYPE::SPHERE:
		ImGui::Text("Radius: %.2f                 ", m_fRadius);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(120.0f);
		ImGui::DragFloat("##Radius", &m_fRadius, 0.1f, 0.5f, 20.f);
		break;
	}

	// Fitter들은 넣을 때 frist로 넣어야 함
	ImGui::Text("My Object Group:");
	if (m_iFillterMyGroup <= m_ObjectGroupTags.size() - 1) {
		ImGui::SameLine();
		_string Name = m_pGameInstance->WStringToString(m_ObjectGroupTags[m_iFillterMyGroup].second);
		ImGui::Text("%s        ", Name.c_str());
	}
	Show_MyObjectGroup();

	// Fitter들은 넣을 때 frist로 넣어야 함
	ImGui::Text("Other Group:");
	if (m_iFillterOtherGroup <= m_ObjectGroupTags.size() - 1) {
		ImGui::SameLine();

		for (auto& ObjectGroupTag : m_ObjectGroupTags) {
			if (ObjectGroupTag.first == (m_iTotalOtherGroupMask & ObjectGroupTag.first)) {
				if (0x00 == ObjectGroupTag.first)
					continue;

				_string Name = m_pGameInstance->WStringToString(ObjectGroupTag.second);
				ImGui::Text("%s || ", Name.c_str());
				ImGui::SameLine();
			}
		}
	}

	Show_OtherGroup();

	ImGui::SameLine();
	if (ImGui::Button("Clear Total OtherGroup"))
		m_iTotalOtherGroupMask &= OBJECT_GROUP::NONE;
}

void CLevel_Trigger_Tool::Set_TriggerInfoByType()
{
	ImGui::NewLine();
	ImGui::Dummy(ImVec2((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Centered Text").x) * 0.5f, 0.0f));
	ImGui::SameLine();
	switch (m_iTriggerType) {
	case TRIGGER_TYPE::CAMERA_TRIGGER:
		ImGui::Text("Camera Trigger Info");
		break;
	}
	ImGui::Separator();

	// Set Info
	switch (m_iTriggerType) {
	case TRIGGER_TYPE::CAMERA_TRIGGER:
	{
		// CameraTrigger Type
		ImGui::Text("Camera Trigger Tag");
		if (m_iCameraTriggerType <= m_CameraTriggerTags.size() - 1) {
			ImGui::SameLine();
			_string Name = m_pGameInstance->WStringToString(m_CameraTriggerTags[m_iCameraTriggerType]);
			ImGui::Text("%s     ", Name.c_str());
		}
		Show_CameraTriggerListBox();


		// EventTag
		ImGui::Text("Event Tag Input:  %s", m_szEventTag);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(-1);
		ImGui::InputText("##EventTag", m_szEventTemp, MAX_PATH);

		if (ImGui::Button("Set Tag")) {
			strcpy_s(m_szEventTag, m_szEventTemp);
		}

		// Exit Return Mask
		ImGui::Text("Exit Return Type:");
		if (m_iExitReturnIndex <= m_ExitReturnTags.size() - 1) {
			ImGui::SameLine();

			for (auto& ReturnTag : m_ExitReturnTags) {
				if (ReturnTag.first == (m_iExitReturnMask & ReturnTag.first)) {
					if (0x00 == ReturnTag.first)
						continue;

					_string Name = m_pGameInstance->WStringToString(ReturnTag.second);
					ImGui::Text("%s || ", Name.c_str());
					ImGui::SameLine();
				}
			}
		}

		Show_ExitReturnMaskListBox();

		ImGui::SameLine();
		if (ImGui::Button("Clear Exit Return Type"))
			m_iExitReturnMask &= EXIT_RETURN_MASK::NONE;
	}
		break;
	}

}

void CLevel_Trigger_Tool::Create_Trigger()
{
	if (KEY_PRESSING(KEY::CTRL)) {
		if (MOUSE_DOWN(MOUSE_KEY::LB)) {

			switch (m_iTriggerType) {
			case CAMERA_TRIGGER:
				Create_Camera_Trigger();
				break;
			}

		}
	}
}

void CLevel_Trigger_Tool::Delete_Trigger()
{
	if (KEY_PRESSING(KEY::LSHIFT)) {
		if (MOUSE_DOWN(MOUSE_KEY::LB)) {

			pair<TRIGGEROBJECT_DATA, CTriggerObject*>* pSelectedTrigger = Get_SelectedTrigger();

			if (nullptr == pSelectedTrigger)
				return;
			
			for (auto iterator = m_Triggers.begin(); iterator != m_Triggers.end();) {
				if (pSelectedTrigger == &(*iterator)) {

					if (pSelectedTrigger == m_pCurTrigger) {
						Safe_Release(m_pCurTrigger->second);
						m_pCurTrigger = nullptr;
					}

					Safe_Release((*iterator).second);
					(*iterator).second->Set_Dead();
					iterator = m_Triggers.erase(iterator);

					return;
				}
				else
					iterator++;
			}
		}
	}
}

void CLevel_Trigger_Tool::Edit_Trigger()
{
	if (ImGui::Button("Set Cur Trigger Info")) {

		if (nullptr == m_pCurTrigger)
			return;

		m_iTriggerType = m_pCurTrigger->second->Get_TriggerType();
		m_vPosition = m_pCurTrigger->second->Get_ActorCom()->Get_GlobalPose();
		
		const vector<PxShape*>& Shapes = m_pCurTrigger->second->Get_ActorCom()->Get_Shapes();
		PxTransform ShapeTransform = Shapes[0]->getLocalPose();
		ShapeTransform.q;
		_float3 vRotation = Quaternion_ToEuler({ ShapeTransform.q.x, ShapeTransform.q.y, ShapeTransform.q.z, ShapeTransform.q.w });
		
		m_vRotation = { XMConvertToDegrees(vRotation.x), XMConvertToDegrees(vRotation.y),XMConvertToDegrees(vRotation.z) }; // 흠

		m_eShapeType = (SHAPE_TYPE)m_pCurTrigger->first.iShapeType;
		m_vHalfExtents = m_pCurTrigger->first.vHalfExtents;
		m_fRadius = m_pCurTrigger->first.fRadius;

		_uint iCount = 0;
		for (auto& GroupTag : m_ObjectGroupTags) {
			if (GroupTag.first == m_pCurTrigger->first.iFillterMyGroup ) {
				m_iFillterMyGroup = iCount;
			}
			iCount++;
		}

		m_iTotalOtherGroupMask = m_pCurTrigger->first.iFillterOtherGroupMask;

		switch (m_iTriggerType) {
		case CAMERA_TRIGGER:
			m_iCameraTriggerType = dynamic_cast<CCamera_Trigger*>(m_pCurTrigger->second)->Get_CameraTriggerType();

			_string EventTag = m_pGameInstance->WStringToString(dynamic_cast<CCamera_Trigger*>(m_pCurTrigger->second)->Get_CameraTriggerEventTag());
			strcpy_s(m_szEventTag, EventTag.c_str());
			break;
		}
	}
	ImGui::SameLine();

	if (ImGui::Button("Edit Info")) {
		if (nullptr == m_pCurTrigger)
			return;

		m_pCurTrigger->first.iFillterMyGroup = m_ObjectGroupTags[m_iFillterMyGroup].first;
		m_pCurTrigger->first.iFillterOtherGroupMask = m_iTotalOtherGroupMask;

		switch (m_iTriggerType) {
		case CAMERA_TRIGGER:
			dynamic_cast<CCamera_Trigger*>(m_pCurTrigger->second)->Set_CameraTriggerType(m_iCameraTriggerType);
			dynamic_cast<CCamera_Trigger*>(m_pCurTrigger->second)->Set_CameraTriggerEventTag(m_pGameInstance->StringToWString(m_szEventTag));
			break;
		}
	}

	if (KEY_PRESSING(KEY::SPACE)) {
		if (nullptr == m_pCurTrigger)
			return;

		// Position
		m_pCurTrigger->second->Get_ActorCom()->Set_GlobalPose(m_vPosition);

		// Rotation
		_matrix RotationMat = XMMatrixRotationX(XMConvertToRadians(m_vRotation.x)) * XMMatrixRotationY(XMConvertToRadians(m_vRotation.y)) * XMMatrixRotationZ(XMConvertToRadians(m_vRotation.z));
		m_pCurTrigger->second->Get_ActorCom()->Set_ShapeLocalOffsetMatrix(0, RotationMat);

		switch (m_pCurTrigger->first.iShapeType)
		{
		case (_uint)SHAPE_TYPE::BOX: 
		{
			SHAPE_BOX_DESC ShapeBoxDesc = {};
			ShapeBoxDesc.vHalfExtents = m_vHalfExtents;
			m_pCurTrigger->second->Get_ActorCom()->Set_ShapeGeometry(0, PxGeometryType::eBOX, &ShapeBoxDesc);
			m_pCurTrigger->first.vHalfExtents = m_vHalfExtents;
		}
			break;
		case (_uint)SHAPE_TYPE::SPHERE: 
		{
			SHAPE_SPHERE_DESC ShapeSpereDesc = {};
			ShapeSpereDesc.fRadius = m_fRadius;
			m_pCurTrigger->second->Get_ActorCom()->Set_ShapeGeometry(0, PxGeometryType::eSPHERE, &ShapeSpereDesc);
			m_pCurTrigger->first.vHalfExtents = m_vHalfExtents;
			break;
		}
		}

		
	}
}

void CLevel_Trigger_Tool::Set_CurTrigger()
{
	if (KEY_PRESSING(KEY::ALT)) {
		if (MOUSE_DOWN(MOUSE_KEY::LB)) {

			pair<TRIGGEROBJECT_DATA, CTriggerObject*>* pSelectedTrigger = Get_SelectedTrigger();

			if (nullptr == pSelectedTrigger)
				return;

			if (nullptr != m_pCurTrigger)
				Safe_Release(m_pCurTrigger->second);

			m_pCurTrigger = pSelectedTrigger;

			Safe_AddRef(m_pCurTrigger->second);
		}
	}
}

HRESULT CLevel_Trigger_Tool::Create_Camera_Trigger()
{
	CCamera_Trigger::CAMERA_TRIGGER_DESC Desc;

	Desc.iCameraTriggerType = m_iCameraTriggerType;
	Desc.szEventTag = m_pGameInstance->StringToWString(m_szEventTag);
	Desc.iReturnMask = m_iExitReturnMask;

	Desc.eShapeType = m_eShapeType;
	Desc.vHalfExtents = m_vHalfExtents;
	Desc.fRadius = m_fRadius;

	Desc.iFillterMyGroup = m_ObjectGroupTags[m_iFillterMyGroup].first;
	Desc.iFillterOtherGroupMask = m_iTotalOtherGroupMask;

	Desc.tTransform3DDesc.vInitialPosition = m_vPosition;

	CGameObject* pTrigger = nullptr;
	TRIGGEROBJECT_DATA Data = {};

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Trigger"), LEVEL_TRIGGER_TOOL, TEXT("Layer_Trigger"), &pTrigger, &Desc)))
		return E_FAIL;

	// Rotation
	_matrix RotationMat = XMMatrixRotationX(XMConvertToRadians(m_vRotation.x)) * XMMatrixRotationY(XMConvertToRadians(m_vRotation.y)) * XMMatrixRotationZ(XMConvertToRadians(m_vRotation.z));
	dynamic_cast<CTriggerObject*>(pTrigger)->Get_ActorCom()->Set_ShapeLocalOffsetMatrix(0, RotationMat);

	dynamic_cast<CTriggerObject*>(pTrigger)->Set_TriggerType(m_iTriggerType);
	Data.iShapeType = (_uint)m_eShapeType;
	Data.vHalfExtents = m_vHalfExtents;
	Data.fRadius = m_fRadius;
	Data.iFillterMyGroup = m_ObjectGroupTags[m_iFillterMyGroup].first;
	Data.iFillterOtherGroupMask = m_iTotalOtherGroupMask;

	m_Triggers.push_back(make_pair(Data, dynamic_cast<CTriggerObject*>(pTrigger)));
	
	Safe_AddRef(pTrigger);

	return S_OK;
}

void CLevel_Trigger_Tool::Initialize_ListBoxName()
{
	_wstring wszTagName;

	// Trigger Tag
	for (_uint i = 0; i < TRIGGER_TYPE::TRIGGER_TYPE_END; ++i) {
		
		switch (i) {
		case TRIGGER_TYPE::CAMERA_TRIGGER:
			wszTagName = TEXT("CAMERA_TRIGGER");
			break;
		}

		m_TriggerTags.push_back(wszTagName);
	}

	// Shape Tag
	for (_uint i = 0; i < (_uint)SHAPE_TYPE::LAST; ++i) {
		switch (i) {
		case (_uint)SHAPE_TYPE::BOX:
			wszTagName = TEXT("BOX");
			break;
		case (_uint)SHAPE_TYPE::SPHERE:
			wszTagName = TEXT("SPHERE");
			break;
		}

		m_ShapeTags.push_back(wszTagName);
	}

	// ObjectGroup Tag
	for (_uint i = 0; i < 8; ++i) {
		_uint iID = {};

		switch (i) {
		case 0:
			wszTagName = TEXT("NONE");
			iID = 0x00;
			break;
		case 1:
			wszTagName = TEXT("PLAYER");
			iID = 0x02;
			break;
		case 2:
			wszTagName = TEXT("MONSTER");
			iID = 0x04;
			break;
		case 3:
			wszTagName = TEXT("MAPOBJECT");
			iID = 0x08;
			break;
		case 4:
			wszTagName = TEXT("INTERACTION_OBEJCT");
			iID = 0x10;
			break;
		case 5:
			wszTagName = TEXT("PLAYER_PROJECTILE");
			iID = 0x20;
			break;
		case 6:
			wszTagName = TEXT("MONSTER_PROJECTILE");
			iID = 0x40;
			break;
		case 7:
			wszTagName = TEXT("TRIGGER_OBJECT");
			iID = 0x80;
			break;
		}

		m_ObjectGroupTags.push_back(make_pair(iID, wszTagName));
	}

	// Camera Trigger Tag
	for (_uint i = 0; i < CCamera_Trigger::CAMERA_TRIGGER_TYPE_END; ++i) {
		switch (i) {
		case CCamera_Trigger::ARM_TRIGGER:
			wszTagName = TEXT("ARM_TRIGGER");
			break;
		case CCamera_Trigger::CUTSCENE_TRIGGER:
			wszTagName = TEXT("CUTSCENE_TRIGGER");
			break;
		case CCamera_Trigger::FREEZE_X:
			wszTagName = TEXT("FREEZE_X");
			break;
		case CCamera_Trigger::FREEZE_Z:
			wszTagName = TEXT("FREEZE_Z");
			break;
		}

		m_CameraTriggerTags.push_back(wszTagName);
	}

	for (_int i = 0; i < 5; ++i) {
		_uint iID = {};

		switch (i) {
		case 0:
			wszTagName = TEXT("NONE");
			iID = 0x00;
			break;
		case 1:
			wszTagName = TEXT("RIGHT");
			iID = 0x01;
			break;
		case 2:
			wszTagName = TEXT("LEFT");
			iID = 0x02;
			break;
		case 3:
			wszTagName = TEXT("UP");
			iID = 0x04;
			break;
		case 4:
			wszTagName = TEXT("DOWN");
			iID = 0x08;
			break;
		}

		m_ExitReturnTags.push_back(make_pair(iID, wszTagName));
	}
}

void CLevel_Trigger_Tool::Picking()
{
	if (KEY_PRESSING(KEY::CTRL)) {
		if (MOUSE_DOWN(MOUSE_KEY::LB)) {
			_vector vRayPos, vRayDir;

			Get_RayInfo(&vRayPos, &vRayDir);

			_float2 fMousePos = m_pGameInstance->Get_CursorPos();
			_float fDist = { 99999999.f };
			_vector vClickedPos = {};

			if (0 > fMousePos.x || g_iWinSizeX < fMousePos.x || 0 > fMousePos.y || g_iWinSizeY < fMousePos.y)
				return;

			CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_TRIGGER_TOOL, TEXT("Layer_MapObject"));
			list<CGameObject*> Objects = pLayer->Get_GameObjects();

			CGameObject* pGameObject = nullptr;

			for (auto& GameObject : Objects) {
				CModelObject* pModelObject = dynamic_cast<CModelObject*>(GameObject);
				_float fTemp = {};
				_bool isPicked = pModelObject->Is_PickingCursor_Model_Test(fMousePos, fTemp);

				if (true == isPicked) {
					if (fDist > fTemp) {
						fDist = fTemp;
						vClickedPos = vRayPos + (vRayDir * fDist);
					}
					//XMStoreFloat3(&m_vPosition, vClickedPos);
					//return;
				}
			}

			pLayer = m_pGameInstance->Find_Layer(LEVEL_TRIGGER_TOOL, TEXT("Layer_Room_Environment"));

			if (nullptr == pLayer)
				return;

			Objects = pLayer->Get_GameObjects();

			_float fOtherDist = { 99999999.f };
			_vector vOtherClickedPos = {};

			pGameObject = nullptr;

			for (auto& GameObject : Objects) {
				CModelObject* pModelObject = dynamic_cast<CModelObject*>(GameObject);
				_float fOtherTemp = {};
				_bool isPicked = pModelObject->Is_PickingCursor_Model_Test(fMousePos, fOtherTemp);

				if (true == isPicked) {
					if (fOtherDist > fOtherTemp) {
						fOtherDist = fOtherTemp;
						vOtherClickedPos = vRayPos + (vRayDir * fOtherDist);
					}
					//XMStoreFloat3(&m_vPosition, vOtherClickedPos);
					//return;
				}
			}

			if (fOtherDist > fDist) {
				XMStoreFloat3(&m_vPosition, vClickedPos);
			}
			else
				XMStoreFloat3(&m_vPosition, vOtherClickedPos);

		}
	}
}

void CLevel_Trigger_Tool::Get_RayInfo(_vector* _pRayPos, _vector* _pRayDir)
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	_vector vMousePos = XMVectorSet((_float)pt.x, (_float)pt.y, 0.f, 1.f);

	_uint		iNumViewports = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	// 마우스 -> 투영
	vMousePos = XMVectorSet(pt.x / (ViewportDesc.Width * 0.5f) - 1.f,
		pt.y / -(ViewportDesc.Height * 0.5f) + 1.f,
		0.f,
		1.f);

	// 투영 -> 뷰 스페이스
	_matrix matProj = m_pGameInstance->Get_TransformInverseMatrix(CPipeLine::D3DTS_PROJ);
	vMousePos = XMVector3TransformCoord(vMousePos, matProj);

	_vector vRayPos, vRayDir;

	vRayPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	vRayDir = vMousePos - vRayPos;

	_matrix matView = m_pGameInstance->Get_TransformInverseMatrix(CPipeLine::D3DTS_VIEW);
	vRayPos = XMVector3TransformCoord(vRayPos, matView);
	vRayDir = XMVector3TransformNormal(vRayDir, matView);
	vRayDir = XMVectorSetW(vRayDir, 0.f);
	vRayDir = XMVector3Normalize(vRayDir);

	*_pRayPos = vRayPos;
	*_pRayDir = vRayDir;
}

pair<TRIGGEROBJECT_DATA, CTriggerObject*>* CLevel_Trigger_Tool::Get_SelectedTrigger()
{
	_vector vPos, vDir;
	Get_RayInfo(&vPos, &vDir);

	_float3 vRayPos, vRayDir;
	XMStoreFloat3(&vRayPos, vPos);
	XMStoreFloat3(&vRayDir, vDir);

	CActorObject* pObject = nullptr;

	_bool isSelected = m_pGameInstance->RayCast_Nearest(vRayPos, vRayDir, 2000.f, nullptr, &pObject);

	if (true == isSelected) {
		for (auto& Trigger : m_Triggers) {
			if (pObject == Trigger.second) {
				
				return &Trigger;
			}
		}
	}

	return nullptr;
}

void CLevel_Trigger_Tool::Save_TriggerData()
{
	json Result = json::array();

	for (auto& Trigger : m_Triggers) {

		_float3 vPosition = Trigger.second->Get_ActorCom()->Get_GlobalPose();
		
		json Trigger_json;
		Trigger_json["Trigger Type"] = Trigger.second->Get_TriggerType();
		Trigger_json["Position"] = { vPosition.x, vPosition.y, vPosition.z };
		
		const vector<PxShape*>& Shapes = Trigger.second->Get_ActorCom()->Get_Shapes();
		PxTransform ShapeTransform = Shapes[0]->getLocalPose();
		ShapeTransform.q;
		_float3 vRotation = Quaternion_ToEuler({ ShapeTransform.q.x, ShapeTransform.q.y, ShapeTransform.q.z, ShapeTransform.q.w });
		Trigger_json["Rotation"] = { XMConvertToDegrees(vRotation.x), XMConvertToDegrees(vRotation.y), XMConvertToDegrees(vRotation.z) };
		
		Trigger_json["Shape Type"] = (_uint)Trigger.first.iShapeType;
		Trigger_json["Half Extents"] = { Trigger.first.vHalfExtents.x, Trigger.first.vHalfExtents.y, Trigger.first.vHalfExtents.z };
		Trigger_json["Radius"] = Trigger.first.fRadius;

		Trigger_json["Fillter My Group"] = Trigger.first.iFillterMyGroup;
		Trigger_json["Fillter Other Group Mask"] = Trigger.first.iFillterOtherGroupMask;

		_uint iTriggerType = Trigger.second->Get_TriggerType();
		switch (iTriggerType) {
		case CAMERA_TRIGGER:
			Trigger_json["Camera Trigger Type"] = dynamic_cast<CCamera_Trigger*>(Trigger.second)->Get_CameraTriggerType();

			_string szEventTag = m_pGameInstance->WStringToString(dynamic_cast<CCamera_Trigger*>(Trigger.second)->Get_CameraTriggerEventTag());
			Trigger_json["Camera Trigger Event Tag"] = szEventTag;

			_uint iReturnMask = dynamic_cast<CCamera_Trigger*>(Trigger.second)->Get_ReturnMask();
			Trigger_json["Exit Return Mask"] = iReturnMask;
			break;
		}

		Result.push_back(Trigger_json);
	}

	_wstring wszSavePath = L"../Bin/Resources/DataFiles/TriggerData/";
	_wstring wszSaveName = m_pGameInstance->StringToWString(m_szSaveName);

	ofstream file(wszSavePath + wszSaveName + TEXT(".json"));

	if (!file.is_open())
	{
		MSG_BOX("파일을 저장할 수 없습니다.");
		file.close();
		return;
	}

	file << Result.dump(4);
	file.close();

	_wstring FullPath = wszSavePath + wszSaveName + TEXT(".json");
	m_JsonFilePaths.push_back(m_pGameInstance->WStringToString(FullPath));
}

void CLevel_Trigger_Tool::Load_TriggerData()
{
	//_wstring wszSavePath = L"../Bin/Resources/DataFiles/TriggerData/";
	_wstring wszSaveName = m_pGameInstance->StringToWString(m_JsonFilePaths[m_iCurrentJsonFileIndex]);

	ifstream file(wszSaveName);

	if (!file.is_open())
	{
		MSG_BOX("파일을 열 수 없습니다.");
		file.close();
		return;
	}

	json Result;
	file >> Result;
	file.close();

	for (auto& Trigger_json : Result) {
		TRIGGEROBJECT_DATA Data = {};

		_uint iTriggerType = Trigger_json["Trigger Type"];
		_float3 vPosition = { Trigger_json["Position"][0].get<_float>(),  Trigger_json["Position"][1].get<_float>() , Trigger_json["Position"][2].get<_float>() };
		_float3 vRotation = { Trigger_json["Rotation"][0].get<_float>(),  Trigger_json["Rotation"][1].get<_float>() , Trigger_json["Rotation"][2].get<_float>() };

		Data.iShapeType = Trigger_json["Shape Type"];
		Data.vHalfExtents = { Trigger_json["Half Extents"][0].get<_float>(),  Trigger_json["Half Extents"][1].get<_float>() , Trigger_json["Half Extents"][2].get<_float>() };
		Data.fRadius = Trigger_json["Radius"];

		Data.iFillterMyGroup = Trigger_json["Fillter My Group"];
		Data.iFillterOtherGroupMask = Trigger_json["Fillter Other Group Mask"];

		switch (iTriggerType) {
		case CAMERA_TRIGGER:
		{
			_uint iCameraTriggerType = Trigger_json["Camera Trigger Type"];
			_string szEventTag = Trigger_json["Camera Trigger Event Tag"];

			_uint iReturnMask = Trigger_json["Exit Return Mask"];

			CCamera_Trigger::CAMERA_TRIGGER_DESC Desc;

			Desc.iCameraTriggerType = iCameraTriggerType;
			Desc.szEventTag = m_pGameInstance->StringToWString(szEventTag);
			Desc.iReturnMask = iReturnMask;

			Desc.eShapeType = (SHAPE_TYPE)Data.iShapeType;
			Desc.vHalfExtents = Data.vHalfExtents;
			Desc.fRadius = Data.fRadius;

			Desc.iFillterMyGroup = Data.iFillterMyGroup;
			Desc.iFillterOtherGroupMask = Data.iFillterOtherGroupMask;

			Desc.tTransform3DDesc.vInitialPosition = vPosition;

			CGameObject* pTrigger = nullptr;

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Trigger"), LEVEL_TRIGGER_TOOL, TEXT("Layer_Trigger"), &pTrigger, &Desc))) {
				MSG_BOX("Failed To Load Camera Trigger");
				return;
			}
			
			dynamic_cast<CTriggerObject*>(pTrigger)->Set_TriggerType(iTriggerType);
			// Rotation
			_matrix RotationMat = XMMatrixRotationX(XMConvertToRadians(vRotation.x)) * XMMatrixRotationY(XMConvertToRadians(vRotation.y)) * XMMatrixRotationZ(XMConvertToRadians(vRotation.z));
			dynamic_cast<CTriggerObject*>(pTrigger)->Get_ActorCom()->Set_ShapeLocalOffsetMatrix(0, RotationMat);

			m_Triggers.push_back(make_pair(Data, dynamic_cast<CTriggerObject*>(pTrigger)));
			Safe_AddRef(pTrigger);
		}
			break;
		}
	}
}

_float3 CLevel_Trigger_Tool::Quaternion_ToEuler(const _float4 _q)
{
	_float3 vEuler;

	// 변환 공식 적용
	vEuler.x = atan2(2.0f * (_q.w * _q.x + _q.y * _q.z), 1.0f - 2.0f * (_q.x * _q.x + _q.y * _q.y));
	vEuler.y = asin(2.0f * (_q.w * _q.y - _q.z * _q.x));
	vEuler.z = atan2(2.0f * (_q.w * _q.z + _q.x * _q.y), 1.0f - 2.0f * (_q.y * _q.y + _q.z * _q.z));

	return vEuler;
}

CLevel_Trigger_Tool* CLevel_Trigger_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLevel_Trigger_Tool* pInstance = new CLevel_Trigger_Tool(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Trigger_Tool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Trigger_Tool::Free()
{
	for (auto& Trigger : m_Triggers)
		Safe_Release(Trigger.second);

	if(nullptr != m_pCurTrigger)
		Safe_Release(m_pCurTrigger->second);

	__super::Free();
}
