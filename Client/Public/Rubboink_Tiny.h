#pragma once
#include "SlipperyObject.h"

BEGIN(Engine)
class CModelObject;
END

BEGIN(Client)
class CMudPit;
class CRubboink_Tiny :
    public CSlipperyObject
{
public:
	enum PARTS
	{
		PARTS_BODY,
		PARTS_FACE,
		PARTS_ZZZ,
		PARTS_END
	};
	enum STATE
	{
		ZZZ,
		IMPACT,
		ROLL,
		DIVE,
		STATE_END
	};
	enum BODY_ANIM
	{
		BODY_ANIM_MUD_IDLE,
		BODY_ANIM_MUD_DIVE,
		BODY_ANIM_FACELESS,
		BODY_ANIM_IMPACT,
		BODY_ANIM_ROLL,
		BODY_ANIM_LAST
	};
	enum FACE_ANIM
	{
		FACE_ANIM_SLEEP,
		FACE_ANIM_SLEEPINTO,
		FACE_ANIM_LAST
	};
	enum ZZZ_ANIM
	{
		ZZZ_ANIM_ZZZ,
		ZZZ_ANIM_INTO,
		ZZZ_ANIM_LAST
	};
	typedef struct tagTinyDesc : public CSlipperyObject::SLIPPERY_DESC
	{
		CMudPit* pMudPit = nullptr;
	}TINY_DESC;
public:
	CRubboink_Tiny(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CRubboink_Tiny(const CRubboink_Tiny& _Prototype);
	virtual ~CRubboink_Tiny() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual void Late_Update(_float _fTimeDelta) override;

	HRESULT Ready_Parts();

public:
	virtual void On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx);
private:
	virtual void On_StartSlip(_vector _vDirection) override;
	virtual void On_Impact(CGameObject* _pOtherObject) override;
private:
	CModelObject* m_pBody = nullptr;
	CModelObject* m_pFace = nullptr;
	CModelObject* m_pZZZ = nullptr;

	CMudPit* m_pMudPit = nullptr;
	_bool m_bSlipped = false;
public:
	static CRubboink_Tiny* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};
END
