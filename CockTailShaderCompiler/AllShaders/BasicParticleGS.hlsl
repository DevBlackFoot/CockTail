// ��ƼŬ Geometry Shader �Դϴ�.
#include "ParticleCommon.hlsli"
#include "ParticleHelper.hlsli"

// Random ���..
Texture2D RandomTex : register(t0);

Texture2D NoiseTex : register(t1);

StructuredBuffer<ParticleStruct> particleBuffer : register(t2);

SamplerState Sampler : register(s0);

// Time ���õ� ��� ����
cbuffer cbTime : register(b0)
{
    float TimeStep;
    float GamePlayTime;
    float2 pad0;
};

// b1�� PerObjectParticle�� �� �ֽ��ϴ�.

// ViewProj�� �ʿ��� ��� ����
	// �׸��� �ʿ� ����� Light View Proj ������۸� ������ָ� �� ��.
cbuffer cbViewProj : register(b2)
{
    float4x4 ViewProj;
}

// ī�޶� ��ġ�� ������ �ִ� ��� ����
cbuffer cbEyePos : register(b3)
{
    float4 CameraPosW;
}

struct GeoOut
{
    float4 PosH : SV_POSITION;
    float4 Color : COLOR;
    float2 Tex : TEXCOORD;
};

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

// �ϳ��� ���� 4���� Ȯ���ؼ� �ؽ�ó�� ������ ����Ѵ�..!
[maxvertexcount(4)]
    void DrawGS
    (point VertexIDOut gin[1],
	inout TriangleStream<GeoOut> triStream)
{
    // CS���� ����� ��� �Ϸ�Ǿ� ���� ���ۿ��� ��ƼŬ Struct�� ���ɴϴ�.
    ParticleStruct p = particleBuffer[gin[0].index];
    
	// ������ �Ѿ��
    if (p.Type != PT_EMITTER)
    {
        float3 worldPos = p.PosW.xyz;

        float3 look;
        float3 right = float3(1, 0, 0);
        float3 up = float3(0, 1, 0);

        SetBillBoard(CameraPosW.xyz, p, look, right, up);

        /// ---------------------------- �簢������ ��ƼŬ Ȯ��. ---------------------------
        float halfWidth = 0.5f * p.SizeWStartSize.x;
        float halfHeight = 0.5f * p.SizeWStartSize.y;

        float4 v[4];
        v[0] = float4(worldPos + halfWidth * right - halfHeight * up, 1.0f);
        v[1] = float4(worldPos + halfWidth * right + halfHeight * up, 1.0f);
        v[2] = float4(worldPos - halfWidth * right - halfHeight * up, 1.0f);
        v[3] = float4(worldPos - halfWidth * right + halfHeight * up, 1.0f);

        GeoOut gout;
        
        gout.Color = p.Color;

        gout.PosH = mul(v[0], ViewProj);
        gout.Tex = p.QuadTexC[0];
        triStream.Append(gout);

        gout.PosH = mul(v[1], ViewProj);
        gout.Tex = p.QuadTexC[1];
        triStream.Append(gout);

        gout.PosH = mul(v[2], ViewProj);
        gout.Tex = p.QuadTexC[2];
        triStream.Append(gout);

        gout.PosH = mul(v[3], ViewProj);
        gout.Tex = p.QuadTexC[3];
        triStream.Append(gout);
    }
    
}

// Trail�� ��Ÿ���� ���� Main, MaxvertexCount�� 1024����Ʈ�� ���� �� ����.
// ���� ���ؽ� 1���� 8����Ʈ 100�� �ؼ� 800 ����Ʈ. ���� �ƽ���� �� �� ���� ��.
[maxvertexcount(60)]
    void DrawTrailGS
    (point VertexIDOut gin[1],
	inout TriangleStream<GeoOut> triStream)
{
    
    ParticleStruct p = particleBuffer[gin[0].index];
    
    if (p.Type == PT_TRAIL)
    {
        float t = p.AgeLifeTimeRotationGravity.x;

        float ratio = t / p.AgeLifeTimeRotationGravity.y;
        
        float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
        
        SetColorOverLifeTime(ratio, trails.AlphaRatioLifetime, trails.ColorRatioLifetime, color);
        
        if (trails.TrailsFlag & Use_TrailFlag_InheritParticleColor)
        {
            if (ParticleFlag & Use_ColorOverLifetime)
                color *= p.Color;
        }
        
        float halfWidth = 0.5f * p.TrailWidth;
        
        if (trails.TrailsFlag & Use_TrailFlag_SizeAffectsWidth)
            halfWidth = halfWidth * p.SizeWStartSize.x;
        
        float texCoordStep = 1.0f / (float) (trails.TrailVertexCount - 1);

        GeoOut gout;

        float yOffset = 0;

        float3 worldPos = p.PosW;

        float3 prevPosW = p.PrevPos[0].xyz;

        float3 direction = prevPosW - worldPos;

        float offsetLen = length(direction);

		[unroll(30)]
        for (int i = 1; i <= trails.TrailVertexCount; ++i)
        {
            float trailRatio = (float) i / (float) trails.TrailVertexCount; // ���̿� ���� ���� ��ȭ�� �ֱ� ���� ����.
            float4 trailColor; // ���̿� ���� ����.
            SetColorOverLifeTime(trailRatio, trails.AlphaRatioTrail, trails.ColorRatioTrail, trailColor);
            gout.Color = color * trailColor;
            
            float y = 0;
            
            if (trails.TrailsFlag & Use_TrailFlag_TextureMode_Stretch)
                y = texCoordStep * (i - 1);
            else if (trails.TrailsFlag & Use_TrailFlag_TextureMode_Tile)
                y = yOffset;
            else if (trails.TrailsFlag & Use_TrailFlag_TextureMode_DistributePerSegment)
                y = texCoordStep * (i - 1);
            else if (trails.TrailsFlag & Use_TrailFlag_TextureMode_RepeatPerSegment)
                y = texCoordStep * (i - 1);

            y += p.TrailScrollSpeed.y * commonInfo.ParticlePlayTime;
            
            if (i != trails.TrailVertexCount)
            {
                float3 offsetDir = p.PrevPos[i - 1] - p.PrevPos[i];

                prevPosW = p.PrevPos[i].xyz + normalize(offsetDir) * offsetLen;
            
                direction = prevPosW - worldPos;
            }

            float len = length(direction);

            float3 look = normalize(CameraPosW - worldPos);
            
            float3 up = normalize(direction) * (particleRenderer.LengthScale + length(direction) * particleRenderer.SpeedScale);
           
            float3 right = normalize(cross(up, look));
            
            float4 v[2];
            
            v[0] = float4(worldPos - halfWidth * right - look * 0.1f, 1.0f);
            
            v[1] = float4(worldPos + halfWidth * right - look * 0.1f, 1.0f);
            
            gout.PosH = mul(v[0], ViewProj);
            
            gout.Tex = float2(1 + p.TrailScrollSpeed.x * commonInfo.ParticlePlayTime, y);
            
            triStream.Append(gout);
            
            gout.PosH = mul(v[1], ViewProj);
            
            gout.Tex = float2(0 + p.TrailScrollSpeed.x * commonInfo.ParticlePlayTime, y);
            
            triStream.Append(gout);
            
            yOffset += len;

            worldPos = prevPosW;

        }
    }
}