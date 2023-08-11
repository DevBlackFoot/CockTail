// Deferred 이후 들어온 각각의 Pxiel들에 대한 PostProcessing을 수행하는 Pixel Shader 입니다.
#include "ColorHelper.hlsli"

// 픽셀 쉐이더에 들어오는 값 => QuadVS의 OutPut값
	// 실제로 사용하는 값은 Texture 값만 사용해 줍니다..
struct PostPixelIn
{
    float4 PosH : SV_POSITION;
    float2 Tex : TEXCOORD;
};

// light 연산을 모두 끝낸 RenderTarget의 Texture
	// 해당 텍스쳐에서 샘플한 뒤, 후처리를 해줍니다.
Texture2D EndRenderTarget : register(t0);

// 축소버퍼들
Texture2DArray DownBuffer : register(t1);

// UpSample때 사용할 lowMip
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

// Bloom 값을 지정하는 threshold
cbuffer BloomThreshold : register(b3)
{
    float4 threshold;
}

// Texture 크기를 받아오는 buffer
cbuffer TextureInfo : register(b4)
{
    // x의 값을 fade in, out의 offset으로 지정해도 될 것 같다.
    float4 texInfo;
}

// fadeOut offset을 위한 버퍼
cbuffer FadeOffset : register(b5)
{
    float4 fadeOffset;
}

// Unity의 Neutral Tonemapping 함수를 가져왔습니다.
	// Hable, Heji, Frostbite의 공식을 사용하는 것 같습니다.
// Input 값은 Linear RGB여야 하므로, light Pass의 마지막 감마 인코딩을 제거해줍니다.
// Unity의 real을 float로 취급해주었습니다.
float3 NeutralCurve(float3 x, float a, float b, float c, float d, float e, float f)
{
    return ((x * (a * x + c * b) + d * e) / (x * (a * x + b) + d * f)) - e / f;
}

float3 NeutralTonemap(float3 pixelColor)
{
	// ToneMap 수치들
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

// Unity의 Vignetting 함수.
float3 Vignette(float3 cin, float2 uv, float intensity, float smoothness)
{
	// center 0.5, color black 고정입니다..
    float2 center = float2(0.5f, 0.5f);
    // 검은색..(외곽의 색깔.)
    float3 color = vignetteColor.xyz;
    // 유니티의 내부에 숨겨져 있던 Magic Number
    intensity *= 3.f;
    smoothness *= 5.f;

    float2 dist = abs(uv - center) * intensity;

    float vfactor = pow(saturate(1.0f - dot(dist, dist)), smoothness);

    return lerp(cin, cin * lerp(color, (1.0f).xxx, vfactor), vignetteColor.w);
}

// RGBM이라 가정하고..
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


// 픽셀 쉐이더
float4 main(PostPixelIn ppin) : SV_Target
{
    // 후처리를 받기 전의 색상
    float3 retColor = EndRenderTarget.Sample(Sampler, ppin.Tex).xyz;
    
#ifdef NotPostProcessing
    retColor = LinearToSRGB(retColor);

    return float4(retColor, 1.0f);
#endif

#ifdef UseBlurBloom
    float3 bloomColor = lowMipTarget.Sample(Sampler, ppin.Tex).xyz;
    float3 blurColor = blurTexture.Sample(Sampler, ppin.Tex).xyz;
    
    // 비네틱 공식을 사용해봅니다.
    float2 center = float2(0.5f, 0.5f);
    // 0.6.. 스무스 0으로
    float intensity = 3.f * vignetteBuffer.z;
    float smoothness = 5.f * vignetteBuffer.w;

    float2 dist = abs(ppin.Tex - center) * intensity;
    float vfactor = pow(saturate(1.0f - dot(dist, dist)), smoothness);

    // vfactor를 기준으로 블러 픽셀을 사용할지, retColor를 사용할지 결정합니다.
		// 0에 가까울 수록 blur를 넣습니다.
    retColor = lerp(blurColor, retColor, vfactor);

    retColor += lerp(retColor, bloomColor, threshold.y);
#endif

    // ToneMapping을 해줍니다.
#ifdef UseToneMapping
    retColor = NeutralTonemap(retColor);
    saturate(retColor);
#endif

#ifdef UseVignette
    // 현재는 기본값을 세팅해줍니다. 필요하면 cBuffer로 전달해줍니다.
    retColor = Vignette(retColor, ppin.Tex, vignetteBuffer.x, vignetteBuffer.y);
#endif

    retColor = lerp(0.f, retColor, fadeOffset.x);

    retColor = LinearToSRGB(retColor);

    return float4(retColor, 1.0f);
}


// 블러를 위한 Blur main
float4 blurMain(float4 Unused : COLOR0, float2 TexCoord : TEXCOORD0) : SV_Target0
{
    float4 Color = 0;

    // 블러
    for (int i = 0; i < BlurDepth; i++)
    {
        // 0번 슬롯에 들어온다고 가정.
        Color += EndRenderTarget.Sample(Sampler, TexCoord + sampleOffsets[i].xy) * sampleWeights[i].x;
    }

    return Color;
}

// Unity의 블러 공식을 가져와보겠습니다..
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

// 블룸 커브를 위한 bloom main
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

// 블룸 커브를 위한 bloom main
float4 exceptBloomMain(float4 Color : COLOR0, float2 TexCoord : TEXCOORD0) : SV_Target0
{
    float4 retColor = EndRenderTarget.Sample(Sampler, TexCoord);

    // Clamp
    retColor = min(threshold.x, retColor);

    float brightness = max(max(retColor.r, retColor.g), retColor.b);
    float softness = clamp(brightness - threshold.z + threshold.w, 0.0f, 2.0f * threshold.w);
    softness = (softness * softness) / (4.0f * threshold.w + 1e-4);
    float multiplier = max(brightness - threshold.z, softness) / max(brightness, 1e-4);

    // multiplier가 0보다 클 때 0을 반환한다.
    [flatten]
    if (multiplier > 0.0001f)
    {
        // 0으로 하는 것이 아니라 색을 좀 죽여줍시다.
        //return float4(0.f, 0.f, 0.f, 1.0f);
        retColor *= multiplier;

        retColor = max(retColor, 0);
        return retColor;
        /*
        */
    }
        
    return retColor;
}

// 업샘플링
float4 UpSampleMain(float4 Color : COLOR0, float2 TexCoord : TEXCOORD0) : SV_Target0
{
    // Source로 들어오는 타겟 샘플링
    float4 highMip = EndRenderTarget.Sample(Sampler, TexCoord);
    float4 lowMip = lowMipTarget.Sample(Sampler, TexCoord);

    return lerp(highMip, lowMip, threshold.y);
}