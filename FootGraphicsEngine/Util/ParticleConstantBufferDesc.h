#pragma once

#include "SimpleMath.h"
#include "Define/GraphicsRenderingData.h"

using namespace DirectX;

namespace GraphicsEngineSpace
{
	namespace ConstantBufferDesc
	{
		__declspec(align(16)) struct cbDynamicBuffer
		{
			cbDynamicBuffer()
				: EmissionTime(0.f)
				, dim(0)
				, pad1(0.f)
				, pad2(0.f)
			{

			}
			float EmissionTime;
			int dim;
			float pad1;
			float pad2;
		};

		__declspec(align(16)) struct CommonInfo // 0~8 36
		{
			CommonInfo(ParticleCommonInfo& renderingData)
			{
				memcpy(&DeltaMatrix, &renderingData.deltaMatrix, sizeof(SimpleMath::Matrix));

				memcpy(StartDelay, renderingData.startDelay, sizeof(StartDelay));
				memcpy(StartLifeTime, renderingData.startLifeTime, sizeof(StartLifeTime));

				memcpy(StartSpeed, renderingData.startSpeed, sizeof(StartSpeed));
				memcpy(StartRotation, renderingData.startRotation, sizeof(StartRotation));

				memcpy(StartSize, renderingData.startSize, sizeof(StartSize));

				memcpy(StartColor, renderingData.startColor, sizeof(StartColor));

				memcpy(GravityModifier, renderingData.gravityModifier, sizeof(GravityModifier));
				ParticlePlayTime = renderingData.playTime; // ���� �ð�

				isLooping = static_cast<int>(renderingData.looping);
				SimulationSpeed = renderingData.simulationSpeed;
				Duration = renderingData.duration;
				MaxParticles = renderingData.maxParticles;

			}
			SimpleMath::Matrix TransformMatrix;
			SimpleMath::Matrix DeltaMatrix;

			float	StartDelay[2];				// �� �� �ڿ� ��ƼŬ�� ����� ��.					
			float	StartLifeTime[2];				// �� ��ƼŬ�� ���� �ð�.						

			float	StartSpeed[2];				// ��ƼŬ ������ ���� �ӵ�.
			float	StartRotation[2];			// ��ƼŬ�� ���� ȸ��.							

			float StartSize[4];				// ��ƼŬ�� ���� ũ��.							

			SimpleMath::Vector4 StartColor[2];			// ��ƼŬ�� ���� ����									

			float	GravityModifier[2];			// ��ƼŬ�� �������� �߷�.						
			float ParticlePlayTime;
			float	ObjectID;			// ��ƼŬ�� ������ �ִ� ������Ʈ ID.						

			int	isLooping;					// �ݺ�����.
			float SimulationSpeed;
			float	Duration;				// �� �� ���� ��ƼŬ ��ü�� ����� ��.					
			int		MaxParticles;				// ��ƼŬ �ִ� ��� ������.							

		};

		__declspec(align(16)) struct Emission // 9
		{
			Emission(ParticleEmission& renderingData)
			{
				EmissiveCount = rand() % (abs(renderingData.emissiveCount[1] - renderingData.emissiveCount[0]) + 1) + renderingData.emissiveCount[0];

				EmissiveTime = renderingData.emissiveTime;

				if (renderingData.isRateOverDistance)
					IsRateOverDistance = 1;
				else
					IsRateOverDistance = 0;
			}
			int	EmissiveCount;			// �ѹ��� ��� ���� ��ų��.
			float	EmissiveTime;			// ���� ������� �ɸ��� �ð�.
			int	IsRateOverDistance;			// ���� ������� �ɸ��� �ð�.
			float pad;
		};

		__declspec(align(16)) struct Shape // 10 ~ 13
		{
			Shape(ParticleShape& renderingData)
			{
				memcpy(this, reinterpret_cast<int*>(&renderingData) + 3, sizeof(ParticleShape) - sizeof(int) * 3);

				edgeMode |= 1 << static_cast<unsigned int>(renderingData.edgeMode);
			}

			float Angle;
			float Radius;
			float DonutRadius;
			float Arc;

			SimpleMath::Vector3 Position;
			float RadiusThickness;

			SimpleMath::Vector3 Rotation;
			float pad1;

			SimpleMath::Vector3 Scale;
			float pad2;

			int edgeMode;
			float speed;
			float spread;
			float pad3;
		};

		__declspec(align(16)) struct VelocityOverLifetime // 14 // 4
		{
			VelocityOverLifetime(ParticleVelocityOverLifetime& renderingData)
			{
				memcpy(this, reinterpret_cast<int*>(&renderingData) + 1, sizeof(ParticleVelocityOverLifetime) - sizeof(int));
			}
			SimpleMath::Vector3 Velocity;
			float pad1;
			SimpleMath::Vector3 Orbital;
			unsigned int IsGravity;
			SimpleMath::Vector3 Offset;
			float pad2;
		};

		__declspec(align(16)) struct LimitVelocityOverLifetime // 14 // 4
		{
			LimitVelocityOverLifetime(ParticleLimitVelocityOverLifetime& renderingData)
			{
				memcpy(this, reinterpret_cast<int*>(&renderingData) + 1, sizeof(ParticleLimitVelocityOverLifetime) - sizeof(int));
			}
			SimpleMath::Vector2 PointA;
			SimpleMath::Vector2 PointB;
			SimpleMath::Vector2 PointC;
			SimpleMath::Vector2 PointD;

			float Speed;

			float Dampen;

			SimpleMath::Vector2 pad;
		};

		__declspec(align(16)) struct ForceOverLifeTime // 15 // 4
		{
			ForceOverLifeTime(ParticleForceOverLifeTime& renderingData)
			{
				memcpy(this, reinterpret_cast<int*>(&renderingData) + 1, sizeof(ParticleForceOverLifeTime) - sizeof(int));
			}
			SimpleMath::Vector3 Force;
			float pad;
		};

		__declspec(align(16)) struct ColorOverLifetime //
		{
			ColorOverLifetime(ParticleColorOverLifetime& renderingData)
			{
				memcpy(this, reinterpret_cast<int*>(&renderingData) + 1, sizeof(ParticleColorOverLifetime) - sizeof(int));
			}

			ColorOverLifetime()
			{
				for (int i = 0; i < 8; i++)
				{
					AlphaRatio[i] = SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
					ColorRatio[i] = SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
				}
			}

			SimpleMath::Vector4 AlphaRatio[8];

			SimpleMath::Vector4 ColorRatio[8];
		};

		__declspec(align(16)) struct SizeOverLifetime
		{
			SizeOverLifetime(ParticleSizeOverLifetime& renderingData)
			{
				memcpy(this, reinterpret_cast<int*>(&renderingData) + 1, sizeof(ParticleSizeOverLifetime) - sizeof(int));
			}
			SimpleMath::Vector2 PointA;
			SimpleMath::Vector2 PointB;
			SimpleMath::Vector2 PointC;
			SimpleMath::Vector2 PointD;
		};

		__declspec(align(16)) struct RotationOverLifetime
		{
			RotationOverLifetime(ParticleRotationOverLifetime& renderingData)
			{
				memcpy(this, reinterpret_cast<int*>(&renderingData) + 1, sizeof(ParticleRotationOverLifetime) - sizeof(int));
			}

			float AngularVelocity;

			SimpleMath::Vector3 pad;
		};

		__declspec(align(16)) struct TextureSheetAnimation // 30
		{
			TextureSheetAnimation(ParticleTextureSheetAnimation& renderingData)
			{
				memcpy(this, reinterpret_cast<int*>(&renderingData) + 1, sizeof(ParticleTextureSheetAnimation) - sizeof(int));
			}
			int GridXY[2];
			int TimeMode;
			float pad;
		};

		__declspec(align(16)) struct Noise
		{
			Noise(ParticleNoise& renderingData)
			{
				Strength = renderingData.strength;
				ScrollSpeed = renderingData.scrollSpeed;
				PositionAmount = renderingData.positionAmount;
				RotationAmount = renderingData.rotationAmount;

				SizeAmount = renderingData.sizeAmount;
			}
			float Strength;
			float ScrollSpeed;
			float PositionAmount;
			float RotationAmount;

			float SizeAmount;
			SimpleMath::Vector3 pad;
		};

		__declspec(align(16)) struct Collision
		{
			Collision(ParticleCollision& renderingData)
			{
				memcpy(this, reinterpret_cast<int*>(&renderingData) + 1, sizeof(ParticleCollision) - sizeof(int));

				if (renderingData.useModule)
				{
					for (int i = 0; i < PlaneCount; i++)
					{
						PlaneNormalVec[i].Normalize();
					}
				}

			}
			int PlaneCount;
			float Bounce;
			float LifeTimeLoss;
			float pad;

			SimpleMath::Vector4 PlanePosition[8];
			SimpleMath::Vector4 PlaneNormalVec[8];
		};

		__declspec(align(16)) struct Trails
		{
			Trails(ParticleTrails& renderingData)
			{
				Ratio = renderingData.ratio;
				LifeTime = renderingData.lifeTime;
				MinimumVertexDistance = renderingData.minimumVertexDistance;
				WidthOverTrail[0] = renderingData.widthOverTrail[0];
				WidthOverTrail[1] = renderingData.widthOverTrail[1];
				TrailVertexCount = renderingData.trailVertexCount;

				TrailsFlag = 0;
				if (renderingData.worldSpace)
					TrailsFlag |= 1 << 0;
				if (renderingData.dieWithParticle)
					TrailsFlag |= 1 << 1;
				if (renderingData.sizeAffectsWidth) // o
					TrailsFlag |= 1 << 2;
				if (renderingData.sizeAffectsLifeTime) // o
					TrailsFlag |= 1 << 3;
				if (renderingData.inheritParticleColor) // o
					TrailsFlag |= 1 << 4;
				if (renderingData.generateLightingData)
					TrailsFlag |= 1 << 5;

				if (renderingData.textureMode == ParticleTrails::TextureMode::STRETCH)
					TrailsFlag |= 1 << 6;
				if (renderingData.textureMode == ParticleTrails::TextureMode::TILE)
					TrailsFlag |= 1 << 7;
				if (renderingData.textureMode == ParticleTrails::TextureMode::DISTRIBUTE_PER_SEGMENT)
					TrailsFlag |= 1 << 8;
				if (renderingData.textureMode == ParticleTrails::TextureMode::REPEAT_PER_SEGMENT)
					TrailsFlag |= 1 << 9;

				for (int i = 0; i < 8; i++)
				{
					AlphaRatioLifetime[i] = renderingData.alphaRatioLifetime[i];
					ColorRatioLifetime[i] = renderingData.colorRatioLifetime[i];
					AlphaRatioTrail[i] = renderingData.alphaRatioTrail[i];
					ColorRatioTrail[i] = renderingData.colorRatioTrail[i];
				}

				ScrollXSpeed.x = renderingData.scrollXSpeed[0];
				ScrollXSpeed.y = renderingData.scrollXSpeed[1];
				ScrollYSpeed.x = renderingData.scrollYSpeed[0];
				ScrollYSpeed.y = renderingData.scrollYSpeed[1];

				Condition = 0;
				Condition |= 1 << static_cast<int>(renderingData.condition);
				RecordTime = renderingData.recordTime;
			}
			float Ratio; // o
			float LifeTime; // o
			float MinimumVertexDistance; // o
			int pad1;

			int TrailsFlag;
			int TrailVertexCount;
			float WidthOverTrail[2]; // o

			SimpleMath::Vector4 AlphaRatioLifetime[8]; // o
			SimpleMath::Vector4 ColorRatioLifetime[8]; // o
			SimpleMath::Vector4 AlphaRatioTrail[8]; // o
			SimpleMath::Vector4 ColorRatioTrail[8]; // o

			SimpleMath::Vector2 ScrollXSpeed;
			SimpleMath::Vector2 ScrollYSpeed;

			unsigned int Condition;
			float RecordTime;
			SimpleMath::Vector2 pad2;
		};

		__declspec(align(16)) struct cbParticleRenderer
		{
			cbParticleRenderer(ParticleRenderer& renderingData)

			{
				SpeedScale = renderingData.speedScale;

				LengthScale = renderingData.lengthScale;

				BlendType = static_cast<unsigned int>(renderingData.blendState);

				RenderAlignment = 0;

				RenderAlignment |= 1 << static_cast<unsigned int>(renderingData.renderAlignment);
			}
			float SpeedScale;
			float LengthScale;
			unsigned int BlendType;
			unsigned int RenderAlignment;
		};

		__declspec(align(16)) struct cbPerObjectParticle
		{
			cbPerObjectParticle(RenderingDataParticle& renderingData);

			// ����Ʈ �����͸� ������?
			cbPerObjectParticle(const std::shared_ptr<RenderingDataParticle>& renderingData);

			CommonInfo commonInfo;

			Emission emission;

			Shape shape;

			VelocityOverLifetime velocityOverLifetime;

			LimitVelocityOverLifetime limitVelocityOverLifetime;

			ForceOverLifeTime forceOverLifetime;

			ColorOverLifetime colorOverLifetime;

			SizeOverLifetime sizeOverLifetime;

			RotationOverLifetime rotationOverLifetime;

			Noise noise;

			Collision collision;

			TextureSheetAnimation textureSheetAnimation;

			Trails trails;

			cbParticleRenderer renderer;

			unsigned int flag;
			float pad[3];
		};


		inline cbPerObjectParticle::cbPerObjectParticle(RenderingDataParticle& renderingData)
			: commonInfo(renderingData.commonInfo)
			, emission(renderingData.emission)
			, shape(renderingData.shape)
			, velocityOverLifetime(renderingData.velocityOverLifetime)
			, limitVelocityOverLifetime(renderingData.limitVelocityOverLifetime)
			, forceOverLifetime(renderingData.forceOverLifetime)
			, colorOverLifetime(renderingData.colorOverLifetime)
			, sizeOverLifetime(renderingData.sizeOverLifetime)
			, rotationOverLifetime(renderingData.rotationOverLifetime)
			, textureSheetAnimation(renderingData.textureSheetAnimaition)
			, trails(renderingData.trails)
			, renderer(renderingData.renderer)
			, noise(renderingData.noise)
			, collision(renderingData.collision)
			//_renderer()
		{

			flag = renderingData.GetFlag();

			memcpy(&commonInfo.ObjectID, &renderingData.objectID, sizeof(UINT));
			{
				SimpleMath::Matrix world = renderingData.commonInfo.transformMatrix; // ��Ʈ ��Ʈ����

				// --------------------------------- Set_QuatAndScale ----------------------------------------------
				// ------------------------- ȸ�� Ȥ�� �����Ͽ� ������ �޴� �ɼǵ�. -----------------------------
				SimpleMath::Vector3 s;
				SimpleMath::Quaternion r;
				SimpleMath::Vector3 t;

				world.Decompose(s, r, t);

				world.m[3][0] = 0; world.m[3][1] = 0; world.m[3][2] = 0;

				commonInfo.StartSize[0] = commonInfo.StartSize[0] * s.x;
				commonInfo.StartSize[1] = commonInfo.StartSize[1] * s.y;
				commonInfo.StartSize[2] = commonInfo.StartSize[2] * s.x;
				commonInfo.StartSize[3] = commonInfo.StartSize[3] * s.y;

				if (renderingData.velocityOverLifetime.space == ParticleSpace::LOCAL)
					velocityOverLifetime.Velocity = SimpleMath::Vector3::Transform(velocityOverLifetime.Velocity, world);

				if (renderingData.forceOverLifetime.space == ParticleSpace::LOCAL)
					forceOverLifetime.Force = SimpleMath::Vector3::Transform(forceOverLifetime.Force, world);

				SimpleMath::Matrix shapeTM
					= SimpleMath::Matrix::CreateScale(shape.Scale)
					* SimpleMath::Matrix::CreateFromYawPitchRoll(shape.Rotation.z, shape.Rotation.x, shape.Rotation.y)
					* SimpleMath::Matrix::CreateTranslation(shape.Position);

				world = shapeTM * renderingData.commonInfo.transformMatrix;
				memcpy(&commonInfo.TransformMatrix, &world, sizeof(SimpleMath::Matrix));

			}
		}

		inline cbPerObjectParticle::cbPerObjectParticle(const std::shared_ptr<RenderingDataParticle>& renderingData)
			: commonInfo(renderingData->commonInfo)
			, emission(renderingData->emission)
			, shape(renderingData->shape)
			, velocityOverLifetime(renderingData->velocityOverLifetime)
			, limitVelocityOverLifetime(renderingData->limitVelocityOverLifetime)
			, forceOverLifetime(renderingData->forceOverLifetime)
			, colorOverLifetime(renderingData->colorOverLifetime)
			, sizeOverLifetime(renderingData->sizeOverLifetime)
			, rotationOverLifetime(renderingData->rotationOverLifetime)
			, textureSheetAnimation(renderingData->textureSheetAnimaition)
			, trails(renderingData->trails)
			, renderer(renderingData->renderer)
			, noise(renderingData->noise)
			, collision(renderingData->collision)
		{
			flag = renderingData->GetFlag();

			memcpy(&commonInfo.ObjectID, &renderingData->objectID, sizeof(UINT));
			{
				SimpleMath::Matrix world = renderingData->commonInfo.transformMatrix; // ��Ʈ ��Ʈ����

				// --------------------------------- Set_QuatAndScale ----------------------------------------------
				// ------------------------- ȸ�� Ȥ�� �����Ͽ� ������ �޴� �ɼǵ�. -----------------------------
				SimpleMath::Vector3 s;
				SimpleMath::Quaternion r;
				SimpleMath::Vector3 t;

				world.Decompose(s, r, t);

				world.m[3][0] = 0; world.m[3][1] = 0; world.m[3][2] = 0;

				commonInfo.StartSize[0] = commonInfo.StartSize[0] * s.x;
				commonInfo.StartSize[1] = commonInfo.StartSize[1] * s.y;
				commonInfo.StartSize[2] = commonInfo.StartSize[2] * s.x;
				commonInfo.StartSize[3] = commonInfo.StartSize[3] * s.y;

				if (renderingData->velocityOverLifetime.space == ParticleSpace::LOCAL)
					velocityOverLifetime.Velocity = SimpleMath::Vector3::Transform(velocityOverLifetime.Velocity, world);

				if (renderingData->forceOverLifetime.space == ParticleSpace::LOCAL)
					forceOverLifetime.Force = SimpleMath::Vector3::Transform(forceOverLifetime.Force, world);

				SimpleMath::Matrix shapeTM
					= SimpleMath::Matrix::CreateScale(shape.Scale)
					* SimpleMath::Matrix::CreateFromYawPitchRoll(shape.Rotation.z, shape.Rotation.x, shape.Rotation.y)
					* SimpleMath::Matrix::CreateTranslation(shape.Position);

				world = shapeTM * renderingData->commonInfo.transformMatrix;
				memcpy(&commonInfo.TransformMatrix, &world, sizeof(SimpleMath::Matrix));
			}
		}
	}
}