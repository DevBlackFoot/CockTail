// ���� �⺻���� ��ƼŬ ComputeShader �Դϴ�.
	// GPU�� ������ ����غ��ô�..
#include "ParticleCommon.hlsli"
#include "ParticleHelper.hlsli"

// Random ���..
Texture2D RandomTex : register(t0);

Texture2D NoiseTex : register(t1);

// Slot�� ��� ���� ��İ� ���� �� ���̴��� ������� �־ �����Դϴ�.
cbuffer cbDynamicBuffer : register(b0)
{
    float emissionTime;
    int dim;
    float cbPad1;
    float cbPad2;
};

// b1�� PerObjectParticle�� �� �ֽ��ϴ�.
	// �ٽ��� �Ǵ� Constant Buffer ��ƼŬ�� ��� ������ ���ֽ��ϴ�.

// Time ���õ� ��� ����
cbuffer cbTime : register(b2)
{
    float TimeStep;
    float GamePlayTime;
    float2 cbPad0;
};

struct Counter
{
    int particleCounter;
    int emitterCounter;
    int allGroupSync;
    int pad;
};

RWStructuredBuffer<ParticleStruct> ResultParticleBuffer : register(u0);     // ��� ��ƼŬ�� �����ϴ� ����
RWStructuredBuffer<Counter> CounterBuffer : register(u1);           // �ٸ� ���(ī����)�� �����ϴ� ����.

// ���漺 ���÷��� ���ٰ� ����..
SamplerState Sampler : register(s0);

void ExchangeParticle(int destIndex, int srcIndex)
{

}

float4 RandUnitVec4(float offset)
{
    float u = (commonInfo.ParticlePlayTime + offset);

	// hlsl �� �����Լ��� ����Ǿ� ���� �ʾ� ���� �ؽ��ĸ� ���� ���� ����.
    float4 v = RandomTex.SampleLevel(Sampler, u, 0);

    return normalize(v);
}

float4 RandVec4(float offset)
{
    float u = (commonInfo.ParticlePlayTime + offset);

	// hlsl �� �����Լ��� ����Ǿ� ���� �ʾ� ���� �ؽ��ĸ� ���� ���� ����.
    float4 v = RandomTex.SampleLevel(Sampler, u, 0);

    return v;
}

// �� ID�� ��ƼŬ�� ������ ��� �Ѱ��ݴϴ�. => VS�� ���� GS�� �Ѱ��ݴϴ�
[numthreads(1024, 1, 1)]
void CS_Main(uint3 groupID : SV_GroupID, uint3 groupTreadID : SV_GroupThreadID, uint groupIndex : SV_GroupIndex)
{
    int ID = groupID.x * 1024 + groupID.y * dim * 1024 + groupID.z * dim * dim * 1024 + groupTreadID.x;
   
    int ID_Temp = ID;
   
    if (ID >= commonInfo.MaxParticles)
        return;
    
    ParticleStruct p = ResultParticleBuffer[ID];
    
    if (p.Type == PT_EMITTER)
    {
        if (commonInfo.isLooping || commonInfo.ParticlePlayTime <= commonInfo.Duration)
        {
            if (emissionTime >= emission.EmissiveTime)
            {
                if (emission.IsRateOverDistance == 1)
                {
                    if (length(commonInfo.DeltaMatrix[3].xyz) < 0.0001f)
                        return;
                }
                //���� �ð�����//����
                int count = CounterBuffer[0].emitterCounter; //CounterBuffer[0].count;
                
                if (count > emission.EmissiveCount)
                    return;
            
                InterlockedAdd(CounterBuffer[0].emitterCounter, 1, count);
            
                if (count < emission.EmissiveCount)
                {
                
                    float4 vRandom1 = RandUnitVec4(ID * 0.003f);
                
                    const float4 vRandom2 = RandUnitVec4(vRandom1.x);
                    const float4 vRandom3 = RandUnitVec4(vRandom1.y);
                    const float4 vRandom4 = RandUnitVec4(vRandom1.z);
                    const float4 vRandom5 = RandUnitVec4(vRandom1.w);

                    // ���� �������� 0~1�� ���� �ʿ��ϴ�.
                    float4 vunsignedRandom2 = (RandVec4(vRandom1.x) + float4(1.0f, 1.0f, 1.0f, 1.0f)) / 2.0f;
                    float4 vunsignedRandom3 = (RandVec4(vRandom1.y) + float4(1.0f, 1.0f, 1.0f, 1.0f)) / 2.0f;
                    float4 vunsignedRandom4 = (RandVec4(vRandom1.z) + float4(1.0f, 1.0f, 1.0f, 1.0f)) / 2.0f;

                    float4 vUnUnitRandom2 = RandVec4(vunsignedRandom2.x);
                    float4 vUnUnitRandom3 = RandVec4(vunsignedRandom2.y);
                    float4 vUnUnitRandom4 = RandVec4(vunsignedRandom2.z);
                
                    p.Type = PT_FLARE;
                
                    p.PosW = float3(0, 0, 0);
                    p.VelW = float4(0, 0, 0, 1.0f);

                    p.SizeWStartSize = float4(1.0f, 1.0f, 1.0f, 1.0f);
                    p.AgeLifeTimeRotationGravity = float4(0, 10.0f, 0, 0);
                    
                    p.StartColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
                    p.Color = p.StartColor;

                    p.QuadTexC[0] = float2(0.0f, 1.0f); // lefttop
                    p.QuadTexC[1] = float2(0.0f, 0.0f); //leftbottom
                    p.QuadTexC[2] = float2(1.0f, 1.0f); // righttop
                    p.QuadTexC[3] = float2(1.0f, 0.0f); // ritghtbottom
    

                    p.InitEmitterPos = commonInfo.TransformMatrix[3];
                    
                    [unroll]
                    for (int i = 0; i < TrailCount; i++)
                    {
                        p.PrevPos[i] = float4(p.PosW, 1.0f);
                    }
                    
                    p.LatestPrevPos = float4(p.PosW - p.VelW.xyz, 1.0f);
                
                    ManualShape(vRandom1, vRandom2, vRandom5, vunsignedRandom2, vUnUnitRandom2, p.PosW.xyz, p.VelW.xyz);

                    p.SizeWStartSize.zw = lerp(commonInfo.StartSize.xy, commonInfo.StartSize.zw, vunsignedRandom2.y);

                    ManualSizeOverLifeTime(0, p.SizeWStartSize.xy);
                    
                    p.SizeWStartSize.xy *= p.SizeWStartSize.zw;
                    
                    p.AgeLifeTimeRotationGravity.x = 0.0f;
                    
                    p.AgeLifeTimeRotationGravity.y = lerp(commonInfo.StartLifeTime[0], commonInfo.StartLifeTime[1], vunsignedRandom2.z);

                    p.AgeLifeTimeRotationGravity.z = lerp(commonInfo.StartRotation[0], commonInfo.StartRotation[1], vunsignedRandom3.x);

                    p.AgeLifeTimeRotationGravity.w = lerp(commonInfo.GravityModifier[0], commonInfo.GravityModifier[1], vunsignedRandom3.z) /** gCommonInfo.gSimulationSpeed*/;
                    
                    p.StartColor = lerp(commonInfo.StartColor[0], commonInfo.StartColor[1], vunsignedRandom3.y);

                    ManualColorOverLifeTime(0, colorOverLifetime.AlphaRatio, colorOverLifetime.ColorRatio, p.Color);
                
                    p.Color *= p.StartColor;
                    
                    p.Type = PT_FLARE;
                    
                    if (vunsignedRandom4.w <= trails.Ratio)
                        p.Type = PT_TRAIL;

                    p.InitEmitterPos = commonInfo.TransformMatrix[3];
                    
                    [unroll]
                    for (i = 0; i < TrailCount; i++)
                    {
                        p.PrevPos[i] = float4(p.PosW, 1.0f);

                    }
                    
                    p.LatestPrevPos = float4(p.PosW - p.VelW.xyz, 1.0f);
                    
                    ManualTextureSheetAnimation(vunsignedRandom4, p.QuadTexC);
                
                    p.AgeLifeTimeRotationGravity.x = 0;
                    
                    p.TrailWidth = lerp(trails.WidthOverTrail[0], trails.WidthOverTrail[1], vunsignedRandom4.x);
                    
                    p.TrailScrollSpeed.x = lerp(trails.ScrollXSpeed[0], trails.ScrollXSpeed[1], vunsignedRandom4.y);
                    
                    p.TrailScrollSpeed.y = lerp(trails.ScrollYSpeed[0], trails.ScrollYSpeed[1], vunsignedRandom4.z);
                    
                    p.TrailRecordTime = 0;
                    
                }
            }
        }
    }
    else
    {
        float deltaTime = TimeStep * commonInfo.SimulationSpeed;
    
        p.AgeLifeTimeRotationGravity.x += deltaTime;
        
        p.TrailRecordTime += deltaTime;
        
        if (p.AgeLifeTimeRotationGravity.x <= p.AgeLifeTimeRotationGravity.y) // ��ƼŬ�� ��� �ִٸ� ������Ʈ�� ������...!
        {
            
            InterlockedAdd(CounterBuffer[0].particleCounter, 1, ID_Temp);
            // ��ƼŬ�� �����ð�
            p.LatestPrevPos = float4(p.PosW, 1.0f);

            float t = p.AgeLifeTimeRotationGravity.x;

            float ratio = t / p.AgeLifeTimeRotationGravity.y;

            if (!(ParticleFlag & Use_Commoninfo_WorldSpace))
            {
                p.PosW = mul(float4(p.PosW, 1.0f), commonInfo.DeltaMatrix).xyz;

                p.InitEmitterPos = commonInfo.TransformMatrix[3];
            }
            
            ManualTrail(p.PrevPos, p.TrailRecordTime, p.PosW, p.TrailRecordTime, p.PrevPos);
            
            ManualForceOverLifeTime(p.VelW.xyz, deltaTime, p.VelW.xyz);
            
            p.VelW.xyz += float3(0, -p.AgeLifeTimeRotationGravity.w, 0) * deltaTime;
            
            ManualLimitVelocityOverLifeTime(ratio, p.VelW.xyz, p.VelW.xyz);
            
            p.PosW += p.VelW.xyz * deltaTime;
            
            ManualVelocityOverLifeTime(p, deltaTime, p); // Velocity�� ���� �������� �ϴ� ������ Orbital�� �ֱ� ����.
                
            ManualRoationOverLifeTime(p.AgeLifeTimeRotationGravity.z, rotationOverLifetime.AngularVelocity, p.AgeLifeTimeRotationGravity.y, deltaTime, p.AgeLifeTimeRotationGravity.z);
            
            float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
            
            ManualColorOverLifeTime(ratio, colorOverLifetime.AlphaRatio, colorOverLifetime.ColorRatio, color);
            
            float2 size = float2(1, 1);
            
            ManualSizeOverLifeTime(ratio, size);
           
            p.Color = p.StartColor * color;

            p.SizeWStartSize.xy = p.SizeWStartSize.zw * size;
            
            ManualNoise(p.SizeWStartSize.xy, p.PosW.xyz, p.AgeLifeTimeRotationGravity.z,
                NoiseTex, Sampler, deltaTime, GamePlayTime,
            p.SizeWStartSize.xy, p.PosW.xyz, p.AgeLifeTimeRotationGravity.z);

            ManualCollision(p.PosW, p.VelW.xyz, deltaTime, p.AgeLifeTimeRotationGravity.x, p.AgeLifeTimeRotationGravity.y
            , p.PosW, p.VelW.xyz, p.AgeLifeTimeRotationGravity.x);
            
            ManualTextureSheetAnimationForLifetime(p.QuadTexC, ratio, p.QuadTexC);

        }
        else
        {
            p.Type = PT_EMITTER;
            p.PosW = float3(0, 0, 0);
            p.VelW = float4(0, 0, 0, 1.0f);

            p.SizeWStartSize = float4(1.0f, 1.0f, 1.0f, 1.0f);
            p.AgeLifeTimeRotationGravity = float4(0, 10.0f, 0, 0);
                    
            p.StartColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
            p.Color = p.StartColor;

            p.QuadTexC[0] = float2(0.0f, 1.0f); // lefttop
            p.QuadTexC[1] = float2(0.0f, 0.0f); //leftbottom
            p.QuadTexC[2] = float2(1.0f, 1.0f); // righttop
            p.QuadTexC[3] = float2(1.0f, 0.0f); // ritghtbottom
    

            p.InitEmitterPos = commonInfo.TransformMatrix[3];
                    
            [unroll]
            for (int i = 0; i < TrailCount; i++)
            {
                p.PrevPos[i] = float4(p.PosW, 1.0f);
            }
                    
            p.LatestPrevPos = float4(p.PosW - p.VelW.xyz, 1.0f);
        }
    }
    
    ResultParticleBuffer[ID] = p;
}

[numthreads(1024, 1, 1)]
void CS_ResetParticleBuffer(uint3 groupID : SV_GroupID, uint3 groupTreadID : SV_GroupThreadID) // CPU Acess Flag�� ����ϸ� ������ ����� �����. ������ GPU���� ó������.
{
    int ID = groupID.x * 1024 + groupID.y * dim * 1024 + groupID.z * dim * dim * 1024 + groupTreadID.x;

    ResultParticleBuffer[ID].Type = PT_EMITTER;
}

[numthreads(1, 1, 1)]
void CS_ClearCounter(uint3 groupID : SV_GroupID, uint3 groupTreadID : SV_GroupThreadID)
{
    CounterBuffer[0].emitterCounter = 0;
    CounterBuffer[0].particleCounter = 0;
    CounterBuffer[0].allGroupSync = 0;
}