#pragma once
#include "UI.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END


BEGIN(Client)

class CNarration : public CUI
{
public:

public:
    struct NarrationAnimation
    {
        wstring strSectionid;       // 노출 시킬 섹션의 id
        wstring strAnimationid;     // 애니메이션의 id
        _float2 vPos;               // UI 오브젝트의 포지션
        _float  fWaitingTime;       // 완료 후 다음 애니메이션까지의 대기 시간
        _float2 vAnimationScale;
    };

    struct NarrationDialogData
    {
		_int iduration = 1000;      // 지속 시간
		_int ispeed;                // 다음 폰트 노출 시간
		_wstring strtext;           // 텍스트의 내용
		_float fscale;              // 텍스트의 크기 변경
        _float2 fpos;               // X의 좌표
        _float fwaitingTime;        // 완료 후 다음 애니메이션까지의 대기 시간

        vector<NarrationAnimation> NarAnim;

    };

    struct NarrationData : public tagUIDesc
    {
        wstring strSectionid;       // 노출 시킬 섹션의 id
        wstring strid;              // 다이얼로그 ID
        LEVEL_ID eCurlevelId;
        _int     LineCount;
        vector<NarrationDialogData> lines;
    };


	//struct NarrationAnimDesc : public tagUIDesc
	//{
    //    NarrationData NarData;
	//};
    //


public:
    explicit CNarration(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit CNarration(const CNarration& Prototype);
    virtual ~CNarration() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float _fTimeDelta);
    virtual HRESULT Render() override;

private:
    HRESULT     LoadFromJson(const std::wstring& filePath); // 데이터 로드
    HRESULT      DisplayText(_float2 _vRTSize); // 타이핑 되게하자.
    void        NextDialogue(_float2 _RTSize);

private:
    //DialogData          m_DialogData;   // 현재 다이얼로그 데이터
    _int                m_iCurrentLineIndex = 0; // 현재 출력 중인 라인의 인덱스 (id)
    _tchar			    m_tDialogIndex[MAX_PATH] = {};

    NarrationData            m_NarrationData;
    vector< NarrationData*>  m_NarrationDatas;

protected:
    virtual HRESULT Ready_Components() override;

public:
    static CNarration* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
    HRESULT      Cleanup_DeadReferences() override;

};

END
