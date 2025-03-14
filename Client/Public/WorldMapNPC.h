#pragma once
#include "ContainerObject.h"

// 이걸 상속 받는 놈들은 함수를 사용하게한다.
// abstract로 사용하게 한다.
// 이건 객체 생성하지 아니한다.
// 캐릭터 이동을 계산해준다.
// json을.. 여기로 받는다?
// 근데 객체 생성은 안하는데..



/// <생각 중>
/// json
/// 시작 위치 및 종료 위치 필요
/// 애니메이션 어떤걸로 변경할지 필요
/// 각 라인 마다 읽어서 해당 위치로 이동한다.
/// 종료 위치로 왔을 때 캐릭터의 애니메이션을 아이들로 변경 시킨다.
/// 
/// 
/// 
/// </생각 중>

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END

BEGIN(Client)
class CWorldMapNPC final  : public CContainerObject
{

protected:
	explicit CWorldMapNPC(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CWorldMapNPC(const CWorldMapNPC& _Prototype);
	virtual ~CWorldMapNPC() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;


protected:
	//virtual HRESULT			Ready_Components() override;

public:
	static CWorldMapNPC*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	HRESULT					Cleanup_DeadReferences() override;
};

END