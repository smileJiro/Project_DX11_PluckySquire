#pragma once
#include "MapObject.h"


BEGIN(Engine)
class CCollider;
END


BEGIN(Client)

class C2DMapObject abstract : public CMapObject
{
public:
	typedef struct tag2DMapObjectDesc : public CMapObject::MODELOBJECT_DESC
	{
		//2D
		_bool is2DImport = false;

		_bool isActive = false;
		_bool isBackGround = false;
		_bool isSorting = false;
		_bool isCollider = false;


		_uint eActiveType;
		_uint eColliderType;

		_float2 fSorting_Offset_Pos = {};
		_float2 fCollider_Offset_Pos = {};
		_float2 fCollider_Extent = {};
		_float	fCollider_Radius = 0.f;

	}MAPOBJ_DESC;
protected:
	C2DMapObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	C2DMapObject(const C2DMapObject& _Prototype);
	virtual ~C2DMapObject() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* _pArg) override;
	virtual void					Priority_Update(_float _fTimeDelta) override;
	virtual void					Update(_float _fTimeDelta) override;
	virtual void					Late_Update(_float _fTimeDelta) override;
	virtual HRESULT					Render() override;
	virtual HRESULT					Render_Shadow() override;


public:
	_bool							Is_Active() { return m_isActive; };
	_bool							Is_Collider() { return m_isCollider; };
	_bool							Is_Sorting() { return m_isSorting; };
	_bool							Is_BackGround() { return m_isBackGround; };

protected:
	HRESULT							Ready_Collider(MAPOBJ_DESC* Desc, _bool _isBlock);

protected:

	_bool							m_isActive = false;
	_bool							m_isCollider = false;
	_bool							m_isBackGround = false;
	_bool							m_isSorting = true;

public:
	virtual void			Free() override;
};
END
