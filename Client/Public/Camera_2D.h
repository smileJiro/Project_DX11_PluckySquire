#pragma once
#include "Camera.h"

BEGIN(Engine)
class CCameraArm;
END

BEGIN(Client)
class CCamera_2D : public CCamera
{
public:
	enum TARGET_STATE { TARGET_RIGHT, TARGET_UP, TARGET_LOOK, TARGET_POS, TARGET_STATE_END };

	enum CAMERA_2D_MODE 
	{ 
		DEFAULT, 
		MOVE_TO_DESIREPOS, 
		MOVE_TO_SHOP, 
		RETURN_TO_DEFUALT, 
		FLIPPING_UP,
		FLIPPING_PAUSE,
		FLIPPING_DOWN,
		CAMERA_2D_MODE_END 
	};

	typedef struct tagCamera2DDesc : public CCamera::CAMERA_DESC
	{
		CAMERA_2D_MODE			eCameraMode = { DEFAULT };
		_float3					vAtOffset = { 0.f, 0.f, 0.f };
		_float					fSmoothSpeed = {};

		const _float4x4* pTargetWorldMatrix = { nullptr };
	}CAMERA_2D_DESC;

private:
	CCamera_2D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_2D(const CCamera_2D& Prototype);
	virtual ~CCamera_2D() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;

public:
	_uint						Get_CameraMode() { return m_eCameraMode; }
	virtual INITIAL_DATA		Get_InitialData() override;

	void						Set_CameraMode(_uint _iCameraMode, _int iNextCameraMode = -1) { m_eCameraMode = (CAMERA_2D_MODE)_iCameraMode; }

public:
	void						Add_CurArm(CCameraArm* _pCameraArm);

	virtual void				Switch_CameraView(INITIAL_DATA* _pInitialData = nullptr) override;

private:
	const _float4x4*			m_pTargetWorldMatrix = { nullptr };
	CAMERA_2D_MODE				m_eCameraMode = { CAMERA_2D_MODE_END };
	CAMERA_2D_MODE				m_ePreCameraMode = { DEFAULT };
	CCameraArm*					m_pCurArm = { nullptr };

	_float						m_fSmoothSpeed = {};

	// TargetPos로 변환한 값 저장
	_float3						m_v2DTargetWorldPos = {};

	// Flipping
	_float2						m_fFlippingTime = {};
	_float3						m_vStartPos = {};

	// Book
	_bool						m_isBook = { true };
	_float						m_fFixedY = {};

private:
	void						Action_Mode(_float _fTimeDelta);
	void						Action_SetUp_ByMode();

	void						Defualt_Move(_float _fTimeDelta);
	void						Move_To_DesirePos(_float _fTimeDelta);
	void						Move_To_Shop(_float _fTimeDelta);
	void						Return_To_Default(_float _fTimeDelta);
	void						Flipping_Up(_float _fTimeDelta);
	void						Flipping_Pause(_float _fTimeDelta);
	void						Flipping_Down(_float _fTimeDelta);
	void						Look_Target(_float fTimeDelta);

	_bool						Calculate_CameraPos(_float _fTimeDelta, _float3* _vCameraPos);
	virtual	void				Switching(_float _fTimeDelta) override;

#ifdef _DEBUG
	void						Key_Input(_float _fTimeDelta);
#endif

public:
	static CCamera_2D*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};
END