#pragma once
#include "Animation2D.h"

BEGIN(AnimTool)
class CToolSpriteFrame :
	public CSpriteFrame
{
private:
	CToolSpriteFrame():CSpriteFrame() {};
	CToolSpriteFrame(const CToolSpriteFrame& _Prototype);
	~CToolSpriteFrame();

public:
	HRESULT			Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, json& _jData, map<string, CTexture*>& _Textures);

	HRESULT				Export(ofstream& _outfile);
private:
	vector<_float4> vBakedRenderDatas;
public:
	static CToolSpriteFrame* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, json& _jData, map<string, CTexture*>& _Textures);
	static CToolSpriteFrame* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _char* _szDirPath, ifstream& _infIle, map<string, CTexture*>& _Textures);
	virtual CToolSpriteFrame* Clone();
	virtual void Free() override;

};
class CToolAnimation2D :
    public CAnimation2D
{
private:
	CToolAnimation2D():CAnimation2D() {};
	CToolAnimation2D(const CToolAnimation2D& _Prototype);

public:
	HRESULT			Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, json& _jData, map<string, json>& _jPaperSprites, map<string, CTexture*>& _Textures);


    HRESULT			Export(ofstream& _outfile);

	//Set
	void						Set_CurrentFrame(_uint _iFrameIndex);
	void						Set_Progerss(_float _fTrackPos);
	//Get
	_uint						Get_CurrentFrame();
	_float					Get_Progerss();
private:
	map<string, CTexture*> m_AnimTextures;

public:
	static CToolAnimation2D* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, json& _jData, map<string, json>& _jPaperSprites, map<string, CTexture*>& _Textures);
	static CToolAnimation2D* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _char* _szDirPath, ifstream& _infIle, map<string, CTexture*>& _Textures);
	virtual CToolAnimation2D* Clone();
	virtual void Free() override;
};

END