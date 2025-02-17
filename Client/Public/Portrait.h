#pragma once
#include "UI.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END


BEGIN(Client)
class CPortrait : public CUI
{
public:


    enum PORT
    {
        PORT_VIOLET,                // 마법사
        PORT_THRASH,                // 트롤
        PORT_MARTINA,               // 상점
        PORT_PIP,                   // 쥐
        PORT_PAGE,                  // 책벌레
        PORT_HUMGRUMP,              // 험그럼프
        PORT_ROLA,                  // 엄마
        PORT_MOONBEARD,             // 달수염
        PORT_MOONBEARD_MONO,        // 회색 달수염
        PORT_MOONBEARD_RUNETOOTH,   // 이어폰낀 달수염
        PORT_MINIBEARD,             // 조그만 달수염
        PORT_MINIBEARD_MONO,        //조그만 회색 달수염
        PORT_KRONG,                 // 쓰레쉬 할아버지
        PORT_KRONG_GHOST,           // 죽은 쓰레쉬 할아버지
        PORT_BENNYBEE,              // 미니게임에 있는 벌
        PORT_CHROMA,                // 여왕님
        PORT_TUMBA,                 // 애기 홍학(삽파는아이)

        PORT_DEFAULT,
        PORT_END
    };



    explicit CPortrait(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit CPortrait(const CPortrait& Prototype);
    virtual ~CPortrait() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float _fTimeDelta);
    virtual HRESULT Render() override;

private:
    HRESULT     DisplayPortrait();
    void        ChangePosition(_bool _isRender, _float2 _RTSize);


private:
    //DialogData          m_DialogData;   // 현재 다이얼로그 데이터
    //_int                m_iCurrentLineIndex = 0; // 현재 출력 중인 라인의 인덱스 (id)
    //
    //vector<DialogData>  m_DialogDatas;
    //
    //_bool               m_isOpenDialogue = { true };
    //
    //vector<DialogData>         Get_Dialogues() { return m_DialogDatas; }

protected:
    virtual HRESULT Ready_Components() override;

public:
    static CPortrait* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
    HRESULT      Cleanup_DeadReferences() override;

    void        Set_AddSectionRender(_bool _OnOff) { m_isAddSectionRender = _OnOff; }

private:
    PORT        m_ePortrait;
    PORT    m_ePortraitFace;
    _tchar			    m_tDialogIndex[MAX_PATH] = {};
    _bool               m_isAddSectionRender = { false };
    _bool               m_isDialoging = { false };
	_float2             m_vDisplay3DSize = { 0.f, 0.f };
	_float2             m_vDisplay2DSize = { 0.f, 0.f };

};

END
