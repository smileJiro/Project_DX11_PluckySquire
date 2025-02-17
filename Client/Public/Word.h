#pragma once
#include "CarriableObject.h"
#include "AnimEventReceiver.h"
#include "Interactable.h"

BEGIN(Client)
class CPlayer;
class CWord final :
	public CCarriableObject
{
	enum WORD_TYPE
	{
		WORD_FULL,
		WORD_BIG,
		WORD_SMALL,
		WORD_CLOSE,
		WORD_OPEN,
		WORD_END,
	};

	//typedef struct tagWordDesc : public CCarriableObject::DESC
	//{

	//};

protected:
	explicit CWord(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CWord(const CWord& _Prototype);
	virtual ~CWord() = default;

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

public:
	static CWord* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;

};

END