#pragma once
#include "Base.h"

BEGIN(Engine)

class CParticle_Module : public CBase
{
public:
	enum MODULE_TYPE { MODULENONE, INIT_VELOCITY};
	enum DATA_TYPE { POINT, LINEAR, DIRECTION, AXIS };
	enum APPLY_DATA { POSITION, VELOCITY, FORCE };
	enum APPLY_TYPE { OVERWRITE, ADD, SUBSTRACT };

private:
	CParticle_Module();
	virtual ~CParticle_Module() = default;

public:
	HRESULT Initialize(const json& _jsonModuleInfo);	
	void	Init_Data(_float4* _pPosition, _float3* _pVelocity, 
		_float3* _pForce, _float4* _pColor);


public:
	_bool		Is_Init() const { return m_isInit; }
	_int		Get_Order() const { return m_iOrder; }

private:
	_bool		m_isInit = { false };
	_int		m_iOrder = { 0 };
	MODULE_TYPE m_eModuleType = {};
	DATA_TYPE	m_eDataType = {};
	APPLY_DATA m_eApplyData = {};
	APPLY_TYPE m_eApplyType = {};

	map<const _string, _float> m_FloatDatas;
	map<const _string, _float3> m_Float3Datas;

private:
	HRESULT	Add_Float3(const _char* _szTag, const json& _jsonInfo);

public:
	static CParticle_Module* Create(const json& _jsonModuleInfo);
	virtual void Free() override;
};

END

BEGIN(Engine)

NLOHMANN_JSON_SERIALIZE_ENUM(CParticle_Module::MODULE_TYPE, {
{CParticle_Module::MODULE_TYPE::MODULENONE, "NONE"},
{CParticle_Module::MODULE_TYPE::INIT_VELOCITY, "INIT_VELOCITY"},
	});

NLOHMANN_JSON_SERIALIZE_ENUM(CParticle_Module::DATA_TYPE, {
{CParticle_Module::DATA_TYPE::POINT, "POINT"},
{CParticle_Module::DATA_TYPE::LINEAR, "LINEAR"},
{CParticle_Module::DATA_TYPE::DIRECTION, "DIRECTION"},
{CParticle_Module::DATA_TYPE::AXIS, "AXIS"},
	});

END

