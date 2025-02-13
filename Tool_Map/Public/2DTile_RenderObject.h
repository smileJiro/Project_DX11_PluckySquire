#pragma once

#include "Map_Tool_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Map_Tool)

class C2DTile_RenderObject final : public CUIObject
{
public :
	typedef struct tagTIle_RenderObjectDesc : CUIObject::UIOBJECT_DESC
	{
		_float2	fRenderTargetSize;
		_uint	iMapSizeWidth	= 0;
		_uint	iMapSizeHeight	= 0;

		_uint	iIndexSIzeX		= 0;
		_uint	iIndexSIzeY		= 0;

		_uint	iIndexCountX		= 0;
		_uint	iIndexCountY		= 0;

	}TILE_RENDEROBJECT_DESC;
	
	
	typedef struct tagTileTextureInfo
	{
		_wstring strTextureTag;

		_uint iTileTextureSizeX = 0;
		_uint iTileTextureSizeY = 0;

		_uint iTileIndexSizeX = 0;
		_uint iTileIndexSizeY = 0;

		_uint iTileIndexCountX = 0;
		_uint iTileIndexCountY = 0;
	}TILE_TEXTURE_INFO;

private:
	C2DTile_RenderObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	C2DTile_RenderObject(const C2DTile_RenderObject& Prototype);
	virtual ~C2DTile_RenderObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	HRESULT Set_Texture(TILE_TEXTURE_INFO& _tTextureInfo, ID3D11ShaderResourceView* _pTexture);
	HRESULT	Set_Index(_uint _iTileIndex, _int _iTextureInfoIndex, _int _iTextureUVIndex);
	void	Set_OutputPath(const _wstring& _strPath) { m_strOutputPath = _strPath; }



private:
	CShader* m_pShader = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	
	vector<TILE_TEXTURE_INFO> m_TIleTextureInfos;
	
	// Pair : Texture Info Index, Texture UV Index
	// vector Index : Tile Pos Index
	vector<XMINT2> m_TextureIndexs;
	
	_float2		m_fRenderTargetSize = {};
	_uint m_iIndexCount = 0;
	
	_uint m_iWidthSIze = 0;
	_uint m_iHeightSIze = 0;
	_uint m_iWidthCount = 0;
	_uint m_iHeightCount = 0;

	_wstring m_strOutputPath = L"";

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	HRESULT Compute_UV(_uint _iIndex, _float2& _fStartUV, _float2& _fEndUV);
	HRESULT Compute_Tile_Pos(_uint _iIndex, _float2& _fPos);



public:
	static C2DTile_RenderObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;



	// CUIObject을(를) 통해 상속됨
	virtual HRESULT Cleanup_DeadReferences() override;

};

END