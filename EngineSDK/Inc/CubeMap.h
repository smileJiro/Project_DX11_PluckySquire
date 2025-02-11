#pragma once
#include "GameObject.h"
BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Cube;
class ENGINE_DLL CCubeMap : public CGameObject
{
public:
	typedef struct tagCubeMapDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_wstring strCubeMapPrototypeTag;
		_wstring strBRDFPrototypeTag;

		_uint	 iRenderGroupID = 0;
		_uint	 iPriorityID = 0;
	}CUBEMAP_DESC;

	typedef struct tagCubePixelConstData
	{
		_int iDrawTextureIndex = 0; // enum 사용
		_float fMipLevel = 0.0f;
		_float dummy1;
		_float dummy2;
	}CONST_PS_CUBE;

	enum TEX_TYPE { TEX_ENV, TEX_SPEC, TEX_IRR, TEX_LAST };

protected:
	CCubeMap(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCubeMap(const CCubeMap& _Prototype);
	virtual ~CCubeMap() = default;

public:
	HRESULT				Initialize_Prototype() override;
	HRESULT				Initialize(void* _pArg) override;
	void				Priority_Update(_float _fTimeDelta) override;				// 특정개체에 대한 참조가 빈번한 객체들이나, 등등의 우선 업데이트 되어야하는 녀석들.
	void				Update(_float _fTimeDelta) override;
	void				Late_Update(_float _fTimeDelta) override;
	HRESULT				Render() override;

public:
	HRESULT				Bind_IBLTexture(CShader* _pShaderCom, const _char* _pBRDFConstName, const _char* _pSpecularConstName, const _char* _pIrradianceConstName);

protected:
	CShader*			m_pShaderCom = nullptr;
	CTexture*			m_pCubeTexturesCom = {};		// multi texture
	CTexture*			m_pBRDFTextureCom = nullptr;
	CVIBuffer_Cube*		m_pVIBufferCom = nullptr;

	CONST_PS_CUBE		m_tPixelConstData = {};
	ID3D11Buffer*		m_pPixeConstBuffer = nullptr;

protected:
	_uint				m_iRenderGroupID = 0;
	_uint				m_iPriorityID = 0;

private:
	HRESULT				Ready_Components(CUBEMAP_DESC* _pDesc);
	HRESULT				Bind_ShaderResources();

public:
	static CCubeMap*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject*		Clone(void* _pArg) override;
	void				Free() override;
	HRESULT				Cleanup_DeadReferences() override;
};

END