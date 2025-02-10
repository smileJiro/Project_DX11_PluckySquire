#pragma once
#include "Model.h"
#include "Animation2D.h"


BEGIN(Engine)
class CVIBuffer_Rect;
class CTexture;
class ENGINE_DLL C2DModel :
    public CModel
{
protected:
    C2DModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    C2DModel(const C2DModel& _Prototype);
    virtual ~C2DModel() = default;
public:
    virtual HRESULT			Initialize_Prototype(const _char* _szModel2DFilePath, _bool _bNoJson = false);
    HRESULT			Initialize(void* _pDesc);

    virtual HRESULT			Render(CShader* _Shader, _uint _iShaderPass)override;
    virtual _bool			Play_Animation(_float _fTimeDelta, _bool _vReverse)override;
    //Get
    virtual _uint Get_AnimCount() override;
	virtual _uint Get_CurrentAnimIndex() override { return m_iCurAnimIdx; }
    virtual _float Get_CurrentAnimProgeress() override;
    virtual CAnimation* Get_Animation(_uint iAnimIndex) override;
    const _matrix* Get_CurrentSpriteTransform();

    //Set
    virtual void Set_AnimationLoop(_uint _iIdx, _bool _bIsLoop)override;
    virtual void Set_Animation(_uint _iIdx, _bool _bReverse = false)override;
    virtual void Switch_Animation(_uint _iIdx, _bool _bReverse = false)override;
    virtual void To_NextAnimation()override;
    virtual void			Set_AnimSpeedMagnifier(_uint iAnimIndex, _float _fMag) override;



#ifdef _DEBUG
    CSpriteFrame* Get_SpriteFrame() { return m_pNonAnimSprite; };
    CTexture* Get_Texture() {
        if (m_Textures.empty())
            return nullptr;
        return (*m_Textures.begin()).second;
    };
#endif // _DEBUG


protected:
    CVIBuffer_Rect* m_pVIBufferCom = nullptr;

    map<string, CTexture*> m_Textures;
    vector<CAnimation2D* >m_Animation2Ds ;

    CSpriteFrame* m_pNonAnimSprite = nullptr;

	_uint m_iCurAnimIdx = 0;
public:
    static C2DModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, _bool _bNoJson = false);
   
    virtual CComponent* Clone(void* _pArg) override;
    virtual void Free() override;


};

END