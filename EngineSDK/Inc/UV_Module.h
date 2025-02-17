#pragma once
#include "Effect_Module.h"


BEGIN(Engine)

class CUV_Module : public CEffect_Module
{
public:
	enum MODULE_NAME { UV_ANIM };

private:
	CUV_Module();
	CUV_Module(const CUV_Module& _Prototype);
	virtual ~CUV_Module() = default;

public:
	virtual HRESULT Initialize(const json& _jsonModuleInfo) override;
	virtual _int	Update_Module(class CCompute_Shader* _pCShader) override;

private:
	MODULE_NAME		m_eModuleName;

private:
	map<const _string, _float> m_FloatDatas;
	map<const _string, _float2> m_Float2Datas;


public:
	static CUV_Module* Create(const json& _jsonModuleInfo);
	virtual CEffect_Module* Clone();
	virtual void Free() override;

#ifdef _DEBUG

public:
	HRESULT Initialize(MODULE_NAME eType);
	virtual void	Tool_Module_Update() override;
	virtual HRESULT	Save_Module(json& _jsonModuleInfo) override;

public:
	static const _char* g_szModuleNames[1];
	static CUV_Module* Create(MODULE_NAME _eType);

#endif 
};

END

BEGIN(Engine)

NLOHMANN_JSON_SERIALIZE_ENUM(CUV_Module::MODULE_NAME, {
{CUV_Module::MODULE_NAME::UV_ANIM, "UV_ANIM"}
	});


END

