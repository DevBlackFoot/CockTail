// OIT�� ����� Pixel Shader �Դϴ�.
#include "ParticleHelper.hlsli"
#include "ParticleCommon.hlsli"
#include "PBRModelTypes.hlsli"
#include "ColorHelper.hlsli"

#define EPSILON 0.000001f

Texture2DArray TexArray : register(t0);

Texture2D DepthBuffer : register(t1); // ��ƼŬ�� �׸� �� �տ� ������Ʈ�� ������ �׸��� �ʱ� ���ؼ� �������۸� �����Ѵ�.
// ������ ���� ��, �������� ������Ʈ�� �׸� �������̴�.
Texture2D PreDepthBuffer : register(t2); // ��ƼŬ�� �׸� �� �տ� ������Ʈ�� ������ �׸��� �ʱ� ���ؼ� �������۸� �����Ѵ�.
// OIT Layer�� �׸� �� ���� ���� ���̴�.

RWStructuredBuffer<PixelNode> PixelLinkBuffer : register(u0); // ���� ����Ʈ�� ����� ������ �ϴ� ����. 1920 * 1080 * ������ �ȼ� �� �� ������� �Ҵ��صд�.
RWByteAddressBuffer FirstOffsetBuffer : register(u1); // �ε��� ����. ���������� ��ϵ� �ȼ��� �ε��� ���� �����ϰ� �ִ�. ����Ʈ �������� ����� �ȼ��� �����Ѵ�.

cbuffer ScreenInfo : register(b0)
{
    float2 ScreenXY;
    float2 pad0;
}

SamplerState Sampler : register(s0);

struct PSOut
{
    float4 Color : SV_Target;
    float Depth : SV_Depth;
};
struct GeoOut
{
    float4 PosH : SV_POSITION;
    float4 Color : COLOR;
    float2 Tex : TEXCOORD;
};
//#define Draw_Depth 1

float4 DrawPS(GeoOut pin) : SV_Target
{
    const float2 posTexCoord = pin.PosH.xy / ScreenXY;

    // ���⼭ ���Ǵ� DepthBuffer�� DMRAO �����Դϴ�. x ���� depth ���Դϴ�.
    float4 depth = DepthBuffer.Sample(Sampler, posTexCoord);

    float4 Color = TexArray.Sample(Sampler, float3(pin.Tex, 0)) * pin.Color;
    
    if ((depth.x >= 0.001f))
    {
        clip(depth.x - pin.PosH.z - EPSILON);
    }
    
    clip(Color.a - EPSILON);
    
#ifdef Draw_Object_ID
    return float4(commonInfo.ObjectID, 0, 0, 1.0f);
#elif Draw_Depth
    return float4(pin.PosH.z, pin.PosH.z, pin.PosH.z, pin.PosH.z);
#else
    return Color;
#endif
}

float4 DrawOutLine(GeoOut pin) : SV_Target
{
    return float4(1.0f, 1.0f, 0.0f, 1.0f);
}

PSOut DrawDepthPeelingPS(GeoOut pin) // 1�� ������ �����ߴ� �����ʸ� ����� OIT ����ȭ�� ���� ������..!
{
    const float2 posTexCoord = pin.PosH.xy / ScreenXY;

    float curDepth = pin.PosH.z;

    float4 depth = DepthBuffer.Sample(Sampler, posTexCoord);

    if ((depth.x >= EPSILON))
    {
        clip(depth.x - curDepth - EPSILON);
    }
    float preDepth = PreDepthBuffer.Sample(Sampler, posTexCoord).x;

    clip(curDepth - preDepth - EPSILON); // ���� ���̾��� ������ ���ϱ� ����. ���� �ڿ� ���� ��쿡 �׸��� �տ� �ִٸ� �� �̻� �׸��� �ʾƵ� �ȴ�.

    PSOut output;

    output.Color = TexArray.Sample(Sampler, float3(pin.Tex, 0)) * pin.Color;

    clip(output.Color.a - EPSILON);
    
    output.Depth = curDepth;

    return output;
}

void OIT_Particle_PS(GeoOut pin) // �ȼ��� �����ϴ� pixel shader
{
    float4 texColor = SRGBToLinear(TexArray.Sample(Sampler, float3(pin.Tex, 0))) * SRGBToLinear(pin.Color);
    //float4 texColor = TexArray.Sample(Sampler, float3(pin.Tex, 0)) * pin.Color;


    const float2 posTexCoord = pin.PosH.xy / ScreenXY;

    float4 depth = DepthBuffer.Sample(Sampler, posTexCoord);
    
    clip(texColor.a - 0.0001f);

    if ((depth.x >= 0.001f))
    {
        clip(depth.x - pin.PosH.z - EPSILON);
    }
    
    uint pixelCount = PixelLinkBuffer.IncrementCounter(); // ī��Ʈ�� ����Ѵ�.
    
    uint2 vPos = (uint2) pin.PosH.xy;
    
    uint startOffsetAddress = 4 * (ScreenXY.x * vPos.y + vPos.x);
    uint oldStartOffset;
    
    // FirstOffsetBuffer�� ȭ�鿡 ���������� ��ϵ� �ȼ��� �ε��� ���� �����ϴ� �����̴�..
    FirstOffsetBuffer.InterlockedExchange(
        startOffsetAddress, pixelCount, oldStartOffset);
    
    float strength = length(texColor);
    float4 color = texColor;
    
    PixelNode node;
    node.Data.Color = PackColorFromFloat4(color);
    node.Data.Depth = pin.PosH.z;
    node.Data.BlendType = particleRenderer.BlendType;
    node.Next = oldStartOffset;
    
    PixelLinkBuffer[pixelCount] = node;
}