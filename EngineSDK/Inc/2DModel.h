#pragma once
#include "Component.h"

BEGIN(Engine)
class CVIBuffer_Rect;
class C2DModel :
    public CComponent
{
public:
    HRESULT			Initialize(void* _pDesc);

    HRESULT			Render(_uint iMeshIndex = 0);
public:



private:
    CVIBuffer_Rect* m_pVIBufferCom = nullptr;
};

END