#pragma once
#include "2DMapObject.h"

BEGIN(Engine)
class C2DModel;
END

BEGIN(Client)

class C2DMapWordObject : public C2DMapObject
{
protected:
	enum WORD_ACTION_TYPE
	{
		IMAGE_CHANGE,			//0
		WORD_OBJECT_ACTIVE,		//1
		ANIMATION_CHANGE,		//2
		POSITION_CHANGE_X,		//3
		POSITION_CHANGE_Y,		//4
		COLLIDER_ACTIVE,		//5
		WORD_OBJECT_RENDER,		//6
		PLAY_SFX,				//7
		WORD_ACTION_LAST		//8
	};

public :
	typedef struct tagWordAction
	{
		_uint				iControllerIndex;
		_uint				iContainerIndex;
		_uint				iWordType;
		WORD_ACTION_TYPE	eAction;
		any					anyParam;
	} WORD_ACTION;

public :
	typedef struct tag2DWordObjectDesc : public C2DMapObject::MAPOBJ_DESC
	{
		json WordObjectJson;

	}WORD_OBJECT_DESC;
protected:
	C2DMapWordObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	C2DMapWordObject(const C2DMapWordObject& _Prototype);
	virtual ~C2DMapWordObject() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* _pArg) override;
	virtual void					Priority_Update(_float _fTimeDelta) override;
	virtual void					Update(_float _fTimeDelta) override;
	virtual void					Late_Update(_float _fTimeDelta) override;
	virtual HRESULT					Render() override;

	virtual _bool					Action_Execute(_uint _iControllerIndex, _uint _iContainerIndex, _uint _iWordIndex, _bool _isFirst);
	virtual _bool					Check_Action(_uint _iControllerIndex, _uint _iContainerIndex, _uint _iWordIndex);
	virtual _bool					Register_Action(_uint _iControllerIndex, _uint _iContainerIndex, _uint _iWordIndex);
	const WORD_ACTION*				Find_Action(_uint _iControllerIndex, _uint _iContainerIndex, _uint _iWordIndex);
	virtual void					Action_Process(_float _fTimeDelta);
	
	virtual void					On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void					Active_OnEnable();
	virtual void					Active_OnDisable();
	void							MapWordObject_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx);



protected :
	_wstring				m_strWordObjectTag;
	_uint					m_iModelIndex = 0 ;
	vector <_wstring>		m_ModelNames;
	vector <_wstring>		m_SFXNames;
	vector <WORD_ACTION>	m_Actions;

	_bool					m_isObjectChange = false;
	_bool					m_isPlaySFX = false;
	_bool					m_isRegistered = false;
	_uint					m_iControllerIndex = 0;	
	_uint					m_iContainerIndex = 0;
	_uint					m_iWordIndex = 0;


	_bool					m_isStartChase = { false };
	_bool					m_isStartAction = { false };
	_float					m_fTargetDiff = { -1.f };
	_float					m_fActionIntervalSecond = { 2.f };
	_float					m_fAccTime = { 0.f };


	_bool					m_IsWordActive = true;
	_bool					m_IsWordRender = true;


public:
	static C2DMapWordObject*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*		Clone(void* _pArg) override;
	virtual void				Free() override;
};
END
