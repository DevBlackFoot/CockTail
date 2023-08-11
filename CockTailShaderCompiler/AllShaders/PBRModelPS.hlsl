// PBR ���� �׷��ִ� shader �Դϴ�. ���۵带 ���� �� ������ ���� �ʾҽ��ϴ�.

// ���Ŀ� �ʿ��ϴٸ� �߰��ϰ����� �� ���̴� �ڵ�� �⺻������ ���� ������ �����մϴ�
#include "PBRModelTypes.hlsli"
#include "PBRLightUtils.hlsli"

// �ؽ��� ��
Texture2D AlbedoMap : register(t0);
Texture2D NormalMap : register(t1);
Texture2D MetallicMap : register(t2);
Texture2D RoughnessMap : register(t3);
Texture2D AmbientOcclusionMap : register(t4);
Texture2D EmissiveMap : register(t5);

// ���� ���۸� �޾ƿɴϴ�.
Texture2D DepthBuffer : register(t6);

Texture2D NoiseMap : register(t7);

// ���ٽ� ���۸� �޾ƿ����� �մϴ�.
	// ������ �޶� Array�� ���� �� �������ϴ�.
Texture2D<uint2> StencilBuffer : register(t8);


#define EPSILON 0.000001f

// ���÷�
SamplerState Sampler : register(s0);

// PBR�� ���õ� �Ķ����.
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

// Texture������ ���� => ��Ȯ�ϰԴ� ��ũ���� ���� ����
cbuffer cbScreenInfo : register(b1)
{
    float4 screenInfo;
}

// EyePos => View Vector�� �̾Ƴ��� ���� ����
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
	// ����� Render Ÿ�ٿ� ������ ������ Output ���ݴϴ�.
struct PS_Output
{
    // albedo => Texture��
    float4 Albedo : SV_TARGET0;
    // depth
    float4 DMRAO : SV_TARGET1; // Depth, Metallic, Roughness, Ambient Occlusion
    // normal
    float4 Normal : SV_TARGET2;
    // ����
    float4 WorldPos : SV_TARGET3;
    // emissive
    float4 Emissive : SV_TARGET4;
};

// �ȼ� ���̴�
PS_Output main(VS_Output pin) : SV_Target
{
    float4 color = pin.Color;
    float metallic = pin.metallic;
    float roughness = pin.roughness;
    float ambientOcclusion = 1.f;
    // emissive�� w ���� Rim Light value�� ���� �����Դϴ�.
    float4 emissive = float4(0.0f, 0.0f, 0.0f, 1.0f);

#ifdef USE_ALBEDO
    // Diffuse(Albdeo texture)�� ���� ���ڵ� �Ͽ� ���� �� ��,
    color = SRGBToLinear(AlbedoMap.Sample(Sampler, pin.Tex));

	// ����Ƽ���� ������ rgb �÷� ���׸����� ���ڵ� �ؼ� ����(�� ����) ���ݴϴ�.
    //color.rgb *= SRGBToLinear(AddColor.rgb);
#else
    // rgb�÷� ���׸����� ���ڵ� �ؼ�(�⺻������ �÷� ���� ���� ���ڵ� �ؼ� ���� �־��شٰ� ����) �־��ݴϴ�.
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
    // silhouette ���� Depth Clip
    //const float2 posTexCoord = pin.PosH.xy / screenInfo.xy;

    uint2 stencil = StencilBuffer.Load(int3(pin.PosH.xy, 0));
    
    // ���ٽ��� ����ϸ� �� �׸���.
		// ������ Ư���� ���ٽ��� g�� ��ϵ˴ϴ�.
    clip( stencil.g != 1 ? -1 : 1 );
#endif

#ifdef EDGE
    uint2 stencil = StencilBuffer.Load(int3(pin.PosH.xy, 0));

    // �ܰ����� �׷��ݴϴ�.
		// ���ο� �׷��� 3�� �ƴ� ���� �׷��ݴϴ�.
    clip(stencil.g != 3 ? 1 : -1);
#endif
    
#ifdef TRANSPARENCYZTEST
    // ������ ������Ʈ�� ���� Depth Clip
    const float2 posTexCoord = pin.PosH.xy / screenInfo.xy;

	float4 depth = DepthBuffer.Sample(Sampler, posTexCoord);

    // ������ ������Ʈ�� �׷��� ���� ���� ��.
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
    // �� ���� ����
    pout.WorldPos = float4(pin.PosW, IsLight);
#endif

    pout.Albedo = color;

    pout.Emissive = emissive;

    // Emissive�� w ���� RimValue�� �־��ְ�, �ش� �κ��� ��Ų ������Ʈ�� �����ϰ� ���ش�..
#ifdef IsSkinned
    float3 toEye = normalize(viewPos.xyz - pin.PosW);
    float v = dot(pin.NormalW, toEye);
    // w�� RimWidth.
    float RimLightColor = smoothstep(1.0f - rimColor.w, 1.0f, 1 - max(0, v));
    
	pout.Emissive.rgb += (RimLightColor * SRGBToLinear(rimColor.rgb));
#endif

#ifdef EDGE
    // ������ ��� �����긦 �຾�ô�.
    // ���̵� ������.
		// x ���� VS���� ����ߴٰ� ����
    pout.Emissive = lerp(0.f, pout.Emissive, offset.y);
    //pout.Albedo.a = lerp(0.f, pout.Albedo.a, offset.y);
#endif
    return pout;
}

// ���� ���忡�� ������ִ�..
PS_Output monMain(VS_Monster_Output pin) : SV_Target
{
    float yVal = pin.PosL.y;

    // 1�� �Ѵ� �ֵ��� �������ϴ�.
		// screenInfo�� w���� offset���� ����մϴ�.
    float offset = screenInfo.w;
    clip(offset - yVal);

    float4 color = pin.Color;
    float metallic = pin.metallic;
    float roughness = pin.roughness;
    float ambientOcclusion = 1.f;
    // emissive�� w ���� Rim Light value�� ���� �����Դϴ�.
    float4 emissive = float4(0.0f, 0.0f, 0.0f, 1.0f);

	// ��� �ؽ��� ���� ������� �ʽ��ϴ�.
    // rgb�÷� ���׸����� ���ڵ� �ؼ�(�⺻������ �÷� ���� ���� ���ڵ� �ؼ� ���� �־��شٰ� ����) �־��ݴϴ�.
    color = SRGBToLinear(AddColor);

    pin.NormalW = normalize(pin.NormalW);

    //emissive.rgb = SRGBToLinear(EmissiveColor);
    emissive.rgb = EmissiveColor.rgb;

    PS_Output pout = (PS_Output) 0;
    
    // ������ ������Ʈ�� ���� Depth Clip
    const float2 posTexCoord = pin.PosH.xy / screenInfo.xy;

	float4 depth = DepthBuffer.Sample(Sampler, posTexCoord);

    // ������ ������Ʈ�� �׷��� ���� ���� ��.
    if (depth.x > 0.0001f)
    {
        clip(depth.x - pin.PosH.z);
    }

    pout.Normal = float4(pin.NormalW, 1.0f);

    pout.DMRAO = float4(pin.PosH.z, metallic, roughness, ambientOcclusion);

    // �� ���δ� �ϴ� ���� �ʴ� ������ �մϴ�.
    pout.WorldPos = float4(pin.PosW, false);

    pout.Albedo = color;

    pout.Emissive = emissive;

    // Emissive�� w ���� RimValue�� �־��ְ�, �ش� �κ��� ��Ų ������Ʈ�� �����ϰ� ���ش�..

    /*
	*/
    float3 toEye = normalize(viewPos.xyz - pin.PosW);
    float v = dot(pin.NormalW, toEye);
    float RimLightColor = smoothstep(0.8f, 1.0f, 1 - max(0, v));

	//pout.Albedo.rgb += (RimLightColor * SRGBToLinear(float3(0.9f, 0.5f, 1.0f)));
    pout.Albedo.rgb += (RimLightColor * SRGBToLinear(rimColor.rgb));


    return pout;
}