#pragma once
#include "ActorObject.h"

BEGIN(Engine)

class ENGINE_DLL CTriggerObject abstract : public CActorObject
{
public:
	typedef struct tagTriggerObjectDesc : public CActorObject::ACTOROBJECT_DESC
	{
		SHAPE_TYPE					eShapeType = { SHAPE_TYPE::LAST };
		_float3						vHalfExtents = { 0.5f, 0.5f, 0.5f };	// Box
		_float						fRadius = { 0.5f };						// Sphere
	
		_uint						iFillterMyGroup = {};
		_uint						iFillterOtherGroupMask = {};
	}TRIGGEROBJECT_DESC;

protected:
	CTriggerObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTriggerObject(const CTriggerObject& _Prototype);
	virtual ~CTriggerObject() = default;


public:
#ifdef _DEBUG
	// Tool에서 확인용으로 쓰는 것
	_uint						Get_TriggerType() { return m_iTriggerType; }		
	void						Set_TriggerType(_uint _iTriggerType) { m_iTriggerType = _iTriggerType; }
#endif

public:
	virtual HRESULT				Initialize_Prototype();								// 프로토 타입 전용 Initialize
	virtual HRESULT				Initialize(void* _pArg);							// 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
	virtual void				Late_Update(_float _fTimeDelta);

protected:
	SHAPE_TYPE					m_eShapeType = { SHAPE_TYPE::LAST };

	_int						m_iTriggerID = {};

private:
	static _int					g_iNextID;

#ifdef _DEBUG
	_uint						m_iTriggerType;
#endif
public:
	void						Free() override;
	//HRESULT					Cleanup_DeadReferences() override;
};

END