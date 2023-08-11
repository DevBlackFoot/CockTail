// 가장 기본적인 파티클 VS입니다.
#include "ParticleCommon.hlsli"
#include "ParticleHelper.hlsli"


//***********************************************
// STREAM-OUT TECH                              *
//***********************************************
	// 해당 부분은 Compute Shader 사용으로 이제는 사용하지 않는 부분입니다.

// b1은 PerObjectParticle이 들어가 있습니다.

struct Particle
{
    uint Type : TYPE; // 방출기인가
    uint VertexID : SV_VertexID;
    
    float3 PosW : POSITION;
    float3 VelW : VELOCITY; // Start speed
    
    float4 SizeWStartSize : SIZE_STARTSIZE; // Start size
    float4 AgeLifeTimeRotationGravity : AGE_LIFETIME_ROTATION_GRAVITY;
    
    float4 StartColor : STARTCOLOR;
    float4 Color : COLOR; // Start Color

    float2 QuadTexC[4] : QUADTEX;
    float3 InitEmitterPos : EMITTERPOS;
    float3 PrevPos[30] : PREVPOS; // Trail을 그리기 위해 일정 거리마다 위치를 기록한다. 
    
    float3 LatestPrevPos : LASTESTPREVPOS;
};


StreamOutParticle StreamOutVS(Particle vin)
{
    StreamOutParticle vout;
    vout.PosW = vin.PosW;
    vout.VelW = vin.VelW;
    vout.SizeWStartSize = vin.SizeWStartSize;
    vout.AgeLifeTimeRotationGravity = vin.AgeLifeTimeRotationGravity;
    vout.Type = vin.Type;
    vout.VertexID = vin.VertexID;
    vout.StartColor = vin.StartColor;
    vout.Color = vin.Color;
    vout.QuadTexC = vin.QuadTexC;
    vout.InitEmitterPos = vin.InitEmitterPos;
    vout.PrevPos = vin.PrevPos;
    vout.LatestPrevPos = vin.LatestPrevPos;
    
    return vout;
}

ParticleVertexOut DrawVS(Particle vin)
{

    ParticleVertexOut vout;

    vout.PosW = vin.PosW;

    vout.LatestPrevPos = vin.LatestPrevPos;
    
    vout.SizeW = vin.SizeWStartSize.xy;

    vout.Color = vin.Color;

    vout.Type = vin.Type;

    vout.AgeLifeTimeRotation = vin.AgeLifeTimeRotationGravity.xyz;
    
    vout.QuadTexC = vin.QuadTexC;

    vout.PrevPos = vin.PrevPos; // 트레일 이펙트 때문에 넘겨야 함.
    
    return vout;
}


struct vertexID
{
    uint index : SV_VertexID; // 방출기인가
};

// 실질적으로 사용해주는 VS는 아래의 VS입니다.
struct VertexIDIn
{
    uint index : SV_VERTEXID; // 방출기인가
};
VertexIDOut ComputeShaderDrawVS(VertexIDIn vin)
{
    VertexIDOut vout;
    
    vout.index = vin.index;
    
    return vout;
}