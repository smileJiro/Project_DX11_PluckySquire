#include "..\Public\GameInstance.h"

#include "Graphic_Device.h"
#include "Timer_Manager.h"
#include "Light_Manager.h"
#include "Level_Manager.h"
#include "Prototype_Manager.h"
#include "Object_Manager.h"
#include "Key_Manager.h"
#include "PipeLine.h"
#include "Font_Manager.h"
#include "Target_Manager.h"
#include "Sound_Manager.h"
#include "Json_Manager.h"
#include "Imgui_Manager.h"
#include "Camera_Manager_Engine.h"
#include "Physx_Manager.h"
#include "NewRenderer.h"
#include "Frustum.h"
#include "CubeMap.h"
#include "GlobalFunction_Manager.h"
#include "Collision_Manager.h"

#include "Physx_EventCallBack.h"
#include "Layer.h"
#include "ModelObject.h"
#include "ContainerObject.h"


IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
}

HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	m_hWnd = EngineDesc.hWnd;
	m_hInstance = EngineDesc.hInst;
	m_iViewportWidth = EngineDesc.iViewportWidth;
	m_iViewportHeight = EngineDesc.iViewportHeight;
	m_iStaticLevelID = EngineDesc.iStaticLevelID;
	m_isNewRenderer = EngineDesc.isNewRenderer;
	m_eImportMode = (IMPORT_MODE)EngineDesc.eImportMode;

	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.isWindowed, EngineDesc.iViewportWidth, EngineDesc.iViewportHeight, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pPhysx_Manager = CPhysx_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pPhysx_Manager)
		return E_FAIL;

	_uint iNumThreads = m_pPhysx_Manager->Get_NumThreads();
	unsigned int numCores = std::thread::hardware_concurrency();
	std::cout << "Logical cores: " << numCores << std::endl;
	_uint iNumThreadPoolSize = numCores - iNumThreads;
	m_pThreadPool = CThreadPool::Create(iNumThreadPoolSize);
	if (nullptr == m_pThreadPool)
		return E_FAIL;

	m_pD3DUtils = CD3DUtils::Create(*ppDevice, *ppContext);
	if (nullptr == m_pD3DUtils)
		return E_FAIL;

	m_pLight_Manager = CLight_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	m_pTarget_Manager = CTarget_Manager::Create(*ppDevice, *ppContext); // Renderer 전에 위치해야함. 
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	if (false == m_isNewRenderer)
	{
		m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
		if (nullptr == m_pRenderer)
			return E_FAIL;
	}
	else
	{
		m_pNewRenderer = CNewRenderer::Create(*ppDevice, *ppContext);
		if (nullptr == m_pNewRenderer)
			return E_FAIL;
	}

	
	m_pPrototype_Manager = CPrototype_Manager::Create(EngineDesc.iNumLevels, EngineDesc.iStaticLevelID);
	if (nullptr == m_pPrototype_Manager)
		return E_FAIL;

	m_pObject_Manager = CObject_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	
	m_pLevel_Manager = CLevel_Manager::Create(EngineDesc.iNumLevels, EngineDesc.ppLevelNames);
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pCollision_Manager = CCollision_Manager::Create();
	if (nullptr == m_pCollision_Manager)
		return E_FAIL;

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pKey_Manager = CKey_Manager::Create(EngineDesc.hInst, EngineDesc.hWnd);
	if (nullptr == m_pKey_Manager)
		return E_FAIL;

	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	m_pShadow = CShadow::Create(*ppDevice, *ppContext);
	if (nullptr == m_pShadow)
		return E_FAIL;

	m_pSound_Manager = CSound_Manager::Create(EngineDesc.hWnd);
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	m_pJson_Manager = CJson_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pJson_Manager)
		return E_FAIL;
	
	if ((_uint)IMPORT_MODE::IMPORT_IMGUI & (_uint)EngineDesc.eImportMode)
	{
		m_pImgui_Manager = CImgui_Manager::Create(EngineDesc.hWnd, *ppDevice, *ppContext, _float2((_float)EngineDesc.iViewportWidth, (_float)EngineDesc.iViewportHeight));
		if (nullptr == m_pImgui_Manager)
			return E_FAIL;
	}

	m_pGlobalFunction_Manager = CGlobalFunction_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pGlobalFunction_Manager)
		return E_FAIL;

	m_pCamera_Manager = CCamera_Manager_Engine::Create();
	if (nullptr == m_pCamera_Manager)
		return E_FAIL;

	m_pFrustum = CFrustum::Create();
	if (nullptr == m_pFrustum)
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Priority_Update_Engine(_float fTimeDelta)
{
	m_pTimer_Manager->Update(fTimeDelta);

	if (m_pImgui_Manager && m_pImgui_Manager->Is_ImguiFocused())
	{
		m_pKey_Manager->Set_EnableInputUpdate(false);
	}
	else
	{
		m_pKey_Manager->Set_EnableInputUpdate(true);
	}
	m_pKey_Manager->Update();
	m_pObject_Manager->Priority_Update(fTimeDelta);
	m_pSound_Manager->Update(fTimeDelta);

	m_pFrustum->Update();
}

void CGameInstance::Update_Engine(_float fTimeDelta)
{
	m_pObject_Manager->Update(fTimeDelta);
	//m_pCamera_Manager->Update(fTimeDelta);


	/* 아래부턴 Update와 LateUpdate의 사이 시점으로 본다.*/
	m_pLevel_Manager->Update(fTimeDelta); // 현재 여기서 Physx 돌리고있음.

	m_pCollision_Manager->Update();
}

void CGameInstance::Late_Update_Engine(_float fTimeDelta)
{
	m_pObject_Manager->Late_Update(fTimeDelta); // Late_Update 수행 후, DeadObject Safe_Release() + erase();

	m_pPipeLine->Update();
	m_pLight_Manager->Update(fTimeDelta);
#ifdef _DEBUG
	if (m_pNewRenderer)
	{
		m_pNewRenderer->Update_Imgui();
		if(nullptr != m_pImgui_Manager)
			m_pImgui_Manager->Imgui_Debug_Render();
	}
#endif


}

HRESULT CGameInstance::Render_Begin(const _float4& vClearColor)
{
	m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
	m_pGraphic_Device->Clear_DepthStencil_View();

	return S_OK;
}

HRESULT CGameInstance::Draw()
{
	if (false == m_isNewRenderer)
		m_pRenderer->Draw_RenderObject();
	else
	{
		m_pNewRenderer->Draw_RenderObject();
	}


	m_pLevel_Manager->Render();

	return S_OK;
}

HRESULT CGameInstance::Render_End()
{
	return m_pGraphic_Device->Present();
}

_float CGameInstance::Compute_Random_Normal()
{
	return (_float)rand() / RAND_MAX;
}

_float CGameInstance::Compute_Random(_float _fMin, _float _fMax)
{
	if (_fMin > _fMax)
		return _fMax + (_fMin - _fMax) * Compute_Random_Normal();
	return _fMin + (_fMax - _fMin) * Compute_Random_Normal();
}

HRESULT CGameInstance::Engine_Level_Enter(_int _iChangeLevelID)
{
	/* Engine Manager 의 Level Enter 시점. 이벤트 매니저에서 새로운 레벨을 생성한 후, 호출된다. */

	m_pCollision_Manager->Level_Enter();
	m_pPhysx_Manager->Level_Enter();
	return S_OK;
}

HRESULT CGameInstance::Engine_Level_Exit(_int _iChangeLevelID, _int _iNextChangeLevelID)
{
	_int iCurLevelID = m_pLevel_Manager->Get_CurLevelID();
	if (iCurLevelID == -1)
		return S_OK;

	/* _iNextChangeLevelID가 유효한 경우					>>>  _iChangeLevelID == LEVEL_LOADING */
	/* _iNextChangeLevelID가 유효하지 않은 경우.			>>>  _iChangeLevelID != LEVEL_LOADING */

	/* Engine Manager 의 Level Exit 시점. 이벤트 매니저에서 새로운 레벨을 생성하기 전, 호출된다. */
	m_pCollision_Manager->Level_Exit();


	m_pObject_Manager->Level_Exit((_uint)iCurLevelID);
	m_pPrototype_Manager->Level_Exit((_uint)iCurLevelID);
	m_pPhysx_Manager->Level_Exit();
	m_pLight_Manager->Level_Exit();
	m_pJson_Manager->Level_Exit(iCurLevelID);
	//m_pCollision_Manager->Level_Exit();

	return S_OK;
}



_float CGameInstance::Get_TimeDelta(const _wstring& _strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.f;

	return m_pTimer_Manager->Get_TimeDelta(_strTimerTag);
}

void CGameInstance::Render_FPS(const _wstring& _strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return;

	return m_pTimer_Manager->Render_FPS(m_hWnd, _strTimerTag);
}

_int CGameInstance::Get_FPS()
{
	if (nullptr == m_pTimer_Manager)
		return 0;

	return m_pTimer_Manager->Get_FPS();
}

_uint CGameInstance::Get_FPS(const _wstring& _strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0;

	return m_pTimer_Manager->Get_FPS(_strTimerTag);
}

HRESULT CGameInstance::Add_Timer(const _wstring& _strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timer(_strTimerTag);
}

void CGameInstance::Update_TimeDelta(const _wstring& _strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return;

	return m_pTimer_Manager->Update_Timer(_strTimerTag);
}

HRESULT CGameInstance::Set_TimeScale(_float _fTimeScale, const _wstring& _strTimeTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Set_TimeScale(_fTimeScale, _strTimeTag);
}

HRESULT CGameInstance::Reset_TimeScale(const _wstring& _strTimeTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Reset_TimeScale(_strTimeTag);
}

HRESULT CGameInstance::Level_Manager_Enter(_int _iLevelID, CLevel* _pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Level_Enter(_iLevelID, _pNewLevel);
}

HRESULT CGameInstance::Level_Manager_Exit(_int _iChangeLevelID, _int _iNextChangeLevelID)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Level_Exit();
}

_int CGameInstance::Get_CurLevelID() const
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Get_CurLevelID();
}

const vector<string>& CGameInstance::Get_LevelNames() const
{
	return m_pLevel_Manager->Get_LevelNames();
	// TODO: 여기에 return 문을 삽입합니다.
}

HRESULT CGameInstance::Add_Prototype(_uint _iLevelID, const _wstring& _strPrototypeTag, CBase* _pPrototype)
{
	if (nullptr == m_pPrototype_Manager)
		return E_FAIL;

	return m_pPrototype_Manager->Add_Prototype(_iLevelID, _strPrototypeTag, _pPrototype);
}

CBase* CGameInstance::Clone_Prototype(Engine::PROTOTYPE _eType, _uint _iLevelID, const _wstring& _strPrototypeTag, void* _pArg)
{
	if (nullptr == m_pPrototype_Manager)
		return nullptr;

	return m_pPrototype_Manager->Clone_Prototype(_eType, _iLevelID, _strPrototypeTag, _pArg);
}

CBase* CGameInstance::Find_Prototype(_uint _iLevelID, const _wstring& _strPrototypeTag)
{
	if (nullptr == m_pPrototype_Manager)
		return nullptr;

	return m_pPrototype_Manager->Find_Prototype(_iLevelID, _strPrototypeTag);
}

HRESULT CGameInstance::Add_GameObject_ToLayer(_uint _iPrototypeLevelID, const _wstring& _strPrototypeTag, _uint _iLevelID, const _wstring& _strLayerTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObject_ToLayer(_iPrototypeLevelID, _strPrototypeTag, _iLevelID, _strLayerTag, pArg);
}

HRESULT CGameInstance::Add_GameObject_ToLayer(_uint _iPrototypeLevelID, const _wstring& _strPrototypeTag, _uint _iLevelID, const _wstring& _strLayerTag, CGameObject** _ppOut,void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObject_ToLayer(_iPrototypeLevelID, _strPrototypeTag, _iLevelID, _strLayerTag, _ppOut, pArg);
}

HRESULT CGameInstance::Add_GameObject_ToLayer(_uint _iLevelID, const _wstring& _strLayerTag, CGameObject* _pGameObject)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;
	return m_pObject_Manager->Add_GameObject_ToLayer(_iLevelID, _strLayerTag, _pGameObject);
}

void CGameInstance::Set_Layer_Culling(_uint _iLevelID, const _wstring& _strLayerTag, _bool _isCulling)
{
	if (nullptr == m_pObject_Manager)
		return;
	return m_pObject_Manager->Set_Layer_Culling(_iLevelID, _strLayerTag, _isCulling);
}

_bool CGameInstance::Is_EmptyLayer(_uint _iLevelID, const _wstring& _strLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return false;
	return m_pObject_Manager->Is_EmptyLayer(_iLevelID, _strLayerTag);
}

CLayer* CGameInstance::Find_Layer(_uint _iLevelID, const _wstring& _strLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Find_Layer(_iLevelID, _strLayerTag);
}

CGameObject* CGameInstance::Get_PickingModelObjectByCursor(_uint _iLevelID, const _wstring& _strLayerTag, _float2 _fCursorPos)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_PickingModelObjectByCursor(_iLevelID, _strLayerTag, _fCursorPos);
}

CGameObject* CGameInstance::Find_NearestObject_Scaled(_uint _iLevelID, const _wstring& _strLayerTag, CController_Transform* const _pConTransform, CGameObject* pCurTargetObject)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	if (nullptr == pCurTargetObject)
	{
		return m_pObject_Manager->Find_NearestObject_Scaled(_iLevelID, _strLayerTag, _pConTransform);
	}

	return m_pObject_Manager->Find_NearestObject_Scaled(_iLevelID, _strLayerTag, _pConTransform, pCurTargetObject);
}

CGameObject* CGameInstance::Get_GameObject_Ptr(_int _iLevelID, const _wstring& _strLayerTag, _int _iObjectIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_GameObject_Ptr(_iLevelID, _strLayerTag, _iObjectIndex);
}

#ifdef _DEBUG
map<const _wstring, class CLayer*>* CGameInstance::Get_Layers_Ptr()
{
	return m_pObject_Manager->Get_Layers_Ptr();
}
#endif // _DEBUG



HRESULT CGameInstance::Add_RenderObject(CRenderer::RENDERGROUP _eRenderGroup, CGameObject* _pRenderObject)
{
	if (nullptr == m_pRenderer)
		return E_FAIL;

	return m_pRenderer->Add_RenderObject(_eRenderGroup, _pRenderObject);
}

#ifdef _DEBUG
HRESULT CGameInstance::Add_DebugComponent(CComponent* _pDebugCom)
{
	if (true == m_isNewRenderer)
	{
		Add_DebugComponent_New(_pDebugCom);
		return S_OK;
	}

	if (nullptr == m_pRenderer)
		return E_FAIL;

	return m_pRenderer->Add_DebugComponent(_pDebugCom);
}
void CGameInstance::Set_DebugRender(_bool _isBool)
{
	if (nullptr == m_pRenderer)
		return;

	return m_pRenderer->Set_DebugRender(_isBool);
}
#endif // _DEBUG
HRESULT CGameInstance::Add_RenderGroup(_int _iGroupID, _int _iPriorityID, CRenderGroup* _pRenderGroup)
{
	if (nullptr == m_pNewRenderer)
		return E_FAIL;


	return m_pNewRenderer->Add_RenderGroup(_iGroupID, _iPriorityID, _pRenderGroup);
}

CRenderGroup* CGameInstance::Find_RenderGroup(_int _iGroupID, _int _iPriorityID)
{
	if (nullptr == m_pNewRenderer)
		return nullptr;


	return m_pNewRenderer->Find_RenderGroup(_iGroupID, _iPriorityID);
}

HRESULT CGameInstance::Add_RenderObject_New(_int _iGroupID, _int _iPriorityID, CGameObject* _pGameObject)
{
	return m_pNewRenderer->Add_RenderObject(_iGroupID, _iPriorityID, _pGameObject);
}

HRESULT CGameInstance::Erase_RenderGroup_New(_int _iGroupID, _int _iPriorityID)
{
	return m_pNewRenderer->Erase_RenderGroup(_iGroupID, _iPriorityID);
}

void CGameInstance::Set_Active_RenderGroup_New(_int _iGroupID, _int _iPriorityID, _bool _isActive)
{
	m_pNewRenderer->Set_Active_RenderGroup(_iGroupID, _iPriorityID, _isActive);
}

HRESULT CGameInstance::Add_DSV_ToRenderer(const _wstring _strDSVTag, _float2 _vDSVSize)
{
	if (nullptr == m_pNewRenderer)
		return E_FAIL;

	return m_pNewRenderer->Add_DSV(_strDSVTag, _vDSVSize);
}

HRESULT CGameInstance::Add_DSV_ToRenderer(const _wstring _strDSVTag, _uint _iWidth, _uint _iHeight)
{
	if (nullptr == m_pNewRenderer)
		return E_FAIL;

	return m_pNewRenderer->Add_DSV(_strDSVTag, _iWidth, _iHeight);
}

HRESULT CGameInstance::Add_DSV_ToRenderer(const _wstring _strDSVTag, ID3D11DepthStencilView* _pDSV)
{
	if (nullptr == m_pNewRenderer)
		return E_FAIL;

	return m_pNewRenderer->Add_DSV(_strDSVTag, _pDSV);
}

HRESULT CGameInstance::Erase_DSV_ToRenderer(const _wstring _strDSVTag)
{
	if (nullptr == m_pNewRenderer)
		return E_FAIL;

	return m_pNewRenderer->Erase_DSV(_strDSVTag);
}

ID3D11DepthStencilView* CGameInstance::Find_DSV(const _wstring& _strDSVTag)
{
	if (nullptr == m_pNewRenderer)
		return nullptr;

	return m_pNewRenderer->Find_DSV(_strDSVTag);
}

const _float4x4* CGameInstance::Get_WorldMatrix_Renderer() const
{
	return m_pNewRenderer->Get_WorldMatrix();
}

const _float4x4* CGameInstance::Get_ViewMatrix_Renderer() const
{
	return m_pNewRenderer->Get_ViewMatrix();
}

const _float4x4* CGameInstance::Get_ProjMatrix_Renderer() const
{
	return m_pNewRenderer->Get_ProjMatrix();
}

CONST_IBL CGameInstance::Get_GlobalIBLData() const
{
	return m_pNewRenderer->Get_GlobalIBLData();
}

void CGameInstance::Set_GlobalIBLData(const CONST_IBL& _tGlobalIBLData, _bool _isUpdateConstBuffer)
{
	m_pNewRenderer->Set_GlobalIBLData(_tGlobalIBLData, _isUpdateConstBuffer);
}

HRESULT CGameInstance::Load_IBL(const _wstring& _strIBLJsonPath)
{
	return m_pNewRenderer->Load_IBL(_strIBLJsonPath);
}

HRESULT CGameInstance::Bind_DofConstBuffer(const _char* _szConstBufferName, ID3D11Buffer* _pConstBuffer)
{
	return m_pNewRenderer->Bind_DofConstBuffer(_szConstBufferName, _pConstBuffer);
}

void CGameInstance::Set_PlayerHideColor(const _float3 _vHideColor, _bool _isUpdate)
{
	return m_pNewRenderer->Set_PlayerHideColor(_vHideColor, _isUpdate);
}

HRESULT CGameInstance::Add_ShadowLight(class CLight* _pShadowLight)
{
	return m_pNewRenderer->Add_ShadowLight(_pShadowLight);
}

HRESULT CGameInstance::Remove_ShadowLight(_int _iShadowLightID)
{
	return m_pNewRenderer->Remove_ShadowLight(_iShadowLightID);
}

HRESULT CGameInstance::Clear_ShadowLight()
{
	return m_pNewRenderer->Clear_ShadowLight();
}

void CGameInstance::Set_VtxMesh(CShader* _pShader)
{
	assert(_pShader);
	m_pNewRenderer->Set_VtxMesh(_pShader);
}

void CGameInstance::Set_VtxAnimMesh(CShader* _pShader)
{
	assert(_pShader);
	m_pNewRenderer->Set_VtxAnimMesh(_pShader);
}

void CGameInstance::Set_VtxPosTex(CShader* _pShader)
{
	assert(_pShader);
	m_pNewRenderer->Set_VtxPosTex(_pShader);
}

void CGameInstance::Set_GrayScale_VtxMesh(_int _isGrayScale)
{
	m_pNewRenderer->Set_GrayScale_VtxMesh(_isGrayScale);
}

void CGameInstance::Set_GrayScale_VtxAnimMesh(_int _isGrayScale)
{
	m_pNewRenderer->Set_GrayScale_VtxAnimMesh(_isGrayScale);
}

void CGameInstance::Set_GrayScale_VtxPosTex(_int _isGrayScale)
{
	m_pNewRenderer->Set_GrayScale_VtxPosTex(_isGrayScale);
}

#ifdef _DEBUG

HRESULT CGameInstance::Add_DebugComponent_New(CComponent* _pDebugCom)
{
	if (nullptr == m_pNewRenderer)
		return E_FAIL;

	return m_pNewRenderer->Add_DebugComponent(_pDebugCom);
}

void CGameInstance::Set_DebugRender_New(_bool _isBool)
{
	if (nullptr == m_pNewRenderer)
		return;

	return m_pNewRenderer->Set_DebugRender(_isBool);
}

HRESULT CGameInstance::Add_BaseDebug(CBase* _pBaseDebug)
{
	if (nullptr == m_pNewRenderer)
		return E_FAIL;

	return m_pNewRenderer->Add_BaseDebug(_pBaseDebug);
}


#endif // _DEBUG

const KEY_STATE& CGameInstance::GetKeyState(KEY _eKEY)
{
	assert(m_pKey_Manager);

	return m_pKey_Manager->GetKeyState(_eKEY);
}

const KEY_STATE& CGameInstance::GetMouseKeyState(MOUSE_KEY _eMouse)
{
	assert(m_pKey_Manager);

	return m_pKey_Manager->GetMouseKeyState(_eMouse);
}

_long CGameInstance::GetDIMouseMove(MOUSE_AXIS eMouseMove)
{
	if (nullptr == m_pKey_Manager)
		return 0;

	return m_pKey_Manager->GetDIMouseMove(eMouseMove);
}

void CGameInstance::Set_Transform(CPipeLine::D3DTRANSFORMSTATE _eState, _fmatrix _TransformMatrix)
{
	if (nullptr == m_pPipeLine)
		return;

	return m_pPipeLine->Set_Transform(_eState, _TransformMatrix);
}

_matrix CGameInstance::Get_TransformMatrix(CPipeLine::D3DTRANSFORMSTATE _eState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_TransformMatrix(_eState);
}

_float4x4 CGameInstance::Get_TransformFloat4x4(CPipeLine::D3DTRANSFORMSTATE _eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4(_eState);
}

_matrix CGameInstance::Get_TransformInverseMatrix(CPipeLine::D3DTRANSFORMSTATE _eState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_TransformInverseMatrix(_eState);
}

_float4x4 CGameInstance::Get_TransformInverseFloat4x4(CPipeLine::D3DTRANSFORMSTATE _eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformInverseFloat4x4(_eState);
}

const _float4* CGameInstance::Get_CamPosition() const
{
	if (nullptr == m_pPipeLine)
		return nullptr;

	return m_pPipeLine->Get_CamPosition();
}

_vector CGameInstance::Get_vCamPosition()
{
	if (nullptr == m_pPipeLine)
		return XMVectorZero();

	return m_pPipeLine->Get_vCamPosition();
}

_float2 CGameInstance::Transform_WorldToScreen(_fvector _vWorldPos, _float2 _fScreenSize)
{
	if (nullptr == m_pPipeLine)
	{
		MSG_BOX("Not Initialize PipeLine");
		return _float2(0.0f, 0.0f);
	}

	return m_pPipeLine->Transform_WorldToScreen(_vWorldPos, _fScreenSize);
}

void CGameInstance::Set_NearFarZ(_float2 _vNearFarZ)
{
	if (nullptr == m_pPipeLine)
	{
		MSG_BOX("Failed Initialize PipeLine");
		return;
	}
	m_pPipeLine->Set_NearFarZ(_vNearFarZ);
}

_float* CGameInstance::Get_NearZ()
{
	if (nullptr == m_pPipeLine)
	{
		MSG_BOX("Failed Initialize PipeLine");
		return nullptr;
	}

	return m_pPipeLine->Get_NearZ();
}

_float* CGameInstance::Get_FarZ()
{
	if (nullptr == m_pPipeLine)
	{
		MSG_BOX("Failed Initialize PipeLine");
		return nullptr;
	}

	return m_pPipeLine->Get_FarZ();
}


HRESULT CGameInstance::Add_Light(const CONST_LIGHT& _LightDesc, LIGHT_TYPE _eType)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Light(_LightDesc, _eType);
}

HRESULT CGameInstance::Add_Light_Target(const CONST_LIGHT& _LightDesc, LIGHT_TYPE _eType, CGameObject* _pTargetOwner, const _float3& _vOffsetPostion, class CLight_Target** _ppOut, _bool _isNotClear)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Light_Target(_LightDesc, _eType, _pTargetOwner, _vOffsetPostion, _ppOut, _isNotClear);
}

const CONST_LIGHT* CGameInstance::Get_LightDesc_Ptr(_uint _iIndex) const
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_LightDesc_Ptr(_iIndex);
}

HRESULT CGameInstance::Render_Lights(CShader* _pShader, CVIBuffer_Rect* _pVIBuffer)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Render(_pShader, _pVIBuffer);
}

HRESULT CGameInstance::Load_Lights(const _wstring& _strLightsJsonPath)
{
	return m_pLight_Manager->Load_Lights(_strLightsJsonPath);
}

const list<class CLight*>& CGameInstance::Get_Lights() const
{
	return m_pLight_Manager->Get_Lights();
}

HRESULT CGameInstance::Delete_Light(_uint _iLightIndex)
{
	return m_pLight_Manager->Delete_Light(_iLightIndex);
}


//void CGameInstance::Add_CollisionLayerCheckInfo(COLL_CHECK* _pCollCheckLayerData)
//{
//	if (nullptr == m_pCollision_Manager)
//		assert(nullptr);
//
//	return m_pCollision_Manager->Add_CollisionLayerCheckInfo(_pCollCheckLayerData);
//}
//
//_bool CGameInstance::Intersect_RayCollision(_fvector _vRayStart, _fvector _vRayDir, const _wstring& _strLayerTag, const CGameObject* _pThis, _int _iOtherPartIndex)
//{
//	if (nullptr == m_pCollision_Manager)
//		assert(nullptr);
//
//	return m_pCollision_Manager->Intersect_RayCollision(_vRayStart, _vRayDir, _strLayerTag, _iOtherPartIndex, _pThis);
//}
//
//_bool CGameInstance::Intersect_RayCollision_Nearest(_fvector _vRayStart, _fvector _vRayDir, const _wstring& _strLayerTag, _float* _pOutDst, CGameObject** _ppOutNearestObj, const CGameObject* _pThis, _uint _iCollidersIndex, _int _iOtherPartIndex)
//{
//	if (nullptr == m_pCollision_Manager)
//		assert(nullptr);
//
//	return m_pCollision_Manager->Intersect_RayCollision_Nearest(_vRayStart, _vRayDir, _strLayerTag, _pOutDst, _ppOutNearestObj, _pThis, _iCollidersIndex, _iOtherPartIndex );
//}
//
//_bool CGameInstance::Intersect_RayCollision_NearestDistance(_fvector _vRayStart, _fvector _vRayDir, const _wstring& _strLayerTag, _float* _pOutDst, const CGameObject* _pThis, _uint _iCollidersIndex, _int _iOtherPartIndex)
//{
//	if (nullptr == m_pCollision_Manager)
//		assert(nullptr);
//
//	return m_pCollision_Manager->Intersect_RayCollision_NearestDistance(_vRayStart, _vRayDir, _strLayerTag, _pOutDst,  _pThis, _iCollidersIndex, _iOtherPartIndex);
//}


HRESULT CGameInstance::Add_Font(const _wstring& _strFontTag, const _tchar* _pFontFilePath)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Add_Font(_strFontTag, _pFontFilePath);
}

HRESULT CGameInstance::Render_Font(const _wstring& _strFontTag, const _tchar* _pText, const _float2& _vPosition, _fvector _vColor, _float _fRotation, const _float2& _vOrigin)
{
	return m_pFont_Manager->Render_Font(_strFontTag, _pText, _vPosition, _vColor, _fRotation, _vOrigin);
}

HRESULT CGameInstance::Render_Scaling_Font(const _wstring& _strFontTag, const _tchar* _pText, const _float2& _vPosition, _fvector _vColor, _float _fRotation, const _float2& _vOrigin, _float _fScale)
{
	return m_pFont_Manager->Render_Scaling_Font(_strFontTag, _pText, _vPosition, _vColor, _fRotation, _vOrigin, _fScale);
}

_vector CGameInstance::Measuring(const _wstring& _strFontTag, _wstring _text)
{
	return m_pFont_Manager->Measuring(_strFontTag, _text);
}

HRESULT CGameInstance::Add_RenderTarget(const _wstring& _strTargetTag, _uint _iWidth, _uint _iHeight, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor, CRenderTarget** _ppOut)
{
	return m_pTarget_Manager->Add_RenderTarget(_strTargetTag, _iWidth, _iHeight, _ePixelFormat, _vClearColor, _ppOut);
}

HRESULT CGameInstance::Add_RenderTarget(const _wstring& _strTargetTag, CRenderTarget* _pRenderTarget)
{
	return m_pTarget_Manager->Add_RenderTarget(_strTargetTag, _pRenderTarget);
}

HRESULT CGameInstance::Add_RenderTarget_MSAA(const _wstring& _strTargetTag, _uint _iWidth, _uint _iHeight, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor, CRenderTarget** _ppOut)
{
	return m_pTarget_Manager->Add_RenderTarget_MSAA(_strTargetTag, _iWidth, _iHeight, _ePixelFormat, _vClearColor, _ppOut);
}

HRESULT CGameInstance::Add_MRT(const _wstring& _strMRTTag, const _wstring& _strTargetTag)
{
	return m_pTarget_Manager->Add_MRT(_strMRTTag, _strTargetTag);
}

HRESULT CGameInstance::Begin_MRT(const _wstring& _strMRTTag, ID3D11DepthStencilView* _pDSV, _bool _isClear)
{
	return m_pTarget_Manager->Begin_MRT(_strMRTTag, _pDSV, _isClear);
}

HRESULT CGameInstance::Begin_MRT(const vector<CRenderTarget*>& _MRT, ID3D11DepthStencilView* _pDSV, _bool _isClear)
{
	return m_pTarget_Manager->Begin_MRT(_MRT, _pDSV, _isClear);
}

HRESULT CGameInstance::End_MRT()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	return m_pTarget_Manager->End_MRT();
}

HRESULT CGameInstance::Bind_RT_ShaderResource(CShader* _pShader, const _char* _pConstantName, const _wstring& _strTargetTag)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	return m_pTarget_Manager->Bind_ShaderResource(_pShader, _pConstantName, _strTargetTag);
}

HRESULT CGameInstance::Copy_RT_Resource(const _wstring& _strTargetTag, ID3D11Texture2D* _pDest)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	return m_pTarget_Manager->Copy_RT_Resource(_strTargetTag, _pDest);
}

HRESULT CGameInstance::Copy_BackBuffer_RT_Resource(const _wstring& _strTargetTag)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	return m_pTarget_Manager->Copy_BackBuffer_RT_Resource(_strTargetTag);
}

HRESULT CGameInstance::Clear_MRT(const _wstring& _strMRTTag, ID3D11DepthStencilView* _pDSV, _bool isClear)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	return m_pTarget_Manager->Clear_MRT(_strMRTTag, _pDSV, isClear);
}

ID3D11ShaderResourceView* CGameInstance::Get_RT_SRV(const _wstring& _strTargetTag)
{
	if (nullptr == m_pTarget_Manager)
		return nullptr;

	return m_pTarget_Manager->Get_SRV(_strTargetTag);

}

_float2 CGameInstance::Get_RT_Size(const _wstring& _strTargetTag)
{
	if (nullptr == m_pTarget_Manager)
	{
		MSG_BOX("Failed Get_RT_Size");
		return _float2();
	}

	return m_pTarget_Manager->Get_RT_Size(_strTargetTag);
}

HRESULT CGameInstance::Erase_RenderTarget(const _wstring& _strTargetTag)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	return m_pTarget_Manager->Erase_RenderTarget(_strTargetTag);
}

HRESULT CGameInstance::Erase_MRT(const _wstring& _strMRTTag)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	return m_pTarget_Manager->Erase_MRT(_strMRTTag);
}

CRenderTarget* CGameInstance::Find_RenderTarget(const _wstring& _strTargetTag)
{
	if (nullptr == m_pTarget_Manager)
		return nullptr;

	return m_pTarget_Manager->Find_RenderTarget(_strTargetTag);
}

HRESULT CGameInstance::Resolve_RT_MSAA(const _wstring& _strTargetTag)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	return m_pTarget_Manager->Resolve_RT_MSAA(_strTargetTag);
}

HRESULT CGameInstance::Resolve_MRT_MSAA(const _wstring& _strMRTTag)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	return m_pTarget_Manager->Resolve_MRT_MSAA(_strMRTTag);
}


LPDIRECTSOUND8 CGameInstance::Get_SoundDevice()
{
	if (nullptr == m_pSound_Manager)
		return nullptr;

	return m_pSound_Manager->Get_SoundDevice();
}

HRESULT CGameInstance::Load_BGM(const wstring& strBGMTag, const wstring& strBGMFilePath)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->Load_BGM(strBGMTag, strBGMFilePath);
}

void CGameInstance::Start_BGM(const wstring& strBGMTag, _float _fVolume)
{
	if (nullptr == m_pSound_Manager)
		return;

	return m_pSound_Manager->Start_BGM(strBGMTag, _fVolume);
}

void CGameInstance::Transition_BGM(const _wstring& strBGMTag, _float _fVolume, _float _fFactor)
{
	if (nullptr == m_pSound_Manager)
		return;

	return m_pSound_Manager->Transition_BGM(strBGMTag, _fVolume, _fFactor);
}

void CGameInstance::Stop_BGM()
{
	if (nullptr == m_pSound_Manager)
		return;

	return m_pSound_Manager->Stop_BGM();
}

void CGameInstance::End_BGM()
{
	if (nullptr == m_pSound_Manager)
		return;

	return m_pSound_Manager->End_BGM();
}

_float CGameInstance::Get_BGMTrackPosition(const wstring& strBGMTag)
{
	if (nullptr == m_pSound_Manager)
		return -1.0f;

	return m_pSound_Manager->Get_BGMTrackPosition(strBGMTag);
}

_bool CGameInstance::Is_BGMPlaying(const wstring& strBGMTag)
{
	if (nullptr == m_pSound_Manager)
		return false;

	return m_pSound_Manager->Is_BGMPlaying(strBGMTag);
}

_float CGameInstance::Get_BGMVolume()
{
	if (nullptr == m_pSound_Manager)
		return -1.0f;

	return m_pSound_Manager->Get_BGMVolume();
}

void CGameInstance::Set_BGMVolume(_float _fVolume)
{
	if (nullptr == m_pSound_Manager)
		return;

	return m_pSound_Manager->Set_BGMVolume(_fVolume);
}

void CGameInstance::Set_SFXTargetVolume(const _wstring& _strSFXTag, _float _fTargetVolume, _float _fFactor)
{
	if (nullptr == m_pSound_Manager)
		return;

	return m_pSound_Manager->Set_SFXTargetVolume(_strSFXTag, _fTargetVolume, _fFactor);
}

void CGameInstance::Set_BGMTargetVolume(_float _fTargetVolume, _float _fFactor)
{
	if (nullptr == m_pSound_Manager)
		return;

	return m_pSound_Manager->Set_BGMTargetVolume(_fTargetVolume, _fFactor);
}

HRESULT CGameInstance::Load_SFX(const wstring& strSFXTag, const wstring& strSFXFilePath)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->Load_SFX(strSFXTag, strSFXFilePath);
}

void CGameInstance::Start_SFX(const wstring& strSFXTag, _float _fVolume, bool _isLoop)
{
	if (nullptr == m_pSound_Manager)
		return;

	return m_pSound_Manager->Start_SFX(strSFXTag, _fVolume, _isLoop);
}

void CGameInstance::Start_SFX_Delay(const wstring& strSFXTag, _float _fDelayTime, _float _fVolume, _bool _isLoop)
{
	if (nullptr == m_pSound_Manager)
		return;

	return m_pSound_Manager->Start_SFX_Delay(strSFXTag, _fDelayTime, _fVolume, _isLoop);
}

void CGameInstance::Stop_SFX(const wstring& strSFXTag)
{
	if (nullptr == m_pSound_Manager)
		return;

	return m_pSound_Manager->Stop_SFX(strSFXTag);
}

void CGameInstance::End_SFX(const wstring& strSFXTag)
{
	if (nullptr == m_pSound_Manager)
		return;

	return m_pSound_Manager->End_SFX(strSFXTag);
}

_float CGameInstance::Get_SFXTrackPosition(const wstring& strSFXTag)
{
	if (nullptr == m_pSound_Manager)
		return -1.0f;

	return m_pSound_Manager->Get_SFXTrackPosition(strSFXTag);
}

void CGameInstance::Start_SFX_Distance(const _wstring& strSFXTag, _fvector _vPosition, _float _fMaxVolume, _float _fMinVolume, _float _fMaxVolumeDist, _float _fFactor, _bool _isLoop)
{
	if (nullptr == m_pSound_Manager)
		return;

	return m_pSound_Manager->Start_SFX_Distance(strSFXTag, _vPosition, _fMaxVolume, _fMinVolume, _fMaxVolumeDist, _fFactor, _isLoop);

}

void CGameInstance::Start_SFX_Distance_Delay(const _wstring& strSFXTag, _fvector _vPosition, _float _fDelayTime, _float _fMaxVolume, _float _fMinVolume, _float _fMaxVolumeDist, _float _fFactor, _bool _isLoop)
{
	if (nullptr == m_pSound_Manager)
		return;

	return m_pSound_Manager->Start_SFX_Distance_Delay(strSFXTag, _vPosition, _fDelayTime, _fMaxVolume, _fMinVolume, _fMaxVolumeDist, _fFactor, _isLoop);
}

void CGameInstance::Start_SFX_Distance2D(const _wstring& strSFXTag, _fvector _vPosition, _fvector _vCenter, _float _fMaxVolume, _float _fMinVolume, _float _fMaxVolumeDist, _float _fFactor, _bool _isLoop)
{
	if (nullptr == m_pSound_Manager)
		return;

	return m_pSound_Manager->Start_SFX_Distance2D(strSFXTag, _vPosition, _vCenter, _fMaxVolume, _fMinVolume, _fMaxVolumeDist, _fFactor, _isLoop);

}

void CGameInstance::Start_SFX_Distance2D_Delay(const _wstring& strSFXTag, _fvector _vPosition, _fvector _vCenter, _float _fDelayTime, _float _fMaxVolume, _float _fMinVolume, _float _fMaxVolumeDist, _float _fFactor, _bool _isLoop)
{
	if (nullptr == m_pSound_Manager)
		return;

	return m_pSound_Manager->Start_SFX_Distance2D_Delay(strSFXTag, _vPosition, _vCenter, _fDelayTime, _fMaxVolume, _fMinVolume, _fMaxVolumeDist, _fFactor, _isLoop);

}


_bool CGameInstance::Is_SFXPlaying(const wstring& strSFXTag)
{
	if (nullptr == m_pSound_Manager)
		return false;

	return m_pSound_Manager->Is_SFXPlaying(strSFXTag);
}

_float CGameInstance::Get_SFXVolume(const wstring& strSFXTag)
{
	if (nullptr == m_pSound_Manager)
		return -1.0f;

	return m_pSound_Manager->Get_SFXVolume(strSFXTag);
}

void CGameInstance::Set_SFXVolume(const wstring& strSFXTag, _float _fVolume)
{
	if (nullptr == m_pSound_Manager)
		return;

	return m_pSound_Manager->Set_SFXVolume(strSFXTag, _fVolume);
}

HRESULT CGameInstance::Clear_Sound()
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->Clear_Sound();
}

HRESULT CGameInstance::Load_Json(const _tchar* _szFilePath, _Out_ json* _pOutJson)
{
	if (nullptr == m_pJson_Manager)
		return E_FAIL;


	return m_pJson_Manager->Load_Json(_szFilePath, _pOutJson);
}

HRESULT CGameInstance::Load_Json_InLevel(const _tchar* _szFilePath, const _wstring& _strKey, _uint _iLevelIndex)
{
	if (nullptr == m_pJson_Manager)
		return E_FAIL;


	return m_pJson_Manager->Load_Json_InLevel(_szFilePath, _strKey, _iLevelIndex);
}

const json* CGameInstance::Find_Json_InLevel(const _wstring& _strKey, _uint _iLevelIndex)
{
	if (nullptr == m_pJson_Manager)
		return nullptr;


	return m_pJson_Manager->Find_Json_InLevel(_strKey, _iLevelIndex);
}

HRESULT CGameInstance::Start_Imgui()
{
	if (nullptr == m_pImgui_Manager)
		return E_FAIL;


	return m_pImgui_Manager->Start_Imgui();
}

HRESULT CGameInstance::End_Imgui()
{
	if (nullptr == m_pImgui_Manager)
		return E_FAIL;

	return m_pImgui_Manager->End_Imgui();
}

void CGameInstance::Render_DrawData_Imgui()
{
	if (nullptr == m_pImgui_Manager)
		return;

	return m_pImgui_Manager->Render_DrawData();
}
#ifdef _DEBUG
HRESULT	CGameInstance::Imgui_Select_Debug_ObjectInfo(const wstring _strLayerTag, _uint _iObjectId)
{
	if (nullptr == m_pImgui_Manager)
		return E_FAIL;

	return m_pImgui_Manager->Imgui_Select_Debug_ObjectInfo(_strLayerTag, _iObjectId);
}
#endif // _DEBUG


_float2 CGameInstance::Get_CursorPos()
{
	return m_pGlobalFunction_Manager->Get_CursorPos(m_hWnd);
}

_vector CGameInstance::Get_OnClickPos()
{
	return m_pGlobalFunction_Manager->Get_OnClickPos(m_hWnd);
}

string CGameInstance::WStringToString(const _wstring& _wstr)
{
	return m_pGlobalFunction_Manager->WStringToString(_wstr);
}

_wstring CGameInstance::StringToWString(const string& _str)
{
	return m_pGlobalFunction_Manager->StringToWString(_str);
}

_float3 CGameInstance::Get_RotationFromMatrix_Quaternion(const _fmatrix _WorldMatrix)
{
	return m_pGlobalFunction_Manager->Get_RotationFromMatrix_Quaternion(_WorldMatrix);
}

_float CGameInstance::Nomalize_Angle(_float _fAngle)
{
	return m_pGlobalFunction_Manager->Nomalize_Angle(_fAngle);
}

_float CGameInstance::Lerp(_float _fLeft, _float _fRight, _float _fRatio)
{
	return m_pGlobalFunction_Manager->Lerp(_fLeft, _fRight, _fRatio);
}

_fvector CGameInstance::Get_BezierCurve(_fvector _vStartPoint, _fvector _vGuidePoint, _fvector _vEndPoint, _float _fRatio)
{
	return m_pGlobalFunction_Manager->Get_BezierCurve(_vStartPoint, _vGuidePoint, _vEndPoint, _fRatio);
}

_bool CGameInstance::MatrixDecompose(_float3* _vScale, _float4* _vQuaternion, _float3* _vPosition, FXMMATRIX _Matrix)
{
	return m_pGlobalFunction_Manager->MatrixDecompose(_vScale, _vQuaternion, _vPosition, _Matrix);
}

_float CGameInstance::Get_Angle_Between_Vectors(_fvector _vNormal, _fvector _vVector1, _fvector _vVector2)
{
	return m_pGlobalFunction_Manager->Get_Angle_Between_Vectors(_vNormal, _vVector1, _vVector2);
}

_float CGameInstance::Clamp_Degrees(_float _fDegrees)
{
	return m_pGlobalFunction_Manager->Clamp_Degrees(_fDegrees);
}

_fvector CGameInstance::Rotate_Vector(_fvector _vAxis, _fvector _vVector, _float _fDegrees)
{
	return m_pGlobalFunction_Manager->Rotate_Vector(_vAxis, _vVector, _fDegrees);
}

_fvector CGameInstance::Direction_To_Quaternion(_fvector _vDefaulfDir, _fvector _vDirection)
{
	return m_pGlobalFunction_Manager->Direction_To_Quaternion(_vDefaulfDir, _vDirection);
}

_uint CGameInstance::Compare_VectorLength(_fvector _vVector1, _fvector _vVector2)
{
	return m_pGlobalFunction_Manager->Compare_VectorLength(_vVector1, _vVector2);
}

_float CGameInstance::Calculate_Ratio(_float2* _fTime, _float _fTimeDelta, _uint _iRatioType)
{
	return m_pGlobalFunction_Manager->Calculate_Ratio(_fTime, _fTimeDelta, _iRatioType);
}

CCamera* CGameInstance::Get_CurrentCamera()
{
	return m_pCamera_Manager->Get_CurrentCamera();
}

CCamera* CGameInstance::Get_Camera(_uint _eType)
{
	return  m_pCamera_Manager->Get_Camera(_eType);
}

_vector CGameInstance::Get_CameraVector(CTransform::STATE _eState)
{
	return m_pCamera_Manager->Get_CameraVector(_eState);
}

_uint CGameInstance::Get_CameraType()
{
	return m_pCamera_Manager->Get_CameraType();
}

void CGameInstance::Add_Camera(_uint _iCurrentCameraType, CCamera* _pCamera)
{
	m_pCamera_Manager->Add_Camera(_iCurrentCameraType, _pCamera);
}

void CGameInstance::Change_CameraType(_uint _iCurrentCameraType)
{
	m_pCamera_Manager->Change_CameraType(_iCurrentCameraType);
}

void CGameInstance::Physx_Update(_float _fTimeDelta)
{
	if (nullptr == m_pPhysx_Manager)
		return;

	m_pPhysx_Manager->Update(_fTimeDelta);
}

PxPhysics* CGameInstance::Get_Physics() const
{
	if (nullptr == m_pPhysx_Manager)
		return nullptr;

	return m_pPhysx_Manager->Get_Physics();
}

PxCooking* CGameInstance::Get_Cooking() const
{
	if (nullptr == m_pPhysx_Manager)
		return nullptr;

	return m_pPhysx_Manager->Get_Cooking();
}

_float CGameInstance::Get_Gravity()
{
	if (nullptr == m_pPhysx_Manager)
		return 0.f;

	return m_pPhysx_Manager->Get_Gravity();
}

PxScene* CGameInstance::Get_Physx_Scene() const
{
	if (nullptr == m_pPhysx_Manager)
		return nullptr;

	return m_pPhysx_Manager->Get_Scene();
}

PxMaterial* CGameInstance::Get_Material(ACTOR_MATERIAL _eType) const
{
	if (nullptr == m_pPhysx_Manager)
		return nullptr;

	return m_pPhysx_Manager->Get_Material(_eType);
}

void CGameInstance::Add_ShapeUserData(SHAPE_USERDATA* _pUserData)
{
	if (nullptr == m_pPhysx_Manager)
		return;

	return m_pPhysx_Manager->Add_ShapeUserData(_pUserData);
}

void CGameInstance::Add_ActorUserData(ACTOR_USERDATA* _pUserData)
{
	if (nullptr == m_pPhysx_Manager)
		return;

	return m_pPhysx_Manager->Add_ActorUserData(_pUserData);
}

_uint CGameInstance::Create_ShapeID()
{
	if (nullptr == m_pPhysx_Manager)
		assert(nullptr);

	return m_pPhysx_Manager->Create_ShapeID();
}

_bool CGameInstance::RayCast_Nearest(const _float3& _vOrigin, const _float3& _vRayDir, _float _fMaxDistance, _float3* _pOutPos, CActorObject** _ppOutActorObject)
{
	if (nullptr == m_pPhysx_Manager)
		assert(nullptr);

	return m_pPhysx_Manager->RayCast_Nearest(_vOrigin, _vRayDir, _fMaxDistance, _pOutPos, _ppOutActorObject);
}

_bool CGameInstance::RayCast_Nearest_GroupFilter(const _float3& _vOrigin, const _float3& _vRayDir, _float _fMaxDistance, _int _iGroupNum, _float3* _pOutPos, CActorObject** _ppOutActorObject)
{
	return m_pPhysx_Manager->RayCast_Nearest_GroupFilter(_vOrigin, _vRayDir, _fMaxDistance, _iGroupNum, _pOutPos, _ppOutActorObject);
}

_bool CGameInstance::RayCast(const _float3& _vOrigin, const _float3& _vRayDir, _float _fMaxDistance, list<CActorObject*>& _OutActors, list<RAYCASTHIT>& _OutRaycastHits)
{
	if (nullptr == m_pPhysx_Manager)
		assert(nullptr);

	return m_pPhysx_Manager->RayCast(_vOrigin, _vRayDir, _fMaxDistance, _OutActors, _OutRaycastHits);
}

_bool CGameInstance::Overlap(SHAPE_TYPE _eShapeType, SHAPE_DESC* _pShape, _fvector _vPos, list<CActorObject*>& _OutActors)
{
	if (nullptr == m_pPhysx_Manager)
		assert(nullptr);

	return m_pPhysx_Manager->Overlap(_eShapeType, _pShape, _vPos, _OutActors);
}

_bool CGameInstance::Overlap(PxGeometry* pxGeom, _fvector _vPos, list<CActorObject*>& _OutActors)
{
	if (nullptr == m_pPhysx_Manager)
		assert(nullptr);

	return m_pPhysx_Manager->Overlap(pxGeom, _vPos, _OutActors);
}

_bool CGameInstance::SingleSweep(PxGeometry* pxGeom, const _float3& _vOrigin, const _float3& _vRayDir, _float _fDistance, CActorObject** _ppOutActor, RAYCASTHIT* _pOutHit)
{
	if (nullptr == m_pPhysx_Manager)
		assert(nullptr);

	return m_pPhysx_Manager->SingleSweep(pxGeom, _vOrigin, _vRayDir, _fDistance, _ppOutActor, _pOutHit);
}

_bool CGameInstance::SingleSweep(PxGeometry* pxGeom, const _float4x4& _matShpeOffsetMatrix, const _float3& _vOrigin, const _float3& _vRayDir, _float _fDistance, CActorObject** _ppOutActor, RAYCASTHIT* _pOutHit)
{
	if (nullptr == m_pPhysx_Manager)
		assert(nullptr);

	return m_pPhysx_Manager->SingleSweep(pxGeom, _matShpeOffsetMatrix, _vOrigin, _vRayDir, _fDistance, _ppOutActor, _pOutHit);
}

_bool CGameInstance::SingleSweep_GroupFilter(PxGeometry* pxGeom, const _float4x4& _matShpeOffsetMatrix, const _float3& _vOrigin, const _float3& _vRayDir, _float _fDistance, _int _iGroupNum, CActorObject** _ppOutActor, RAYCASTHIT* _pOutHit)
{
	if (nullptr == m_pPhysx_Manager)
		assert(nullptr);

	return m_pPhysx_Manager->SingleSweep_GroupFilter(pxGeom, _matShpeOffsetMatrix, _vOrigin, _vRayDir, _fDistance, _iGroupNum, _ppOutActor, _pOutHit);
}

_bool CGameInstance::MultiSweep(PxGeometry* pxGeom, const _float4x4& _matShpeOffsetMatrix, const _float3& _vOrigin, const _float3& _vRayDir, _float _fDistance, list<CActorObject*>& _OutActors, list<RAYCASTHIT>& _OutRaycastHits)
{
	if (nullptr == m_pPhysx_Manager)
		assert(nullptr);

	return m_pPhysx_Manager->MultiSweep(pxGeom, _matShpeOffsetMatrix,_vOrigin, _vRayDir, _fDistance, _OutActors, _OutRaycastHits);
}


void CGameInstance::Set_Physx_DebugRender(_bool _isDebugRender)
{
	if (nullptr == m_pPhysx_Manager)
		assert(nullptr);

	return m_pPhysx_Manager->Set_DebugRender(_isDebugRender);
}

_bool CGameInstance::isIn_Frustum_InWorldSpace(_fvector _vWorldPos, _float _fRange)
{
	if (nullptr == m_pFrustum)
		return true;

	return m_pFrustum->isIn_InWorldSpace(_vWorldPos, _fRange);
}

HRESULT CGameInstance::Create_DSV(_uint _iWidth, _uint _iHeight, ID3D11DepthStencilView** _ppOutDSV)
{
	return m_pD3DUtils->Create_DSV(_iWidth, _iHeight, _ppOutDSV);
}

void CGameInstance::Set_CubeMap(CCubeMap* _pCubeMap)
{
	if (nullptr != m_pCubeMap)
		Safe_Release(m_pCubeMap);

	m_pCubeMap = _pCubeMap;
	Safe_AddRef(_pCubeMap);
}

HRESULT CGameInstance::Bind_IBLTexture(CShader* _pShaderCom, const _char* _pBRDFConstName, const _char* _pSpecularConstName, const _char* _pIrradianceConstName)
{
	if (nullptr == m_pCubeMap)
		return E_FAIL;

	return m_pCubeMap->Bind_IBLTexture(_pShaderCom, _pBRDFConstName, _pSpecularConstName, _pIrradianceConstName);
}

HRESULT CGameInstance::Change_CubeMap(const _wstring& _strCubeMapPrototypeTag)
{
	if (nullptr == m_pCubeMap)
		return E_FAIL;

	return m_pCubeMap->Change_CubeMap(_strCubeMapPrototypeTag);
}


HRESULT CGameInstance::Register_Section(const _wstring& _strSectionKey)
{
	if (nullptr == m_pCollision_Manager)
		return E_FAIL;
	return m_pCollision_Manager->Register_Section(_strSectionKey);
}
HRESULT CGameInstance::Unregister_Section(const _wstring& _strSectionKey)
{
	if (nullptr == m_pCollision_Manager)
		return E_FAIL;

	return m_pCollision_Manager->Unregister_Section(_strSectionKey);

}
void CGameInstance::Check_GroupFilter(_uint _iGroupFilterLeft, _uint _iGroupFilterRight)
{
	m_pCollision_Manager->Check_GroupFilter(_iGroupFilterLeft, _iGroupFilterRight);
}

void CGameInstance::Erase_GroupFilter(_uint _iGroupFilterLeft, _uint _iGroupFilterRight)
{
	m_pCollision_Manager->Erase_GroupFilter(_iGroupFilterLeft, _iGroupFilterRight);
}

void CGameInstance::Clear_GroupFilter()
{
	m_pCollision_Manager->Clear_GroupFilter();
}

HRESULT CGameInstance::Add_Collider(const _wstring& _strSectionKey, _uint _iGroupFilter, CCollider* _pCollider)
{
	return m_pCollision_Manager->Add_Collider(_strSectionKey, _iGroupFilter, _pCollider);
}

HRESULT CGameInstance::Physx_Render()
{
	if (nullptr == m_pPhysx_Manager)
		return E_FAIL;

	return m_pPhysx_Manager->Render();
}


#ifdef _DEBUG

HRESULT CGameInstance::Ready_RT_Debug(const _wstring& _strTargetTag, _float _fX, _float _fY, _float _fSizeX, _float _fSizeY)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	return m_pTarget_Manager->Ready_Debug(_strTargetTag, _fX, _fY, _fSizeX, _fSizeY);
}

HRESULT CGameInstance::Render_RT_Debug(const _wstring& _strMRTTag, CShader* _pShader, CVIBuffer_Rect* _pVIBufferRect)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	return m_pTarget_Manager->Render_Debug(_strMRTTag, _pShader, _pVIBufferRect);
}

map<const _wstring, CRenderTarget*>& CGameInstance::Get_RenderTargets()
{
	return m_pTarget_Manager->Get_RenderTargets();
}

map<const _wstring, list<CRenderTarget*>>& CGameInstance::Get_MRTs()
{
	return m_pTarget_Manager->Get_MRTs();
}
#endif // _DEBUG



void CGameInstance::Release_Engine()
{
	// 1. CGameInstance에 포함 된 멤버변수(Manager Class)에 대한 Release.
	CGameInstance::GetInstance()->Free();
	// 2. CGameInstance의 RefCount 가 반드시 0이어야 함.해당 함수 호출 타이밍엔.
	CGameInstance::GetInstance()->DestroyInstance();
}

void CGameInstance::Free() // 예외적으로 Safe_Release()가 아닌, Release_Engine() 함수를 통해서 호출 한다.
{	
	// Engine Manager Class Release
	// 여기서 Manger Class->Free() 호출 >>> 참조 중이던 CGameInstance에 대한 Safe_Release() 호출 됌.


	Safe_Release(m_pFrustum);
	Safe_Release(m_pCamera_Manager);
	Safe_Release(m_pGlobalFunction_Manager);
	Safe_Release(m_pImgui_Manager);
	Safe_Release(m_pJson_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pKey_Manager);
	Safe_Release(m_pShadow);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pD3DUtils);
	Safe_Release(m_pTarget_Manager);

	/* 임시 코드 */
	if (false == m_isNewRenderer)
		Safe_Release(m_pRenderer);
	else
		Safe_Release(m_pNewRenderer);

	Safe_Release(m_pCollision_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pPrototype_Manager);
	Safe_Release(m_pPhysx_Manager);
	Safe_Release(m_pCubeMap);
	Safe_Release(m_pThreadPool);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pGraphic_Device);

	// Base Free 호출.
	__super::Free();
}
