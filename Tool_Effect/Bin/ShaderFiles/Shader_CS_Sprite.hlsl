#include "../../../EngineSDK/hlsl/Engine_Shader_Define.hlsli"

struct Sprite_Particle
{
    row_major float4x4 InstancingMatrix : WORLD;
    float2 vLifeTime : TEXCOORD0;
    float4 vColor : TEXCOORD1;
    float4 vTexcoord : TEXCOORD2;
    float3 vVelocity : TEXCOORD3;
    float3 vAcceleration : TEXCOORD4;
    float fAlive : TEXCOORD5;
};

RWStructuredBuffer<Sprite_Particle> Particles : register(u0);
StructuredBuffer<Sprite_Particle> InitialParticle : register(t0);

float3 g_vOrigin;
float3 g_vAxis;
float g_fAmount;
float g_Pull;
float3 g_vAmount;
float g_fTimeDelta;
float4x4 g_SpawnMatrix;
float g_fAbsolute;
float g_fKill;


[numthreads(256, 1, 1)]
void CS_Update(int3 dispatchThreadID : SV_DispatchThreadID)
{
    Particles[dispatchThreadID.x].vLifeTime.y += g_fTimeDelta;
    
    // kill or revive
    float fLifeOn = step(0.f, Particles[dispatchThreadID.x].vLifeTime.y);
    // y가 크면 -> 0의 크기
    float fDead = step(Particles[dispatchThreadID.x].vLifeTime.x, Particles[dispatchThreadID.x].vLifeTime.y) * fLifeOn;    
     
    
    //  revive
    Particles[dispatchThreadID.x].vLifeTime.y = Particles[dispatchThreadID.x].vLifeTime.y * (1.f - fDead)
    + min(InitialParticle[dispatchThreadID.x].vLifeTime.y, g_fKill) * fDead;
    Particles[dispatchThreadID.x].fAlive = Particles[dispatchThreadID.x].fAlive * (1.f - fDead)
    + InitialParticle[dispatchThreadID.x].fAlive * fDead; 
        
    
    // 속도 적용
    Particles[dispatchThreadID.x].vVelocity += Particles[dispatchThreadID.x].vAcceleration * g_fTimeDelta;
   
    float4 vPosition = Particles[dispatchThreadID.x].InstancingMatrix[3] + float4(Particles[dispatchThreadID.x].vVelocity, 0.f) * g_fTimeDelta;
    Particles[dispatchThreadID.x].InstancingMatrix[3] = vPosition;

}

[numthreads(256, 1, 1)]
void CS_Spawn(int3 dispatchThreadID : SV_DispatchThreadID)
{ 
    // LifeTime 더하기
    
    //// y가 0보다 크게된 시기가 되면 Spawn.
    float fSpawnOn = step(0.f, Particles[dispatchThreadID.x].vLifeTime.y) * (1 - Particles[dispatchThreadID.x].fAlive);
    
    
    Particles[dispatchThreadID.x].InstancingMatrix = Particles[dispatchThreadID.x].InstancingMatrix * (1.f - fSpawnOn)
    + (InitialParticle[dispatchThreadID.x].InstancingMatrix * (1.f - g_fAbsolute) + g_SpawnMatrix * g_fAbsolute) * fSpawnOn;
    Particles[dispatchThreadID.x].fAlive = max(Particles[dispatchThreadID.x].fAlive, fSpawnOn);
    Particles[dispatchThreadID.x].vColor = Particles[dispatchThreadID.x].vColor * (1.f - fSpawnOn)
    + InitialParticle[dispatchThreadID.x].vColor * fSpawnOn;
    Particles[dispatchThreadID.x].vTexcoord = Particles[dispatchThreadID.x].vTexcoord * (1.f - fSpawnOn)
    + InitialParticle[dispatchThreadID.x].vTexcoord * fSpawnOn;
    Particles[dispatchThreadID.x].vVelocity = Particles[dispatchThreadID.x].vVelocity * (1.f - fSpawnOn)
    + InitialParticle[dispatchThreadID.x].vVelocity * fSpawnOn;
    Particles[dispatchThreadID.x].vAcceleration = Particles[dispatchThreadID.x].vAcceleration * (1.f - fSpawnOn)
    + InitialParticle[dispatchThreadID.x].vAcceleration * fSpawnOn;
    
}

[numthreads(256, 1, 1)]
void CS_Burst(int3 dispatchThreadID : SV_DispatchThreadID)
{
    
    Particles[dispatchThreadID.x].InstancingMatrix = InitialParticle[dispatchThreadID.x].InstancingMatrix * (1.f - g_fAbsolute)
    + g_SpawnMatrix * g_fAbsolute;
    Particles[dispatchThreadID.x].fAlive = 1.f;
    Particles[dispatchThreadID.x].vLifeTime.y = 0.f;
    Particles[dispatchThreadID.x].vColor = InitialParticle[dispatchThreadID.x].vColor;
    Particles[dispatchThreadID.x].vTexcoord = InitialParticle[dispatchThreadID.x].vTexcoord;
    Particles[dispatchThreadID.x].vVelocity = InitialParticle[dispatchThreadID.x].vVelocity;
    Particles[dispatchThreadID.x].vAcceleration = InitialParticle[dispatchThreadID.x].vAcceleration;
}


[numthreads(256, 1, 1)]
void CS_POINTVELOCITY(int3 dispatchThreadID : SV_DispatchThreadID)
{
    Particles[dispatchThreadID.x].vVelocity = Particles[dispatchThreadID.x].InstancingMatrix[3].xyz - g_vOrigin * g_fAmount;
}

[numthreads(256, 1, 1)]
void CS_LINEARVELOCITY(int3 dispatchThreadID : SV_DispatchThreadID)
{
    Particles[dispatchThreadID.x].vVelocity = g_vAmount;
}

[numthreads(256, 1, 1)]
void CS_INIT_ACCELERATION(int3 dispatchThreadID : SV_DispatchThreadID)
{
    Particles[dispatchThreadID.x].vAcceleration = Particles[dispatchThreadID.x].vVelocity;
}

[numthreads(256, 1, 1)]
void CS_GRAVITY(int3 dispatchThreadID : SV_DispatchThreadID)
{
    Particles[dispatchThreadID.x].vAcceleration += g_vAmount * g_fTimeDelta;   
}

[numthreads(256, 1, 1)]
void CS_DRAG(int3 dispatchThreadID : SV_DispatchThreadID)
{
    Particles[dispatchThreadID.x].vVelocity = (1.f - g_fAmount * g_fTimeDelta) * Particles[dispatchThreadID.x].vVelocity;
}

[numthreads(256, 1, 1)]
void CS_VORTEX_ACCELERATION(int3 dispatchThreadID : SV_DispatchThreadID)
{
    float3 vDiff = Particles[dispatchThreadID.x].InstancingMatrix[3].xyz - g_vOrigin;
    float3 vR = vDiff - dot(normalize(g_vAxis), vDiff) * normalize(g_vAxis);
    
    float3 vVortex = normalize(cross(normalize(g_vAxis), vR)) * g_fAmount;
    float vPull = vR * g_Pull * -1.f;
    
    Particles[dispatchThreadID.x].vAcceleration += (vVortex + vPull) * g_fTimeDelta;

}

[numthreads(256, 1, 1)]
void CS_POINT_ACCELERATION(int3 dispatchThreadID : SV_DispatchThreadID)
{
    Particles[dispatchThreadID.x].vAcceleration += length((Particles[dispatchThreadID.x].InstancingMatrix[3].xyz - g_vOrigin)) * g_vAmount * g_fTimeDelta;
}

[numthreads(256, 1, 1)]
void CS_LIMIT_ACCELERATION(int3 dispatchThreadID : SV_DispatchThreadID)
{
    Particles[dispatchThreadID.x].vAcceleration = normalize(Particles[dispatchThreadID.x].vAcceleration) * max(g_fAmount, length(Particles[dispatchThreadID.x].vAcceleration));

}





technique11 DefaultTechnique
{
    pass Update // 0
    {
        VertexShader = NULL;
        GeometryShader = NULL;
        PixelShader = NULL;
        ComputeShader = compile cs_5_0 CS_Update();
    }

    pass Spawn // 1
    {
        VertexShader = NULL;
        GeometryShader = NULL;
        PixelShader = NULL;
        ComputeShader = compile cs_5_0 CS_Spawn();
    }

    pass Burst // 2
    {
        VertexShader = NULL;
        GeometryShader = NULL;
        PixelShader = NULL;
        ComputeShader = compile cs_5_0 CS_Burst();
    }


    pass PointVelocity // 3
    {
        VertexShader = NULL;
        GeometryShader = NULL;
        PixelShader = NULL;
        ComputeShader = compile cs_5_0 CS_POINTVELOCITY();
    }

    pass LinearVelocity // 4
    {
        VertexShader = NULL;
        GeometryShader = NULL;
        PixelShader = NULL;
        ComputeShader = compile cs_5_0 CS_LINEARVELOCITY();
    }
    pass InitAccel // 5
    {
        VertexShader = NULL;
        GeometryShader = NULL;
        PixelShader = NULL;
        ComputeShader = compile cs_5_0 CS_INIT_ACCELERATION();
    }
    pass Gravity // 6
    {
        VertexShader = NULL;
        GeometryShader = NULL;
        PixelShader = NULL;
        ComputeShader = compile cs_5_0 CS_GRAVITY();

    }
    pass Drag // 7
    {
        VertexShader = NULL;
        GeometryShader = NULL;
        PixelShader = NULL;
        ComputeShader = compile cs_5_0 CS_DRAG();
    }
    pass Vortex // 8
    {
        VertexShader = NULL;
        GeometryShader = NULL;
        PixelShader = NULL;
        ComputeShader = compile cs_5_0 CS_VORTEX_ACCELERATION();
    }

    pass PointAccel // 9
    {
        VertexShader = NULL;
        GeometryShader = NULL;
        PixelShader = NULL;
        ComputeShader = compile cs_5_0 CS_POINT_ACCELERATION();
    }

    pass LimitAccel // 10
    {
        VertexShader = NULL;
        GeometryShader = NULL;
        PixelShader = NULL;
        ComputeShader = compile cs_5_0 CS_LIMIT_ACCELERATION();
    }


}