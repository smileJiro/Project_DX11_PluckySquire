
#define PI 3.1415926

#define TONE_LINEAR 0
#define TONE_FILMIC 1
#define TONE_UNCHARTED2 2
#define TONE_LUMAREINHARD 3


#define RT_RENDER_DEAFULT 0
#define RT_RENDER_ROTATE  1



struct Material_PS
{
    float4 Albedo; // baseColor
    float Roughness;
    float Metallic;
    float AO;
    float dummy;
};



/* D3D11_SAMPLER_DESC */
sampler			LinearSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = wrap;
	AddressV = wrap;
    AddressW = wrap;
};

sampler LinearSampler_Clamp = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = clamp;
    AddressV = clamp;
    AddressW = clamp;
};

sampler			PointSampler = sampler_state
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = wrap;
	AddressV = wrap;
	AddressW = wrap;
};

sampler MirrorSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Mirror;
    AddressV = Mirror;
};

RasterizerState		RS_Default
{
	FillMode = SOLID;
	CullMode = BACK;
	FrontCounterClockwise = false;
};

RasterizerState RS_Cull_None
{
    FillMode = SOLID;
    CullMode = NONE;
    FrontCounterClockwise = false;
};

RasterizerState		RS_Cull_Front
{
	FillMode = SOLID;
	CullMode = Front;
	FrontCounterClockwise = false;
};

RasterizerState		RS_Wireframe
{
	FillMode = Wireframe;
	CullMode = BACK;
	FrontCounterClockwise = false;
};


DepthStencilState		DSS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;
};

DepthStencilState		DSS_None
{
	DepthEnable = false;
	DepthWriteMask = zero;
};

DepthStencilState DSS_WriteNone
{
    DepthEnable = true;
    DepthWriteMask = zero;
};

BlendState		BS_Default
{
	BlendEnable[0] = false;


};

BlendState		BS_AlphaBlend_OnlyDiffuse
{
	BlendEnable[0] = true;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
	BlendOp = Add;	
};

BlendState BS_AlphaBlend_WithDepth
{
    BlendEnable[0] = true;
    BlendEnable[2] = true;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = Add;
};


BlendState BS_AlphaBlend
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = Add;
};

BlendState		BS_OneBlend
{
	// 0¹ø ·»´õÅ¸°Ù, 1¹ø ·»´õÅ¸°Ù µÑ´Ù ÇØ´ç ºí·»µå¸¦ Àû¿ë½ÃÅ°°Ú´Ù. (shade, specular)
    BlendEnable[0] = true;
    BlendEnable[1] = true;

	// sourColor.rgb * 1 + DestColor.rgb * 1
    SrcBlend = ONE;
    DestBlend = ONE;
    BlendOp = Add;
};

BlendState BS_WeightAccumulate
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;
    BlendEnable[2] = true;

    SrcBlend[0] = ONE;
    DestBlend[0] = ONE;
    BlendOp[0] = Add;
    SrcBlendAlpha[0] = ONE;
    DestBlendAlpha[0] = ONE;
    BlendOpAlpha[0] = Add;

    SrcBlend[1] = ZERO;
    DestBlend[1] = INV_SRC_COLOR;
    BlendOp[1] = Add;

    SrcBlend[2] = ONE;
    DestBlend[2] = ONE;
    BlendOp[2] = Add;
    SrcBlendAlpha[2] = ONE;
    DestBlendAlpha[2] = ONE;
    BlendOpAlpha[2] = Add;
    
};

BlendState BS_InvAlphaBlend
{
    BlendEnable[0] = true;

    SrcBlend = INV_SRC_ALPHA;
    DestBlend = SRC_ALPHA;
    BlendOp = Add;
};





