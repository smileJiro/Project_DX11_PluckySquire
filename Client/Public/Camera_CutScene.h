#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_CutScene final : public CCamera
{
private:
	CCamera_CutScene(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCamera_CutScene(const CCamera_CutScene& _Prototype);
	virtual ~CCamera_CutScene() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* _pArg) override;
	virtual void				Priority_Update(_float _fTimeDelta) override;
	virtual void				Update(_float _fTimeDelta) override;
	virtual void				Late_Update(_float _fTimeDelta) override;

public:
	static CCamera_CutScene*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*		Clone(void* _pArg);
	virtual void				Free() override;
};

END