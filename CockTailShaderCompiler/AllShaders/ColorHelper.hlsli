/// 대부분의 셰이더에서 사용해줄 Gamma 관련 함수를 여기로 이전합니다.

// Linear Space 값을 Gamma Space로
float3 LinearToSRGB(float3 color)
{
    float3 sRGBLo = color * 12.92f;
    float3 sRGBHi = (pow(color, 1 / 2.4f) * 1.055f) - 0.055f;
    float3 sRGB = (color <= 0.0031308f) ? sRGBLo : sRGBHi;

    return sRGB;
}

float4 LinearToSRGB(float4 color)
{
    return float4(LinearToSRGB(color.rgb), color.a);
}

float3 SRGBToLinear(float3 color)
{
    float3 linearRGBLo = color / 12.92f;
    float3 linearRGBHi = pow((color + 0.055f) / 1.055f, 2.4f);
    float3 linearRGB = (color <= 0.04045f) ? linearRGBLo : linearRGBHi;
    return linearRGB;
}

float4 SRGBToLinear(float4 color)
{
    return float4(SRGBToLinear(color.rgb), color.a);
}

// float4 color 정보를 Uint 하나로 저장
uint PackColorFromFloat4(float4 color)
{
    uint4 colorUInt4 = (uint4) (color * 255.f);
    // 비트 Shift를 이용해서 1바이트(8bit)씩 r,g,b,a값을 저장한다. => 0xAABBGGRR 과 같은 식으로 저장이 된다.
    return colorUInt4.r | (colorUInt4.g << 8) | (colorUInt4.b << 16) | (colorUInt4.a << 24);
}

// Hex Code로 저장한 RGB 값은 Float4의 형태로 변경해줍니다.
float4 UnpackColorFromUint(uint color)
{
    // 비트 연산자를 이용해 마지막 1바이트씩을 뽑아서 저장해준다.
		// 8비트 씩 shift하면서 최후미의 1바이트를 각각 r, g, b, a로 만들어 준뒤,
		// 비트 연산을 통해 1인 비트를 뽑아낸다.
		// 0x000000FF => (최후미 8비트의 경우) 0b11111111이기 때문에 1에 대응하는 값을 뽑아낼 수 있다.
    uint4 colorUint4 = uint4(color, color >> 8, color >> 16, color >> 24) & (0x000000FF);
    return (float4) colorUint4 / 255.0f;
}