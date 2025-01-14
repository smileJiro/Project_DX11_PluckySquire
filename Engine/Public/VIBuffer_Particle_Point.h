#pragma once
#include "VIBuffer_Particle_Instance.h"
BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Particle_Point final: public CVIBuffer_Particle_Instance
{
private:
	CVIBuffer_Particle_Point(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CVIBuffer_Particle_Point(const CVIBuffer_Particle_Point& _Prototype);
	virtual ~CVIBuffer_Particle_Point() = default;

public:
	virtual HRESULT Initialize_Prototype(const json& _json);
	virtual HRESULT Initialize(void* _pArg) override;

private:


public:
	static CVIBuffer_Particle_Point* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const json& _json);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};
END
