#include "../../../EngineSDK/hlsl/Engine_Shader_Define.hlsli"

Texture2D g_InputTexture;
RWTexture2D<float4> g_OutputTexture;

[numthreads(256, 1, 1)]
void main(int3 dispatchThreadID : SV_DispatchThreadID)
{
    
}

//technique11 DefaultTechnique
//{
//    pass DefaultPass // 0
//    {
//        VertexShader = NULL;
//        GeometryShader = NULL;
//        PixelShader = NULL;
//        ComputeShader = compile cs_5_0 CSTest();

//    }
//}