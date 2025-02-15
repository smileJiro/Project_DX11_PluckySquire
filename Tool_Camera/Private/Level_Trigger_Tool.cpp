#include "stdafx.h"
#include "Level_Trigger_Tool.h"
#include "Level_Loading.h"
#include "GameInstance.h"

#include "Camera_Free.h"
#include "Camera_Manager_Tool.h"

#include "TriggerObject.h"

#include "ModelObject.h"
#include "Layer.h"

#include "BulbLine.h"
#include "Bulb.h"
#include "Test_Terrain.h"
#include "CubeMap.h"
#include "MainTable.h"

CLevel_Trigger_Tool::CLevel_Trigger_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_Trigger_Tool::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;
	Ready_CubeMap(TEXT("Layer_CubeMap"));
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"), nullptr)))
		return E_FAIL;

	if (FAILED(Ready_Layer_MainTable(TEXT("Layer_MainTable"))))
		return E_FAIL;

	Ready_DataFiles();
	Initialize_ListBoxName();

	m_pCurBulbLine.first = nullptr;
	m_pCurBulbLine.second = nullptr;

	/* Blur RenderGroupOn */
	m_pGameInstance->Set_Active_RenderGroup_New(RENDERGROUP::RG_3D, PR3D_POSTPROCESSING, true);
	return S_OK;
}

void CLevel_Trigger_Tool::Update(_float _fTimeDelta)
{
	
	Picking();
	Show_TriggerTool();
	Show_Info();
	Show_SaveLoadFileWindow();

	Show_BulbTool();

	m_pGameInstance->Physx_Update(_fTimeDelta);

}

HRESULT CLevel_Trigger_Tool::Render()
{
#ifdef _DEBUG
	//SetWindowText(g_hWnd, TEXT("Camera Tool Level"));
	//m_pGameInstance->Render_FPS(TEXT("Timer_Default"));

	//m_pEffect->SetWorld(XMMatrixIdentity());
 //   m_pEffect->SetView(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
 //   m_pEffect->SetProjection(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

 //   m_pEffect->Apply(m_pContext);

 //   m_pContext->OMSetDepthStencilState(m_pDepthStencilState, 0);
 //   m_pContext->IASetInputLayout(m_pInputLayout);

 //   m_pBatch->Begin();

 //   _vector vColor = { 1.f, 1.f, 0.f };

 //   m_pBatch->DrawLine(
 //       VertexPositionColor(XMLoadFloat3(&m_vBulbCreatedPoint), vColor),  // 시작점, 빨간색
 //       VertexPositionColor(XMLoadFloat3(&m_vBulbLine), vColor)     // 끝점, 빨간색
 //   );

 //   m_pBatch->End();
#endif

	return S_OK;
}

HRESULT CLevel_Trigger_Tool::Ready_Lights()
{
	CONST_LIGHT LightDesc{};

	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.vPosition = _float3(0.0f, 20.0f, 0.0f);
	LightDesc.fFallOutStart = 20.0f;
	LightDesc.fFallOutEnd = 1000.0f;
	LightDesc.vRadiance = _float3(1.0f, 1.0f, 1.0f);
	LightDesc.vDiffuse = _float4(1.0f, 0.0f, 0.0f, 1.0f);
	LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 1.0f);
	LightDesc.vSpecular = _float4(1.0f, 0.0f, 0.0f, 1.0f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc, LIGHT_TYPE::POINT)))
		return E_FAIL;

	//LIGHT_DESC LightDesc{};

	//ZeroMemory(&LightDesc, sizeof LightDesc);

	//LightDesc.eType = LIGHT_DESC::TYPE_DIRECTOINAL;
	//LightDesc.vDirection = _float4(-1.f, -1.f, 0.5f, 0.f);
	//LightDesc.vDiffuse = _float4(1.0f, 1.0f, 1.0f, 1.f);
	//LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	//if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Trigger_Tool::Ready_CubeMap(const _wstring& _strLayerTag)
{
	CGameObject* pCubeMap = nullptr;
	CCubeMap::CUBEMAP_DESC Desc;
	Desc.iCurLevelID = LEVEL_TRIGGER_TOOL;
	Desc.iRenderGroupID = RG_3D;
	Desc.iPriorityID = PR3D_PRIORITY;
	Desc.strBRDFPrototypeTag = TEXT("Prototype_Component_Texture_BRDF_Shilick");
	Desc.strCubeMapPrototypeTag = TEXT("Prototype_Component_Texture_TestEnv");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_CubeMap"),
		LEVEL_TRIGGER_TOOL, _strLayerTag, &pCubeMap, &Desc)))
		return E_FAIL;

	m_pGameInstance->Set_CubeMap(static_cast<CCubeMap*>(pCubeMap));
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

HRESULT CLevel_Trigger_Tool::Ready_Layer_MainTable(const _wstring& _strLayerTag)
{
	CModelObject::MODELOBJECT_DESC NormalDesc = {};
	NormalDesc.strModelPrototypeTag_3D = L"SM_desk_split_topboard_02";
	NormalDesc.strShaderPrototypeTag_3D = L"Prototype_Component_Shader_VtxMesh";
	NormalDesc.isCoordChangeEnable = false;
	NormalDesc.iModelPrototypeLevelID_3D = LEVEL_TRIGGER_TOOL;
	NormalDesc.eStartCoord = COORDINATE_3D;
	CGameObject* pGameObject = nullptr;
	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_MapObject"),
		LEVEL_TRIGGER_TOOL,
		L"Layer_MapObject",
		&pGameObject,
		(void*)&NormalDesc);


	// MainTable Actor
	CMainTable::ACTOROBJECT_DESC Desc;
	Desc.iCurLevelID = LEVEL_TRIGGER_TOOL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_MainTable"),
		LEVEL_TRIGGER_TOOL, _strLayerTag, &Desc)))
		return E_FAIL;

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

	ImGui::Text("Event Tag: ");
	ImGui::SameLine();
	_string EventTag = m_pGameInstance->WStringToString(dynamic_cast<CTriggerObject*>(m_pCurTrigger->second)->Get_TriggerEventTag());
	ImGui::Text("%s     ", EventTag.c_str());

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
	case TRIGGER_TYPE::ARM_TRIGGER:
	{
		// Exit Return
		ImGui::Text("Exit Enable Return Type:");
		for (auto& ReturnTag : m_ExitReturnTags) {
			if (ReturnTag.first == (any_cast<_uint>(dynamic_cast<CTriggerObject*>(m_pCurTrigger->second)->Get_CustomData(TEXT("ReturnMask"))) & ReturnTag.first)) {
				if (0x00 == ReturnTag.first)
					continue;

				_string Name = m_pGameInstance->WStringToString(ReturnTag.second);
				ImGui::Text("%s | ", Name.c_str());
				ImGui::SameLine();
			}
		}
	}
		break;
	case TRIGGER_TYPE::ENABLE_LOOKAT_TRIGGER:
	{
		_bool	isLookAt = (any_cast<_bool>(dynamic_cast<CTriggerObject*>(m_pCurTrigger->second)->Get_CustomData(TEXT("IsLookAt"))));
		if (true == isLookAt)
			ImGui::Text("Loot Target: TRUE             ");
		else
			ImGui::Text("Loot Target: FALSE            ");

		// Exit Return
		ImGui::Text("Exit Enable LookAt Type:"); 
		for (auto& ExitLookAtTag : m_ExitReturnTags) {
			if (ExitLookAtTag.first == (any_cast<_uint>(dynamic_cast<CTriggerObject*>(m_pCurTrigger->second)->Get_CustomData(TEXT("ExitLookAtMask"))) & ExitLookAtTag.first)) {
				if (0x00 == ExitLookAtTag.first)
					continue;

				_string Name = m_pGameInstance->WStringToString(ExitLookAtTag.second);
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

	ImGui::Checkbox("Trigger", &m_isTrigger);
	ImGui::SameLine();
	ImGui::Checkbox("Bulb", &m_isBulb);

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

			if (true == m_isTrigger && false == m_isBulb)
				Save_TriggerData();
			else if (true == m_isBulb && false == m_isTrigger)
				Save_BulbPosition();
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
		if (true == m_isTrigger && false == m_isBulb)
			Load_TriggerData();
		else if (true == m_isBulb && false == m_isTrigger)
			Load_BulbPosition();	
	}

	ImGui::End();
}

void CLevel_Trigger_Tool::Show_BulbTool()
{
	ImGui::Begin("Bulb Tool");

	Set_BulbBasicInfo();

	ImGui::NewLine();
	ImGui::Checkbox("Create Bulb By Line", &m_isCreateByLine);
	ImGui::SameLine();
	ImGui::Checkbox("Create Bulb By Point", &m_isCreateByPoint);
	ImGui::SameLine();
	ImGui::Checkbox("Edit Bulb", &m_isEditBulb);
	
	Show_BulbInfo();
	Set_CurBulbLine();
	Delete_Bulb();

	if (true == m_isCreateByLine && false == m_isCreateByPoint && false == m_isEditBulb) {
		Create_Bulb_ByLine();
	}

	if (false == m_isCreateByLine && true == m_isCreateByPoint && false == m_isEditBulb) {
		Create_Bulb_ByPoint();
	}

	if (false == m_isCreateByLine && false == m_isCreateByPoint && true == m_isEditBulb) {
		Edit_Bulb();
	}

	ImGui::End();
}

void CLevel_Trigger_Tool::Show_BulbInfo()
{
	ImGui::NewLine();
	ImGui::Dummy(ImVec2((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Centered Text").x) * 0.5f, 0.0f));
	ImGui::SameLine();
	ImGui::Text("Cur Bulb Info");
	ImGui::Separator();

	if (nullptr != m_pCurBulbLine.first) {
		if (CREATED_BYLINE == m_iCurBulbCreateType) {
			_int iBulbCount = (_int)m_pCurBulbLine.first->Get_BulbCount();
			ImGui::Text("Bulb Count: %d", iBulbCount);
		}
	}
	
	if (nullptr != m_pCurBulb) {
		switch (m_iCurBulbCreateType) {
		case CREATED_BYLINE:
			ImGui::Text("Created By: LINE");
			break;
		case CREATED_BYPOINT:
			ImGui::Text("Created By: POINT");
			break;
		}
	}
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

void CLevel_Trigger_Tool::Show_ExitLookAtMaskListBox()
{
	ImGui::NewLine();
	ImGui::SetNextItemWidth(120.0f);

	if (m_ExitReturnTags.size() <= 0)
		return;

	_string Name = m_pGameInstance->WStringToString(m_ExitReturnTags[m_iExitLookAtIndex].second);

	if (ImGui::BeginCombo("##ExitLookAt", Name.c_str())) {
		for (_int i = 0; i < m_ExitReturnTags.size(); ++i) {
			_bool bSelected = (m_iExitLookAtIndex == i);

			if (ImGui::Selectable(m_pGameInstance->WStringToString(m_ExitReturnTags[i].second).c_str(), bSelected)) {
				m_iExitLookAtIndex = i;

				m_iExitLookAtMask |= m_ExitReturnTags[m_iExitLookAtIndex].first;
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

	// EventTag
	ImGui::Text("Event Tag Input:  %s", m_szEventTag);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(-1);
	ImGui::InputText("##EventTag", m_szEventTemp, MAX_PATH);

	if (ImGui::Button("Set Tag")) {
		strcpy_s(m_szEventTag, m_szEventTemp);
	}

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
	case TRIGGER_TYPE::ARM_TRIGGER:
		ImGui::Text("Arm Trigger Info");
		break;
	}
	ImGui::Separator();

	// Set Info
	switch (m_iTriggerType) {
	case TRIGGER_TYPE::ARM_TRIGGER:
	{
		// CameraTrigger Type
		ImGui::Text("Arm Trigger Tag");

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
	case TRIGGER_TYPE::FREEZE_X_TRIGGER:
	{
		ImGui::Text("Freeze Exit Arm: %.2f, %.2f, %.2f", m_vFreezeExitArm.x, m_vFreezeExitArm.y, m_vFreezeExitArm.z);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
		ImGui::DragFloat("##FreezeExitArmX", &m_vFreezeExitArm.x, 0.1f, 0.f);
		ImGui::SameLine(0, 10.0f);

		ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
		ImGui::DragFloat("##FreezeExitArmY", &m_vFreezeExitArm.y, 0.1f, 0.f);
		ImGui::SameLine(0, 10.0f);

		ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
		ImGui::DragFloat("##FreezeExitArmZ", &m_vFreezeExitArm.z, 0.1f, 0.f);
	}
		break;
	case TRIGGER_TYPE::FREEZE_Z_TRIGGER:
	{
		ImGui::Text("Freeze Exit Arm: %.2f, %.2f, %.2f", m_vFreezeExitArm.x, m_vFreezeExitArm.y, m_vFreezeExitArm.z);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
		ImGui::DragFloat("##FreezeExitArmX", &m_vFreezeExitArm.x, 0.1f, 0.f);
		ImGui::SameLine(0, 10.0f);

		ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
		ImGui::DragFloat("##FreezeExitArmY", &m_vFreezeExitArm.y, 0.1f, 0.f);
		ImGui::SameLine(0, 10.0f);

		ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
		ImGui::DragFloat("##FreezeExitArmZ", &m_vFreezeExitArm.z, 0.1f, 0.f);
	}
		break;
	case TRIGGER_TYPE::ENABLE_LOOKAT_TRIGGER:
	{
		// CameraTrigger Type
		ImGui::Text("Enable LookAt Trigger Tag");

		// Enter LookAt
		if (true == m_isEnterLookAtMask)
			ImGui::Text("Loot Target: TRUE             ");
		else
			ImGui::Text("Loot Target: FALSE            ");

		ImGui::SameLine();
		if (ImGui::Button("LOOKAT TRUE")) {
			m_isEnterLookAtMask = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("LOOKAT FALSE")) {
			m_isEnterLookAtMask = false;
		}

		// Exit LookAt Mask
		ImGui::Text("Exit Look Type:");
		if (m_iExitLookAtIndex <= m_ExitReturnTags.size() - 1) {
			ImGui::SameLine();

			for (auto& LookAtTag : m_ExitReturnTags) {
				if (LookAtTag.first == (m_iExitLookAtMask & LookAtTag.first)) {
					if (0x00 == LookAtTag.first)
						continue;

					_string Name = m_pGameInstance->WStringToString(LookAtTag.second);
					ImGui::Text("%s || ", Name.c_str());
					ImGui::SameLine();
				}
			}
		}

		Show_ExitLookAtMaskListBox();

		ImGui::SameLine();
		if (ImGui::Button("Clear Exit LookAt Type"))
			m_iExitLookAtMask &= EXIT_RETURN_MASK::NONE;
	}
		break;
	}

}

HRESULT CLevel_Trigger_Tool::Create_Trigger()
{
	if (KEY_PRESSING(KEY::CTRL)) {
		if (MOUSE_DOWN(MOUSE_KEY::LB)) {

			CTriggerObject::TRIGGEROBJECT_DESC Desc;

			Desc.iTriggerType = m_iTriggerType;
			Desc.szEventTag = m_pGameInstance->StringToWString(m_szEventTag);
			Desc.eConditionType = CTriggerObject::CONDITION_END;
			Desc.eStartCoord = COORDINATE::COORDINATE_3D;

			Desc.eShapeType = m_eShapeType;
			Desc.vHalfExtents = m_vHalfExtents;
			Desc.fRadius = m_fRadius;

			Desc.iFillterMyGroup = m_ObjectGroupTags[m_iFillterMyGroup].first;
			Desc.iFillterOtherGroupMask = m_iTotalOtherGroupMask;

			Desc.tTransform3DDesc.vInitialPosition = m_vPosition;

			CGameObject* pTrigger = nullptr;

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_TriggerObject"), LEVEL_TRIGGER_TOOL, TEXT("Layer_Trigger"), &pTrigger, &Desc)))
				return E_FAIL;

			// Rotation
			_matrix RotationMat = XMMatrixRotationX(XMConvertToRadians(m_vRotation.x)) * XMMatrixRotationY(XMConvertToRadians(m_vRotation.y)) * XMMatrixRotationZ(XMConvertToRadians(m_vRotation.z));
			dynamic_cast<CTriggerObject*>(pTrigger)->Get_ActorCom()->Set_ShapeLocalOffsetMatrix(0, RotationMat);

			TRIGGEROBJECT_DATA Data = {};
			
			Data.iShapeType = (_uint)m_eShapeType;
			Data.vHalfExtents = m_vHalfExtents;
			Data.fRadius = m_fRadius;
			
			Data.iFillterMyGroup = m_ObjectGroupTags[m_iFillterMyGroup].first;
			Data.iFillterOtherGroupMask = m_iTotalOtherGroupMask;

			Data.iTriggerType = m_iTriggerType;
			Data.szEventTag = m_pGameInstance->StringToWString(m_szEventTag);

			switch (m_iTriggerType) {
			case ARM_TRIGGER:
			{
				dynamic_cast<CTriggerObject*>(pTrigger)->Set_CustomData(TEXT("ReturnMask"), m_iExitReturnMask);
			}
				break;
			case FREEZE_X_TRIGGER:
			{
				dynamic_cast<CTriggerObject*>(pTrigger)->Set_CustomData(TEXT("FreezeExitArm"), m_vFreezeExitArm);
			}
				break;
			case FREEZE_Z_TRIGGER:
			{
				dynamic_cast<CTriggerObject*>(pTrigger)->Set_CustomData(TEXT("FreezeExitArm"), m_vFreezeExitArm);
			}
				break;
			case ENABLE_LOOKAT_TRIGGER:
			{
				dynamic_cast<CTriggerObject*>(pTrigger)->Set_CustomData(TEXT("IsLookAt"), m_isEnterLookAtMask);
				dynamic_cast<CTriggerObject*>(pTrigger)->Set_CustomData(TEXT("ExitLookAtMask"), m_iExitLookAtMask);
			}
				break;
			}

			m_Triggers.push_back(make_pair(Data, dynamic_cast<CTriggerObject*>(pTrigger)));

			Safe_AddRef(pTrigger);
		}
	}
	return S_OK;
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
		_string EventTag = m_pGameInstance->WStringToString(dynamic_cast<CTriggerObject*>(m_pCurTrigger->second)->Get_TriggerEventTag());
		strcpy_s(m_szEventTag, EventTag.c_str());

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
		case ARM_TRIGGER:
			m_iExitReturnMask =  any_cast<_uint>(dynamic_cast<CTriggerObject*>(m_pCurTrigger->second)->Get_CustomData(TEXT("ReturnMask")));
			break;
		case FREEZE_X_TRIGGER:
			m_vFreezeExitArm = any_cast<_float3>(dynamic_cast<CTriggerObject*>(m_pCurTrigger->second)->Get_CustomData(TEXT("FreezeExitArm")));
			break;
		case FREEZE_Z_TRIGGER:
			m_vFreezeExitArm = any_cast<_float3>(dynamic_cast<CTriggerObject*>(m_pCurTrigger->second)->Get_CustomData(TEXT("FreezeExitArm")));
			break;
		case ENABLE_LOOKAT_TRIGGER:			
			m_isEnterLookAtMask = any_cast<_bool>(dynamic_cast<CTriggerObject*>(m_pCurTrigger->second)->Get_CustomData(TEXT("IsLookAt")));
			m_iExitLookAtMask = any_cast<_uint>(dynamic_cast<CTriggerObject*>(m_pCurTrigger->second)->Get_CustomData(TEXT("ExitLookAtMask")));
			break;
		}
	}
	ImGui::SameLine();

	if (ImGui::Button("Edit Info")) {
		if (nullptr == m_pCurTrigger)
			return;

		m_pCurTrigger->first.iFillterMyGroup = m_ObjectGroupTags[m_iFillterMyGroup].first;
		m_pCurTrigger->first.iFillterOtherGroupMask = m_iTotalOtherGroupMask;
		dynamic_cast<CTriggerObject*>(m_pCurTrigger->second)->Set_TriggerEventTag(m_pGameInstance->StringToWString(m_szEventTag));

		switch (m_iTriggerType) {
		case ARM_TRIGGER:
			dynamic_cast<CTriggerObject*>(m_pCurTrigger->second)->Set_CustomData(TEXT("ReturnMask"), m_iExitReturnMask);
			break;
		case FREEZE_X_TRIGGER:
			dynamic_cast<CTriggerObject*>(m_pCurTrigger->second)->Set_CustomData(TEXT("FreezeExitArm"), m_vFreezeExitArm);
			break;
		case FREEZE_Z_TRIGGER:
			dynamic_cast<CTriggerObject*>(m_pCurTrigger->second)->Set_CustomData(TEXT("FreezeExitArm"), m_vFreezeExitArm);
			break;
		case ENABLE_LOOKAT_TRIGGER:
			dynamic_cast<CTriggerObject*>(m_pCurTrigger->second)->Set_CustomData(TEXT("IsLookAt"), m_isEnterLookAtMask);
			dynamic_cast<CTriggerObject*>(m_pCurTrigger->second)->Set_CustomData(TEXT("ExitLookAtMask"), m_iExitLookAtMask);
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

void CLevel_Trigger_Tool::Initialize_ListBoxName()
{
	_wstring wszTagName;

	// Trigger Tag
	for (_uint i = 0; i < TRIGGER_TYPE::TRIGGER_TYPE_END; ++i) {
		
		switch (i) {
		case TRIGGER_TYPE::ARM_TRIGGER:
			wszTagName = TEXT("ARM_TRIGGER");
			break;
		case TRIGGER_TYPE::CUTSCENE_TRIGGER:
			wszTagName = TEXT("CUTSCENE_TRIGGER");
			break;
		case TRIGGER_TYPE::FREEZE_X_TRIGGER:
			wszTagName = TEXT("FREEZE_X_TRIGGER");
			break;
		case TRIGGER_TYPE::FREEZE_Z_TRIGGER:
			wszTagName = TEXT("FREEZE_Z_TRIGGER");
			break;
		case TRIGGER_TYPE::TELEPORT_TRIGGER:
			wszTagName = TEXT("TELEPORT_TRIGGER");
			break;
		case TRIGGER_TYPE::EVENT_TRIGGER:
			wszTagName = TEXT("EVENT_TRIGGER");
			break;
		case TRIGGER_TYPE::ENABLE_LOOKAT_TRIGGER:
			wszTagName = TEXT("ENABLE_LOOKAT_TRIGGER");
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

			XMStoreFloat3(&m_vPosition, vClickedPos);
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

pair<CLevel_Trigger_Tool::TRIGGEROBJECT_DATA, CTriggerObject*>* CLevel_Trigger_Tool::Get_SelectedTrigger()
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
		auto& Collider_Json = Trigger_json["Collider_Info"];
		Trigger_json["Trigger_Type"] = Trigger.second->Get_TriggerType();

		_string szEventTag = m_pGameInstance->WStringToString(dynamic_cast<CTriggerObject*>(Trigger.second)->Get_TriggerEventTag());
		Trigger_json["Trigger_EventTag"] = szEventTag;
		Trigger_json["Trigger_Coordinate"] = Trigger.second->Get_CurCoord();
		Trigger_json["Trigger_ConditionType"] = Trigger.second->Get_ConditionType();

		Collider_Json["Position"] = { vPosition.x, vPosition.y, vPosition.z };

		const vector<PxShape*>& Shapes = Trigger.second->Get_ActorCom()->Get_Shapes();
		PxTransform ShapeTransform = Shapes[0]->getLocalPose();
		ShapeTransform.q;
		_float3 vRotation = Quaternion_ToEuler({ ShapeTransform.q.x, ShapeTransform.q.y, ShapeTransform.q.z, ShapeTransform.q.w });
		Collider_Json["Rotation"] = { XMConvertToDegrees(vRotation.x), XMConvertToDegrees(vRotation.y), XMConvertToDegrees(vRotation.z) };

		Collider_Json["ShapeType"] = (_uint)Trigger.first.iShapeType;
		Collider_Json["HalfExtents"] = { Trigger.first.vHalfExtents.x, Trigger.first.vHalfExtents.y, Trigger.first.vHalfExtents.z };
		Collider_Json["Radius"] = Trigger.first.fRadius;

		Trigger_json["Fillter_MyGroup"] = Trigger.first.iFillterMyGroup;
		Trigger_json["Fillter_OtherGroupMask"] = Trigger.first.iFillterOtherGroupMask;

		_uint iTriggerType = Trigger.second->Get_TriggerType();
		switch (iTriggerType) {
		case ARM_TRIGGER:
		{
			_uint iReturnMask = any_cast<_uint>(dynamic_cast<CTriggerObject*>(Trigger.second)->Get_CustomData(TEXT("ReturnMask")));
			Trigger_json["Arm_Info"]["Exit_Return_Mask"] = { {"ReturnMask", iReturnMask} };
		}
			break;

		case FREEZE_X_TRIGGER:
		{
			_float3 vFreezeExit = any_cast<_float3>(dynamic_cast<CTriggerObject*>(Trigger.second)->Get_CustomData(TEXT("FreezeExitArm")));

			Trigger_json["Freeze_X_Info"]["Freeze_Exit_Arm"]["CustomData_Tag"] = "FreezeExitArm";
			Trigger_json["Freeze_X_Info"]["Freeze_Exit_Arm"]["CustomData"] = { vFreezeExit.x, vFreezeExit.y, vFreezeExit.z };
		}
			break;
		case FREEZE_Z_TRIGGER:
		{
			_float3 vFreezeExit = any_cast<_float3>(dynamic_cast<CTriggerObject*>(Trigger.second)->Get_CustomData(TEXT("FreezeExitArm")));
			
			Trigger_json["Freeze_Z_Info"]["Freeze_Exit_Arm"]["CustomData_Tag"] = "FreezeExitArm";
			Trigger_json["Freeze_Z_Info"]["Freeze_Exit_Arm"]["CustomData"] = {vFreezeExit.x, vFreezeExit.y, vFreezeExit.z};
		}
			break;
		case ENABLE_LOOKAT_TRIGGER: 
		{
			_bool isEnterLookAt = any_cast<_bool>(dynamic_cast<CTriggerObject*>(Trigger.second)->Get_CustomData(TEXT("IsLookAt")));

			Trigger_json["Enable_LookAt_Info"]["Is_LookAt"]["CustomData_Tag"] = "IsLookAt";
			Trigger_json["Enable_LookAt_Info"]["Is_LookAt"]["CustomData"] = isEnterLookAt;

			_uint iExitLookAtMask = any_cast<_uint>(dynamic_cast<CTriggerObject*>(Trigger.second)->Get_CustomData(TEXT("ExitLookAtMask")));
			Trigger_json["Enable_LookAt_Info"]["Exit_LookAt_Mask"] = { {"ExitLookAtMask", iExitLookAtMask} };
		}
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
		CTriggerObject::TRIGGEROBJECT_DESC Desc;

		auto& Collider_Json = Trigger_json["Collider_Info"];

		Desc.iTriggerType = Trigger_json["Trigger_Type"];
		Desc.szEventTag = m_pGameInstance->StringToWString(Trigger_json["Trigger_EventTag"]);
		Desc.eStartCoord = Trigger_json["Trigger_Coordinate"];
		Desc.eConditionType = Trigger_json["Trigger_ConditionType"];

		Desc.tTransform3DDesc.vInitialPosition = { Collider_Json["Position"][0].get<_float>(),  Collider_Json["Position"][1].get<_float>() , Collider_Json["Position"][2].get<_float>() };
		_float3 vRotation = { Collider_Json["Rotation"][0].get<_float>(),  Collider_Json["Rotation"][1].get<_float>() , Collider_Json["Rotation"][2].get<_float>() };

		Desc.eShapeType = Collider_Json["ShapeType"];
		Desc.vHalfExtents = { Collider_Json["HalfExtents"][0].get<_float>(),  Collider_Json["HalfExtents"][1].get<_float>() , Collider_Json["HalfExtents"][2].get<_float>() };
		Desc.fRadius = Collider_Json["Radius"];

		Desc.iFillterMyGroup = Trigger_json["Fillter_MyGroup"];
		Desc.iFillterOtherGroupMask = Trigger_json["Fillter_OtherGroupMask"];

		CGameObject* pTrigger = nullptr;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_TriggerObject"), LEVEL_TRIGGER_TOOL, TEXT("Layer_Trigger"), &pTrigger, &Desc))) {
			MSG_BOX("Failed To Load Camera Trigger");
			return;
		}

		// Rotation
		_matrix RotationMat = XMMatrixRotationX(XMConvertToRadians(vRotation.x)) * XMMatrixRotationY(XMConvertToRadians(vRotation.y)) * XMMatrixRotationZ(XMConvertToRadians(vRotation.z));
		dynamic_cast<CTriggerObject*>(pTrigger)->Get_ActorCom()->Set_ShapeLocalOffsetMatrix(0, RotationMat);

		// Custom Data
		_string szKey;
		_uint iReturnMask;
		_float3 vFreezeExitArm;
		_bool iEnterLookAtMask;
		_uint iExitLookAtMask;

		switch (Desc.iTriggerType) {
		case ARM_TRIGGER:
		{
			if (Trigger_json.contains("Arm_Info")) {
				json exitReturnMask = Trigger_json["Arm_Info"]["Exit_Return_Mask"];

				for (auto& [key, value] : exitReturnMask.items()) {
					szKey = key;
					iReturnMask = value;
				}
			}

			dynamic_cast<CTriggerObject*>(pTrigger)->Set_CustomData(m_pGameInstance->StringToWString(szKey), iReturnMask);
		}
			break;
		case FREEZE_X_TRIGGER:
		{
			if (Trigger_json.contains("Freeze_X_Info")) {
				json FreezeExit = Trigger_json["Freeze_X_Info"]["Freeze_Exit_Arm"];

				szKey = FreezeExit["CustomData_Tag"];
				vFreezeExitArm = { FreezeExit["CustomData"][0].get<_float>(), FreezeExit["CustomData"][1].get<_float>(),FreezeExit["CustomData"][2].get<_float>() };
			}
			
			dynamic_cast<CTriggerObject*>(pTrigger)->Set_CustomData(m_pGameInstance->StringToWString(szKey), vFreezeExitArm);
		}
			break;
		case FREEZE_Z_TRIGGER:
		{
			if (Trigger_json.contains("Freeze_Z_Info")) {
				json FreezeExit = Trigger_json["Freeze_Z_Info"]["Freeze_Exit_Arm"];

				szKey = FreezeExit["CustomData_Tag"];
				vFreezeExitArm = { FreezeExit["CustomData"][0].get<_float>(), FreezeExit["CustomData"][1].get<_float>(),FreezeExit["CustomData"][2].get<_float>() };
			}

			dynamic_cast<CTriggerObject*>(pTrigger)->Set_CustomData(m_pGameInstance->StringToWString(szKey), vFreezeExitArm);
		}
			break;
		case ENABLE_LOOKAT_TRIGGER:
		{

			if (Trigger_json.contains("Enable_LookAt_Info")) {
				json isLook = Trigger_json["Enable_LookAt_Info"]["Is_LookAt"];

				szKey = isLook["CustomData_Tag"];
				iEnterLookAtMask = isLook["CustomData"];

				dynamic_cast<CTriggerObject*>(pTrigger)->Set_CustomData(m_pGameInstance->StringToWString(szKey), iEnterLookAtMask);

				json ExitLookAt = Trigger_json["Enable_LookAt_Info"]["Exit_LookAt_Mask"];
				for (auto& [key, value] : ExitLookAt.items()) {
					szKey = key;
					iExitLookAtMask = value;
				}

				dynamic_cast<CTriggerObject*>(pTrigger)->Set_CustomData(m_pGameInstance->StringToWString(szKey), iExitLookAtMask);
			}
		}
			break;
		}

		// Data
		TRIGGEROBJECT_DATA Data;

		Data.iTriggerType = Desc.iTriggerType;
		Data.szEventTag = Desc.szEventTag;
		
		Data.iShapeType = (_uint)Desc.eShapeType;
		Data.vHalfExtents = Desc.vHalfExtents;
		Data.fRadius = Desc.fRadius;

		Data.iFillterMyGroup = Desc.iFillterMyGroup;
		Data.iFillterOtherGroupMask = Desc.iFillterOtherGroupMask;

		m_Triggers.push_back(make_pair(Data, dynamic_cast<CTriggerObject*>(pTrigger)));
		Safe_AddRef(pTrigger);
	}
}

void CLevel_Trigger_Tool::Create_Bulb_ByLine()
{
	if (KEY_PRESSING(KEY::CTRL)) {
		if (MOUSE_DOWN(MOUSE_KEY::LB)) {

			if (false == m_isCreateByLine)
				return;

			_float2 fMousePos = m_pGameInstance->Get_CursorPos();

			if (0 > fMousePos.x || g_iWinSizeX < fMousePos.x || 0 > fMousePos.y || g_iWinSizeY < fMousePos.y)
				return;

			_vector vRayPos, vRayDir;

			Get_RayInfo(&vRayPos, &vRayDir);

			_float fDist = {};

			for (auto& BulbLine : m_BulbLines) {
				pair<CModelObject*, CModelObject*>* pLine = BulbLine->Get_Line();

				if (true == pLine->first->Is_PickingCursor_Model(fMousePos, fDist)
					|| true == pLine->second->Is_PickingCursor_Model(fMousePos, fDist)) {
					return;
				}
			}

			if (nullptr != m_pMakingBulbLine) {
				pair<CModelObject*, CModelObject*>* pLine = m_pMakingBulbLine->Get_Line();

				if (true == pLine->first->Is_PickingCursor_Model(fMousePos, fDist)) {
					return;
				}

				m_pMakingBulbLine->Add_Point(dynamic_cast<CModelObject*>(Create_BulbPoint()));
				m_BulbLines.push_back(m_pMakingBulbLine);
				m_pMakingBulbLine = nullptr;
			}
			else {
				CBulbLine::BULBLINE_DESC Desc;
				Desc.fBulbPosOffset = m_fBulbPosOffset;

				CBulbLine* pBulbLine = CBulbLine::Create(m_pDevice, m_pContext, &Desc);
				pBulbLine->Add_Point(dynamic_cast<CModelObject*>(Create_BulbPoint()));

				m_pMakingBulbLine = pBulbLine;
			}
		}
	}
}

void CLevel_Trigger_Tool::Create_Bulb_ByPoint()
{
	if (KEY_PRESSING(KEY::CTRL)) {
		if (MOUSE_DOWN(MOUSE_KEY::LB)) {

			// Create
			CGameObject* pBulb;
			CBulb::BULB_DESC Desc = {};
			Desc.eStartCoord = COORDINATE_3D;
			Desc.tTransform3DDesc.vInitialPosition = m_vPosition;

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Bulb"),
				LEVEL_TRIGGER_TOOL, TEXT("Layer_Bulb"), &pBulb, &Desc);

			m_Bulbs.push_back(dynamic_cast<CBulb*>(pBulb));

			Safe_AddRef(pBulb);

		}
	}
	
}

void CLevel_Trigger_Tool::Edit_Bulb()
{
	if (nullptr == m_pCurBulbLine.first)
		return;

	if (KEY_PRESSING(KEY::SPACE)) {
	
		m_pCurBulbLine.second->Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vPosition), 1.f));
		
		m_pCurBulbLine.first->Edit_BulbInfo();
	}
}

void CLevel_Trigger_Tool::Delete_Bulb()
{
	if (KEY_PRESSING(KEY::LSHIFT)) {
		if (MOUSE_DOWN(MOUSE_KEY::LB)) {

			_float2 fMousePos = m_pGameInstance->Get_CursorPos();

			CGameObject* pCube = m_pGameInstance->Get_PickingModelObjectByCursor(LEVEL_TRIGGER_TOOL, TEXT("Layer_Cube"), fMousePos);

			if (nullptr != pCube) {
				for (auto iterator = m_BulbLines.begin(); iterator != m_BulbLines.end();) {

					pair<CModelObject*, CModelObject*>* pPoints = (*iterator)->Get_Line();

					if (pPoints->first == pCube || pPoints->second == pCube) {
						if (m_pCurBulbLine.first == *iterator) {
							Safe_Release(m_pCurBulbLine.first);
							m_pCurBulbLine.first = nullptr;
							m_pCurBulbLine.second = nullptr;
						}

						(*iterator)->Clear();
						Safe_Release(*iterator);
						m_BulbLines.erase(iterator);

						return;
					}
					else {
						++iterator;
					}
				}
			}

			_vector vPos, vDir;
			Get_RayInfo(&vPos, &vDir);

			_float3 vRayPos, vRayDir;
			XMStoreFloat3(&vRayPos, vPos);
			XMStoreFloat3(&vRayDir, vDir);

			CActorObject* pObject = nullptr;

			_bool isSelected = m_pGameInstance->RayCast_Nearest(vRayPos, vRayDir, 2000.f, nullptr, &pObject);

			if (true == isSelected) {
				for (auto iterator = m_Bulbs.begin(); iterator != m_Bulbs.end();) {
					if (*iterator == pObject) {
						if (pObject == m_pCurBulb) {
							Safe_Release(m_pCurBulb);
						}

						Event_DeleteObject(*iterator);
						Safe_Release(*iterator);
						m_Bulbs.erase(iterator);

						return;
					}
					else {
						++iterator;
					}
				}
			}
			
		}
	}
}

void CLevel_Trigger_Tool::Set_CurBulbLine()
{
	if (KEY_PRESSING(KEY::ALT)) {
		if (MOUSE_DOWN(MOUSE_KEY::LB)) {

			_float2 fMousePos = m_pGameInstance->Get_CursorPos();

			CGameObject* pCube = m_pGameInstance->Get_PickingModelObjectByCursor(LEVEL_TRIGGER_TOOL, TEXT("Layer_Cube"), fMousePos);

			if (nullptr != pCube) {
				for (auto& BulbLine : m_BulbLines) {

					pair<CModelObject*, CModelObject*>* pPoints = BulbLine->Get_Line();

					if (pPoints->first == pCube || pPoints->second == pCube) {

						Safe_Release(m_pCurBulbLine.first);
						m_pCurBulbLine.first = BulbLine;
						m_pCurBulbLine.second = dynamic_cast<CModelObject*>(pCube);
						Safe_AddRef(BulbLine);

						m_iCurBulbCreateType = CREATED_BYLINE;
						return;
					}
				}
			}
			
			_vector vPos, vDir;
			Get_RayInfo(&vPos, &vDir);

			_float3 vRayPos, vRayDir;
			XMStoreFloat3(&vRayPos, vPos);
			XMStoreFloat3(&vRayDir, vDir);

			CActorObject* pObject = nullptr;

			_bool isSelected = m_pGameInstance->RayCast_Nearest(vRayPos, vRayDir, 2000.f, nullptr, &pObject);

			if (true == isSelected) {
				for (auto& Bulb : m_Bulbs) {
					if (pObject == Bulb) {

						Safe_Release(m_pCurBulb);
						m_pCurBulb = Bulb;
						Safe_AddRef(Bulb);

						m_iCurBulbCreateType = CREATED_BYPOINT;
						return;
					}
				}
			}
		}
	}
}

CGameObject* CLevel_Trigger_Tool::Create_BulbPoint()
{
	CTest_Terrain::MODELOBJECT_DESC TerrainDesc{};

	TerrainDesc.eStartCoord = COORDINATE_3D;
	TerrainDesc.iCurLevelID = LEVEL_TRIGGER_TOOL;
	TerrainDesc.isCoordChangeEnable = false;
	TerrainDesc.iModelPrototypeLevelID_3D = LEVEL_TRIGGER_TOOL;

	TerrainDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
	TerrainDesc.strModelPrototypeTag_3D = TEXT("alphabet_blocks_d_mesh");

	TerrainDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;

	TerrainDesc.tTransform3DDesc.vInitialPosition = m_vPosition;
	TerrainDesc.tTransform3DDesc.vInitialScaling = _float3(0.1f, 0.1f, 0.1f);
	TerrainDesc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	TerrainDesc.tTransform3DDesc.fSpeedPerSec = 0.f;

	CGameObject* pCube;

	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_TRIGGER_TOOL, TEXT("Prototype_GameObject_Test_Terrain"), LEVEL_TRIGGER_TOOL, TEXT("Layer_Cube"), &pCube, &TerrainDesc);
	Safe_AddRef(pCube);

	return pCube;
}

void CLevel_Trigger_Tool::Set_BulbBasicInfo()
{
	ImGui::NewLine();
	ImGui::Dummy(ImVec2((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Centered Text").x) * 0.5f, 0.0f));
	ImGui::SameLine();
	ImGui::Text("Bulb Info");
	ImGui::Separator();

	ImGui::Text("Position: %.2f, %.2f, %.2f", m_vPosition.x, m_vPosition.y, m_vPosition.z);
	ImGui::SameLine();

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##BulbX", &m_vPosition.x, 0.1f);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##BulbY", &m_vPosition.y, 0.1f);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##BulbZ", &m_vPosition.z, 0.1f);

	ImGui::Text("Bulb PosOffset: %.2f", m_fBulbPosOffset);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##BulbPosOffset", &m_fBulbPosOffset, 0.1f, 1.f, 10.f);
}

void CLevel_Trigger_Tool::Save_BulbPosition()
{
	json Result = json::array();

	// BulbPosition

	for (auto& BulbLine : m_BulbLines) {
		list<CBulb*>* pBulbs = BulbLine->Get_Bulbs();

		for (auto& Bulb : *pBulbs) {
			json Bulb_json;

			_vector vPos = Bulb->Get_ControllerTransform()->Get_State(CTransform::STATE_POSITION);
			Bulb_json["Bulb_Position"] = { XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos) };

			Result.push_back(Bulb_json);
		}
	}

	for (auto& Bulb : m_Bulbs) {
		json Bulb_json;

		_vector vPos = Bulb->Get_ControllerTransform()->Get_State(CTransform::STATE_POSITION);
		Bulb_json["Bulb_Position"] = { XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos) };

		Result.push_back(Bulb_json);
	}

	_wstring wszSavePath = L"../Bin/Resources/DataFiles/TriggerData/BulbData/";
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

void CLevel_Trigger_Tool::Load_BulbPosition()
{
	_wstring wszLoadPath = m_pGameInstance->StringToWString(m_JsonFilePaths[m_iCurrentJsonFileIndex]);

	ifstream file(wszLoadPath);

	if (!file.is_open())
	{
		MSG_BOX("파일을 열 수 없습니다.");
		file.close();
		return;
	}

	json Result;
	file >> Result;
	file.close();

	json Bulb_json;

	for (auto& Bulb_json : Result) {
		_float3 vPosition = { Bulb_json["Bulb_Position"][0].get<_float>(), Bulb_json["Bulb_Position"][1].get<_float>(), Bulb_json["Bulb_Position"][2].get<_float>() };
	
		// Create
		CGameObject* pBulb;
		CBulb::BULB_DESC Desc = {};
		Desc.eStartCoord = COORDINATE_3D;
		Desc.tTransform3DDesc.vInitialPosition = vPosition;

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Bulb"),
			LEVEL_TRIGGER_TOOL, TEXT("Layer_Bulb"), &pBulb, &Desc);

		m_Bulbs.push_back(dynamic_cast<CBulb*>(pBulb));

		Safe_AddRef(pBulb);
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

	for (auto& BulbLine : m_BulbLines) {
		Safe_Release(BulbLine);
	}

	Safe_Release(m_pMakingBulbLine);
	Safe_Release(m_pCurBulbLine.first);

	for (auto& Bulb : m_Bulbs) {
		Safe_Release(Bulb);
	}

	Safe_Release(m_pCurBulb);

	__super::Free();
}
