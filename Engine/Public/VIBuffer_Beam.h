#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Beam : public CVIBuffer
{
public:
	typedef struct tagVIBufferBeamDesc
	{
		_float3 vRandomMin = { 0.f, 0.f, 0.f };
		_float3 vRandomMax = { 0.f, 0.f, 0.f };
	} VIBUFFERBEAM_DESC;

private:
	CVIBuffer_Beam(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CVIBuffer_Beam(const CVIBuffer_Beam& _Prototype);
	virtual ~CVIBuffer_Beam() = default;

public:
	HRESULT Initialize_Prototype(_uint _iMaxVertexCount);
	virtual HRESULT Initialize(void* _pArg) override;

	void	Initialize_Positions(const _float3& _vStartPos, const _float3& _vEndPos); // 시작/끝 위치 설정 + 그냥 나머지 랜덤 세팅임..
	
	HRESULT	Bind_PointBufferDesc();
	HRESULT Render_Points();

public:
	void			Begin_Update();
	void			End_Update();
	void			Set_StartPosition(_fvector _vStartPosition, _bool _isUpdateAll = true);		// Beam의 처음 위치 설정
	void			Set_EndPosition(_fvector _vEndPosition, _bool _isUpdateAll = true);			// Beam의 마지막 위치 설정
	void			Reset_Positions();

	void			Update_RandomPoints();
	void			Converge_Points(_float _fSpeeds);
public:
	void			Set_RandomMin(const _float3& _vMin) { m_vRandomMin = _vMin; }
	void			Set_RandomMax(const _float3& _vMax) { m_vRandomMax = _vMax; }


private:
	_float3					m_vRandomMin = { 0.f, 0.f, 0.f };
	_float3					m_vRandomMax = { 0.f, 0.f, 0.f };

private:
	_bool					m_isUpdate = { false };
	VTXBEAM*				m_pVertices = { nullptr };
	VTXBEAM*				m_pUpdateVertices = { nullptr };

public:
	static CVIBuffer_Beam* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iMaxVertexCount = 2); // default : 처음위치 & 끝점위치
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;

};

END