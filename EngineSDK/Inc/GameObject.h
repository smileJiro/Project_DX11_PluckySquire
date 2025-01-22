#pragma once
#include "Controller_Transform.h"

BEGIN(Engine)
class CGameInstance;
class CController_Transform;
class CRay;
class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct tagGameObjectDesc: public CController_Transform::CON_TRANSFORM_DESC
	{
		// 객체의 레벨 아이디
		_uint iCurLevelID;
	}GAMEOBJECT_DESC;

protected:
	CGameObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;

public:
	virtual HRESULT				Initialize_Prototype();								// 프로토 타입 전용 Initialize
	virtual HRESULT				Initialize(void* _pArg);							// 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
	virtual void				Priority_Update(_float _fTimeDelta);				// 특정개체에 대한 참조가 빈번한 객체들이나, 등등의 우선 업데이트 되어야하는 녀석들.
	virtual void				Update(_float _fTimeDelta);
	virtual void				Late_Update(_float _fTimeDelta);
	virtual HRESULT				Render();

protected:/* Component Update */
	void						Priority_Update_Component(_float _fTimeDelta);
	void						Update_Component(_float _fTimeDelta);
	void						Late_Update_Component(_float _fTimeDelta);

public:
	CComponent*					Find_Component(const _wstring& _strComponentTag);
	
public: /* 모드 전환 */
	virtual HRESULT				Change_Coordinate(COORDINATE _eCoordinate, const _float3& _vPosition);
	virtual void                On_CoordinateChange() {}

public:
	// Get
	const _wstring&				Get_Name() const								{ return m_strName; }
	COORDINATE					Get_CurCoord() const							{ return m_pControllerTransform->Get_CurCoord(); }
	CController_Transform*		Get_ControllerTransform() const					{ return m_pControllerTransform; }
	_matrix						Get_WorldMatrix()								{ return m_pControllerTransform->Get_WorldMatrix(); }
 	_vector						Get_Position() const							{ return m_pControllerTransform->Get_State(CTransform::STATE_POSITION); }
	_float3						Get_Scale() const								{ return m_pControllerTransform->Get_Scale(); }
	_bool						Is_Dead() const									{ return m_isDead; }
	_bool						Is_Render() const								{ return m_isRender; }
	_bool						Is_Pooling() const								{ return m_isPooling; }
	_int						Get_CurLevelID() const							{ return m_iCurLevelID; }
	_uint						Get_GameObjectInstanceID() const				{ return m_iInstanceID; }

	// Set
	void						Set_Name(const _wstring& _strName)				{ m_strName = _strName; }
	void						Set_WorldMatrix(_float4x4 _WorldMatrix)			{ m_pControllerTransform->Set_WorldMatrix(_WorldMatrix); }
	void						Set_Position(_fvector _vPos)					{ m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(_vPos, 1.0f)); }
	void						Set_Scale(_float _fX, _float _fY, _float _fZ)	{ m_pControllerTransform->Set_Scale(_fX, _fY, _fZ); }
	void						Set_Scale(const _float3& _vScale)				{ m_pControllerTransform->Set_Scale(_vScale); }
	void						Set_Dead()										{ m_isDead = true; }
	void						Set_Alive()										{ m_isDead = false; }
	virtual void				Set_Render(_bool _isRender)						{ m_isRender = _isRender; }
	void						Set_Pooling(_bool _isPooling)					{ m_isPooling = _isPooling; }
protected:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;
	CGameInstance*				m_pGameInstance = nullptr;
	CController_Transform*		m_pControllerTransform = nullptr; 
	CRay*						m_pRayCom = nullptr;

private:
	static _uint				g_iInstanceIDCount;

protected:
	_uint						m_iInstanceID;
	_wstring					m_strName;
	_uint						m_iCurLevelID = 0;
	_bool						m_isDead = false;
	_bool						m_isRender = true;
	_bool						m_isPooling = false;
protected:
	map<const _wstring, CComponent*> m_Components;

protected:
	HRESULT					Add_Component(_uint _iPrototypeLevelID, const _wstring& _strPrototypeTag, const _wstring& _strComponentTag, CComponent** _ppOut, void* _pArg = nullptr);
	HRESULT					Add_Component(const _wstring& _strComponentTag, CComponent* _pComponent);

public:
	virtual CGameObject*	Clone(void* _pArg) = 0; // Clone() 프로토 타입이나 객체의 복사시 사용된다.
	virtual void			Free() override;
	virtual HRESULT			Cleanup_DeadReferences() = 0; // 참조 중인 게임오브젝트들 중 죽은 Dead상태인 오브젝트를 체크해서 참조해제.(액티브 false인 애들때매 만듬)

#ifdef _DEBUG
public:
	virtual HRESULT Imgui_Render_ObjectInfos();
#endif // _DEBUG

};

END