#pragma once
#include "Model.h"

BEGIN(Engine)
class CVIBuffer_Rect;
class C2DModel :
    public CModel
{
private:
    C2DModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    C2DModel(const C2DModel& _Prototype);
    virtual ~C2DModel() = default;
public:
    HRESULT			Initialize(void* _pDesc);

    virtual HRESULT			Render(CShader* _Shader, _uint _iShaderPass)override;
    virtual _bool			Play_Animation(_float fTimeDelta)override;

    virtual void Set_AnimationLoop(_uint iIdx, _bool bIsLoop)override;
    virtual void Set_Animation(_uint iIdx)override;
    virtual void Switch_Animation(_uint iIdx)override;


private:
    CVIBuffer_Rect* m_pVIBufferCom = nullptr;
    _uint					m_iSRVIndex = 0;
};

END