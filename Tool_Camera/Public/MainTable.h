#pragma once
#include "ActorObject.h"

// 메인 책상 역할을 하는 Static Actor를 가진 빈객체
BEGIN(Camera_Tool)
class CMainTable : public CActorObject
{
private:
	CMainTable(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CMainTable(const CMainTable& _Prototype);
	virtual ~CMainTable() = default;
public:
	virtual HRESULT				Initialize_Prototype();								// 프로토 타입 전용 Initialize
	virtual HRESULT				Initialize(void* _pArg);							// 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.

public:
	static CMainTable*			Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject*				Clone(void* _pArg) override;
	void						Free() override;
};
END
