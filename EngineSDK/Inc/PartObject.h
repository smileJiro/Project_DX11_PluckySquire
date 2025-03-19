#pragma once

#include "ActorObject.h"

BEGIN(Engine)

class ENGINE_DLL CPartObject abstract : public CActorObject
{
public:
	typedef struct tagPartObjectDesc : public CActorObject::ACTOROBJECT_DESC
	{
		// 2D, 3D 개별적인 부모 매트릭스
		const _float4x4* pParentMatrices[COORDINATE_LAST] = {};
	}PARTOBJECT_DESC;
protected:
	CPartObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPartObject(const CPartObject& _Prototype);
	virtual ~CPartObject() = default;

	
public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	_vector					Get_OffsetPosition() { return __super::Get_FinalPosition(); }; // PartObject는 최종 매트릭스에서 위치값 추출해서 리턴.
	_vector					Get_FinalPosition() const override; // PartObject는 최종 매트릭스에서 위치값 추출해서 리턴.
	virtual _vector			Get_FinalPosition(COORDINATE _eCoord) const override;

	_float3					Get_FinalScale() const override; // PartObject는 최종 매트릭스에서 위치값 추출해서 리턴.
	virtual _float3			Get_FinalScale(COORDINATE _eCoord) const override;
	const _float4x4*		Get_FinalWorldMatrix_Ptr(COORDINATE _eCoord) { return &m_WorldMatrices[_eCoord]; }
	virtual _matrix			Get_FinalWorldMatrix() override;
	void Set_SocketMatrix(COORDINATE _eCoord, const _float4x4* _pSocketMatrix) { m_pSocketMatrix[_eCoord] = _pSocketMatrix; }
	void Set_ParentMatrix(COORDINATE _eCoord, const _float4x4* _pSocketMatrix) { m_pParentMatrices[_eCoord] = _pSocketMatrix; }

protected:
	_float4x4				m_WorldMatrices[COORDINATE_LAST] = {}; // 자기자신의 최종 행렬 
	const _float4x4*		m_pParentMatrices[COORDINATE_LAST] = {nullptr}; // 부모의 월드 행렬의 주소
	const _float4x4*		m_pSocketMatrix[COORDINATE_LAST] = { nullptr };
public:
	virtual void			Free() override;
	HRESULT					Cleanup_DeadReferences() override; 

#ifdef NDEBUG
public:
	HRESULT Imgui_Render_ObjectInfos() override;
#endif // _DEBUG
};

END