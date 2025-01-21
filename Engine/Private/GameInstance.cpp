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
#include "GlobalFunction_Manager.h"
#include "Camera_Manager_Engine.h"
#include "Physx_Manager.h"
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

	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.isWindowed, EngineDesc.iViewportWidth, EngineDesc.iViewportHeight, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pPhysx_Manager = CPhysx_Manager::Create(*ppDevice, *ppContext); /* PhysX 역시 PxPhysics* 를 Object 생성 시 전달할 예정. (유사 Device, Context 개념.)*/
	if (nullptr == m_pPhysx_Manager)
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

	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;
	
	m_pPrototype_Manager = CPrototype_Manager::Create(EngineDesc.iNumLevels, EngineDesc.iStaticLevelID);
	if (nullptr == m_pPrototype_Manager)
		return E_FAIL;

	m_pObject_Manager = CObject_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	//m_pCollision_Manager = CCollision_Manager::Create();
	//if (nullptr == m_pCollision_Manager)
	//	return E_FAIL;

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

	m_pJson_Manager = CJson_Manager::Create();
	if (nullptr == m_pJson_Manager)
		return E_FAIL;
	
	m_pImgui_Manager = CImgui_Manager::Create(EngineDesc.hWnd, *ppDevice, *ppContext, _float2((_float)EngineDesc.iViewportWidth, (_float)EngineDesc.iViewportHeight));
	if (nullptr == m_pImgui_Manager)
		return E_FAIL;

	m_pGlobalFunction_Manager = CGlobalFunction_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pGlobalFunction_Manager)
		return E_FAIL;

	m_pCamera_Manager = CCamera_Manager_Engine::Create();
	if (nullptr == m_pCamera_Manager)
		return E_FAIL;



	return S_OK;
}

void CGameInstance::Priority_Update_Engine(_float fTimeDelta)
{
	m_pTimer_Manager->Update(fTimeDelta);
	m_pKey_Manager->Update();
	m_pObject_Manager->Priority_Update(fTimeDelta);
	m_pSound_Manager->Update(fTimeDelta);

	m_pPipeLine->Update(); 
}

void CGameInstance::Update_Engine(_float fTimeDelta)
{
	m_pObject_Manager->Update(fTimeDelta);
	m_pCamera_Manager->Update(fTimeDelta);


	/* 현재는 임시적으로 Physx_Manager가 Scene Update를 돌리며 테스트 예정. 
	추후 콜리전 매니저 설계시 scene 관리방식 변경.*/
	m_pPhysx_Manager->Update(fTimeDelta);
	//m_pCollision_Manager->Update(); /* 충돌 검사 수행. */
}

void CGameInstance::Late_Update_Engine(_float fTimeDelta)
{
	m_pObject_Manager->Late_Update(fTimeDelta); // Late_Update 수행 후, DeadObject Safe_Release() + erase();
	m_pLevel_Manager->Update(fTimeDelta);

#ifdef _DEBUG
	m_pImgui_Manager->Imgui_Debug_Render();
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
	m_pRenderer->Draw_RenderObject();

	m_pLevel_Manager->Render();

	return S_OK;
}

HRESULT CGameInstance::Render_End()
{
	return m_pGraphic_Device->Present();
}


void CGameInstance::Set_CurLevelID(_uint _iLevelID)
{
	/* 레벨 인덱스 전달해줘야하는 매니저들 여기서 전달. */
	if (nullptr == m_pCollision_Manager)
		return;

	m_pCollision_Manager->Set_CurLevelID(_iLevelID);
}

_float CGameInstance::Compute_Random_Normal()
{
	return (_float)rand() / RAND_MAX;
}

_float CGameInstance::Compute_Random(_float _fMin, _float _fMax)
{
	return _fMin + (_fMax - _fMin) * Compute_Random_Normal();
}

HRESULT CGameInstance::Engine_Level_Enter(_int _iChangeLevelID)
{
	/* Engine Manager 의 Level Enter 시점. 이벤트 매니저에서 새로운 레벨을 생성한 후, 호출된다. */
	//m_pCollision_Manager->Level_Enter(_iChangeLevelID);
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
	m_pObject_Manager->Level_Exit((_uint)iCurLevelID);
	m_pPrototype_Manager->Level_Exit((_uint)iCurLevelID);
	m_pLight_Manager->Level_Exit();
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
	if (nullptr == m_pRenderer)
		return E_FAIL;

	return m_pRenderer->Add_DebugComponent(_pDebugCom);
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

_long CGameInstance::GetDIMouseMove(MOUSE_MOVE eMouseMove)
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


HRESULT CGameInstance::Add_Light(const LIGHT_DESC& _LightDesc)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Light(_LightDesc);
}

const LIGHT_DESC* CGameInstance::Get_LightDesc(_uint _iIndex) const
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_LightDesc(_iIndex);
}

HRESULT CGameInstance::Render_Lights(CShader* _pShader, CVIBuffer_Rect* _pVIBuffer)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Render(_pShader, _pVIBuffer);
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
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Render_Font(_strFontTag, _pText, _vPosition, _vColor, _fRotation, _vOrigin);
}

HRESULT CGameInstance::Add_RenderTarget(const _wstring& _strTargetTag, _uint _iWidth, _uint _iHeight, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	return m_pTarget_Manager->Add_RenderTarget(_strTargetTag, _iWidth, _iHeight, _ePixelFormat, _vClearColor);
}

HRESULT CGameInstance::Add_MRT(const _wstring& _strMRTTag, const _wstring& _strTargetTag)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	return m_pTarget_Manager->Add_MRT(_strMRTTag, _strTargetTag);
}

HRESULT CGameInstance::Begin_MRT(const _wstring& _strMRTTag, ID3D11DepthStencilView* _pDSV, _bool isClear)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	return m_pTarget_Manager->Begin_MRT(_strMRTTag, _pDSV, isClear);
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

const _float4x4* CGameInstance::Get_Shadow_Transform_Ptr(CShadow::D3DTRANSFORMSTATE _eState)
{
	return m_pShadow->Get_Shadow_Transform_Ptr(_eState);
}

void CGameInstance::SetUp_Shadow_TransformMatrix(CShadow::D3DTRANSFORMSTATE _eState, _fmatrix _TransformMatrix)
{
	m_pShadow->SetUp_TransformMatrix(_eState, _TransformMatrix);
}

void CGameInstance::SetUp_TargetShadowMatrix(_fvector _vLightDirection, _fvector _vWorldPos, _float _fDistance, _float2 _vNearFar)
{
	m_pShadow->SetUp_TargetShadowMatrix(_vLightDirection, _vWorldPos, _fDistance, _vNearFar);
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

HRESULT CGameInstance::Load_Json(const _tchar* _szFilePath, _Out_ json* _pOutJson)
{
	if (nullptr == m_pJson_Manager)
		return E_FAIL;


	return m_pJson_Manager->Load_Json(_szFilePath, _pOutJson);
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

PxMaterial* CGameInstance::Get_Material(ACTOR_MATERIAL _eType) const
{
	if (nullptr == m_pPhysx_Manager)
		return nullptr;

	return m_pPhysx_Manager->Get_Material(_eType);
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

	Safe_Release(m_pPhysx_Manager); /* 태웅 : 물리적인 처리를하는 rigid 객체를 생성하기 위해 PxPhysics 객체를 오브젝트에 device처럼 포인터를 던질 예정. */
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
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pCollision_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pPrototype_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pGraphic_Device);



	// Base Free 호출.
	__super::Free();
}
