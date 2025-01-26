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
    virtual HRESULT			Initialize_Prototype(const _char* _szModel2DFilePath);
    HRESULT			Initialize(void* _pDesc);

    virtual HRESULT			Render(CShader* _Shader, _uint _iShaderPass)override;
    virtual _bool			Play_Animation(_float _fTimeDelta)override;

    //Get
    virtual _uint Get_AnimCount() override;
	virtual _uint Get_CurrentAnimIndex() override { return m_iCurAnimIdx; }
    virtual _float Get_CurrentAnimProgeress() override;
    virtual CAnimation* Get_Animation(_uint iAnimIndex) override;
    const _matrix* Get_CurrentSpriteTransform();

    //Set
    virtual void Set_AnimationLoop(_uint _iIdx, _bool _bIsLoop)override;
    virtual void Set_Animation(_uint _iIdx)override;
    virtual void Switch_Animation(_uint _iIdx)override;
    virtual void To_NextAnimation()override;
    virtual void			Set_AnimSpeedMagnifier(_uint iAnimIndex, _float _fMag) override;
protected:
    CVIBuffer_Rect* m_pVIBufferCom = nullptr;

    map<string, CTexture*> m_Textures;
    vector<CAnimation2D* >m_Animation2Ds ;

	vector<CTexture*> m_NonAnimTextures ;
    _float2 m_vNonAnimSpriteStartUV = { 0,0 };
    _float2 m_vNonAnimSpriteEndUV = { 1,1 };

	_uint m_iCurAnimIdx = 0;
public:
    static C2DModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath);
    virtual CComponent* Clone(void* _pArg) override;
    virtual void Free() override;


};

END