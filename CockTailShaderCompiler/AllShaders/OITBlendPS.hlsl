// OIT�� ����� Pixel Shader �Դϴ�.
	// OIT�� ���� ������� ��ƼŬ���� Deferred�� �� ������ ���� �ؽ��Ŀ� ���� ���ִ� ���̴� �Դϴ�.
#include "ParticleHelper.hlsli"
#include "ParticleCommon.hlsli"
#include "PBRModelTypes.hlsli"
#include "ColorHelper.hlsli"

// ��ƼŬ ���� ��, ���������� �׷��� �ؽ��ĸ� �ϳ� �޾ƿɴϴ�.
Texture2D BackGround : register(t0);

// �� ȭ���� �ȼ��� ��ƼŬ�� ������ ����ִ� ���۸� ���� �ɴϴ�.
StructuredBuffer<PixelNode> PixelLinkBuffer : register(t1);
ByteAddressBuffer FirstOffsetBuffer : register(t2);

static PixelData SortedPixels[MAX_SORTED_PIXELS];

cbuffer ScreenInfo : register(b0)
{
    float2 ScreenXY;
    float2 pad0;
}

SamplerState Sampler : register(s0);

// �⺻������ ������ �ȼ��� quad vertex�� �����Դϴ�.
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