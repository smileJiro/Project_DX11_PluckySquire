#pragma once
#include "GameObject.h"
BEGIN(Engine)
class CVIBuffer_Mesh;
class CShader;
class CTexture;
END

BEGIN(Client)
class CBackGroundObject : public CGameObject
{
public:
	typedef struct tagBackGroundObjectDesc : CGameObject::GAMEOBJECT_DESC
	{
		_wstring strTexturePrototypeTag;
		PASS_VTXMESH eShaderPass = PASS_VTXMESH::DEFAULT;
	}BACKGROUNDOBJ_DESC;
private:
	CBackGroundObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CBackGroundObject(const CBackGroundObject& _Prototype);
	virtual ~CBackGroundObject() = default;

public:
	virtual HRESULT				Initialize_Prototype();								// 프로토 타입 전용 Initialize
	virtual HRESULT				Initialize(void* _pArg);							// 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
	virtual void				Priority_Update(_float _fTimeDelta);				// 특정개체에 대한 참조가 빈번한 객체들이나, 등등의 우선 업데이트 되어야하는 녀석들.
	virtual void				Update(_float _fTimeDelta);
	virtual void				Late_Update(_float _fTimeDelta);
	virtual HRESULT				Render();

public:
	HRESULT						Chage_BackGroundTexture(const _wstring& _strTexturePrototypeTag);

public:
	CVIBuffer_Mesh*				m_pVIBufferCom = nullptr;
	CShader*					m_pShaderCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;
	PASS_VTXMESH				m_eShaderPass = PASS_VTXMESH::BACKGROUND;

	_float3						m_vFilterColor = { 0.73f, 0.882f, 1.0f };
	_float						m_fBrightness = 1.0f;

private:
	HRESULT						Ready_Components(BACKGROUNDOBJ_DESC* _pDesc);
	HRESULT						Bind_ShaderResources();
public:
	static CBackGroundObject*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject*				Clone(void* _pArg)override;
	void						Free() override;

public:
	HRESULT						Cleanup_DeadReferences() override;
};

END