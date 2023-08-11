// ���� �⺻���� ��ƼŬ VS�Դϴ�.
#include "ParticleCommon.hlsli"
#include "ParticleHelper.hlsli"


//***********************************************
// STREAM-OUT TECH                              *
//***********************************************
	// �ش� �κ��� Compute Shader ������� ������ ������� �ʴ� �κ��Դϴ�.

// b1�� PerObjectParticle�� �� �ֽ��ϴ�.

struct Particle
{
    uint Type : TYPE; // ������ΰ�
    uint VertexID : SV_VertexID;
    
    float3 PosW : POSITION;
    float3 VelW : VELOCITY; // Start speed
    
    float4 SizeWStartSize : SIZE_STARTSIZE; // Start size
    float4 AgeLifeTimeRotationGravity : AGE_LIFETIME_ROTATION_GRAVITY;
    
    float4 StartColor : STARTCOLOR;
    float4 Color : COLOR; // Start Color

    float2 QuadTexC[4] : QUADTEX;
    float3 InitEmitterPos : EMITTERPOS;
    float3 PrevPos[30] : PREVPOS; // Trail�� �׸��� ���� ���� �Ÿ����� ��ġ�� ����Ѵ�. 
    
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

    vout.PrevPos = vin.PrevPos; // Ʈ���� ����Ʈ ������ �Ѱܾ� ��.
    
    return vout;
}


struct vertexID
{
    uint index : SV_VertexID; // ������ΰ�
};

// ���������� ������ִ� VS�� �Ʒ��� VS�Դϴ�.
struct VertexIDIn
{
    uint index : SV_VERTEXID; // ������ΰ�
};
VertexIDOut ComputeShaderDrawVS(VertexIDIn vin)
{
    VertexIDOut vout;
    
    vout.index = vin.index;
    
    return vout;
}