// SpriteEffect�� �׸��� ���� �ȼ� ���̴� �ڵ� �Դϴ�.

#include "SpriteEffectTypes.hlsli"
#include "ColorHelper.hlsli"

// ����Ʈ �ؽ���
Texture2D EffectMap : register( t0 );

// ���÷�
SamplerState Sampler : register( s0 );

// ���⿡ Dissolve�� ���� ���ĸ� �߰��մϴ�.
cbuffer cbColor : register(b0)
{
    // �ʿ��ϴٸ� ���� �߰��� ���� �ֽ��ϴ�.
    float4 maskColor;
}

cbuffer cbEmissiveColor : register(b1)
{
    float4 emissive;
}

cbuffer cbSRCOffset : register(b2)
{
    // �������� �ƴ� srcEffect..
		// ��� ���� ������ ������ ������.
		// x - minX, y - minY, z - maxX, w - maxY
		// minX ~ maxX ���� ����
		// minY ~ maxY ���� ����.
		// �Ѵ� ģ������ Ŭ��..
    float4 SRCOffset;
};

// RasterizerState�� DepthStencilState�� ��� �ۿ��� �������ش�..
// ���� Ÿ�ٿ� �� ������ output�� ���ô�..
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
    // �ؽ��ĸ� �����ؼ� �ٷ� �������� �ȴ�.
    PS_Output pout = (PS_Output) 0;
#ifdef USE_SRCOFFSET
    // src Ŭ��
    clip(pin.Tex.x - SRCOffset.x);
    clip(SRCOffset.z - pin.Tex.x);
    clip(pin.Tex.y - SRCOffset.y);
    clip(SRCOffset.w - pin.Tex.y);
#endif

	// ������ �� �� srcOffset�� ����..
    pout.Albedo = SRGBToLinear(EffectMap.Sample(Sampler, pin.Tex));

	// ���İ��� ���� ���������ϴ�..
		// �̷��� ������ �ȵ˴ϴ�.. ���� ���� �־��־�� �ϴµ�.
    //pout.WorldPos = lerp(float4(0.f, 0.f, 0.f, 0.f), float4(pin.PosW, 0.f), pout.Albedo.a);

    // ����ũ ���ĸ� �������� ���� ���ݴϴ�.
    pout.Albedo.a = lerp(0.f, pout.Albedo.a, maskColor.a);
    // ���� ���� 0�� ���, �ش� �ȼ��� ó������ �ʽ��ϴ�
		// Plane�� ���� �� alpha�� �ش��ϴ� �κ��� ���������� ������ �� �־����ϴ�.
    clip(pout.Albedo.a - 0.0001f);

    // ����ũ �÷��� ���� �غ��ϴ�.
		// ������ �����ָ� �˴ϴ�.
    pout.Albedo.rgb *= maskColor.rgb;

    pout.WorldPos = float4(pin.PosW, 0.f);

    pout.Emissive = emissive;

    return pout;
}