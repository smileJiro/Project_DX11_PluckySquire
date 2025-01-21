#pragma once

#include "Renderer.h" // Renderer Enum 사용을 위해, Base.h는 포함되어있음.
#include "Prototype_Manager.h"// Prototype_Manager.h 안에 Component들 헤더 인클루드 되어있음.
#include "Key_Manager.h"
#include "Collision_Manager.h"
#include "PipeLine.h"
#include "Shadow.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public: /* For.GameInstance */
	HRESULT				Initialize_Engine(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext);
	void				Priority_Update_Engine(_float fTimeDelta);
	void				Update_Engine(_float fTimeDelta);
	void				Late_Update_Engine(_float fTimeDelta);
	HRESULT				Render_Begin(const _float4& vClearColor = _float4(0.f, 0.f, 1.f, 1.f));
	HRESULT				Draw();
	HRESULT				Render_End();
	void				Set_CurLevelID(_uint _iLevelID);
	HWND				Get_HWND() const { return m_hWnd; }
	HINSTANCE			Get_HINSTANCE() const { return m_hInstance; }
	_uint				Get_ViewportWidth() const { return m_iViewportWidth; }
	_uint				Get_ViewportHeight() const { return m_iViewportHeight; }

	_float				Compute_Random_Normal(); // 0 ~ 1사이 
	_float				Compute_Random(_float _fMin, _float _fMax); // 범위 지정.

	HRESULT				Engine_Level_Enter(_int _iChangeLevelID);
	HRESULT				Engine_Level_Exit(_int _iChangeLevelID, _int _iNextChangeLevelID = -1);

	_int				Get_StaticLevelID() const { return m_iStaticLevelID; }


public: /* For.Timer_Manager */
	_float				Get_TimeDelta(const _wstring& _strTimerTag);
	void				Render_FPS(const _wstring& _strTimerTag);
	_int				Get_FPS();
	HRESULT				Add_Timer(const _wstring& _strTimerTag);
	void				Update_TimeDelta(const _wstring& _strTimerTag);
	HRESULT				Set_TimeScale(_float _fTimeScale, const _wstring& _strTimeTag);
	HRESULT				Reset_TimeScale(const _wstring& _strTimeTag);

public: /* For.Level_Manager */
	HRESULT				Level_Manager_Enter(_int _iLevelID, class CLevel* _pNewLevel); // Level 진입 시 해야 할 처리.
	HRESULT				Level_Manager_Exit(_int _iChangeLevelID, _int _iNextChangeLevelID = -1); // 현재 레벨에 대한 오브젝트들을 삭제한다.
	_int				Get_CurLevelID() const;

public: /* For. Proto_Manager */
	HRESULT				Add_Prototype(_uint _iLevelID, const _wstring& _strPrototypeTag, class CBase* _pPrototype);
	class CBase*		Clone_Prototype(Engine::PROTOTYPE _eType, _uint _iLevelID, const _wstring& _strPrototypeTag, void* _pArg);
	class CBase*		Find_Prototype(_uint _iLevelID, const _wstring& _strPrototypeTag);

public: /* For. Object_Manager */
	HRESULT				Add_GameObject_ToLayer(_uint _iPrototypeLevelID, const _wstring& _strPrototypeTag, _uint _iLevelID, const _wstring& _strLayerTag, void* pArg = nullptr); // Default
	HRESULT				Add_GameObject_ToLayer(_uint _iPrototypeLevelID, const _wstring& _strPrototypeTag, _uint _iLevelID, const _wstring& _strLayerTag, CGameObject** _ppOut, void* pArg = nullptr); // Add Object를 Return
	HRESULT				Add_GameObject_ToLayer(_uint _iLevelID, const _wstring& _strLayerTag, class CGameObject* _pGameObject); // Add Object를 직접 생성 후 Add To Layer

	class CLayer*		Find_Layer(_uint _iLevelID, const _wstring& _strLayerTag);
	class CGameObject*	Get_PickingModelObjectByCursor(_uint _iLevelID, const _wstring& _strLayerTag, _float2 _fCursorPos); // 마우스 커서로 피킹체크 후 충돌 오브젝트중 가장 가까운 오브젝트 리턴.
	class CGameObject*	Find_NearestObject_Scaled(_uint _iLevelID, const _wstring& _strLayerTag, CController_Transform* const _pTransform, CGameObject* pCurTargetObject = nullptr);
	class CGameObject*	Get_GameObject_Ptr(_int _iLevelID, const _wstring& _strLayerTag, _int _iObjectIndex);
#ifdef _DEBUG
	map<const _wstring, class CLayer*>* Get_Layers_Ptr();
#endif

public: /* For.Renderer */
	HRESULT				Add_RenderObject(CRenderer::RENDERGROUP _eRenderGroup, class CGameObject* _pRenderObject);
#ifdef _DEBUG
	HRESULT				Add_DebugComponent(class CComponent* _pDebugCom);
#endif

public: /* For. Key_Manager */
	const KEY_STATE&	GetKeyState(KEY _eKEY); // 키입력 동기화 o 
	const KEY_STATE&	GetMouseKeyState(MOUSE_KEY _eMouse);
	_long				GetDIMouseMove(MOUSE_MOVE eMouseMove);

public: /* For. PipeLine */
	void				Set_Transform(CPipeLine::D3DTRANSFORMSTATE _eState, _fmatrix _TransformMatrix);
	_matrix				Get_TransformMatrix(CPipeLine::D3DTRANSFORMSTATE _eState);
	_float4x4			Get_TransformFloat4x4(CPipeLine::D3DTRANSFORMSTATE _eState);
	_matrix				Get_TransformInverseMatrix(CPipeLine::D3DTRANSFORMSTATE _eState);
	_float4x4			Get_TransformInverseFloat4x4(CPipeLine::D3DTRANSFORMSTATE _eState);
	const _float4*		Get_CamPosition() const;
	_float2				Transform_WorldToScreen(_fvector _vWorldPos, _float2 _fScreenSize);
	void				Set_NearFarZ(_float2 _vNearFarZ);
	_float*				Get_NearZ();
	_float*				Get_FarZ();

public: /* For. Light_Manager */
	HRESULT				Add_Light(const LIGHT_DESC& _LightDesc);
	const LIGHT_DESC*	Get_LightDesc(_uint _iIndex) const;
	HRESULT				Render_Lights(class CShader* _pShader, class CVIBuffer_Rect* _pVIBuffer);

public: /* For. Collision_Manager */
	//void				Add_CollisionLayerCheckInfo(COLL_CHECK* _pCollCheckLayerData);	/* 충돌 검사 수행 대상 정보 수집. */
	//
	//_bool				Intersect_RayCollision(_fvector _vRayStart, _fvector _vRayDir, const _wstring& _strLayerTag, const CGameObject* _pThis, _int _iOtherPartIndex = -1);
	//_bool				Intersect_RayCollision_Nearest(_fvector _vRayStart, _fvector _vRayDir, const _wstring& _strLayerTag, _float* _pOutDst, CGameObject** _ppOutNearestObj, const CGameObject* _pThis, _uint _iCollidersIndex, _int _iOtherPartIndex = -1);
	//_bool				Intersect_RayCollision_NearestDistance(_fvector _vRayStart, _fvector _vRayDir, const _wstring& _strLayerTag, _float* _pOutDst, const CGameObject* _pThis, _uint _iCollidersIndex, _int _iOtherPartIndex = -1);


public: /* For. Font_Manager s*/
	HRESULT				Add_Font(const _wstring& _strFontTag, const _tchar* _pFontFilePath);
	HRESULT				Render_Font(const _wstring& _strFontTag, const _tchar* _pText, const _float2& _vPosition, _fvector _vColor, _float _fRotation = 0.f, const _float2& _vOrigin = _float2(0.f, 0.f));

public: /* For. Target_Manager */
	HRESULT				Add_RenderTarget(const _wstring& _strTargetTag, _uint _iWidth, _uint _iHeight, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor);
	HRESULT				Add_MRT(const _wstring& _strMRTTag, const _wstring& _strTargetTag);
	HRESULT				Begin_MRT(const _wstring& _strMRTTag, ID3D11DepthStencilView* _pDSV = nullptr, _bool isClear = true);	/* 지정한 RenderTargets를 셰이더에 바인딩하고 그리기 준비를 하는 */
	HRESULT				End_MRT(); /* 렌더링을 마친 후, 기존의 BackRTV를 다시 바인딩 한다. */
	HRESULT				Bind_RT_ShaderResource(CShader* _pShader, const _char* _pConstantName, const _wstring& _strTargetTag);	/* RenderTarget을 ShaderResource로써 바인딩하는 함수. */
	HRESULT				Copy_RT_Resource(const _wstring& _strTargetTag, ID3D11Texture2D* _pDest);
	HRESULT				Copy_BackBuffer_RT_Resource(const _wstring& _strTargetTag);
	HRESULT				Clear_MRT(const _wstring& _strMRTTag, ID3D11DepthStencilView* _pDSV = nullptr, _bool isClear = true);
	ID3D11ShaderResourceView* Get_RT_SRV(const _wstring& _strTargetTag);
	_float2				Get_RT_Size(const _wstring& _strTargetTag);
#ifdef _DEBUG
	HRESULT				Ready_RT_Debug(const _wstring& _strTargetTag, _float _fX, _float _fY, _float _fSizeX, _float _fSizeY);	/* 렌더타겟을 디버그용으로 렌더하기위한 함수 */
	HRESULT				Render_RT_Debug(const _wstring& _strMRTTag, CShader* _pShader, CVIBuffer_Rect* _pVIBufferRect);			/* 디버그 렌더 함수 */
	map<const _wstring, CRenderTarget*>& Get_RenderTargets();
	map<const _wstring, list<CRenderTarget*>>& Get_MRTs();
#endif //_DEBUG

public: /* For. Shadow */
	const _float4x4*	Get_Shadow_Transform_Ptr(CShadow::D3DTRANSFORMSTATE _eState);
	void				SetUp_Shadow_TransformMatrix(CShadow::D3DTRANSFORMSTATE _eState, _fmatrix _TransformMatrix);
	void				SetUp_TargetShadowMatrix(_fvector _vLightDirection, _fvector _vWorldPos, _float _fDistance, _float2 _vNearFar = _float2(0.1f, 100.f));

public: /* For. Sound_Manager */
	LPDIRECTSOUND8		Get_SoundDevice();

	HRESULT				Load_BGM(const wstring& strBGMTag, const wstring& strBGMFilePath);
	void				Start_BGM(const wstring& strBGMTag, _float _fVolume = 20.f);
	void				Stop_BGM(); // 일시정지
	void				End_BGM(); // 완전 재생 종료
	_float				Get_BGMTrackPosition(const wstring& strBGMTag);
	_bool				Is_BGMPlaying(const wstring& strBGMTag);
	_float				Get_BGMVolume();
	void				Set_BGMVolume(_float _fVolume);

	HRESULT				Load_SFX(const wstring& strSFXTag, const wstring& strSFXFilePath);
	void				Start_SFX(const wstring& strSFXTag, _float _fVolume = 70.f, bool _isLoop = false);
	void				Start_SFX_Delay(const wstring& strSFXTag, _float _fDelayTime, _float _fVolume = 70.f, _bool _isLoop = false);
	void				Stop_SFX(const wstring& strSFXTag); // 일시정지 
	void				End_SFX(const wstring& strSFXTag); // 완전 재생 종료
	_float				Get_SFXTrackPosition(const wstring& strSFXTag);
	_bool				Is_SFXPlaying(const wstring& strSFXTag);
	_float				Get_SFXVolume(const wstring& strSFXTag);
	void				Set_SFXVolume(const wstring& strSFXTag, _float _fVolume);

public: /* For. Json_Manager*/
	HRESULT				Load_Json(const _tchar* _szFilePath, _Out_ json* _pOutJson);



public: /* For. Imgui_Manager */
	HRESULT				Start_Imgui(); // 시작을 알림.
	HRESULT				End_Imgui(); // 종료를 알림.
	void				Render_DrawData_Imgui(); // 수집 된 정보를 기반으로 그리기를 수행함.
#ifdef _DEBUG
	HRESULT				Imgui_Select_Debug_ObjectInfo(const wstring _strLayerTag, _uint _iObjectId);
#endif // _DEBUG
	//HRESULT			LevelChange_Imgui();
	
public: /* For. GlobalFunction_Manager */
	_float2				Get_CursorPos();
	_vector				Get_OnClickPos();
	string				WStringToString(const _wstring& _wstr);
	_wstring			StringToWString(const string& _str);
	_float3				Get_RotationFromMatrix_Quaternion(const _fmatrix _WorldMatrix);
	_float				Nomalize_Angle(_float _fAngle);	// 0-360 사이로 각도 변환
	_float				Lerp(_float _fLeft, _float _fRight, _float _fRatio);
	_fvector			Get_BezierCurve(_fvector _vStartPoint, _fvector _vGuidePoint, _fvector _vEndPoint, _float _fRatio);
	_bool				MatrixDecompose(_float3* _vScale, _float4* _vQuaternion, _float3* _vPosition, FXMMATRIX _Matrix);

public: /* For. Camera_Manager */
	CCamera*			Get_CurrentCamera();
	CCamera*			Get_Camera(_uint _eType);
	_vector				Get_CameraVector(CTransform::STATE _eState);
	_uint				Get_CameraType();

	void				Add_Camera(_uint _iCurrentCameraType, CCamera* _pCamera);
	void				Change_CameraType(_uint _iCurrentCameraType);


public: /* For. Physx_Manager*/
	void				Physx_Update(_float _fTimeDelta);
	PxPhysics*			Get_Physics() const;
	PxMaterial*			Get_Material(ACTOR_MATERIAL _eType) const;

	void				Set_Player(CGameObject* _pPlayer);
private:
	class CGraphic_Device* m_pGraphic_Device = nullptr;
	class CTimer_Manager* m_pTimer_Manager = nullptr;
	class CLevel_Manager* m_pLevel_Manager = nullptr;
	class CPrototype_Manager* m_pPrototype_Manager = nullptr;
	class CObject_Manager* m_pObject_Manager = nullptr;
	class CRenderer* m_pRenderer = nullptr;
	class CKey_Manager* m_pKey_Manager = nullptr;
	class CPipeLine* m_pPipeLine = nullptr;
	class CLight_Manager* m_pLight_Manager = nullptr;
	class CCollision_Manager* m_pCollision_Manager = nullptr;
	class CFont_Manager* m_pFont_Manager = nullptr;
	class CTarget_Manager* m_pTarget_Manager = nullptr;
	class CShadow* m_pShadow = nullptr;
	class CSound_Manager* m_pSound_Manager = nullptr;
	class CJson_Manager* m_pJson_Manager =  nullptr;
	class CImgui_Manager* m_pImgui_Manager = nullptr;
	class CGlobalFunction_Manager* m_pGlobalFunction_Manager = nullptr;
	class CCamera_Manager_Engine* m_pCamera_Manager = nullptr;
	class CPhysx_Manager* m_pPhysx_Manager = nullptr;

private:
	HWND m_hWnd = nullptr;
	HINSTANCE m_hInstance = nullptr;

	_uint m_iViewportWidth = 0;
	_uint m_iViewportHeight = 0;

	_int m_iStaticLevelID = -1;

public:
	// Static
	static void Release_Engine();

public:
	virtual void Free() override;
};

END