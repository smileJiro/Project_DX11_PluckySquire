#pragma once
#include "Camera.h"

BEGIN(Engine)

class ENGINE_DLL CCamera_Free final: public CCamera
{
	// 마우스 감도 같은 변수들을 만들 수도있겠지.
public:
	typedef struct tagCameraFreeDesc : public CCamera::CAMERA_DESC
	{
		_float			fMouseSensor = {};
	}CAMERA_FREE_DESC;
private:
	CCamera_Free(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCamera_Free(const CCamera_Free& _Prototype);
	virtual ~CCamera_Free() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Update(_float _fTimeDelta) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual void Late_Update(_float _fTimeDelta) override;


public:
	// Get
	_float Get_MouseSensor() const { return m_fMouseSensor; };
	_bool Is_CamRotation() const { return m_bCameraRotation; }
	_bool Is_CamMove() const { return m_bCameraMove; }
	// Set
	void Set_MouseSensor(const _float& _fMouseSensor) { m_fMouseSensor = _fMouseSensor; }
	void Set_CamRotation(_bool _b) { m_bCameraRotation = _b; }
	void Set_CamMove(_bool _b) { m_bCameraMove = _b; }
private:
	_float m_fMouseSensor = {};
	_bool m_bCameraRotation = false;
	_bool m_bCameraMove = true;

private:
	void Key_Input(_float _fTimeDelta);

public:
	static CCamera_Free* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg);
	virtual void Free() override;
};

END
