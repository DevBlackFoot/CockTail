// PBR 모델을 그려주는 shader 입니다. 디퍼드를 위해 빛 연산은 하지 않았습니다.

// 이후에 필요하다면 추가하겠지만 이 쉐이더 코드는 기본적으로 모델이 있음을 가정합니다
#include "PBRModelTypes.hlsli"
#include "PBRLightUtils.hlsli"

// 텍스쳐 맵
Texture2D AlbedoMap : register(t0);
Texture2D NormalMap : register(t1);
Texture2D MetallicMap : register(t2);
Texture2D RoughnessMap : register(t3);
Texture2D AmbientOcclusionMap : register(t4);
Texture2D EmissiveMap : register(t5);

// 깊이 버퍼를 받아옵니다.
Texture2D DepthBuffer : register(t6);

Texture2D NoiseMap : register(t7);

// 스텐실 버퍼를 받아오도록 합니다.
	// 포맷이 달라서 Array로 만들 수 없었습니다.
Texture2D<uint2> StencilBuffer : register(t8);


#define EPSILON 0.000001f

// 샘플러
SamplerState Sampler : register(s0);

// PBR과 관련된 파라메터.
cbuffer cbMaterial : register(b0)
{
    float Metallic;
    float Roughness;
    float2 pad1;

    float4 AddColor;

    float3 EmissiveColor;
    float Offset;
	
    bool IsLight;
}

// Texture에대한 정보 => 정확하게는 스크린에 대한 정보
cbuffer cbScreenInfo : register(b1)
{
    float4 screenInfo;
}

// EyePos => View Vector를 뽑아내기 위한 정보
cbuffer cbEyePos : register(b2)
{
    float4 viewPos;
}

// Rim Color Buffer.
cbuffer cbRimColor : register(b3)
{
    float4 rimColor;
}

cbuffer cbTransObj : register(b4)
{
    float4 offset;
}

// outPut
	// 연결된 Render 타겟에 각각의 정보를 Output 해줍니다.
struct PS_Output
{
    // albedo => Texture만
    float4 Albedo : SV_TARGET0;
    // depth
    float4 DMRAO : SV_TARGET1; // Depth, Metallic, Roughness, Ambient Occlusion
    // normal
    float4 Normal : SV_TARGET2;
    // 월드
    float4 WorldPos : SV_TARGET3;
    // emissive
    float4 Emissive : SV_TARGET4;
};

// 픽셀 쉐이더
PS_Output main(VS_Output pin) : SV_Target
{
    float4 color = pin.Color;
    float metallic = pin.metallic;
    float roughness = pin.roughness;
    float ambientOcclusion = 1.f;
    // emissive의 w 값을 Rim Light value로 해줄 생각입니다.
    float4 emissive = float4(0.0f, 0.0f, 0.0f, 1.0f);

#ifdef USE_ALBEDO
    // Diffuse(Albdeo texture)를 감마 디코딩 하여 샘플 한 뒤,
    color = SRGBToLinear(AlbedoMap.Sample(Sampler, pin.Tex));

	// 유니티에서 가져온 rgb 컬러 머테리얼을 디코딩 해서 블렌딩(곱 연산) 해줍니다.
    //color.rgb *= SRGBToLinear(AddColor.rgb);
#else
    // rgb컬러 머테리얼을 디코딩 해서(기본적으로 컬러 값을 감마 인코딩 해서 값을 넣어준다고 가정) 넣어줍니다.
    color = SRGBToLinear(AddColor);
#endif

#ifdef USE_NORMAL
    float3 normalMapSample = NormalMap.Sample(Sampler, pin.Tex).rgb;
    pin.NormalW = NormalSampleToWorldSpace(normalMapSample, pin.NormalW, pin.TangentW);
#else
    pin.NormalW = normalize(pin.NormalW);
#endif

    // ref by. DUOL
#ifdef USE_PAPERBURN
    float noise = NoiseMap.Sample(Sampler, pin.Tex).x;
    float result = (noise * 10.0f / 6.0f) + Offset;

    if (1.0f - result < 0.0f)
    {
        color = float4(0, 0, 0, 1.0f);
    }
    else if (0.98f - result < 0.0f)
    {
        color = color * float4(0.3f, 0.3f, 0.3f, 1.0f);
    }
    else if (0.95f - result < 0.0f)
    {
        color = color * float4(0.6f, 0.6f, 0.6f, 1.0f);
    }

    if (1.5f - result < 0.0f)
    {
        clip(-1.0f);
    }
    else if (1.49f - result < 0.0f)
    {
        color = float4(1.0f, 0.0f, 0.0f, 1.0f);
    }
    else if (1.48f - result < 0.0f)
    {
        color = float4(0.4f, 0.0f, 0.0f, 1.0f);
    }
#endif
    
#ifdef USE_METALLIC
    metallic = MetallicMap.Sample(Sampler, pin.Tex);
#endif

#ifdef USE_ROUGHNESS
    roughness = RoughnessMap.Sample(Sampler, pin.Tex);
#endif

#ifdef USE_AO
    ambientOcclusion = AmbientOcclusionMap.Sample(Sampler, pin.Tex);
#endif

#ifdef USE_EMISSIVE
    emissive = EmissiveMap.Sample(Sampler, pin.Tex);
#else
    //emissive.rgb = SRGBToLinear(EmissiveColor);
    emissive.rgb = EmissiveColor.rgb;
#endif

    PS_Output pout = (PS_Output)0;

#ifdef SILHOUETTE
    // silhouette 위한 Depth Clip
    //const float2 posTexCoord = pin.PosH.xy / screenInfo.xy;

    uint2 stencil = StencilBuffer.Load(int3(pin.PosH.xy, 0));
    
    // 스텐실을 통과하면 다 그린다.
		// 버퍼의 특성상 스텐실은 g에 기록됩니다.
    clip( stencil.g != 1 ? -1 : 1 );
#endif

#ifdef EDGE
    uint2 stencil = StencilBuffer.Load(int3(pin.PosH.xy, 0));

    // 외곽선을 그려줍니다.
		// 내부에 그려진 3이 아닐 때만 그려줍니다.
    clip(stencil.g != 3 ? 1 : -1);
#endif
    
#ifdef TRANSPARENCYZTEST
    // 반투명 오브젝트를 위한 Depth Clip
    const float2 posTexCoord = pin.PosH.xy / screenInfo.xy;

	float4 depth = DepthBuffer.Sample(Sampler, posTexCoord);

    // 불투명 오브젝트가 그려진 것이 있을 때.
    if (depth.x > 0.0001f)
    {
        clip(depth.x - pin.PosH.z);
        //clip(pin.PosH.z - depth.x);
    }
    
#endif
    pout.Normal = float4(pin.NormalW, 1.0f);

    pout.DMRAO = float4(pin.PosH.z, metallic, roughness, ambientOcclusion);

#ifdef SILHOUETTE
    pout.WorldPos = float4(pin.PosW, false);
#else
    // 빛 여부 포함
    pout.WorldPos = float4(pin.PosW, IsLight);
#endif

    pout.Albedo = color;

    pout.Emissive = emissive;

    // Emissive의 w 값에 RimValue를 넣어주고, 해당 부분은 스킨 오브젝트만 가능하게 해준다..
#ifdef IsSkinned
    float3 toEye = normalize(viewPos.xyz - pin.PosW);
    float v = dot(pin.NormalW, toEye);
    // w가 RimWidth.
    float RimLightColor = smoothstep(1.0f - rimColor.w, 1.0f, 1 - max(0, v));
    
	pout.Emissive.rgb += (RimLightColor * SRGBToLinear(rimColor.rgb));
#endif

#ifdef EDGE
    // 엣지일 경우 디졸브를 줘봅시다.
    // 페이드 오프셋.
		// x 값은 VS에서 사용했다고 가정
    pout.Emissive = lerp(0.f, pout.Emissive, offset.y);
    //pout.Albedo.a = lerp(0.f, pout.Albedo.a, offset.y);
#endif
    return pout;
}

// 몬스터 등장에만 사용해주는..
PS_Output monMain(VS_Monster_Output pin) : SV_Target
{
    float yVal = pin.PosL.y;

    // 1을 넘는 애들을 날려봅니다.
		// screenInfo의 w값을 offset으로 사용합니다.
    float offset = screenInfo.w;
    clip(offset - yVal);

    float4 color = pin.Color;
    float metallic = pin.metallic;
    float roughness = pin.roughness;
    float ambientOcclusion = 1.f;
    // emissive의 w 값을 Rim Light value로 해줄 생각입니다.
    float4 emissive = float4(0.0f, 0.0f, 0.0f, 1.0f);

	// 모든 텍스쳐 맵을 사용하지 않습니다.
    // rgb컬러 머테리얼을 디코딩 해서(기본적으로 컬러 값을 감마 인코딩 해서 값을 넣어준다고 가정) 넣어줍니다.
    color = SRGBToLinear(AddColor);

    pin.NormalW = normalize(pin.NormalW);

    //emissive.rgb = SRGBToLinear(EmissiveColor);
    emissive.rgb = EmissiveColor.rgb;

    PS_Output pout = (PS_Output) 0;
    
    // 반투명 오브젝트를 위한 Depth Clip
    const float2 posTexCoord = pin.PosH.xy / screenInfo.xy;

	float4 depth = DepthBuffer.Sample(Sampler, posTexCoord);

    // 불투명 오브젝트가 그려진 것이 있을 때.
    if (depth.x > 0.0001f)
    {
        clip(depth.x - pin.PosH.z);
    }

    pout.Normal = float4(pin.NormalW, 1.0f);

    pout.DMRAO = float4(pin.PosH.z, metallic, roughness, ambientOcclusion);

    // 빛 여부는 일단 받지 않는 것으로 합니다.
    pout.WorldPos = float4(pin.PosW, false);

    pout.Albedo = color;

    pout.Emissive = emissive;

    // Emissive의 w 값에 RimValue를 넣어주고, 해당 부분은 스킨 오브젝트만 가능하게 해준다..

    /*
	*/
    float3 toEye = normalize(viewPos.xyz - pin.PosW);
    float v = dot(pin.NormalW, toEye);
    float RimLightColor = smoothstep(0.8f, 1.0f, 1 - max(0, v));

	//pout.Albedo.rgb += (RimLightColor * SRGBToLinear(float3(0.9f, 0.5f, 1.0f)));
    pout.Albedo.rgb += (RimLightColor * SRGBToLinear(rimColor.rgb));


    return pout;
}