#pragma once
#include "Base.h"

BEGIN(Engine)

class CTranslation_Module : public CBase
{

public:
	enum MODULE_TYPE { MODULENONE, POINT_VELOCITY, LINEAR_VELOCITY, INIT_ACCELERATION, GRAVITY, DRAG, VORTEX_ACCELERATION, POINT_ACCELERATION, LIMIT_ACCELERATION,   MODULE_END };
	enum APPLY_DATA { TRANSLATION, COLOR, ROTATION };


private:
	CTranslation_Module();
	virtual ~CTranslation_Module() = default;

public:
	HRESULT Initialize(const json& _jsonModuleInfo);	
	void	Update_Translations(_float _fTimeDelta, _float4* _pPosition, _float3* _pVelocity, _float3* _pAcceleration);
public:
	_bool		Is_Init() const { return m_isInit; }
	_int		Get_Order() const { return m_iOrder; }

private:
	_bool		m_isInit = { false };
	_int		m_iOrder = { 0 };
	MODULE_TYPE m_eModuleType = {};
	APPLY_DATA m_eApplyData = {};


	map<const _string, _float> m_FloatDatas;
	map<const _string, _float3> m_Float3Datas;

private:
	HRESULT	Add_Float3(const _char* _szTag, const json& _jsonInfo);

public:
	static CTranslation_Module* Create(const json& _jsonModuleInfo);
	virtual void Free() override;

#ifdef _DEBUG

public:
	HRESULT Initialize(MODULE_TYPE eType, const _string& _strTypeName);
	void	Tool_Module_Update();
	HRESULT	Save_Module(json& _jsonModuleInfo);

public:
	void			Set_Order(_int _iOrder) { m_iOrder = _iOrder; }
	const _string	Get_TypeName() const { return m_strTypeName; }
	_bool			Is_Changed() { if (m_isChanged) { m_isChanged = false; return true; } else return false; }

	
private:
	_bool	m_isChanged = { false };
	_string m_strTypeName;

public:
	static CTranslation_Module* Create(MODULE_TYPE _eType, const _string& _strTypeName);

#endif 
};

END

BEGIN(Engine)

NLOHMANN_JSON_SERIALIZE_ENUM(CTranslation_Module::MODULE_TYPE, {
{CTranslation_Module::MODULE_TYPE::MODULENONE, "NONE"},
{CTranslation_Module::MODULE_TYPE::POINT_VELOCITY, "POINT_VELOCITY"},
{CTranslation_Module::MODULE_TYPE::LINEAR_VELOCITY, "LINEAR_VELOCITY"},
{CTranslation_Module::MODULE_TYPE::INIT_ACCELERATION, "INIT_ACCELERATION"},
{CTranslation_Module::MODULE_TYPE::GRAVITY, "GRAVITY"},
{CTranslation_Module::MODULE_TYPE::DRAG, "DRAG"},
{CTranslation_Module::MODULE_TYPE::VORTEX_ACCELERATION, "VORTEX_ACCELERATION"},
{CTranslation_Module::MODULE_TYPE::POINT_ACCELERATION, "POINT_ACCELERATION"},
{CTranslation_Module::MODULE_TYPE::LIMIT_ACCELERATION, "LIMIT_ACCELERATION"},

	});


END


