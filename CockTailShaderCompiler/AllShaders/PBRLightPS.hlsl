// Deferred Rendering�� ���� �� ������ �ϱ� ���� Light Shader �Դϴ�.
#include "PBRLightUtils.hlsli"

struct LightPixelIn
{
    float4 PosH : SV_POSITION;
    float2 Tex : TEXCOORD;
};


cbuffer cbLight : register(b0)
{
    DirectionalLight DirLights[DIRECTION_LIGHT_COUNT];
    PointLight PointLights[POINT_LIGHT_COUNT];
    SpotLight SpotLights[SPOT_LIGHT_COUNT];

    float3 EyePosW; // CameraPos

    int DirLightCnt;
    int PointLightCnt;
    int SpotLightCnt;
}

// �ؽ��Ŀ� ���� ������ ��� �ִ� texture �������
	// �ؼ��� ũ�⿡ ���� ������ ��� �ִ�.
cbuffer cbTexture : register(b1)
{
    float4 textureInfo;
}

// AmbientColor�� ���� ������ ����ִ� Color ��� ����
cbuffer cbColor : register(b2)
{
    float4 environmentColor;
}

// DLightTexture dirLight �׸����� ������ ��� �ִ� �����Դϴ�.
cbuffer cbDirTex : register(b3)
{
    float4 dirTextureInfo;
}

// environment ���� ����
cbuffer cbIBL : register(b4)
{
    float4 IBLOffset;
}

// RenderTarget Texture

Texture2D Albedo : register(t0);

Texture2D DMRAO : register(t1); // depth, metallic, roughness, ambient occlusion

Texture2D Normal : register(t2);

Texture2D Position : register(t3);

Texture2D Emissive : register(t4);

Texture2D Shadow : register(t5);

// PointLight(CubeMap) Shadow
TextureCubeArray PointShadow : register(t6);

// SpotLight (TextureArray) Shadow
Texture2DArray SpotShadow : register(t7);

// Bake Map (Point)
TextureCubeArray BakedPointShadow : register(t8);

// Bake Map (Spot)
Texture2DArray BakedSpotShadow : register(t9);

// IBL���� �ؽ��� �� �� �߰�..
TextureCube IBLDiffuse : register(t10);
TextureCube IBLSpecular : register(t11);
Texture2D IBLBrdf : register(t12);


SamplerState Sampler : register(s0);
SamplerComparisonState samLinearPointBoarder : register(s1);
SamplerState SamplerTriLinear : register(s2);

float4 main(LightPixelIn lpin) : SV_Target
{
    float depth = DMRAO.Sample(Sampler, lpin.Tex).x;

    float3 normal = Normal.Sample(Sampler, lpin.Tex).xyz;

    float4 position = Position.Sample(Sampler, lpin.Tex).xyzw;

    float4 albedo = Albedo.Sample(Sampler, lpin.Tex).xyzw;

    float3 emissive = Emissive.Sample(Sampler, lpin.Tex).xyz;

    float shadowVal = 1.0f;

    // TODO : Shadow�� SSAO
#ifdef IsShadow
    // �޾ƿ� shadow Texture�� �̿����ش�.
    // ����
    // Shadow ��ġ ������ �ٽ� ������ش�.
        // Shadow VS�� ����� �����ϴ�.
    float4 shadow = mul(float4(position.xyz, 1.0f), DirLights[0].lightViewProj);

    shadow.xyz /= shadow.w;

    // NDC �������� �ؽ��� �������� ��ȯ
    shadow.x = 0.5f * shadow.x + 0.5f;
    shadow.y = -0.5f * shadow.y + 0.5f;

    // ���� �׸��� ��� ���
    if(DirLights[0].isShadow != true)
        shadowVal = 1.0f;
    else
		shadowVal = CalcShadowFactor(samLinearPointBoarder, Shadow, shadow, dirTextureInfo);
#endif

    float ambientAccess = 1.f;

    // �ݼӼ�
    float metallic = DMRAO.Sample(Sampler, lpin.Tex).y;

	// ��ĥ��
    float roughness = DMRAO.Sample(Sampler, lpin.Tex).z;

	// ����
    float ambientOcclusion = DMRAO.Sample(Sampler, lpin.Tex).w * ambientAccess;

    float3 toEye = normalize(EyePosW - position.xyz);

    bool isLight = position.w;

    float3 litColor = float3(0.0f, 0.0f, 0.0f);

    // emissive�� ����Ǿ� �ִ� �� ��������
    //albedo = pow(albedo, 2.2f);

    if (isLight)
    {
        litColor = PBR_DirectionalLight(toEye, normal, DirLights[0],
			albedo.xyz, ambientOcclusion, roughness, metallic, shadowVal);
        
        //litColor += PBR_PointLight(toEye, normal, PointLights, PointLightCnt, position.xyz,
		//	albedo.xyz, ambientOcclusion, roughness, metallic, shadowVal);

        /// Point Light
        float3 acc_color = float3(0.f, 0.f, 0.f);
        PointLight light;
        float4 fragToLight = float4(0.f, 0.f, 0.f, 0.f);

        // �⺻������ ���� �Լ� ������ ������ ������ ���� ��.
        [unroll]
        for (int i = 0; i < PointLightCnt; i++)
        {
            light = PointLights[i];

#ifdef IsPointShadow
			fragToLight = float4((position.xyz - light.position), 1.0f);

            if(light.isShadow != true)
                shadowVal = 1.0f;
            else
            {
#ifdef IsIndoor
                shadowVal = CalcPointShadowFactor(samLinearPointBoarder, i, PointShadow, fragToLight, textureInfo, light.range);
	            shadowVal += environmentColor.w;

            	[flatten]
                if(shadowVal > 1.0f)
                    shadowVal = 1.0f;
#else  // IsIndoor
                shadowVal = CalcPointShadowFactor(samLinearPointBoarder, i, PointShadow, BakedPointShadow, fragToLight, textureInfo, light.range);
#endif  // IsIndoor
            }
#endif  // IsPointLight
            /*
			//environmentColor�� ���� ���� �׸��� ���� factor�� ���� �� ���� �� �ϴ�. (�󸶳� ��� �� ���ΰ�..)
             */

            acc_color += PBR_OnePointLight(toEye, normal, light, position.xyz, albedo.xyz,
            ambientOcclusion, roughness, metallic, shadowVal);
        }

        litColor += acc_color;

        //litColor += PBR_SpotLight(toEye, normal, SpotLights, SpotLightCnt, position.xyz,
		//	albedo.xyz, ambientOcclusion, roughness, metallic);

        /// Spot Light
        acc_color = float3(0.f, 0.f, 0.f);
        SpotLight sLight;

        // �׸��� ��ü�� dir Light�� ���� ������ݴϴ�.
        [unroll]
        for (int j = 0; j < SpotLightCnt; j++)
        {
            sLight = SpotLights[j];
            
#ifdef IsSpotShadow
            float4 shadow = mul(float4(position.xyz, 1.0f), sLight.lightViewProj);

            shadow.xyz /= shadow.w;

		// NDC �������� �ؽ��� �������� ��ȯ
            shadow.x = 0.5f * shadow.x + 0.5f;
            shadow.y = -0.5f * shadow.y + 0.5f;

		// ���� �׸��� ��� ���
        	if(sLight.isShadow != true)
                shadowVal = 1.0f;
            else
            {
#ifdef IsIndoor
				shadowVal = CalcShadowFactor(samLinearPointBoarder, SpotShadow, shadow, textureInfo, j);

				shadowVal += environmentColor.w;;

            	[flatten]
                if(shadowVal > 1.0f)
                    shadowVal = 1.0f;
#else  // IsIndoor
				shadowVal = CalcShadowFactor(samLinearPointBoarder, SpotShadow, BakedSpotShadow, shadow, textureInfo, j);
#endif  // IsIndoor
            }
#endif

            acc_color += PBR_OneSpotLight(toEye, normal, sLight, position.xyz, albedo.xyz,
            ambientOcclusion, roughness, metallic, shadowVal);
        }

        litColor += acc_color;

#ifdef IsIndoor
#else
        // Unity�� �ڵ带 �����Խ��ϴ�.
        // ��ǻ��� �븻�� �������ݴϴ�.
        // diffuse ���
        float oneMinusDielectricSpec = kDielectricSpec.a;
        float oneMinusReflectivity = oneMinusDielectricSpec - metallic * oneMinusDielectricSpec;
        float3 c_diff = albedo * oneMinusReflectivity;
        float3 c_spec = lerp(kDielectricSpec.rgb, albedo, metallic) * ambientOcclusion;

        float4 indirectDiffuseM = IBLDiffuse.Sample(Sampler, normal);
        float3 indirectDiffuse = indirectDiffuseM.rgb * 6.0f * indirectDiffuseM.a;

        // IBL Diffuse
        litColor += (c_diff.rgb * indirectDiffuse) * IBLOffset.x;

        // �ݻ纤��
        float3 r = reflect(-toEye, normal.xyz);
        float NoV = dot(normal.xyz, toEye);
        // perceptual Roughness�� ���˴ϴ�. �������� �ʰ� ����ϴ� �����Ͻ� ��.
        float perceptualRoughness = roughness * (1.7f - 0.7f * roughness);
        float mip = perceptualRoughness * 6.0f;
        float4 indirectSpecM = IBLSpecular.SampleLevel(SamplerTriLinear, normalize(r).xyz, mip);
        float3 indirectSpec = indirectSpecM.rgb * 6.0f * indirectSpecM.a;

        float2 BRDF = IBLBrdf.Sample(Sampler, float2(max(NoV, 0.f), roughness)).rg;
        
    	// unity�� grazing term�� lookup table�� ��ü �մϴ�.
        litColor += (indirectSpec.rgb * (c_spec * BRDF.x + BRDF.y)) * IBLOffset.x;

        // ����Ƽ�� �� ���
        //litColor += (indirectSpec * (surfaceReduction * lerp(c_spec, grazingTerm, fresnelTerm))) * IBLOffset.x;
#endif

    	litColor += emissive;
    }
    else
    {
        litColor += albedo.xyz;

        litColor += emissive;
    }

    // �ش� �κ��� �ϵ��ڵ� �غ��ô�..
	//float3 ambientLighting = albedo.xyz * (1.1f - roughness) * 0.3f;

    float3 ambientLighting = float3(0.f, 0.f, 0.f);
#ifdef IsIndoor
    float oneMinusDielectricSpec = kDielectricSpec.a;
    float oneMinusReflectivity = oneMinusDielectricSpec - metallic * oneMinusDielectricSpec;
    // �ش� �κ��� ��Ż���� ���� ������ �ް� ���ݴϴ�.
	ambientLighting = (albedo.xyz * SRGBToLinear(float3(environmentColor.xyz)) * oneMinusReflectivity) * (1.1f - roughness) * 0.7f;
#endif

    // ���� Correction
		// PostProcessing�� ���� ���ݴϴ�.
    //litColor = LinearToSRGB(litColor);

    return float4(litColor + ambientLighting, 1.0f);
}