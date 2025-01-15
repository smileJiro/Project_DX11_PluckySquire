#pragma once

#include "VIBuffer.h"
#include "Map_Tool_Defines.h"

BEGIN(Map_Tool)

class CVIBuffer_CellLine final : public CVIBuffer
{
private:
	CVIBuffer_CellLine(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext);
	CVIBuffer_CellLine(const CVIBuffer_CellLine& _Prototype);
virtual ~CVIBuffer_CellLine() = default;

public:
	virtual HRESULT Initialize_Prototype(const _float3* _pPoints);
	virtual HRESULT Initialize(void* _pArg) override;

public:
	static CVIBuffer_CellLine* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _float3* _pPoints);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END