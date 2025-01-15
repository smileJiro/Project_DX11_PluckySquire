#pragma once

#include "VIBuffer.h"
#include "Map_Tool_Defines.h"

BEGIN(Map_Tool)

class CVIBuffer_CellLine final : public CVIBuffer
{
private:
	CVIBuffer_CellLine(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CVIBuffer_CellLine(const CVIBuffer_CellLine& Prototype);
virtual ~CVIBuffer_CellLine() = default;

public:
	virtual HRESULT Initialize_Prototype(const _float3* pPoints);
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CVIBuffer_CellLine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END