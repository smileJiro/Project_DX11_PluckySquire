#pragma once
#include "VIBuffer_Instance.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Particle : public CVIBuffer_Instance
{
private:
	CVIBuffer_Point_Particle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CVIBuffer_Point_Particle(const CVIBuffer_Point_Particle& _Prototype);
	virtual ~CVIBuffer_Point_Particle() = default;

public:
	HRESULT Initialize_Prototype(const json& _jsonBufferInfo);
	virtual HRESULT Initialize(void* _pArg) override;


private:
	VTXPOINTINSTANCE* m_pInstanceVertices = { nullptr };


public:
	static CVIBuffer_Point_Particle* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const json& _jsonBufferInfo);
	virtual CComponent* Clone(void* _pArg);
	virtual void Free() override;
};
END
