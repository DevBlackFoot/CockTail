// Deferred ���� ���� ������ Pxiel�鿡 ���� PostProcessing�� �����ϴ� Pixel Shader �Դϴ�.
#include "ColorHelper.hlsli"

// �ȼ� ���̴��� ������ �� => QuadVS�� OutPut��
	// ������ ����ϴ� ���� Texture ���� ����� �ݴϴ�..
struct PostPixelIn
{
    float4 PosH : SV_POSITION;
    float2 Tex : TEXCOORD;
};

// light ������ ��� ���� RenderTarget�� Texture
	// �ش� �ؽ��Ŀ��� ������ ��, ��ó���� ���ݴϴ�.
Texture2D EndRenderTarget : register(t0);

// ��ҹ��۵�
Texture2DArray DownBuffer : register(t1);

// UpSample�� ����� lowMip
Texture2D lowMipTarget : register(t2);
Texture2D blurTexture : register(t3);

SamplerState Sampler : register(s0);

cbuffer VignetteColor : register(b0)
{
    float4 vignetteColor;
}

cbuffer VignetteBuffer : register(b1)
{
    float4 vignetteBuffer;
}

#define BlurDepth 9

cbuffer BlurOption : register(b2)
{
    float4 sampleOffsets[BlurDepth];
    float4 sampleWeights[BlurDepth];
}

// Bloom ���� �����ϴ� threshold
cbuffer BloomThreshold : register(b3)
{
    float4 threshold;
}

// Texture ũ�⸦ �޾ƿ��� buffer
cbuffer TextureInfo : register(b4)
{
    // x�� ���� fade in, out�� offset���� �����ص� �� �� ����.
    float4 texInfo;
}

// fadeOut offset�� ���� ����
cbuffer FadeOffset : register(b5)
{
    float4 fadeOffset;
}

// Unity�� Neutral Tonemapping �Լ��� �����Խ��ϴ�.
	// Hable, Heji, Frostbite�� ������ ����ϴ� �� �����ϴ�.
// Input ���� Linear RGB���� �ϹǷ�, light Pass�� ������ ���� ���ڵ��� �������ݴϴ�.
// Unity�� real�� float�� ������־����ϴ�.
float3 NeutralCurve(float3 x, float a, float b, float c, float d, float e, float f)
{
    return ((x * (a * x + c * b) + d * e) / (x * (a * x + b) + d * f)) - e / f;
}

float3 NeutralTonemap(float3 pixelColor)
{
	// ToneMap ��ġ��
    const float a = 0.2f;
    const float b = 0.29f;
    const float c = 0.24f;
    const float d = 0.272f;
    const float e = 0.02f;
    const float f = 0.3f;
    const float whiteLevel = 5.3f;
    const float whiteClip = 1.0f;

    float3 whiteScale = (1.0f).xxx / NeutralCurve(whiteLevel, a, b, c, d, e, f);
    pixelColor = NeutralCurve(pixelColor * whiteScale, a, b, c, d, e, f);
    pixelColor *= whiteScale;

    pixelColor /= whiteClip.xxx;

    return pixelColor;
}

// Unity�� Vignetting �Լ�.
float3 Vignette(float3 cin, float2 uv, float intensity, float smoothness)
{
	// center 0.5, color black �����Դϴ�..
    float2 center = float2(0.5f, 0.5f);
    // ������..(�ܰ��� ����.)
    float3 color = vignetteColor.xyz;
    // ����Ƽ�� ���ο� ������ �ִ� Magic Number
    intensity *= 3.f;
    smoothness *= 5.f;

    float2 dist = abs(uv - center) * intensity;

    float vfactor = pow(saturate(1.0f - dot(dist, dist)), smoothness);

    return lerp(cin, cin * lerp(color, (1.0f).xxx, vfactor), vignetteColor.w);
}

// RGBM�̶� �����ϰ�..
static const float kRGBMRange = 8.0f;

float4 EncodeRGBM(float3 color)
{
    color *= 1.0f / kRGBMRange;
    float m = max(max(color.x, color.y), max(color.z, 1e-5));
    m = ceil(m * 255) / 255;
    return half4(color / m, m);
}

float3 DecodeRGBM(float4 rgbm)
{
    return rgbm.xyz * rgbm.w * kRGBMRange;
}


// �ȼ� ���̴�
float4 main(PostPixelIn ppin) : SV_Target
{
    // ��ó���� �ޱ� ���� ����
    float3 retColor = EndRenderTarget.Sample(Sampler, ppin.Tex).xyz;
    
#ifdef NotPostProcessing
    retColor = LinearToSRGB(retColor);

    return float4(retColor, 1.0f);
#endif

#ifdef UseBlurBloom
    float3 bloomColor = lowMipTarget.Sample(Sampler, ppin.Tex).xyz;
    float3 blurColor = blurTexture.Sample(Sampler, ppin.Tex).xyz;
    
    // ���ƽ ������ ����غ��ϴ�.
    float2 center = float2(0.5f, 0.5f);
    // 0.6.. ������ 0����
    float intensity = 3.f * vignetteBuffer.z;
    float smoothness = 5.f * vignetteBuffer.w;

    float2 dist = abs(ppin.Tex - center) * intensity;
    float vfactor = pow(saturate(1.0f - dot(dist, dist)), smoothness);

    // vfactor�� �������� �� �ȼ��� �������, retColor�� ������� �����մϴ�.
		// 0�� ����� ���� blur�� �ֽ��ϴ�.
    retColor = lerp(blurColor, retColor, vfactor);

    retColor += lerp(retColor, bloomColor, threshold.y);
#endif

    // ToneMapping�� ���ݴϴ�.
#ifdef UseToneMapping
    retColor = NeutralTonemap(retColor);
    saturate(retColor);
#endif

#ifdef UseVignette
    // ����� �⺻���� �������ݴϴ�. �ʿ��ϸ� cBuffer�� �������ݴϴ�.
    retColor = Vignette(retColor, ppin.Tex, vignetteBuffer.x, vignetteBuffer.y);
#endif

    retColor = lerp(0.f, retColor, fadeOffset.x);

    retColor = LinearToSRGB(retColor);

    return float4(retColor, 1.0f);
}


// ���� ���� Blur main
float4 blurMain(float4 Unused : COLOR0, float2 TexCoord : TEXCOORD0) : SV_Target0
{
    float4 Color = 0;

    // ��
    for (int i = 0; i < BlurDepth; i++)
    {
        // 0�� ���Կ� ���´ٰ� ����.
        Color += EndRenderTarget.Sample(Sampler, TexCoord + sampleOffsets[i].xy) * sampleWeights[i].x;
    }

    return Color;
}

// Unity�� �� ������ �����ͺ��ڽ��ϴ�..
float4 UnityBlurH(float4 Unused : COLOR0, float2 TexCoord : TEXCOORD0) : SV_Target0
{
    float4 Color = 0;

    // texInfo.z = 1 / tex Width
    float texelSize = texInfo.z;

    float4 c0 = EndRenderTarget.Sample(Sampler, TexCoord - float2(texelSize * 4.0f, 0.0f));
    float4 c1 = EndRenderTarget.Sample(Sampler, TexCoord - float2(texelSize * 3.0f, 0.0f));
    float4 c2 = EndRenderTarget.Sample(Sampler, TexCoord - float2(texelSize * 2.0f, 0.0f));
    float4 c3 = EndRenderTarget.Sample(Sampler, TexCoord - float2(texelSize * 1.0f, 0.0f));
    float4 c4 = EndRenderTarget.Sample(Sampler, TexCoord);
    float4 c5 = EndRenderTarget.Sample(Sampler, TexCoord + float2(texelSize * 1.0f, 0.0f));
    float4 c6 = EndRenderTarget.Sample(Sampler, TexCoord + float2(texelSize * 2.0f, 0.0f));
    float4 c7 = EndRenderTarget.Sample(Sampler, TexCoord + float2(texelSize * 3.0f, 0.0f));
    float4 c8 = EndRenderTarget.Sample(Sampler, TexCoord + float2(texelSize * 4.0f, 0.0f));

    Color = c0 * 0.01621622 + c1 * 0.05405405 + c2 * 0.12162162 + c3 * 0.19459459
            + c4 * 0.22702703
            + c5 * 0.19459459 + c6 * 0.12162162 + c7 * 0.05405405 + c8 * 0.01621622;

    return Color;
}

float4 UnityBlurV(float4 Unused : COLOR0, float2 TexCoord : TEXCOORD0) : SV_Target0
{
    float4 Color = 0;

    // texInfo.w = 1 / tex Height
    float texelSize = texInfo.w * 0.5f;

    float4 c0 = EndRenderTarget.Sample(Sampler, TexCoord - float2(0.0f, texelSize * 3.23076923f));
    float4 c1 = EndRenderTarget.Sample(Sampler, TexCoord - float2(0.0f, texelSize * 1.38461538f));
    float4 c2 = EndRenderTarget.Sample(Sampler, TexCoord);
    float4 c3 = EndRenderTarget.Sample(Sampler, TexCoord + float2(0.0f, texelSize * 3.23076923f));
    float4 c4 = EndRenderTarget.Sample(Sampler, TexCoord + float2(0.0f, texelSize * 1.38461538f));

    Color = c0 * 0.07027027 + c1 * 0.31621622
          + c2 * 0.22702703
          + c3 * 0.31621622 + c4 * 0.07027027;
    /*
    */

    /*
    float4 c0 = EndRenderTarget.Sample(Sampler, TexCoord - float2(0.0f, texelSize * 4.0f));
    float4 c1 = EndRenderTarget.Sample(Sampler, TexCoord - float2(0.0f, texelSize * 3.0f));
    float4 c2 = EndRenderTarget.Sample(Sampler, TexCoord - float2(0.0f, texelSize * 2.0f));
    float4 c3 = EndRenderTarget.Sample(Sampler, TexCoord - float2(0.0f, texelSize * 1.0f));
    float4 c4 = EndRenderTarget.Sample(Sampler, TexCoord);
    float4 c5 = EndRenderTarget.Sample(Sampler, TexCoord + float2(0.0f, texelSize * 1.0f));
    float4 c6 = EndRenderTarget.Sample(Sampler, TexCoord + float2(0.0f, texelSize * 2.0f));
    float4 c7 = EndRenderTarget.Sample(Sampler, TexCoord + float2(0.0f, texelSize * 3.0f));
    float4 c8 = EndRenderTarget.Sample(Sampler, TexCoord + float2(0.0f, texelSize * 4.0f));

    Color = c0 * 0.01621622 + c1 * 0.05405405 + c2 * 0.12162162 + c3 * 0.19459459
            + c4 * 0.22702703
            + c5 * 0.19459459 + c6 * 0.12162162 + c7 * 0.05405405 + c8 * 0.01621622;
     */

    return Color;
}

// ��� Ŀ�긦 ���� bloom main
float4 bloomMain(float4 Color : COLOR0, float2 TexCoord : TEXCOORD0) : SV_Target0
{
    float4 retColor = EndRenderTarget.Sample(Sampler, TexCoord);

        // Clamp
    retColor = min(threshold.x, retColor);

    float brightness = max(max(retColor.r, retColor.g), retColor.b);
    float softness = clamp(brightness - threshold.z + threshold.w, 0.0f, 2.0f * threshold.w);
    softness = (softness * softness) / (4.0f * threshold.w + 1e-4);
    float multiplier = max(brightness - threshold.z, softness) / max(brightness, 1e-4);
    retColor *= multiplier;

    retColor = max(retColor, 0);
    return retColor;
}

// ��� Ŀ�긦 ���� bloom main
float4 exceptBloomMain(float4 Color : COLOR0, float2 TexCoord : TEXCOORD0) : SV_Target0
{
    float4 retColor = EndRenderTarget.Sample(Sampler, TexCoord);

    // Clamp
    retColor = min(threshold.x, retColor);

    float brightness = max(max(retColor.r, retColor.g), retColor.b);
    float softness = clamp(brightness - threshold.z + threshold.w, 0.0f, 2.0f * threshold.w);
    softness = (softness * softness) / (4.0f * threshold.w + 1e-4);
    float multiplier = max(brightness - threshold.z, softness) / max(brightness, 1e-4);

    // multiplier�� 0���� Ŭ �� 0�� ��ȯ�Ѵ�.
    [flatten]
    if (multiplier > 0.0001f)
    {
        // 0���� �ϴ� ���� �ƴ϶� ���� �� �׿��ݽô�.
        //return float4(0.f, 0.f, 0.f, 1.0f);
        retColor *= multiplier;

        retColor = max(retColor, 0);
        return retColor;
        /*
        */
    }
        
    return retColor;
}

// �����ø�
float4 UpSampleMain(float4 Color : COLOR0, float2 TexCoord : TEXCOORD0) : SV_Target0
{
    // Source�� ������ Ÿ�� ���ø�
    float4 highMip = EndRenderTarget.Sample(Sampler, TexCoord);
    float4 lowMip = lowMipTarget.Sample(Sampler, TexCoord);

    return lerp(highMip, lowMip, threshold.y);
}