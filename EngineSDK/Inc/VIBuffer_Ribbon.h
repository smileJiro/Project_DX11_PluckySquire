#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Ribbon : public CVIBuffer
{
private:
	CVIBuffer_Ribbon(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CVIBuffer_Ribbon(const CVIBuffer_Ribbon& _Prototype);
	virtual ~CVIBuffer_Ribbon() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* _pArg);

public:
	void			Begin_Update();
	void			End_Update();
	void			Set_StartPosition(_fvector _vStartPosition); // Beam의 처음 위치 설정
	void			Set_EndPosition(_fvector _vEndPosition);	 // Beam의 마지막 위치 설정

private:
	VTXPOS*				m_pVertices = { nullptr };
	VTXPOS*				m_pUpdateVertices = { nullptr };

public:
	static CVIBuffer_Ribbon* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext); // 출발점, 끝점만 있는 Buffer
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;

};

END