// PBR Model 관련 사용한 Texture, Skinned 여부에 따라 분기되는 Pixel Shader 들입니다.
#define USE_ALBEDO
#define USE_NORMAL
#define USE_METALLIC
#define USE_ROUGHNESS
#define IsSkinned

#include "../AllShaders/PBRModelPS.hlsl"