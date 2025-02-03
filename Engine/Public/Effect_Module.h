#pragma once
#include "Base.h"
class CEffect_Module : public CBase
{
public:
	enum MODULE_TYPE { MODULENONE, POINT_VELOCITY, LINEAR_VELOCITY, INIT_ACCELERATION, GRAVITY, DRAG, VORTEX_ACCELERATION, POINT_ACCELERATION, LIMIT_ACCELERATION, MODULE_END };
	enum APPLY_DATA {TRANSLATION, COLOR, ROTATION };

private:
	CEffect_Module();
	virtual ~CEffect_Module() = default;

public:
	HRESULT Initialize(const json& _jsonModuleInfo);
	void	Update_Translations(_float _fTimeDelta, _float4* _pPosition, _float3* _pVelocity, _float3* _pAcceleration);

public:
	_bool		Is_Init() const { return m_isInit; }
	_int		Get_Order() const { return m_iOrder; }

protected:
	_bool		m_isInit = { false };
	_int		m_iOrder = { 0 };
	MODULE_TYPE m_eModuleType = {};
	APPLY_DATA m_eApplyData = {};
};

