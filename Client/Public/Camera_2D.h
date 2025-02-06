#pragma once
#include "Camera.h"

BEGIN(Engine)
class CCameraArm;
END

BEGIN(Client)
class CCamera_2D : public CCamera
{
public:
	enum CAMERA_2D_MODE 
	{ 
		DEFAULT, 
		MOVE_TO_DESIREPOS, 
		MOVE_TO_SHOP, 
		RETURN_TO_DEFUALT, 
		FLIPPING, 
		CAMERA_2D_MODE_END 
	};

	typedef struct tagCamera2DDesc : public CCamera::CAMERA_DESC
	{
		CAMERA_2D_MODE			eCameraMode = { DEFAULT };
		_float3					vAtOffset = { 0.f, 0.f, 0.f };
		_float					fSmoothSpeed = {};
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
	void						Add_CurArm(CCameraArm* _pCameraArm);

private:
	CAMERA_2D_MODE				m_eCameraMode = { CAMERA_2D_MODE_END };
	CCameraArm*					m_pCurArm = { nullptr };

	_float						m_fSmoothSpeed = {};

private:
	void						Action_Mode(_float _fTimeDelta);
	void						Defualt_Move(_float fTimeDelta);
	void						Move_To_DesirePos(_float _fTimeDelta);
	void						Move_To_Shop(_float _fTimeDelta);
	void						Return_To_Default(_float _fTimeDelta);
	void						Fliping(_float _fTimeDelta);
	void						Look_Target(_float fTimeDelta);

public:
	static CCamera_2D*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};
END