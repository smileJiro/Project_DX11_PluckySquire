#pragma once
#include "CarriableObject.h"

BEGIN(Client)
class CRabbitLunch :
    public CCarriableObject
{
public:
	enum LUNCH_TYPE
	{
		CARROT_1,
		CARROT_2,
		CARROT_3,
		GRAPE_1,
		GRAPE_2,
		GRAPE_3,
		LUNCH_TYPE_LAST
	};

	typedef struct tagRabbitLunchDesc : public CCarriableObject::CARRIABLE_DESC
	{
		LUNCH_TYPE eLunchType = LUNCH_TYPE::LUNCH_TYPE_LAST;
	}RABBITLUNCH_DESC;
protected:
	explicit CRabbitLunch(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CRabbitLunch(const CRabbitLunch& _Prototype);
	virtual ~CRabbitLunch() = default;
public:
	HRESULT Initialize(void* _pArg);
	virtual void		Update(_float _fTimeDelta) override;
	virtual void		Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	LUNCH_TYPE m_eLunchType = LUNCH_TYPE::LUNCH_TYPE_LAST;
public:
	static CRabbitLunch* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END