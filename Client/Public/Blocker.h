#pragma once
#include "PartObject.h"
/* 2D, 3D 모두 가능하게, 다만 2D와 3D를 동시에 할순없다? */
/* 초기 좌표 상태에 대한 밸류를 가지고, 해당 밸류에 맞는 2D Collision 혹은, Static Actor를 생성해주는 클래스 */
/* 기능. 없음 >>> 콜라이더 컴포넌트를 콜리전매니저에 던져주는 기능. */
BEGIN(Client)
class CBlocker final : public CPartObject
{
public:
	typedef struct tagBlockerDesc : public CPartObject::tagPartObjectDesc
	{
		_bool isFloor = false; /* 오브젝트가 밟고 설수있는 판별의 기준 (점프판별시 활용)*/
	}BLOCKER_DESC;
	typedef struct tag2DBlockerDesc : public BLOCKER_DESC
	{
		_float2 vColliderExtents = { 50.f, 50.f };
		_float2 vColliderScale = { 1.0f, 1.0f };
		_float2 vOffsetPosition = {};
	}BLOCKER2D_DESC;
	typedef struct tag3DBlockerDesc : public BLOCKER_DESC
	{

	}BLOCKER3D_DESC;
protected:
	CBlocker(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, COORDINATE _eCoord);
	CBlocker(const CBlocker& _Prototype);
	virtual ~CBlocker() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;


private:
	COORDINATE				m_eBlockerCoord = COORDINATE_LAST;

public:
	_bool					Is_Floor() const { return m_isFloor; }
private:
	_bool					m_isFloor = false;

private:
	HRESULT					Ready_Component(BLOCKER_DESC* _pDesc);
public:
	/* 2d용 3d용 desc 따로만들고 둘중 하나 넣기? 프로토타입 시점에 2D 3D 프로토타입 각자 만들고, */
	static CBlocker*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, COORDINATE _eCoord);
	CGameObject*		Clone(void* _pArg) override;
	void				Free() override;
};
END
