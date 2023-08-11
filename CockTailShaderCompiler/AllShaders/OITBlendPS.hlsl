// OIT를 사용한 Pixel Shader 입니다.
	// OIT를 통해 만들어진 파티클들을 Deferred로 빛 연산이 끝난 텍스쳐에 블랜딩 해주는 셰이더 입니다.
#include "ParticleHelper.hlsli"
#include "ParticleCommon.hlsli"
#include "PBRModelTypes.hlsli"
#include "ColorHelper.hlsli"

// 파티클 렌더 전, 최종적으로 그려진 텍스쳐를 하나 받아옵니다.
Texture2D BackGround : register(t0);

// 각 화면의 픽셀에 파티클의 정보가 담겨있는 버퍼를 가져 옵니다.
StructuredBuffer<PixelNode> PixelLinkBuffer : register(t1);
ByteAddressBuffer FirstOffsetBuffer : register(t2);

static PixelData SortedPixels[MAX_SORTED_PIXELS];

cbuffer ScreenInfo : register(b0)
{
    float2 ScreenXY;
    float2 pad0;
}

SamplerState Sampler : register(s0);

// 기본적으로 들어오는 픽셀은 quad vertex의 정보입니다.
struct BlendPixelIn
{
    float4 PosH : SV_POSITION;
    float2 Tex : TEXCOORD;
};

void SortPixelInPlace(int numPixels)
{
    PixelData temp;
    for (int i = 1; i < numPixels; ++i)
    {
        for (int j = i - 1; j >= 0; --j)
        {
            if (SortedPixels[j].Depth < SortedPixels[j + 1].Depth)
            {
                temp = SortedPixels[j];
                SortedPixels[j] = SortedPixels[j + 1];
                SortedPixels[j + 1] = temp;
            }
            else
            {
                break;
            }
        }
    }
}

float4 OITMain(BlendPixelIn pin) : SV_Target
{
    uint2 vPos = (uint2) pin.PosH.xy;
    int startOffsetAddress = 4 * (ScreenXY.x * vPos.y + vPos.x);
    int numPixels = 0;
    uint offset = FirstOffsetBuffer.Load(startOffsetAddress);
    
    PixelNode element;
    
    while (offset != 0xFFFFFFFF)
    {
        element = PixelLinkBuffer[offset];
        SortedPixels[numPixels++] = element.Data;
        offset = (numPixels >= MAX_SORTED_PIXELS) ?
            0xFFFFFFFF : element.Next;
    }
    
    SortPixelInPlace(numPixels);
    
    float4 currColor = BackGround.Sample(Sampler, pin.Tex);
    
    PixelData data;
    
	[unroll]
    for (int i = 0; i < numPixels; ++i)
    {
        data = SortedPixels[i];
        float4 pixelColor = UnpackColorFromUint(data.Color);
        
        if (data.BlendType == 0)
            currColor.xyz = lerp(currColor.xyz, pixelColor.xyz, pixelColor.w);
        else if (data.BlendType == 1)
            currColor.xyz = lerp(currColor.xyz, pixelColor.xyz + currColor.xyz, pixelColor.w);
        else
            currColor.xyz = lerp(currColor.xyz, pixelColor.xyz, pixelColor.w);
    }
    
    return currColor;
}