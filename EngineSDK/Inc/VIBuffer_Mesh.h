#pragma once
#include "VIBuffer.h"
BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Mesh final : public CVIBuffer
{
private:
	CVIBuffer_Mesh(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CVIBuffer_Mesh(const CVIBuffer_Mesh& _Prototype);
	virtual ~CVIBuffer_Mesh() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* _pArg);

public:
	
public:
	static CVIBuffer_Mesh* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END