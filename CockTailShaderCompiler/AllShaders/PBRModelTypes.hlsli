// PBRModel ���� �ʿ��� �⺻���� Ÿ���� ��Ÿ���ϴ�.
// ���ؽ� ���̴� ���� �ȼ� ���̴��� �˾ƾ��ϴ� ����ü�� �����մϴ�.
// == vout�� ����ϴ�.

struct VS_Output
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
    float2 Tex : TEXCOORD;
    float4 Color : COLOR0;

    float metallic : METALLIC;
    float roughness : ROUGHNESS;
};

// ���� �Ƿ翧�� ���� ���ο� ����ü�� �ϳ� ����� ���ϴ�.
struct VS_Monster_Output
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITIONT;
    float3 PosL : POSITION;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
    float2 Tex : TEXCOORD;
    float4 Color : COLOR0;

    float metallic : METALLIC;
    float roughness : ROUGHNESS;
};

// OIT�� ���� Pixel �������� ����غ��ô�..
	// ref by. �ż���
#define MAX_SORTED_PIXELS 16

// Pixel �ϳ��ϳ��� ����
struct PixelData
{
    uint Color;     // Color ������ 1����Ʈ(8bit)�� ����ȭ
    float Depth;
    uint BlendType;     // 0 Default 1 Additive
};

struct PixelNode
{
    PixelData Data;
    uint Next;
};
