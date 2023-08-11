// SpriteEffect를 그리기 위한 픽셀 셰이더 코드 입니다.

#include "SpriteEffectTypes.hlsli"
#include "ColorHelper.hlsli"

// 이펙트 텍스쳐
Texture2D EffectMap : register( t0 );

// 샘플러
SamplerState Sampler : register( s0 );

// 여기에 Dissolve를 위한 알파를 추가합니다.
cbuffer cbColor : register(b0)
{
    // 필요하다면 색을 추가할 수도 있습니다.
    float4 maskColor;
}

cbuffer cbEmissiveColor : register(b1)
{
    float4 emissive;
}

cbuffer cbSRCOffset : register(b2)
{
    // 오프셋이 아닌 srcEffect..
		// 어디 부터 어디까지 가져올 것인지.
		// x - minX, y - minY, z - maxX, w - maxY
		// minX ~ maxX 까지 렌더
		// minY ~ maxY 까지 렌더.
		// 넘는 친구들은 클립..
    float4 SRCOffset;
};

// RasterizerState와 DepthStencilState의 경우 밖에서 지정해준다..
// 렌더 타겟에 각 정보를 output해 봅시다..
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
    // 텍스쳐를 샘플해서 바로 내보내면 된다.
    PS_Output pout = (PS_Output) 0;
#ifdef USE_SRCOFFSET
    // src 클립
    clip(pin.Tex.x - SRCOffset.x);
    clip(SRCOffset.z - pin.Tex.x);
    clip(pin.Tex.y - SRCOffset.y);
    clip(SRCOffset.w - pin.Tex.y);
#endif

	// 샘플을 할 때 srcOffset에 따라..
    pout.Albedo = SRGBToLinear(EffectMap.Sample(Sampler, pin.Tex));

	// 알파값에 따라서 날려버립니다..
		// 이러니 렌더가 안됩니다.. 기존 값을 넣어주어야 하는데.
    //pout.WorldPos = lerp(float4(0.f, 0.f, 0.f, 0.f), float4(pin.PosW, 0.f), pout.Albedo.a);

    // 마스크 알파를 기준으로 블렌딩 해줍니다.
    pout.Albedo.a = lerp(0.f, pout.Albedo.a, maskColor.a);
    // 알파 값이 0인 경우, 해당 픽셀을 처리하지 않습니다
		// Plane의 여백 중 alpha에 해당하는 부분을 정상적으로 렌더할 수 있었습니다.
    clip(pout.Albedo.a - 0.0001f);

    // 마스크 컬러를 블렌딩 해봅니다.
		// 색상을 곱해주면 됩니다.
    pout.Albedo.rgb *= maskColor.rgb;

    pout.WorldPos = float4(pin.PosW, 0.f);

    pout.Emissive = emissive;

    return pout;
}