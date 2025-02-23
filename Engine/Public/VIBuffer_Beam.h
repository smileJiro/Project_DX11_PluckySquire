#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Beam : public CVIBuffer
{
private:
	CVIBuffer_Beam(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CVIBuffer_Beam(const CVIBuffer_Beam& _Prototype);
	virtual ~CVIBuffer_Beam() = default;

public:
	HRESULT Initialize_Prototype(_uint _iMaxVertexCount);
	virtual HRESULT Initialize(void* _pArg) override;

	void	Initialize_Positions(const _float3& _vMin, const _float3& _vMax); // 그냥 랜덤 세팅임..
	void	Initialize_StartEndPositions(const _float3& _vStart, const _float3& _vEnd); // 그냥 시작/끝 위치 설정임..
public:
	void			Begin_Update();
	void			End_Update();
	void			Update_StartPosition(_fvector _vStartPosition); // Beam의 처음 위치 설정
	void			Update_EndPosition(_fvector _vEndPosition);	 // Beam의 마지막 위치 설정
	void			Reset_Positions();

private:
	VTXBEAM*				m_pVertices = { nullptr };
	VTXBEAM*				m_pUpdateVertices = { nullptr };

public:
	static CVIBuffer_Beam* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iMaxVertexCount = 2); // default : 처음위치 & 끝점위치
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;

};

END