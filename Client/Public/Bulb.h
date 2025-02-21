#pragma once
#include "TriggerObject.h"

BEGIN(Engine)
class CGameObject;
class CShader;
class C3DModel;
class C2DModel;
END

BEGIN(Client)
class CBulb  final : public CTriggerObject
{
	typedef struct tagFresnels
	{
		FRESNEL_INFO tInner;
		FRESNEL_INFO tOuter;
	} MULTI_FRESNEL_INFO;

public:	
	typedef struct tagBulbDesc : public CTriggerObject::TRIGGEROBJECT_DESC
	{

	}BULB_DESC;

	enum BULB_SHAPE_USE
	{
		SHAPE_BODY,
		SHAPE_STICKING,
	};

	enum BULB_STATE
	{
		NONE,
		BULB_UP = 1,
		BULB_DOWN = 2,
		BULB_ALL = 3,
		BULB_STATE_END
	};


private:
	explicit CBulb(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CBulb(const CBulb& _Prototype);
	virtual ~CBulb() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	virtual HRESULT			Initialize_3D_Object(BULB_DESC* _pDesc);							// 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
	virtual HRESULT			Initialize_2D_Object(BULB_DESC* _pDesc);

	virtual void			OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other) override;

	virtual void				On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject) override;

private:
	//CShader*				m_pShaderCom[COORDINATE_LAST] = {nullptr};
	//CModel*				m_pModelCom[COORDINATE_LAST] = { nullptr };

	C3DModel*				m_p3DModelCom = { nullptr };
	C2DModel*				m_p2DModelCom = { nullptr };
	CShader*				m_p3DShaderCom = { nullptr };
	CShader*				m_p2DShaderCom = { nullptr };
	ID3D11Buffer*			m_pFresnelBuffer = { nullptr };

	MULTI_FRESNEL_INFO			m_tFresnelInfo;

private:
	const _float4x4*		m_pTargetWorld = { nullptr };

	_bool					m_isSticking = { false };
	_float					m_fMin = 1.5f;

	// Time
	_float2					m_fBulbTime = { 0.8f, 0.f };
	BULB_STATE				m_eBulbState = { BULB_UP };

private:
	HRESULT					Ready_Components(BULB_DESC* _pArg);
	HRESULT					Ready_Buffer(); // Initialize_Prototype에서 준비,
	HRESULT					Bind_ShaderResources_WVP(COORDINATE eCoordinate);

	void					Add_Shape();
	void					Sticking(_float _fTimeDelta);
	void					Default_Move(_float _fTimeDelta);

public:
	static CBulb*			Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg) override;
	virtual void			Free() override;


};
END
