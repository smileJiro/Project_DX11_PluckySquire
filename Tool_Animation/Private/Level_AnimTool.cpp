#include "stdafx.h"
#include "Level_AnimTool.h"
#include "GameInstance.h"
#include "ModelObject.h"
#include "Test2DModel.h"
#include "Test3DModel.h"
#include "TestModelObject.h"
#include "Camera_Target.h"
#include "CameraArm.h"
#include "TestTerrain.h"
#include "AnimEventGenerator.h"
#include "Backgorund.h"

CLevel_AnimTool::CLevel_AnimTool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_AnimTool::Initialize()
{
	m_fDefault2DCamSize = m_pGameInstance->Get_RT_Size(L"Target_Book_2D");

	Ready_Lights();
	CModelObject::MODELOBJECT_DESC tModelObjDesc = {};
	tModelObjDesc.Build_2D_Model(LEVEL_ANIMTOOL, TEXT("Prototype_Component_Background"), TEXT("Prototype_Component_Shader_VtxPosTex"), (_uint)PASS_VTXPOSTEX::SPRITE2D, false);
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_ANIMTOOL, TEXT("Prototype_GameObject_BackGround"), LEVEL_ANIMTOOL, TEXT("Background"), (CGameObject**)&m_pBackground, &tModelObjDesc)))
	{
		Safe_Release(m_pBackground);
		return E_FAIL;
	}
	m_pBackground->Get_ControllerTransform()->Set_Scale({g_iWinSizeX,g_iWinSizeY,1});
	//Ready_Layer_TestTerrain(TEXT("Terrain"));
	//Create_Camera(TEXT("Camera"));
	SetWindowText(g_hWnd, TEXT("애니메이션 툴입니다."));
	return S_OK;
}

void CLevel_AnimTool::Update(_float _fTimeDelta)
{

	ImGui::Begin("Animation Tool");
	Update_ImportImgui();
	Update_ExportImgui();
	if (m_pTestModelObj )
	{
		ImGui::Separator();
		Update_AnimationEditImgui();
	}
	ImGui::End();
	ImGui::Begin("Animation Event");
	Update_AnimEventImgui();
	ImGui::End();

	//CONTROLL
	if (m_pTestModelObj)
	{
		_float fMove = (_float)MOUSE_MOVE(MOUSE_AXIS::Z) / 100.f;
		if (COORDINATE_3D == m_pTestModelObj->Get_CurCoord())
		{
			m_fZoomMultiplier += fMove * m_f3DZoomSpeed * _fTimeDelta;
			m_fZoomMultiplier = max(m_fZoomMultiplier, 0.2f);
			m_fZoomMultiplier = min(m_fZoomMultiplier, 3.f);
			m_pTargetCam->Set_Fovy(m_fZoomMultiplier * m_fDefault3DCamFovY);
		}
		else
		{
			m_fZoomMultiplier += fMove * m_f2DZoomSpeed * _fTimeDelta;
			m_fZoomMultiplier = max(m_fZoomMultiplier, 0.2f);
			m_pTestModelObj->Set_2DProjMatrix(XMMatrixOrthographicLH((_float)m_fDefault2DCamSize.x * m_fZoomMultiplier, (_float)m_fDefault2DCamSize.y * m_fZoomMultiplier, 0.0f, 1.0f));
		}
	}
}

HRESULT CLevel_AnimTool::Render()
{
#ifdef _DEBUG
	if (m_pTestModelObj)
		SetWindowText(g_hWnd, m_szLoadedPath.c_str());
#endif

	return S_OK;
}

void CLevel_AnimTool::Update_ImportImgui()
{
	LOADMODEL_TYPE	eLoadModelTYpe = LOADMODEL_TYPE::LOAD_LAST;
	std::wstring wstrSelectedPath = TEXT("");

	//IMPORT
	if (ImGui::Button("Open Model File")) {
		wstrSelectedPath = OpenFileDialog(TEXT("Model3D Files\0 * .model\0Model2D Files\0 * .model2d")); // 파일 다이얼로그 호출
		if (false == wstrSelectedPath.empty()) {
			if (wstrSelectedPath.find(L".model2d") != std::wstring::npos)
			{
				eLoadModelTYpe = LOAD_2D;
			}
			else if (wstrSelectedPath.find(L".model") != std::wstring::npos)
			{
				eLoadModelTYpe = LOAD_3D;
			}
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Open Raw 2DModel Data")) {
		wstrSelectedPath = OpenDirectoryDialog(); // 폴더 다이얼로그 호출
		if (false == wstrSelectedPath.empty())
		{
			wstrSelectedPath += L"\\";
			eLoadModelTYpe = LOAD_RAW2D;
		}
	}
	if (LOAD_LAST != eLoadModelTYpe)
	{
		if (SUCCEEDED(Load_Model(eLoadModelTYpe, wstrSelectedPath)))
		{
			m_szLoadedPath = wstrSelectedPath;
			Create_Camera(TEXT("Camera"), m_pTestModelObj);
			m_iCurrentAnimIndex = 0;
			m_bLoop = m_pTestModelObj->Is_LoopAnimation(m_pTestModelObj->Get_CurCoord(),m_iCurrentAnimIndex);
			m_fAnimaSpeedMagnifier = m_pTestModelObj->Get_AnimSpeedMagnifier(m_pTestModelObj->Get_CurCoord(), m_iCurrentAnimIndex);
			Set_Animation(m_iCurrentAnimIndex, m_bLoop);
		}
	}
}

void CLevel_AnimTool::Update_ExportImgui()
{
	wstring wstrSelectedPath = TEXT("");
	if (ImGui::Button("Convert SingleSprite2DModels")) {
		wstrSelectedPath = OpenDirectoryDialog(); // 폴더 다이얼로그 호출
		if (false == wstrSelectedPath.empty())
		{
			wstrSelectedPath += L"\\";
			Convert_SingleSprite2DModels(wstrSelectedPath);
		}
	}
	//EXPORT
	if (nullptr != m_pTestModelObj)
	{
		wstrSelectedPath = TEXT("");
		if (ImGui::Button("Export Model")) {
			wstring wstrFilter = COORDINATE_2D == m_pTestModelObj->Get_CurCoord() ? TEXT("Model2D Files\0 * .model2d") : TEXT("Model3D Files\0 * .model");
			wstrSelectedPath = SaveFileDialog(wstrFilter.c_str()); // 파일 다이얼로그 호출
		}
		ImGui::SameLine();
		ImGui::Checkbox("Copy Textures", &m_bExportTextures);

		if (false == wstrSelectedPath.empty())
		{
			Export_Model(wstrSelectedPath);
			if (m_bExportTextures)
			{
				std::filesystem::path pathDstPath = wstrSelectedPath;
				Copy_Textures(m_pTestModelObj, m_szLoadedPath.remove_filename(), pathDstPath.remove_filename());
			}
		}
	}

}

void CLevel_AnimTool::Update_AnimationEditImgui()
{
	if (nullptr != m_pTestModelObj)
	{
		COORDINATE eCoord = m_pTestModelObj->Get_CurCoord();
		CModel* pModel = m_pTestModelObj->Get_Model(eCoord);
		if(false == pModel->Is_AnimModel())
			return;
		ImGui::Text("Animation Controller");
		ImGui::Separator();
		if (ImGui::Checkbox("Loop", &m_bLoop))
		{
			Set_Animation(m_iCurrentAnimIndex, m_bLoop);
		}
		if (ImGui::InputInt("Current Animation", &m_iCurrentAnimIndex))
		{
			m_iCurrentAnimIndex = max(0, m_iCurrentAnimIndex);
			m_iCurrentAnimIndex = min(m_iCurrentAnimIndex, (_int)m_pTestModelObj->Get_AnimationCount()-1);
			m_bLoop = m_pTestModelObj->Is_LoopAnimation(m_pTestModelObj->Get_CurCoord(), m_iCurrentAnimIndex);
			m_fAnimaSpeedMagnifier = m_pTestModelObj->Get_AnimSpeedMagnifier(m_pTestModelObj->Get_CurCoord(), m_iCurrentAnimIndex);
			Set_Animation(m_iCurrentAnimIndex, m_bLoop);
		}
		if (ImGui::Button(m_bPlaying ? "Stop" :"Play")) 
		{
			m_bPlaying ^= 1;
			m_pTestModelObj->Set_PlayingAnim(m_bPlaying);
		}
		ImGui::SameLine();
		if (ImGui::SliderFloat("Progress", &m_fCurrentProgerss, 0.f, 1.f))
			m_pTestModelObj->Set_Progerss(m_fCurrentProgerss);
		else
			m_fCurrentProgerss = m_pTestModelObj->Get_Progress();
		if (ImGui::InputFloat("AnimSpeedMagnify", &m_fAnimaSpeedMagnifier))
		{
			m_pTestModelObj->Set_AnimSpeedMagnifier(m_pTestModelObj->Get_CurCoord(),m_iCurrentAnimIndex, m_fAnimaSpeedMagnifier);
		}



		_uint iAnimIdx = 0;
		if (ImGui::BeginListBox("AnimationList"))
		{
			list<string> AnimNames;
			m_pTestModelObj->Get_AnimatinNames(AnimNames);
			for (auto& strAnimationName : AnimNames)
			{
				_bool isSelected = iAnimIdx == m_iCurrentAnimIndex;
				string strLable = to_string(iAnimIdx) + " " + strAnimationName;
				if (ImGui::Selectable(strLable.c_str(), isSelected))
				{
					m_iCurrentAnimIndex = iAnimIdx; // 선택된 항목 갱신
					m_bLoop = m_pTestModelObj->Is_LoopAnimation(m_pTestModelObj->Get_CurCoord(), m_iCurrentAnimIndex);
					m_fAnimaSpeedMagnifier = m_pTestModelObj->Get_AnimSpeedMagnifier(m_pTestModelObj->Get_CurCoord(), m_iCurrentAnimIndex);
					Set_Animation(m_iCurrentAnimIndex, m_bLoop);
				}
				// 선택된 항목 강조
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
				iAnimIdx++;
			}
			ImGui::EndListBox();
		}
	}
}

void CLevel_AnimTool::Update_AnimEventImgui()
{

	if (ImGui::Button("Open AnimEvents File"))
	{
		wstring wstrSelectedPath = OpenFileDialog(TEXT("AnimEvent Files\0 * .animevt")); // 파일 다이얼로그 호출
		if (false == wstrSelectedPath.empty())
		{
			if(FAILED(Load_AnimEvents(wstrSelectedPath)))
			{
				MSG_BOX("Failed to Load AnimEvents");
			}
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Export AnimEvents")) 
	{
		wstring wstrSelectedPath = SaveFileDialog(TEXT("AnimEvent Files\0 * .animevt")); // 파일 다이얼로그 호출

		if (false == wstrSelectedPath.empty())
		{
			Export_AnimEvents(wstrSelectedPath);
		}
	}
	ImGui::Separator();

	ImGui::Text("Animation Index : %d", m_iCurrentAnimIndex);
	ImGui::Text("Progress : %f", m_fCurrentProgerss);

	if (ImGui::Button("Add AnimEvent"))
	{
		ANIM_EVENT tEvent{};
		tEvent.fProgress = m_fCurrentProgerss;
		tEvent.iAnimIndex = m_iCurrentAnimIndex;
		m_AnimEvents[m_iCurrentAnimIndex].push_back(tEvent);
	}
	ImGui::SameLine();
	if (ImGui::Button("Remove AnimEvent"))
	{
		if (m_iSelectedAnimEventIndex != -1)
		{
			_int iTmpIndex = m_iSelectedAnimEventIndex;
			for (auto& pairAnimEvt : m_AnimEvents)
			{
				if ((iTmpIndex - (_int)pairAnimEvt.second.size()) < 0)
				{
					pairAnimEvt.second.erase(pairAnimEvt.second.begin() + iTmpIndex);
					break;
				}
				iTmpIndex -= (_int)pairAnimEvt.second.size();
			}
		}
	}
	ImGui::Separator();

	_uint iEventIdx = 0;
	if (ImGui::BeginListBox("AnimEventList")) 
	{
		for (auto& pairAnimEvt : m_AnimEvents)
		{
			for (auto& tEvent : pairAnimEvt.second)
			{
				_bool isSelected = iEventIdx == m_iSelectedAnimEventIndex;
				string strLable = to_string(iEventIdx) +" AnimIdx : " + to_string(pairAnimEvt.first) +"/ Progress : "+ to_string(tEvent.fProgress);
				if (ImGui::Selectable(strLable.c_str(), isSelected))
				{
					m_iSelectedAnimEventIndex = iEventIdx; // 선택된 항목 갱신
				}
				// 선택된 항목 강조
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
				iEventIdx++;
			}
		}
		ImGui::EndListBox();
	}
	ImGui::Separator();
	if (m_iSelectedAnimEventIndex != -1)
	{
		ANIM_EVENT* tEvent = nullptr;
		_int iTmpIdx = m_iSelectedAnimEventIndex;
		for (auto& pairAnimEvt : m_AnimEvents)
		{
			if ((iTmpIdx - (_int)pairAnimEvt.second.size()) < 0)
			{
				tEvent = &pairAnimEvt.second[iTmpIdx];
				break;
			}
			iTmpIdx -= (_int)pairAnimEvt.second.size();
		}
		if (tEvent)
		{
			ImGui::Text("Selected Animation Event Data");
			_int iAniimIndex = (_uint)tEvent->iAnimIndex;
			if (ImGui::InputInt("Animation Index", &iAniimIndex))
			{
				_int iClampedAnimIndex = iAniimIndex;
				iClampedAnimIndex = max(0, iAniimIndex);
				if(m_pTestModelObj)
					iClampedAnimIndex = min(iAniimIndex, (_int)m_pTestModelObj->Get_AnimationCount() - 1);
				m_AnimEvents[iClampedAnimIndex].push_back(*tEvent);
				m_AnimEvents[(_uint)tEvent->iAnimIndex].erase(m_AnimEvents[(_uint)tEvent->iAnimIndex].begin() + iTmpIdx);
				tEvent = &m_AnimEvents[iClampedAnimIndex].back();
				tEvent->iAnimIndex = iClampedAnimIndex;
			}
			_float fProgress = tEvent->fProgress;
			if (ImGui::InputFloat("Progress", &fProgress))
			{
				fProgress = max(0, fProgress);
				fProgress = fmin(fProgress, 1.f);
				tEvent->fProgress = fProgress;
			}

			char buffer[128] = "";
			strcpy_s(buffer, tEvent->strFuncName.c_str());
			if (ImGui::InputText("FunctionName", buffer, 128)) {
				tEvent->strFuncName = buffer;
			}
		}

	}
}

void CLevel_AnimTool::Set_Animation(_uint _iAnimIdx, _bool _bLoop)
{
	assert(m_pTestModelObj);
	m_pTestModelObj->Switch_Animation(_iAnimIdx);
	m_pTestModelObj->Set_AnimationLoop(m_pTestModelObj->Get_CurCoord(), _iAnimIdx, _bLoop);

}


HRESULT CLevel_AnimTool::Ready_Lights()
{

	CONST_LIGHT LightDesc{};

	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.vDirection = { 0.0f, -1.0f, 1.0f };
	LightDesc.vRadiance = _float3(1.0f, 1.0f, 1.0f);
	LightDesc.vDiffuse = _float4(0.7f, 0.7f, 0.7f, 1.0f);
	LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 1.0f);
	LightDesc.vSpecular = _float4(1.0f, 1.0f, 1.0f, 1.0f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc, LIGHT_TYPE::DIRECTOINAL)))
		return E_FAIL;

	/* 점광원 */
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


	return S_OK;
}



HRESULT CLevel_AnimTool::Create_Camera(const _wstring& _strLayerTag, CGameObject* _pTarget)
{
	// Target Camera
	assert(nullptr != m_pTestModelObj);
	if(m_pTargetCam)
		m_pTargetCam->Set_Dead();
	m_pTargetCam = nullptr;
	CCamera_Target::CAMERA_TARGET_DESC TargetDesc{};

	TargetDesc.fSmoothSpeed = 5.f;
	TargetDesc.eCameraMode = CCamera_Target::DEFAULT;
	TargetDesc.iCameraType = 1;
	TargetDesc.vAtOffset = _float3(0.0f, 0.5f, 0.0f);
	TargetDesc.pTargetWorldMatrix = _pTarget->Get_ControllerTransform()->Get_WorldMatrix_Ptr();

	TargetDesc.fFovy = XMConvertToRadians(60.f);
	TargetDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	TargetDesc.fNear = 0.1f;
	TargetDesc.fFar = 1000.f;
	TargetDesc.vEye = _float3(0.f, 10.f, -7.f);
	TargetDesc.vAt = _float3(0.f, 0.f, 0.f);
	TargetDesc.eZoomLevel = CCamera::LEVEL_6;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_ANIMTOOL, TEXT("Prototype_GameObject_Camera_Target"),
		LEVEL_ANIMTOOL, _strLayerTag, (CGameObject**)&m_pTargetCam, &TargetDesc)))
		return E_FAIL;

	_vector vTargetLook = _pTarget->Get_ControllerTransform()->Get_State(CTransform::STATE_LOOK);
	CCameraArm::CAMERA_ARM_DESC Desc{};

	XMStoreFloat3(&Desc.vArm, vTargetLook);
	Desc.vPosOffset = { 0.f, 0.f, 0.f };
	Desc.vRotation = { XMConvertToRadians(-30.f), XMConvertToRadians(0.f), 0.f };
	Desc.fLength = 10.f;
	Desc.wszArmTag = TEXT("Cam_Arm");

	CCameraArm* pArm = CCameraArm::Create(m_pDevice, m_pContext, &Desc);

	m_pTargetCam->Add_Arm(pArm);

	return S_OK;
}

HRESULT CLevel_AnimTool::Load_Model(LOADMODEL_TYPE _eType, wstring _wstrPath)
{
	
	assert(LOAD_LAST != _eType);
	assert(!_wstrPath.empty());

	SetWindowText(g_hWnd, TEXT("모델(을)를 로딩중입니다."));

	CModel* pTmpModel = nullptr;
	switch (_eType)
	{
	case AnimTool::CLevel_AnimTool::LOAD_3D:
	{
		XMMATRIX matPretransform = XMMatrixScaling(1 / 150.0f, 1 / 150.0f, 1 / 150.0f);
		matPretransform *= XMMatrixRotationAxis(_vector{ 0,1,0,0 }, XMConvertToRadians(180));
		pTmpModel = CTest3DModel::Create(m_pDevice, m_pContext, WstringToString(_wstrPath).c_str(), matPretransform);
		break;
	}
	case AnimTool::CLevel_AnimTool::LOAD_2D:
	{
		pTmpModel = CTest2DModel::Create(m_pDevice, m_pContext, false, WstringToString(_wstrPath).c_str());
		break;
	}
	case AnimTool::CLevel_AnimTool::LOAD_RAW2D:
	{
		pTmpModel = CTest2DModel::Create(m_pDevice, m_pContext, true, WstringToString(_wstrPath).c_str());
		break;
	}
	case AnimTool::CLevel_AnimTool::LOAD_LAST:
	default:
		break;
	}

	CTestModelObject::TESTMODELOBJ_DESC tModelObjDesc{};
	tModelObjDesc.isCoordChangeEnable = false;
	tModelObjDesc.iCurLevelID = LEVEL_ANIMTOOL;
	switch (_eType)
	{
	case AnimTool::CLevel_AnimTool::LOAD_3D:
		if(pTmpModel->Is_AnimModel())
		{
			tModelObjDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxAnimMesh");
			tModelObjDesc.iShaderPass_3D = (_uint)PASS_VTXANIMMESH::DEFAULT;
		}
		else
		{
			tModelObjDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
			tModelObjDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;
		}
		tModelObjDesc.tTransform3DDesc.vInitialPosition = _float3(0, 0, 0);
		tModelObjDesc.tTransform3DDesc.vInitialScaling = _float3(1, 1, 1);
		tModelObjDesc.eStartCoord = COORDINATE_3D;
		break;
	case AnimTool::CLevel_AnimTool::LOAD_2D:
	case AnimTool::CLevel_AnimTool::LOAD_RAW2D:
		tModelObjDesc.tTransform2DDesc.vInitialPosition = _float3(0, 0, 0);
		tModelObjDesc.tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);
		tModelObjDesc.eStartCoord = COORDINATE_2D;
		tModelObjDesc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
		tModelObjDesc.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
		break;
	case AnimTool::CLevel_AnimTool::LOAD_LAST:
		break;
	default:
		break;
	}


	
	if (pTmpModel)
	{
		if(FAILED(pTmpModel->Initialize(nullptr)))
		{
			Safe_Release(pTmpModel);
			return E_FAIL;
		}
		if (m_pTestModelObj)
			m_pTestModelObj->Set_Dead();

		tModelObjDesc.pModel = pTmpModel;
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_TestModelObject"), LEVEL_ANIMTOOL, TEXT("MainModel"), (CGameObject**)&m_pTestModelObj, &tModelObjDesc)))
		{
			Safe_Release(pTmpModel);
			return E_FAIL;
		}
		

		return S_OK;
	}
	else
		return E_FAIL;
}

HRESULT CLevel_AnimTool::Export_Model(const wstring& _wstrPath)
{

	assert(m_pTestModelObj);
	wstring wstrExt = COORDINATE_2D == m_pTestModelObj->Get_CurCoord() ? L".model2d" : L".model";
	std::filesystem::path path = _wstrPath;
	if (path.extension().wstring() != wstrExt)
	{
		path.replace_extension(wstrExt);
	}
	std::ofstream outFile(path, std::ios::binary);
	if (!outFile)
	{
		MSG_BOX("파일 열기 실패.");
	}

	if (FAILED(m_pTestModelObj->Export_Model(outFile, path.remove_filename().string().c_str(), m_bExportTextures)))
	{
		MSG_BOX("내보내기 실패.");
		outFile.close();
		return E_FAIL;
	}
	outFile.close();
	return S_OK;
}


HRESULT CLevel_AnimTool::Load_AnimEvents(wstring _wstrPath)
{
	assert(!_wstrPath.empty());

	SetWindowText(g_hWnd, TEXT("모델(을)를 로딩중입니다."));

	std::ifstream inFile(_wstrPath, std::ios::binary);
	if (!inFile) {
		string str = "파일을 열 수 없습니다.";
		str += WstringToString( _wstrPath);
		MessageBoxA(NULL, str.c_str(), "에러", MB_OK);
		inFile.close();
		return E_FAIL;
	}
	m_AnimEvents.clear();
	_uint iAnimIndexCount = 0;
	inFile.read(reinterpret_cast<char*>(&iAnimIndexCount), sizeof(_uint));
	for (_uint i = 0; i < iAnimIndexCount; i++)
	{
		_uint iAnimIndex = 0;
		_uint iEventCount = 0;
		inFile.read(reinterpret_cast<char*>(&iAnimIndex), sizeof(_uint));
		inFile.read(reinterpret_cast<char*>(&iEventCount), sizeof(_uint));
		m_AnimEvents.emplace(iAnimIndex, vector<ANIM_EVENT>());
		m_AnimEvents[iAnimIndex].resize(iEventCount);
		for (_uint j = 0; j < iEventCount; j++)
		{
			m_AnimEvents[iAnimIndex][j].ReadFile(inFile);
		}
	}

	inFile.close();

	return S_OK;
}

HRESULT CLevel_AnimTool::Export_AnimEvents(const wstring& _wstrPath)
{
	if (m_AnimEvents.empty())
	{
		MSG_BOX("이벤트가 없는데요?");
		return E_FAIL;
	}
	wstring wstrExt = TEXT(".animevt");
	std::filesystem::path path = _wstrPath;
	if (path.extension().wstring() != wstrExt)
	{
		path.replace_extension(wstrExt);
	}
	std::ofstream outFile(path, std::ios::binary);
	if (!outFile)
	{
		outFile.close();
		MSG_BOX("파일 열기 실패.");
		return E_FAIL;
	}
	_uint iAnimIndexCount = (_uint)m_AnimEvents.size();
	outFile.write(reinterpret_cast<char*>(&iAnimIndexCount), sizeof(_uint));
	for (auto& pairAnimEvt : m_AnimEvents)
	{
		_uint iAnimIndex = pairAnimEvt.first;
		_uint iEventCount = (_uint)pairAnimEvt.second.size();
		outFile.write(reinterpret_cast<char*>(&iAnimIndex), sizeof(_uint));
		outFile.write(reinterpret_cast<char*>(&iEventCount), sizeof(_uint));
		for (auto& tEvent : pairAnimEvt.second)
		{
			tEvent.WriteFile(outFile);
		}
	}
	outFile.close();
	return S_OK;
}

HRESULT CLevel_AnimTool::Convert_SingleSprite2DModels(const wstring& _wstrPath)
{
	std::filesystem::path path;
	path = _wstrPath;
	for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
		if (".json" != entry.path().extension())
			continue;
		
		json jFile;
		std::ifstream input_file(entry.path());
		if (!input_file.is_open())
			return E_FAIL;
		input_file >> jFile;
		input_file.close();

		for (auto& jObj : jFile)
		{
			string strType = jObj["Type"];
			if (false == strType._Equal("PaperSprite"))
				continue;

			CTest2DModel* pTmpModel = nullptr;
			pTmpModel = CTest2DModel::Create(m_pDevice, m_pContext, path.remove_filename(), jFile);
			if (nullptr == pTmpModel)
			{
				MSG_BOX("2D모델 만들기 실패.");
				return E_FAIL;
			}
			pTmpModel->Initialize(nullptr);
			std::filesystem::path strNewPath = path.remove_filename();
			strNewPath += entry.path().filename().replace_extension(".model2d");
			std::ofstream outFile(strNewPath, std::ios::binary);
			if (!outFile)
			{
				MSG_BOX("파일 열기 실패.");
				outFile.close();
				return E_FAIL;
			}
			pTmpModel->Export_Model(outFile);
			outFile.close();
		}
		
	}
	return S_OK;
}

HRESULT CLevel_AnimTool::Copy_Textures(CTestModelObject* _pModel, std::filesystem::path& _wstrSrcPath, std::filesystem::path& _wstrDstPath)
{
	assert(_pModel);
	assert(!_wstrSrcPath.empty());
	assert(!_wstrDstPath.empty());

	_pModel->Copy_Textures( _wstrDstPath);

	return S_OK;
}

CLevel_AnimTool* CLevel_AnimTool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLevel_AnimTool* pInstance = new CLevel_AnimTool(_pDevice, _pContext);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_AnimTool");
		Safe_Release(pInstance);
	}
	return pInstance;

}

void CLevel_AnimTool::Free()
{
	//Safe_Release(m_pTestModelObj);
	__super::Free();
}
