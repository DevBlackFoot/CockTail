// OIT를 사용한 Pixel Shader 입니다.
#include "ParticleHelper.hlsli"
#include "ParticleCommon.hlsli"
#include "PBRModelTypes.hlsli"
#include "ColorHelper.hlsli"

#define EPSILON 0.000001f

Texture2DArray TexArray : register(t0);

Texture2D DepthBuffer : register(t1); // 파티클을 그릴 때 앞에 오브젝트가 있으면 그리지 않기 위해서 뎁스버퍼를 참조한다.
// 기존의 뎁스 값, 불투명한 오브젝트를 그린 뎁스값이다.
Texture2D PreDepthBuffer : register(t2); // 파티클을 그릴 때 앞에 오브젝트가 있으면 그리지 않기 위해서 뎁스버퍼를 참조한다.
// OIT Layer를 그릴 때 생긴 뎁스 값이다.

RWStructuredBuffer<PixelNode> PixelLinkBuffer : register(u0); // 정적 리스트의 저장소 역할을 하는 버퍼. 1920 * 1080 * 저장할 픽셀 수 의 사이즈로 할당해둔다.
RWByteAddressBuffer FirstOffsetBuffer : register(u1); // 인덱스 버퍼. 마지막으로 기록된 픽셀의 인덱스 값을 저장하고 있다. 리스트 형식으로 저장된 픽셀을 추적한다.

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

    // 여기서 사용되는 DepthBuffer는 DMRAO 버퍼입니다. x 값이 depth 값입니다.
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

PSOut DrawDepthPeelingPS(GeoOut pin) // 1차 적으로 구현했던 뎁스필링 방식의 OIT 최적화를 위해 버린다..!
{
    const float2 posTexCoord = pin.PosH.xy / ScreenXY;

    float curDepth = pin.PosH.z;

    float4 depth = DepthBuffer.Sample(Sampler, posTexCoord);

    if ((depth.x >= EPSILON))
    {
        clip(depth.x - curDepth - EPSILON);
    }
    float preDepth = PreDepthBuffer.Sample(Sampler, posTexCoord).x;

    clip(curDepth - preDepth - EPSILON); // 이전 레이어의 뎁스와 비교하기 위함. 더욱 뒤에 있을 경우에 그리고 앞에 있다면 더 이상 그리지 않아도 된다.

    PSOut output;

    output.Color = TexArray.Sample(Sampler, float3(pin.Tex, 0)) * pin.Color;

    clip(output.Color.a - EPSILON);
    
    output.Depth = curDepth;

    return output;
}

void OIT_Particle_PS(GeoOut pin) // 픽셀을 저장하는 pixel shader
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
    
    uint pixelCount = PixelLinkBuffer.IncrementCounter(); // 카운트를 기록한다.
    
    uint2 vPos = (uint2) pin.PosH.xy;
    
    uint startOffsetAddress = 4 * (ScreenXY.x * vPos.y + vPos.x);
    uint oldStartOffset;
    
    // FirstOffsetBuffer는 화면에 마지막으로 기록된 픽셀의 인덱스 값을 저장하는 버퍼이다..
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