#pragma once

using namespace DirectX;

namespace GraphicsEngineSpace
{
	namespace Structure
	{
		// ������ ��ƼŬ���� ���� ����.
		struct CounterBuffer
		{
			CounterBuffer()
				: particleCounter(0)
				, emitterCounter(0)
				, allGroupSync(0)
				, pad(0)
			{
				
			}
			int particleCounter;
			int emitterCounter;
			int allGroupSync;
			int pad;
		};

		// OIT���� ����� Pixel Data Struct
		struct PixelData
		{
			PixelData()
				: color(0xffffffff)
				, depth(1.0f)
				, blendType(0xffffffff)
			{}
			unsigned int color;
			float depth;
			unsigned int blendType;
		};

		struct PixelNode
		{
			PixelNode()
				: data()
				, next(0xffffffff)
			{}
			PixelData data;
			unsigned int next;
		};
	}

	// ���� ���ؽ� ����ü ����.
	namespace VertexStruct
	{
		// SkyBox�� ���� ��ġ ������ �ִ� ���ؽ�
		struct PosVertex
		{
			SimpleMath::Vector3 Pos;
		};

		// ���� �⺻���� ���� ��ġ ������ ������ �ִ� ���ؽ� ����ü
		struct ColorVertex
		{
			SimpleMath::Vector3 Pos;
			SimpleMath::Vector4 Color;
		};

		// ��ġ�� �ؽ��� ��ǥ ������ ������ �ִ� TexVertex
		struct TexVertex
		{
			SimpleMath::Vector3 Pos;
			SimpleMath::Vector2 Tex;
		};

		// PBR static Mesh ������ ��� vertex
		struct PBRStaticVertex
		{
			SimpleMath::Vector3 Pos;
			SimpleMath::Vector3 Normal;
			SimpleMath::Vector3 TangentU;
			SimpleMath::Vector2 Tex;
			SimpleMath::Vector4 Color;
		};

		// PBR skinned Mesh ������ ��� vertex	
		struct PBRSkinnedVertex
		{
			SimpleMath::Vector3 Pos;
			SimpleMath::Vector3 Normal;
			SimpleMath::Vector3 TangentU;
			SimpleMath::Vector2 Tex;
			SimpleMath::Vector4 Color;
			SimpleMath::Vector4 Weight;
			SimpleMath::Vector4 Weights;
			XMUINT4 BoneIndex;
			XMUINT4 BoneIndices;
		};

		// ��ƼŬ�� ���� ������ ��� ���ؽ�
		struct Particle // 14
		{
			Particle()
				: type(0)
				, initialPos(0.f, 0.f, 0.f)
				, initialVel(0.f, 0.f, 0.f, 1.0f)
				, sizeStartSize(1.f, 1.f, 1.f, 1.f)
				, ageLifeTimeRotationGravity(0.f, 0.f, 0.f, 0.f)
				, startColor(1.f, 1.f, 1.f, 1.f)
				, color(1.f, 1.f, 1.f, 1.f)
				, texIndex()
				, initEmitterPos(0.f, 0.f, 0.f, 1.f)
				, trailWidth(1.0f)
				, trailRecordTime(0.0f)
			{
			}
			unsigned int type;

			SimpleMath::Vector3 initialPos;
			SimpleMath::Vector4 initialVel;

			SimpleMath::Vector4 sizeStartSize;

			SimpleMath::Vector4 ageLifeTimeRotationGravity;

			SimpleMath::Vector4 startColor;
			SimpleMath::Vector4 color;

			SimpleMath::Vector2 texIndex[4];

			SimpleMath::Vector4 initEmitterPos;

			// Ʈ������ ���� prevPos
			SimpleMath::Vector4 prevPos[30];

			SimpleMath::Vector4 latestPrevPos;

			float trailWidth;
			float trailRecordTime;
			SimpleMath::Vector2 TrailScrollSpeed;
		};
	}

}