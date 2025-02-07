#pragma once
#include "ContainerObject.h"

BEGIN(Client)

class CRayShape : public CContainerObject
{
public:
	typedef struct tagRayShapeDesc : public CContainerObject::CONTAINEROBJ_DESC
	{
		SHAPE_TYPE eShapeType;
		_float fHalfHeight;
		_float fRadius;
		_float3 vHalfExtents;
		_float3 vOffsetTrans = { 0.f,0.f,0.f };
		_float fRotAngle = { 0.f };
	}RAYSHAPE_DESC;

protected:
	CRayShape(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CRayShape(const CRayShape& _Prototype);
	virtual ~CRayShape() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* _pArg) override;
	virtual void		Priority_Update(_float _fTimeDelta) override;
	virtual void		Update(_float _fTimeDelta) override;
	virtual void		Late_Update(_float _fTimeDelta) override;
	virtual HRESULT		Render() override;

private:
	virtual HRESULT Ready_ActorDesc(void* _pArg);
	virtual HRESULT Ready_Components();
	virtual HRESULT Ready_PartObjects();

public:
	static CRayShape* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject* Clone(void* _pArg);
	virtual void Free() override;
};

END