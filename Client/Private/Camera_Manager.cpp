#include "stdafx.h"
#include "../Public/Camera_Manager.h"

#include "GameInstance.h"

#include "Camera_Target.h"
#include "Camera_CutScene.h"
#include "Camera_2D.h"
#include "Section_Manager.h"

IMPLEMENT_SINGLETON(CCamera_Manager)

CCamera_Manager::CCamera_Manager()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCamera_Manager::Initialize()
{
	for (auto& Camera : m_Cameras)
		Camera = nullptr;

	return S_OK;
}

void CCamera_Manager::Update(_float fTimeDelta)
{
	/*if (KEY_DOWN(KEY::G))
	{
		Start_FadeIn(2.5f);
	}
	if (KEY_DOWN(KEY::H))
	{
		Start_FadeOut(1.5f);
	}*/

#ifdef _DEBUG
	if(IS_IMPORT_IMGUI)
		Imgui_Dof();

#endif // _DEBUG

}

void CCamera_Manager::Level_Exit(_int _iChangeLevelID, _int _iNextChangeLevelID)
{
	m_iCurLevelID = _iChangeLevelID;

	for (auto& Camera : m_Cameras) {
		Safe_Release(Camera);
		Camera = nullptr;
	}
}

_vector CCamera_Manager::Get_CameraVector(CTransform::STATE _eState)
{
	CController_Transform* pConTrans = m_Cameras[m_eCurrentCameraType]->Get_ControllerTransform();
	return pConTrans->Get_State(_eState);
}

_uint CCamera_Manager::Get_CameraMode(_uint _iCameraType)
{
	if (TARGET != _iCameraType && TARGET_2D != _iCameraType)
		return INT16_MAX;

	if (TARGET == _iCameraType)
		return dynamic_cast<CCamera_Target*>(m_Cameras[TARGET])->Get_CameraMode();
	else if (TARGET_2D == _iCameraType)
		return dynamic_cast<CCamera_2D*>(m_Cameras[TARGET_2D])->Get_CameraMode();

	return _uint();
}

_uint CCamera_Manager::Get_CurCameraMode()
{
	if (TARGET != m_eCurrentCameraType && TARGET_2D != m_eCurrentCameraType)
		return INT16_MAX;

	if (TARGET == m_eCurrentCameraType)
		return dynamic_cast<CCamera_Target*>(m_Cameras[TARGET])->Get_CameraMode();
	else if (TARGET_2D == m_eCurrentCameraType)
		return dynamic_cast<CCamera_2D*>(m_Cameras[TARGET_2D])->Get_CameraMode();

	return _uint();
}
#ifdef _DEBUG

void CCamera_Manager::Start_ZoomIn()
{
	_uint iZoomLevel = m_Cameras[m_eCurrentCameraType]->Get_CurrentZoomLevel();

	if (iZoomLevel <= CCamera::LEVEL_1)
		return;

	m_Cameras[m_eCurrentCameraType]->Start_Zoom(0.5f, (CCamera::ZOOM_LEVEL)(iZoomLevel - 1), RATIO_TYPE::EASE_IN);
}

void CCamera_Manager::Start_ZoomOut()
{
	_uint iZoomLevel = m_Cameras[m_eCurrentCameraType]->Get_CurrentZoomLevel();

	if (iZoomLevel >= CCamera::LEVEL_10)
		return;

	m_Cameras[m_eCurrentCameraType]->Start_Zoom(0.5f, (CCamera::ZOOM_LEVEL)(iZoomLevel + 1), RATIO_TYPE::EASE_IN);
}

void CCamera_Manager::Imgui_Dof()
{
	if (nullptr == m_Cameras[m_eCurrentCameraType])
		return;

	ImGui::Begin("Set Dof");

	CONST_DOF tDofData = m_Cameras[m_eCurrentCameraType]->Get_DofBufferData();
	_float fFovy = m_Cameras[m_eCurrentCameraType]->Get_Fovy();
	fFovy = XMConvertToDegrees(fFovy);
	if (ImGui::SliderFloat("Fovy##Camera", &fFovy, 20.f, 120.f))
	{
		m_Cameras[m_eCurrentCameraType]->Set_Fovy(XMConvertToRadians(fFovy));
	}
	if (ImGui::SliderFloat("Aperture##Camera", &tDofData.fAperture, 0.95f, 50.f))
	{
		m_Cameras[m_eCurrentCameraType]->Set_DofBufferData(tDofData, true);
	}
	if (ImGui::SliderFloat("SensorHeight##Camera", &tDofData.fSensorHeight, 4.f, 50.f))
	{
		m_Cameras[m_eCurrentCameraType]->Set_DofBufferData(tDofData, true);
	}
	if (ImGui::SliderFloat("FocusDistance##Camera", &tDofData.fFocusDistance, 0.1f, 200.f))
	{
		m_Cameras[m_eCurrentCameraType]->Set_DofBufferData(tDofData, true);
	}
	if (ImGui::SliderFloat("DofBrightness##Camera", &tDofData.fDofBrightness, 0.01f, 10.f))
	{
		m_Cameras[m_eCurrentCameraType]->Set_DofBufferData(tDofData, true);
	}
	if (ImGui::SliderFloat("BaseBlurPower##Camera", &tDofData.fBaseBlurPower, 0.0f, 1.0f))
	{
		m_Cameras[m_eCurrentCameraType]->Set_DofBufferData(tDofData, true);
	}
	if (ImGui::DragFloat3("BlurColor##Camera", &(tDofData.vBlurColor.x), 0.01f,0.0f, 2.0f))
	{
		m_Cameras[m_eCurrentCameraType]->Set_DofBufferData(tDofData, true);
	}

	// Save 
	static char DofPathBuffer[MAX_PATH] = "../Bin/DataFiles/DOF/default.json";
	ImGui::InputText("Save Path", DofPathBuffer, sizeof(DofPathBuffer));
	if (ImGui::Button("Save DOF Data"))
	{
		json DOFJson;
		DOFJson["fAperture"] = tDofData.fAperture;
		DOFJson["fSensorHeight"] = tDofData.fSensorHeight;
		DOFJson["fFocusDistance"] = tDofData.fFocusDistance;
		DOFJson["fDofBrightness"] = tDofData.fDofBrightness;
		DOFJson["fBaseBlurPower"] = tDofData.fBaseBlurPower;
		DOFJson["vBlurColor"]["x"] = tDofData.vBlurColor.x;
		DOFJson["vBlurColor"]["y"] = tDofData.vBlurColor.y;
		DOFJson["vBlurColor"]["z"] = tDofData.vBlurColor.z;


		std::ofstream outFile(DofPathBuffer);
		if (false == outFile.is_open())
		{
			MSG_BOX("Failed IBL Data Save Path Open");
		}
		outFile << DOFJson.dump(4);
		outFile.close();
	}
	// Load Files
	ImGui::Separator();
	ImGui::Separator();

	if (ImGui::Button("Load DOF Data##Dof"))
	{
		OPENFILENAME ofn = {};
		ZeroMemory(&ofn, sizeof(ofn));
		_tchar szName[MAX_PATH] = {};
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = m_pGameInstance->Get_HWND();
		ofn.lpstrFile = szName;
		ofn.nMaxFile = sizeof(szName);
		ofn.lpstrFilter = L".json\0*.json\0";
		ofn.nFilterIndex = 0;
		ofn.lpstrFileTitle = nullptr;
		ofn.nMaxFileTitle = 0;
		wstring strPath = std::filesystem::absolute(L"../Bin/DataFiles/DOF/").wstring();
		ofn.lpstrInitialDir = strPath.c_str();
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileName(&ofn))
		{
			//받아온 파일입니다
			const _wstring strFilePath = szName;
			Load_DOF(strFilePath, m_eCurrentCameraType);
		}
	}


	ImGui::End();
}
#endif // _DEBUG


void CCamera_Manager::Add_Camera(_uint _iCurrentCameraType, CCamera* _pCamera)
{
	if (nullptr == _pCamera)
		return;

	m_eCurrentCameraType = _iCurrentCameraType;
	m_Cameras[_iCurrentCameraType] = _pCamera;

	Safe_AddRef(m_Cameras[_iCurrentCameraType]);
}

void CCamera_Manager::Add_ArmData(_uint _iCameraType, _wstring wszArmTag, ARM_DATA* _pArmData, SUB_DATA* _pSubData)
{
	if (nullptr == m_Cameras[_iCameraType])
		return;

	switch (_iCameraType) {
	case TARGET:
	{
		static_cast<CCamera_Target*>(m_Cameras[_iCameraType])->Add_ArmData(wszArmTag, _pArmData, _pSubData);
	}
		break;
	case TARGET_2D:
	{
		static_cast<CCamera_2D*>(m_Cameras[_iCameraType])->Add_ArmData(wszArmTag, _pArmData, _pSubData);
	}
		break;
	}
}

void CCamera_Manager::Add_CutScene(_wstring _wszCutSceneTag, pair<CUTSCENE_SUB_DATA, vector<CUTSCENE_DATA*>> _CutSceneData)
{
	if (nullptr == m_Cameras[CUTSCENE])
		return;

	static_cast<CCamera_CutScene*>(m_Cameras[CUTSCENE])->Add_CutScene(_wszCutSceneTag, _CutSceneData);
}

void CCamera_Manager::Load_ArmData(_uint _iCameraType, _wstring _szFilePath)
{
	ifstream file(_szFilePath);

	if (!file.is_open())
	{
		MSG_BOX("파일을 열 수 없습니다.");
		file.close();
		return;
	}

	json Result;
	file >> Result;
	file.close();

	json Trigger_json;

	for (auto& Trigger_json : Result) {

		_string szArmTag = Trigger_json["Arm_Tag"];

		// Arm Data
		ARM_DATA* tArmData = new ARM_DATA();

		tArmData->fLength = Trigger_json["Arm_Data"]["Arm_Length"];
		tArmData->fLengthTime = { Trigger_json["Arm_Data"]["Arm_Time"][0].get<_float>(), Trigger_json["Arm_Data"]["Arm_Time"][1].get<_float>() };
		tArmData->iLengthRatioType = Trigger_json["Arm_Data"]["Arm_Length_RatioType"];

		tArmData->fMoveTimeAxisY = { Trigger_json["Arm_Data"]["Arm_MoveTime_AxisY"][0].get<_float>(), Trigger_json["Arm_Data"]["Arm_MoveTime_AxisY"][1].get<_float>() };
		tArmData->fMoveTimeAxisRight = { Trigger_json["Arm_Data"]["Arm_MoveTime_AxisRight"][0].get<_float>(), Trigger_json["Arm_Data"]["Arm_MoveTime_AxisRight"][1].get<_float>() };
		tArmData->fRotationPerSecAxisY = { Trigger_json["Arm_Data"]["Arm_RotationPerSec_AxisY"][0].get<_float>(), Trigger_json["Arm_Data"]["Arm_RotationPerSec_AxisY"][1].get<_float>() };
		tArmData->fRotationPerSecAxisRight = { Trigger_json["Arm_Data"]["Arm_RotationPerSec_AxisRight"][0].get<_float>(), Trigger_json["Arm_Data"]["Arm_RotationPerSec_AxisRight"][1].get<_float>() };

		tArmData->vDesireArm = { Trigger_json["Arm_Data"]["Desire_Arm"][0].get<_float>(), Trigger_json["Arm_Data"]["Desire_Arm"][1].get<_float>(), Trigger_json["Arm_Data"]["Desire_Arm"][2].get<_float>() };

		// Sub Data
		_bool IsUseSubData = Trigger_json["Sub_Data"]["Use_SubData"];
		SUB_DATA* pSubData = nullptr;

		if (true == IsUseSubData) {
			pSubData = new SUB_DATA();

			pSubData->fZoomTime = Trigger_json["Sub_Data"]["Zoom_Time"];
			pSubData->iZoomLevel = Trigger_json["Sub_Data"]["Zoom_Level"];
			pSubData->iZoomRatioType = Trigger_json["Sub_Data"]["Zoom_RatioType"];

			pSubData->fAtOffsetTime = Trigger_json["Sub_Data"]["AtOffset_Time"];
			pSubData->vAtOffset = { Trigger_json["Sub_Data"]["AtOffset"][0].get<_float>(), Trigger_json["Sub_Data"]["AtOffset"][1].get<_float>(), Trigger_json["Sub_Data"]["AtOffset"][2].get<_float>() };
			pSubData->iAtRatioType = Trigger_json["Sub_Data"]["AtOffset_RatioType"];
		}

		CCamera_Manager::GetInstance()->Add_ArmData(_iCameraType, m_pGameInstance->StringToWString(szArmTag), tArmData, pSubData);
	}
}

void CCamera_Manager::Change_CameraMode(_uint _iCameraMode, _int _iNextMode)
{
	if (TARGET == m_eCurrentCameraType) {
		static_cast<CCamera_Target*>(m_Cameras[m_eCurrentCameraType])->Set_CameraMode(_iCameraMode, _iNextMode);
	}
	else if (TARGET_2D == m_eCurrentCameraType) {
		static_cast<CCamera_2D*>(m_Cameras[m_eCurrentCameraType])->Set_CameraMode(_iCameraMode);
	}
		return;
}

void CCamera_Manager::Change_CameraType(_uint _iCurrentCameraType, _bool _isInitialData, _float _fInitialTime)
{
	if (nullptr == m_Cameras[_iCurrentCameraType])
		return;

	for (auto& Camera : m_Cameras) {
		if (nullptr == Camera)
			continue;

		if (_iCurrentCameraType == Camera->Get_CamType())
		{
			Camera->Set_Active(true);
			Camera->Compute_FocalLength();
			Camera->Bind_DofConstBuffer();
		}
		else
		{
			Camera->Set_Active(false);
			Camera->Compute_FocalLength();
			Camera->Bind_DofConstBuffer();
		}

	}

	switch (_iCurrentCameraType) {
	case FREE:
	{
		if (nullptr == m_Cameras[TARGET])
			return;
		CController_Transform* pTargetamTrans = m_Cameras[TARGET]->Get_ControllerTransform();
		CController_Transform* pFreeCamTrans = m_Cameras[FREE]->Get_ControllerTransform();
		pFreeCamTrans->Set_WorldMatrix(pTargetamTrans->Get_WorldMatrix());
	}
	break;
	case TARGET_2D:
	{
		if (nullptr == m_Cameras[TARGET_2D])
			return;

		CGameObject* pPlayer = m_pGameInstance->Get_GameObject_Ptr(SECTION_MGR->Get_SectionLeveID(), L"Layer_Player", 0);
		
		if (nullptr != pPlayer)
			static_cast<CCamera_2D*>(m_Cameras[TARGET_2D])->Set_Include_Section_Name(pPlayer->Get_Include_Section_Name());
	}
	break;
	}

	if (true == _isInitialData) {
		INITIAL_DATA CurInitialData = m_Cameras[m_eCurrentCameraType]->Get_InitialData();
		CurInitialData.fInitialTime = _fInitialTime;
		m_Cameras[_iCurrentCameraType]->Switch_CameraView(&CurInitialData);
	}
	else 
		m_Cameras[_iCurrentCameraType]->Switch_CameraView(nullptr);
	
	m_eCurrentCameraType = _iCurrentCameraType;
}

void CCamera_Manager::Change_CameraTarget(const _float4x4* _pTargetWorldMatrix)
{
	if (nullptr == m_Cameras[TARGET] || TARGET != m_eCurrentCameraType)
		return;

	static_cast<CCamera_Target*>(m_Cameras[TARGET])->Change_Target(_pTargetWorldMatrix);
}

_bool CCamera_Manager::Set_NextArmData(_wstring _wszNextArmName, _int _iTriggerID)
{
	if (nullptr == m_Cameras[TARGET])
		return false;

	return static_cast<CCamera_Target*>(m_Cameras[TARGET])->Set_NextArmData(_wszNextArmName, _iTriggerID);
}

_bool CCamera_Manager::Set_NextCutSceneData(_wstring _wszCutSceneName)
{
	if (nullptr == m_Cameras[CUTSCENE])
		return false;

	return static_cast<CCamera_CutScene*>(m_Cameras[CUTSCENE])->Set_NextCutScene(_wszCutSceneName);
}

void CCamera_Manager::Set_PreArmDataState(_int _iTriggerID, _bool _isReturn)
{
	if (nullptr == m_Cameras[TARGET])
		return;

	static_cast<CCamera_Target*>(m_Cameras[TARGET])->Set_PreArmDataState(_iTriggerID, _isReturn);
}

void CCamera_Manager::Set_FreezeEnter(_uint _iFreezeMask, _fvector _vExitArm, _int _iTriggerID)
{
	if (nullptr == m_Cameras[TARGET])
		return;

	static_cast<CCamera_Target*>(m_Cameras[TARGET])->Set_FreezeEnter(_iFreezeMask, _vExitArm, _iTriggerID);
}

void CCamera_Manager::Set_FreezeExit(_uint _iFreezeMask, _int _iTriggerID)
{
	if (nullptr == m_Cameras[TARGET])
		return;

	static_cast<CCamera_Target*>(m_Cameras[TARGET])->Set_FreezeExit(_iFreezeMask, _iTriggerID);
}

void CCamera_Manager::Start_Zoom(CAMERA_TYPE _eCameraType, _float _fZoomTime, _uint _iZoomLevel, _uint _iRatioType)
{
	m_Cameras[_eCameraType]->Start_Zoom(_fZoomTime, (CCamera::ZOOM_LEVEL)_iZoomLevel, (RATIO_TYPE)_iRatioType);
}

void CCamera_Manager::Start_Changing_AtOffset(CAMERA_TYPE _eCameraType, _float _fOffsetTime, _vector _vNextOffset, _uint _iRatioType)
{
	if (FREE == m_eCurrentCameraType)
		return;

	m_Cameras[_eCameraType]->Start_Changing_AtOffset(_fOffsetTime, _vNextOffset, _iRatioType);
}

void CCamera_Manager::Start_Shake_ByTime(CAMERA_TYPE _eCameraType, _float _fShakeTime, _float _fShakeForce, _float _fShakeCycleTime, _uint _iShakeType, _float _fDelayTime)
{
	if (FREE == m_eCurrentCameraType)
		return;

	m_Cameras[_eCameraType]->Start_Shake_ByTime(_fShakeTime, _fShakeForce, _fShakeCycleTime, (CCamera::SHAKE_TYPE)_iShakeType, _fDelayTime);
}

void CCamera_Manager::Start_Shake_ByCount(CAMERA_TYPE _eCameraType, _float _fShakeTime, _float _fShakeForce, _uint _iShakeCount, _uint _iShakeType, _float _fDelayTime)
{
	if (FREE == m_eCurrentCameraType)
		return;

	m_Cameras[_eCameraType]->Start_Shake_ByCount(_fShakeTime, _fShakeForce, _iShakeCount, (CCamera::SHAKE_TYPE)_iShakeType, _fDelayTime);
}

void CCamera_Manager::Start_FadeIn(_float _fFadeTime)
{
	m_Cameras[m_eCurrentCameraType]->Start_PostProcessing_Fade(CCamera::FADE_IN, _fFadeTime);
}

void CCamera_Manager::Start_FadeOut(_float _fFadeTime)
{
	m_Cameras[m_eCurrentCameraType]->Start_PostProcessing_Fade(CCamera::FADE_OUT, _fFadeTime);
}

void CCamera_Manager::Load_ArmData(_wstring _sz3DFileName, _wstring _sz2DFileName)
{
	_wstring wszLoadPath = L"../Bin/DataFiles/Camera/ArmData/";
	Load_ArmData(TARGET, wszLoadPath + _sz3DFileName);
	Load_ArmData(TARGET_2D, wszLoadPath + _sz2DFileName);
}

void CCamera_Manager::Load_CutSceneData()
{
	_wstring wszLoadPath = L"../Bin/DataFiles/Camera/CutSceneData/Chapter2_CutScene.json";

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

	if (Result.is_array()) {
		for (auto& CutScene_json : Result) {
			pair<CUTSCENE_SUB_DATA, vector<CUTSCENE_DATA*>> CutSceneData;
			
			// CutScene Tag 읽기
			_string szCutSceneTag = CutScene_json["CutScene_Tag"];
			// CutScene Total Time 읽기
			CutSceneData.first.fTotalTime = { CutScene_json["CutScene_Time"][0].get<_float>(), CutScene_json["CutScene_Time"][1].get<_float>() };
			// Next CameraType
			CutSceneData.first.iNextCameraType = CutScene_json["CutScene_Next_CameraType"];

			// CutScene Data 읽기
			if (CutScene_json["Datas"].is_array()) {
				for (auto& Data : CutScene_json["Datas"]) {
					CUTSCENE_DATA* pData = new CUTSCENE_DATA();

					pData->vPosition = { Data["Position"][0].get<_float>(), Data["Position"][1].get<_float>(), Data["Position"][2].get<_float>() };
					pData->vRotation = { Data["Rotation"][0].get<_float>(), Data["Rotation"][1].get<_float>(), Data["Rotation"][2].get<_float>() };
					pData->vAt = { Data["At"][0].get<_float>(), Data["At"][1].get<_float>(), Data["At"][2].get<_float>() };
					pData->fFovy = Data["Fovy"];

					CutSceneData.second.push_back(pData);
				}
			}

			Add_CutScene(m_pGameInstance->StringToWString(szCutSceneTag), CutSceneData);
		}
	}
}

HRESULT CCamera_Manager::Load_DOF(const _wstring& _strJsonPath, _uint _iCameraType)
{
	if (nullptr == m_Cameras[_iCameraType])
		return E_FAIL;

	m_Cameras[_iCameraType]->Load_DOF(_strJsonPath);

	return S_OK;
}

CCamera_Manager* CCamera_Manager::Create()
{
	CCamera_Manager* pInstance = new CCamera_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CCamera_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Manager::Free()
{
	Safe_Release(m_pGameInstance);

	for (auto& Camera : m_Cameras)
		Safe_Release(Camera);

	__super::Free();
}
