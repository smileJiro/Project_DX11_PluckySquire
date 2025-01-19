#pragma once
#include "Client_Defines.h"
#include "Component.h"
class CDetectionField final : public CComponent
{
public:
	typedef struct tagDetectionFieldDesc
	{
		_float fRadius;
	}DETECTIONFIELDDESC;

private:
	CDetectionField(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CDetectionField(const CDetectionField& _Prototype);
	virtual ~CDetectionField() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* _pArg);
#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif

private:
	_float m_fRadius = { 0.f };

public:
	static CDetectionField* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CDetectionField* Clone(void* _pArg);
	virtual void Free() override;
};

