/// ��κ��� ���̴����� ������� Gamma ���� �Լ��� ����� �����մϴ�.

// Linear Space ���� Gamma Space��
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

// float4 color ������ Uint �ϳ��� ����
uint PackColorFromFloat4(float4 color)
{
    uint4 colorUInt4 = (uint4) (color * 255.f);
    // ��Ʈ Shift�� �̿��ؼ� 1����Ʈ(8bit)�� r,g,b,a���� �����Ѵ�. => 0xAABBGGRR �� ���� ������ ������ �ȴ�.
    return colorUInt4.r | (colorUInt4.g << 8) | (colorUInt4.b << 16) | (colorUInt4.a << 24);
}

// Hex Code�� ������ RGB ���� Float4�� ���·� �������ݴϴ�.
float4 UnpackColorFromUint(uint color)
{
    // ��Ʈ �����ڸ� �̿��� ������ 1����Ʈ���� �̾Ƽ� �������ش�.
		// 8��Ʈ �� shift�ϸ鼭 ���Ĺ��� 1����Ʈ�� ���� r, g, b, a�� ����� �ص�,
		// ��Ʈ ������ ���� 1�� ��Ʈ�� �̾Ƴ���.
		// 0x000000FF => (���Ĺ� 8��Ʈ�� ���) 0b11111111�̱� ������ 1�� �����ϴ� ���� �̾Ƴ� �� �ִ�.
    uint4 colorUint4 = uint4(color, color >> 8, color >> 16, color >> 24) & (0x000000FF);
    return (float4) colorUint4 / 255.0f;
}