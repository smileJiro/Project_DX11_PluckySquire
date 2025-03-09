#pragma once
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
END 

BEGIN(Client)

class CSimple_UI : public CGameObject
{
public:
	typedef struct tagSimpleUI : CGameObject::GAMEOBJECT_DESC
	{
		_float4 vUIInfo = {}; // x : posX, y : posX, z : sizeX, w : sizeX
		_wstring strTexturePrototypeTag;
		PASS_VTXPOSTEX ePassIndex = PASS_VTXPOSTEX::DEFAULT;
		_uint iTextureIndex = 0;
	}SIMPLE_UI_DESC;
private:
	CSimple_UI(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CSimple_UI(const CSimple_UI& _Prototype);
	virtual ~CSimple_UI() = default;

public:
	virtual HRESULT				Initialize_Prototype();								// 프로토 타입 전용 Initialize
	virtual HRESULT				Initialize(void* _pArg);							// 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
	virtual void				Priority_Update(_float _fTimeDelta);				// 특정개체에 대한 참조가 빈번한 객체들이나, 등등의 우선 업데이트 되어야하는 녀석들.
	virtual void				Update(_float _fTimeDelta);
	virtual void				Late_Update(_float _fTimeDelta);
	virtual HRESULT				Render();

public:
	// Get
	_int Get_TextureIndex(_int _iTextureIndex) const { return m_iTextureIndex; }
	// Set
	void Set_TextureIndex(_uint _iTextureIndex);
	void Set_UIPosition(_float2 _vUIPosition) { m_fX = _vUIPosition.x;  m_fY = _vUIPosition.y; }
	void Set_UISize(_float2 _vUISize) { m_fSizeX = _vUISize.x; m_fSizeY = _vUISize.y; }
protected:
	CTexture*					m_pTextureCom = nullptr;
	_int						m_iTextureIndex = 0;
	CShader*					m_pShaderCom = nullptr;
	PASS_VTXPOSTEX				m_ePassIndex = PASS_VTXPOSTEX::DEFAULT;
	CVIBuffer_Rect*				m_pVIBufferCom = nullptr;

private: /* UI 처럼 렌더링을 해야해 직교투영으로 */
	_float						m_fX = 0.0f;
	_float						m_fY = 0.0f;
	_float						m_fSizeX = 1.f;
	_float						m_fSizeY = 1.f;

	_float4x4					m_ViewMatrix = {};
	_float4x4					m_ProjMatrix = {};

private:
	HRESULT						Bind_ShaderResources_WVP();
	void						Apply_UIPosition();
	void						Apply_UIScaling();

private:
	HRESULT						Ready_Components(SIMPLE_UI_DESC* _pDesc);
public:
	static CSimple_UI*			Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject*				Clone(void* _pArg) override;
	void						Free() override;

public:
	HRESULT						Cleanup_DeadReferences() override;
};

END