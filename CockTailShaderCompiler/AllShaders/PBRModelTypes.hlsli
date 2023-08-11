// PBRModel 에서 필요한 기본적인 타입을 나타냅니다.
// 버텍스 셰이더 이후 픽셀 셰이더가 알아야하는 구조체를 지정합니다.
// == vout을 만듭니다.

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

// 몬스터 실루엣을 위한 새로운 구조체를 하나 만들어 봅니다.
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

// OIT를 위한 Pixel 정보들을 기록해봅시다..
	// ref by. 신성현
#define MAX_SORTED_PIXELS 16

// Pixel 하나하나의 정보
struct PixelData
{
    uint Color;     // Color 정보를 1바이트(8bit)씩 최적화
    float Depth;
    uint BlendType;     // 0 Default 1 Additive
};

struct PixelNode
{
    PixelData Data;
    uint Next;
};
