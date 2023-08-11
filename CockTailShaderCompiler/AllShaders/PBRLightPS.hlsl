// Deferred Rendering을 통해 빛 연산을 하기 위한 Light Shader 입니다.
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

// 텍스쳐에 대한 정보를 담고 있는 texture 상수버퍼
	// 텍셀에 크기에 대한 정보를 담고 있다.
cbuffer cbTexture : register(b1)
{
    float4 textureInfo;
}

// AmbientColor에 대한 정보를 담고있는 Color 상수 버퍼
cbuffer cbColor : register(b2)
{
    float4 environmentColor;
}

// DLightTexture dirLight 그림자의 정보를 담고 있는 버퍼입니다.
cbuffer cbDirTex : register(b3)
{
    float4 dirTextureInfo;
}

// environment 조정 버퍼
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

// IBL관련 텍스쳐 세 개 추가..
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

    // TODO : Shadow와 SSAO
#ifdef IsShadow
    // 받아온 shadow Texture를 이용해준다.
    // 투영
    // Shadow 위치 정보를 다시 계산해준다.
        // Shadow VS의 내용과 동일하다.
    float4 shadow = mul(float4(position.xyz, 1.0f), DirLights[0].lightViewProj);

    shadow.xyz /= shadow.w;

    // NDC 공간에서 텍스쳐 공간으로 변환
    shadow.x = 0.5f * shadow.x + 0.5f;
    shadow.y = -0.5f * shadow.y + 0.5f;

    // 실제 그림자 계수 계산
    if(DirLights[0].isShadow != true)
        shadowVal = 1.0f;
    else
		shadowVal = CalcShadowFactor(samLinearPointBoarder, Shadow, shadow, dirTextureInfo);
#endif

    float ambientAccess = 1.f;

    // 금속성
    float metallic = DMRAO.Sample(Sampler, lpin.Tex).y;

	// 거칠기
    float roughness = DMRAO.Sample(Sampler, lpin.Tex).z;

	// 차폐도
    float ambientOcclusion = DMRAO.Sample(Sampler, lpin.Tex).w * ambientAccess;

    float3 toEye = normalize(EyePosW - position.xyz);

    bool isLight = position.w;

    float3 litColor = float3(0.0f, 0.0f, 0.0f);

    // emissive에 저장되어 있는 값 가져오기
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

        // 기본적으로 위의 함수 내부의 내용을 밖으로 빼낸 것.
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
			//environmentColor의 알파 값에 그림자 조정 factor를 넣을 수 있을 듯 하다. (얼마나 밝게 할 것인가..)
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

        // 그림자 자체는 dir Light와 같게 계산해줍니다.
        [unroll]
        for (int j = 0; j < SpotLightCnt; j++)
        {
            sLight = SpotLights[j];
            
#ifdef IsSpotShadow
            float4 shadow = mul(float4(position.xyz, 1.0f), sLight.lightViewProj);

            shadow.xyz /= shadow.w;

		// NDC 공간에서 텍스쳐 공간으로 변환
            shadow.x = 0.5f * shadow.x + 0.5f;
            shadow.y = -0.5f * shadow.y + 0.5f;

		// 실제 그림자 계수 계산
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
        // Unity의 코드를 가져왔습니다.
        // 디퓨즈는 노말로 샘플해줍니다.
        // diffuse 계산
        float oneMinusDielectricSpec = kDielectricSpec.a;
        float oneMinusReflectivity = oneMinusDielectricSpec - metallic * oneMinusDielectricSpec;
        float3 c_diff = albedo * oneMinusReflectivity;
        float3 c_spec = lerp(kDielectricSpec.rgb, albedo, metallic) * ambientOcclusion;

        float4 indirectDiffuseM = IBLDiffuse.Sample(Sampler, normal);
        float3 indirectDiffuse = indirectDiffuseM.rgb * 6.0f * indirectDiffuseM.a;

        // IBL Diffuse
        litColor += (c_diff.rgb * indirectDiffuse) * IBLOffset.x;

        // 반사벡터
        float3 r = reflect(-toEye, normal.xyz);
        float NoV = dot(normal.xyz, toEye);
        // perceptual Roughness가 사용됩니다. 제곱하지 않고 사용하는 러프니스 값.
        float perceptualRoughness = roughness * (1.7f - 0.7f * roughness);
        float mip = perceptualRoughness * 6.0f;
        float4 indirectSpecM = IBLSpecular.SampleLevel(SamplerTriLinear, normalize(r).xyz, mip);
        float3 indirectSpec = indirectSpecM.rgb * 6.0f * indirectSpecM.a;

        float2 BRDF = IBLBrdf.Sample(Sampler, float2(max(NoV, 0.f), roughness)).rg;
        
    	// unity의 grazing term을 lookup table로 대체 합니다.
        litColor += (indirectSpec.rgb * (c_spec * BRDF.x + BRDF.y)) * IBLOffset.x;

        // 유니티의 값 사용
        //litColor += (indirectSpec * (surfaceReduction * lerp(c_spec, grazingTerm, fresnelTerm))) * IBLOffset.x;
#endif

    	litColor += emissive;
    }
    else
    {
        litColor += albedo.xyz;

        litColor += emissive;
    }

    // 해당 부분을 하드코딩 해봅시다..
	//float3 ambientLighting = albedo.xyz * (1.1f - roughness) * 0.3f;

    float3 ambientLighting = float3(0.f, 0.f, 0.f);
#ifdef IsIndoor
    float oneMinusDielectricSpec = kDielectricSpec.a;
    float oneMinusReflectivity = oneMinusDielectricSpec - metallic * oneMinusDielectricSpec;
    // 해당 부분이 메탈릭에 의한 영향을 받게 해줍니다.
	ambientLighting = (albedo.xyz * SRGBToLinear(float3(environmentColor.xyz)) * oneMinusReflectivity) * (1.1f - roughness) * 0.7f;
#endif

    // 감마 Correction
		// PostProcessing을 위해 빼줍니다.
    //litColor = LinearToSRGB(litColor);

    return float4(litColor + ambientLighting, 1.0f);
}