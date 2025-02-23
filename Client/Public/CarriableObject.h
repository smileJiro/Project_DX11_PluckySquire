#pragma once
#include "ModelObject.h"
#include "AnimEventReceiver.h"
#include "Interactable.h"

BEGIN(Client)
class CPlayer;
class CCarriableObject :
	public CModelObject, public IAnimEventReceiver, public IInteractable
{
public:
	enum CARRIABLE_OBJ_ID
	{
		DIEC,
		WORD,
		KEY,
		JUMP_PADDLE,
		BOX,
		CARROT,
		GRAPE,
		CARRIABLE_ID_LAST
	};

	typedef struct tagCarriableDesc : public CModelObject::MODELOBJECT_DESC
	{
		CARRIABLE_OBJ_ID eCrriableObjId = CARRIABLE_OBJ_ID::CARRIABLE_ID_LAST;
		_float3 vHeadUpRoolPitchYaw3D = { 0.f,1.f,0.f };
		_float3 vHeadUpOffset3D = { 0.f,1.f,0.f };
		_float fHeadUpHeight2D = 100.0f;
	}CARRIABLE_DESC;
protected:
	explicit CCarriableObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CCarriableObject(const CCarriableObject& _Prototype);
	virtual ~CCarriableObject() = default;

public:
	HRESULT Initialize(void* _pArg);

	virtual void		Update(_float _fTimeDelta) override;
	virtual void		Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

	virtual HRESULT	 Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition = nullptr) override;

public:
	// IInteractable을(를) 통해 상속됨
	virtual void Interact(CPlayer* _pUser) override;
	virtual _bool Is_Interactable(CPlayer* _pUser) override;
	virtual _float Get_Distance(COORDINATE _eCOord, CPlayer* _pUser) override;
	CARRIABLE_OBJ_ID Get_CarriableObjId() const { return m_eCarriableId; }
public:
	_bool Is_Carrying() { return nullptr != m_pCarrier; }
	HRESULT Set_Carrier(CPlayer* _pCarrier);
	void Throw(_fvector _vForce);
	void Set_Kinematic(_bool _bKinematic);
	void Set_ParentBodyMatrix(COORDINATE _eCoord, const _float4x4* _pBodyMatrix) { m_pParentBodyMatrices[_eCoord] = _pBodyMatrix; }
	const _float4x4& Get_HeadUpMatrix(COORDINATE _eCoord) { return m_matHeadUpMatrix[_eCoord]; };

protected:
	CPlayer* m_pCarrier = nullptr;
	CCollider* m_pBody2DColliderCom = nullptr;
	const _float4x4* m_pParentBodyMatrices[COORDINATE_LAST] = { nullptr }; // 부모의 월드 행렬의 주소

	_float4x4 m_matHeadUpMatrix[COORDINATE_LAST];


	//THROW
	//공중에 떠 있기 위한 가상의 Socket 
	_float4x4 m_vThrowSocketOffset2D;

	//실험용
	_float m_fRestitution = 0.5f;

	CARRIABLE_OBJ_ID m_eCarriableId = CARRIABLE_OBJ_ID::CARRIABLE_ID_LAST;
public:
	static CCarriableObject* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;

};

END