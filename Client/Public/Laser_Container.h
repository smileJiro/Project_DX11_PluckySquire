#pragma once
#include "ContainerObject.h"
#include "Client_Defines.h"

BEGIN(Client)
class CLaser_Container final : public CContainerObject
{
public:
	typedef struct tagProjectile_Monster_Desc : public CContainerObject::CONTAINEROBJ_DESC
	{
		_wstring	strInitSectionTag;
		_float2		fStartPos;
		_float2		fEndPos;
		_float		fMoveSpeed;
		F_DIRECTION eDir;
	}LASER_DESC;


	enum LASER_BEAM_ANIM_STATE
	{
		LASER_BEAM_LENGTH_1,
		LASER_BEAM_LENGTH_2,
		LASER_BEAM_LENGTH_3,
		LASER_BEAM_LENGTH_4,
		LASER_BEAM_LENGTH_5,
		LASER_BEAM_BEGIN,
		LASER_BEAM_END,
		LASER_BEAM_LAST,
	};

	enum LASER_MACHINE_ANIM_STATE
	{
		LASER_MACHINE_LENGTH_DOWN,
		LASER_MACHINE_LENGTH_TOP,
		LASER_MACHINE_LENGTH_UP,
		LASER_MACHINE_LENGTH_SIDE,
		LASER_MACHINE_LENGTH_LAST,
	};


	enum LASER_PART { PART_BODY, PART_BEAM_START_EFFECT, PART_BEAM_EFFECT, PART_BEAM_END_EFFECT, PART_END };

private:
	CLaser_Container(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CLaser_Container(const CLaser_Container& _Prototype);
	virtual ~CLaser_Container() = default;

public:
	virtual HRESULT					Initialize_Prototype(); // 프로토 타입 전용 Initialize
	virtual HRESULT					Initialize(void* _pArg); // 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
	virtual void					Priority_Update(_float _fTimeDelta) override;
	virtual void					Update(_float _fTimeDelta) override;
	virtual void					Late_Update(_float _fTimeDelta) override;
	virtual HRESULT					Render() override;

public:
	virtual void					On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject);
	virtual void					On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject);
	virtual void					On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject);

	virtual void					Active_OnEnable() override;
	virtual void					Active_OnDisable() override;

	virtual HRESULT					Ready_Components(LASER_DESC* _pDesc);
	virtual HRESULT					Ready_PartObjects();


	void							Compute_Beam();

private:;
	F_DIRECTION		m_eDir = {};
	_float2			m_fDir = {};

	_bool			m_bIsBeamOn = true;
	_bool			m_bIsBeamRotate = false;
	_bool			m_BackMove = false;

	_float2			m_fStartPos = {};
	_float2			m_fEndPos = {};
	_float2			m_fTargetPos = {};
	_float			m_fMoveSpeed = {};

	_float2			m_fBeamStartPos = {};
	_float2			m_fBeamEndPos = {};
	
	_float			m_fBeamLength = {};

	CCollider*		m_pSizeTriggerCollider = {};
	CCollider*		m_pBeamCollider = {};

	_int			m_iBeamTargetIndex = {};

private:
	//virtual HRESULT					Ready_ActorDesc(void* _pArg);
	//virtual HRESULT					Ready_Components();
	//virtual HRESULT					Ready_PartObjects();

public:
	static CLaser_Container* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
};
END