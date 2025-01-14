#pragma once
#include "Camera_Manager.h"
#include "Base.h"

BEGIN(Engine)
class CCamera_Manager : public CBase
{
private:
	CCamera_Manager();
	virtual ~CCamera_Manager() = default;

public:
	HRESULT Initialize(); // 카메라 생성 후 저장.

	HRESULT Bind_ViewMatrix(class CShader* _pShader, const _char* _pConstantName);
	HRESULT Bind_ProjMatrix(class CShader* _pShader, const _char* _pConstantName);

public:
	void Set_ViewMatrix(const _float4x4& _ViewMatrix);
	void Set_ProjMatrix(const _float4x4& _ProjMatrix);
private:
	_float4x4 m_ViewMatrix = {};
	_float4x4 m_ProjMatrix = {};

public:
	static CCamera_Manager* Create();
	virtual void Free() override;
};
END
