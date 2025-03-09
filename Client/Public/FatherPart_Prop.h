#pragma once
#include "ModelObject.h"

BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CFatherPart_Prop final : public CModelObject
{
public:
	typedef struct tagFatherPartPropDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_uint iFatherPartID = 0;

	}FATHERPART_PROP_DESC;
	enum STATE { STATE_IDLE, STATE_PICKUP, STATE_LAST };
private:
	CFatherPart_Prop(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CFatherPart_Prop(const CFatherPart_Prop& _Prototype);
	virtual ~CFatherPart_Prop() = default;

public:
	virtual HRESULT				Initialize_Prototype();								// 프로토 타입 전용 Initialize
	virtual HRESULT				Initialize(void* _pArg);							// 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
	virtual void				Priority_Update(_float _fTimeDelta);				// 특정개체에 대한 참조가 빈번한 객체들이나, 등등의 우선 업데이트 되어야하는 녀석들.
	virtual void				Update(_float _fTimeDelta);
	virtual void				Late_Update(_float _fTimeDelta);
	virtual HRESULT				Render();

private:
	STATE						m_ePreState = STATE::STATE_LAST;
	STATE						m_eCurState = STATE::STATE_LAST;

public:
	static CFatherPart_Prop*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext); 
	CGameObject*				Clone(void* _pArg) override; 
	void						Free() override;
	HRESULT						Cleanup_DeadReferences() override; 
};
END
