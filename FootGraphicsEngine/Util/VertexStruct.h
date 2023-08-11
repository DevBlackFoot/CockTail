#pragma once

using namespace DirectX;

namespace GraphicsEngineSpace
{
	namespace Structure
	{
		// 각각의 파티클들을 세는 버퍼.
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

		// OIT에서 사용할 Pixel Data Struct
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

	// 각종 버텍스 구조체 정보.
	namespace VertexStruct
	{
		// SkyBox를 위한 위치 정보만 있는 버텍스
		struct PosVertex
		{
			SimpleMath::Vector3 Pos;
		};

		// 가장 기본적인 색과 위치 정보를 가지고 있는 버텍스 구조체
		struct ColorVertex
		{
			SimpleMath::Vector3 Pos;
			SimpleMath::Vector4 Color;
		};

		// 위치와 텍스쳐 좌표 정보를 가지고 있는 TexVertex
		struct TexVertex
		{
			SimpleMath::Vector3 Pos;
			SimpleMath::Vector2 Tex;
		};

		// PBR static Mesh 정보를 담는 vertex
		struct PBRStaticVertex
		{
			SimpleMath::Vector3 Pos;
			SimpleMath::Vector3 Normal;
			SimpleMath::Vector3 TangentU;
			SimpleMath::Vector2 Tex;
			SimpleMath::Vector4 Color;
		};

		// PBR skinned Mesh 정보를 담는 vertex	
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

		// 파티클에 대한 정보를 담는 버텍스
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

			// 트레일을 위한 prevPos
			SimpleMath::Vector4 prevPos[30];

			SimpleMath::Vector4 latestPrevPos;

			float trailWidth;
			float trailRecordTime;
			SimpleMath::Vector2 TrailScrollSpeed;
		};
	}

}