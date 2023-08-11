#pragma once
//#include "Utils/d3d11Utils.h"
#include <string>
#include "SimpleMath.h"
#include "GraphicsTypedef.h"
#include "Resources/Resource.h"

#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/vector.hpp>

using namespace DirectX;

// Mesh 불러오기용 해당 메시들은 리소스매니저에서 제일 먼저 초기화 해줘야한다.
constexpr size_t CUBE_MESH = 0;
constexpr size_t SPHERE_MESH = 1;
constexpr size_t AXIS_MESH = 2;
constexpr size_t GRID_MESH = 3;
constexpr size_t SCREEN_MESH = 4;
constexpr size_t DEBUG_SCREEN_MESH = 5;
constexpr size_t WIRECUBE_MESH = 6;
constexpr size_t SKYBOX_MESH = 7;
constexpr size_t PLANE_MESH = 8;

// 함수 추가용 enum class
enum class TextureMapType
{
	ALBEDO,
	NORMAL,
	METALLIC,
	ROUGHNESS,
	AO,
	EMISSIVE,
	CUBE
};

struct ObjectInfo
{
	uint64 objectID;

	DirectX::SimpleMath::Matrix worldTM;

	DirectX::SimpleMath::Vector3 worldPos;

	DirectX::SimpleMath::Matrix finalBoneListMatrix[96];

	uint64 materialName;
};

// 성현이의 파티클 구조체를 위한 Flag 클래스
namespace BasicParticle
{
	enum class Flags
	{
		NONE = 0,
		PARTICLE_SYSTEM_COMMON_INFO = 1 << 0, // ParticleSystemCommonInfo Module
		EMISSION = 1 << 1, // Emission Module
		SHAPE = 1 << 2, // Shape Module
		VELOCITY_OVER_LIFETIME = 1 << 3, // Velocity_Over_Lifetime Module
		FORCE_OVER_LIFETIME = 1 << 4, // Force_over_Lifetime Module
		COLOR_OVER_LIFETIME = 1 << 5, // Color_over_Lifetime Module
		SIZE_OVER_LIFETIME = 1 << 6, // Size_over_Lifetime Module
		ROTATION_OVER_LIFETIME = 1 << 7, // Rotation_over_Lifetime Module
		NOISE = 1 << 8, // Noise Module
		COLLISION = 1 << 9, // Collision Module
		TEXTURE_SHEET_ANIMATION = 1 << 10, // Texture_Sheet_Animation Module
		TRAILS = 1 << 11, // Trails Module  
		RENDERER = 1 << 12, // Renderer Module
	};

}

/// 성현이의 파티클 구조체를 가져오기 위한 RenderingData_Paticle을 가져와 줍니다.
#pragma region Particle Rendering
namespace GraphicsEngineSpace
{
	/**
	 * \brief 성현이의 파티클 데이터 0.2버전을 가져옵니다.
	 */
	enum class ParticleSpace
	{
		LOCAL,
		WORLD
	};

	/**
	 * \brief CommonInfo는 모든 파티클이 사용하는 가장 기본적인 모듈입니다.
	 */
	struct ParticleCommonInfo
	{
		enum class OptionParticle
		{
			CONSTANT,
			CURVE,
			RANDOM_BETWEEN_TWO_CONSTANT,
			RANDOM_BETWEEN_TWO_CURVE
		};
		ParticleCommonInfo()
			: useModule(true)
			, firstRun(true)
			, duration(5.0f)
			, looping(true)
			, startDelayOption(OptionParticle::CONSTANT)
			, startDelay{ 0.f, 0.f }
			, startLifeTimeOption(OptionParticle::CONSTANT)
			, startLifeTime{ 5.f, 5.f }
			, startSpeedOption(OptionParticle::CONSTANT)
			, startSpeed{ 4.f, 4.f }
			, startSizeOption(OptionParticle::CONSTANT)
			, startSize{ SimpleMath::Vector2(1.0f, 1.0f), SimpleMath::Vector2(1.0f, 1.0f) }
			, startRotationOption(OptionParticle::CONSTANT)
			, startRotation{ 0.f, 0.f }
			, startColorOption(OptionParticle::CONSTANT)
			, startColor{ SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f),SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f) }
			, gravityModifierOption(OptionParticle::CONSTANT)
			, gravityModifier{1.f, 1.f}
			, maxParticles(1000)
			, transformMatrix(SimpleMath::Matrix::Identity)
			, simulationSpeed(1.0f)
			, space(ParticleSpace::WORLD)
			, deltaMatrix(SimpleMath::Matrix::Identity)
		{
		}

		bool operator==(const ParticleCommonInfo& other) const
		{
			if (
				useModule != other.useModule ||
				firstRun != other.firstRun ||
				duration != other.duration ||
				looping != other.looping ||
				startDelayOption != other.startDelayOption ||
				startDelay[0] != other.startDelay[0] ||
				startDelay[1] != other.startDelay[1] ||
				startLifeTimeOption != other.startLifeTimeOption ||
				startLifeTime[0] != other.startLifeTime[0] ||
				startLifeTime[1] != other.startLifeTime[1] ||
				startSpeedOption != other.startSpeedOption ||
				startSpeed[0] != other.startSpeed[0] ||
				startSpeed[1] != other.startSpeed[1] ||
				startSizeOption != other.startSizeOption ||
				startSize[0] != other.startSize[0] ||
				startSize[1] != other.startSize[1] ||
				startRotationOption != other.startRotationOption ||
				startRotation[0] != other.startRotation[0] ||
				startRotation[1] != other.startRotation[1] ||
				startColorOption != other.startColorOption ||
				startColor[0] != other.startColor[0] ||
				startColor[1] != other.startColor[1] ||
				gravityModifierOption != other.gravityModifierOption ||
				gravityModifier[0] != other.gravityModifier[0] ||
				gravityModifier[1] != other.gravityModifier[1] ||
				maxParticles != other.maxParticles ||
				transformMatrix != other.transformMatrix ||
				simulationSpeed != other.simulationSpeed ||
				space != other.space ||
				deltaMatrix != other.deltaMatrix
				)
				return false;
			else
				return true;
		}

		// 멤버변수 목록
		bool useModule;

		bool firstRun;			// 시작?
		float duration;			// 몇 초 동안 파티클 객체가 재생될 지.
		bool looping;			// 반복 여부

		OptionParticle startDelayOption;
		float startDelay[2];		// 몇 초 뒤 파티클 재생

		OptionParticle startLifeTimeOption;
		float startLifeTime[2];		// 한 파티클의 생존시간

		OptionParticle startSpeedOption;
		float startSpeed[2];		// 파티클 생성시 시작속도

		OptionParticle startSizeOption;
		SimpleMath::Vector2 startSize[2];		// 파티클의 시작 크기

		OptionParticle startRotationOption;
		float startRotation[2];				// 파티클의 시작 회전.

		OptionParticle startColorOption;
		SimpleMath::Vector4 startColor[2];		// 파티클의 시작색상

		OptionParticle gravityModifierOption;
		float gravityModifier[2];				// 파티클에 가해지는 중력

		int maxParticles;				// 파티클 최대 출력 사이즈.

		SimpleMath::Matrix transformMatrix;		// 파티클의 생성위치 및 각을 조정

		float playTime;					// play 시작 후 흐른 시간.

		float simulationSpeed;

		ParticleSpace space;		// 생성 된 파티클이 방출기(emitter)의 영향을 받는지.	=> world면 영향을 받지 않고, local이면 받습니다.

		SimpleMath::Matrix deltaMatrix;			// 파티클의 생성 위치 및 각을 조정

	protected:
		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& useModule;

			ar& firstRun;

			ar& duration;

			ar& looping;

			ar& startDelayOption;

			ar& startDelay[0];
			ar& startDelay[1];

			ar& startLifeTimeOption;

			ar& startLifeTime[0];
			ar& startLifeTime[1];

			ar& startSpeedOption;

			ar& startSpeed[0];
			ar& startSpeed[1];

			ar& startSizeOption;

			ar& startSize[0];
			ar& startSize[1];

			ar& startRotationOption;

			ar& startRotation[0];
			ar& startRotation[1];

			ar& startColorOption;

			ar& startColor[0];
			ar& startColor[1];

			ar& gravityModifierOption;

			ar& gravityModifier[0];
			ar& gravityModifier[1];

			ar& maxParticles;

			ar& transformMatrix;

			ar& playTime;

			ar& simulationSpeed;

			ar& space;

			ar& deltaMatrix;
		}

	};

	/**
	 * \brief 방출 관련 구조체 입니다.
	 */
	struct ParticleEmission
	{
		ParticleEmission()
			: useModule(true)
			, emissiveCount{ 1,1 }
			, emissiveTime(0.1f)
			, isRateOverDistance(false)
			, emissiveTimer(0)
			, emissionOption(ParticleCommonInfo::OptionParticle::CONSTANT)
		{
		}
		bool operator==(const ParticleEmission& other) const
		{
			if (memcmp(this, &other, sizeof(ParticleEmission)) == 0)
				return true;
			else
				return false;
		}
		bool useModule;

		int emissiveCount[2];			// 한번에 몇개를 방출 시킬지.

		float emissiveTime;			// 다음 방출까지 걸리는 시간.

		bool isRateOverDistance;			// 움직일 때 파티클 방출.

		float emissiveTimer;			// 방출 타이머.

		ParticleCommonInfo::OptionParticle emissionOption;

	protected:
		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& useModule;
			ar& emissiveCount[0];
			ar& emissiveCount[1];

			ar& emissiveTime;
			ar& isRateOverDistance;
			ar& emissiveTimer;
			ar& emissionOption;
		}
	};

	/**
	 * \brief 파티클의 모양에 관련된 구조체 입니다.
	 */
	struct ParticleShape
	{
		enum class Shape
		{
			SPHERE,
			HEMISPHERE,
			CONE,
			DONUT,
			BOX,
			CIRCLE,
			RECTANGLE,
			EDGE

		};
		enum class EdgeMode
		{
			RANDOM,
			LOOP,
			PING_PONG,
			BURST_SPREAD
		};

		ParticleShape()
			: useModule(true)
			, shape(Shape::CONE)
			, edgeMode(EdgeMode::RANDOM)
			, angle(3.141592f / 6.0f)
			, radius(1.0f)
			, donutRadius(0)
			, arc(3.141592f * 2.0f)
			, position(0, 0, 0)
			, radiusThickness(1.0f)
			, rotation(0, 0, 0)
			, pad1(0.f)
			, scale(1, 1, 1)
			, pad2(0.f)
			, edgeModeFlags(0)
			, speed(1.0f)
			, spread(0.0f)
			, pad3(0.f)
		{
		}
		bool operator==(const ParticleShape& other) const
		{
			if (memcmp(this, &other, sizeof(ParticleShape)) == 0)
				return true;
			else
				return false;
		}
		
		bool useModule;

		Shape shape;

		EdgeMode edgeMode;

		float angle;
		float radius;
		float donutRadius;
		float arc;

		SimpleMath::Vector3 position;
		float radiusThickness;

		SimpleMath::Vector3 rotation;
		float pad1;

		SimpleMath::Vector3 scale;
		float pad2;

		unsigned int edgeModeFlags;
		float speed;
		float spread;
		float pad3;


	protected:
		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& useModule;

			ar& shape;

			ar& edgeMode;

			ar& angle;
			ar& radius;
			ar& donutRadius;
			ar& arc;

			ar& position;
			ar& radiusThickness;

			ar& rotation;
			ar& pad1;

			ar& scale;
			ar& pad2;

			ar& edgeModeFlags;
			ar& speed;
			ar& spread;
			ar& pad3;
		}

	};

	/**
	 * \brief 라이트 타임 동안의 속도 관련 구조체입니다.
	 */
	struct ParticleVelocityOverLifetime
	{
		ParticleVelocityOverLifetime()
			: useModule(false)
			, linearVelocity(0.f, 0.f, 0.f)
			, space(ParticleSpace::LOCAL)
			, orbital(0.f, 0.f, 0.f)
			, isGravity(1)
			, offset(0.f, 0.f, 0.f)
			, pad(0.f)
		{
		}
		bool operator==(const ParticleVelocityOverLifetime& other) const
		{
			if (memcmp(this, &other, sizeof(ParticleVelocityOverLifetime)) == 0)
				return true;
			else
				return false;
		}
		bool useModule;

		SimpleMath::Vector3 linearVelocity;
		ParticleSpace space;

		SimpleMath::Vector3 orbital;
		unsigned int isGravity;

		SimpleMath::Vector3 offset;
		float pad;


	protected:
		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& useModule;

			ar& linearVelocity;

			ar& space;

			ar& orbital;

			ar& isGravity;

			ar& offset;

			ar& pad;
		}
	};

	/**
	 * \brief 각 지점에서의 Limit Velocity..?
	 */
	struct ParticleLimitVelocityOverLifetime
	{
		ParticleLimitVelocityOverLifetime()
			: useModule(false)
			, pointA(0.f, 0.f)
			, pointB(0.3333f, 0.3333f)
			, pointC(0.6666f, 0.6666f)
			, pointD(1.0f, 1.0f)
			, speed(1.0f)
			, dampen(1.0f)
			, pad(0.f, 0.f)
		{
		}
		bool operator==(const ParticleLimitVelocityOverLifetime& other) const
		{
			if (memcmp(this, &other, sizeof(ParticleLimitVelocityOverLifetime)) == 0)
				return true;
			else
				return false;
		}
		bool useModule;

		SimpleMath::Vector2 pointA;

		SimpleMath::Vector2 pointB;

		SimpleMath::Vector2 pointC;

		SimpleMath::Vector2 pointD;

		float speed;
		float dampen;
		SimpleMath::Vector2 pad;
	protected:
		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& useModule;

			ar& pointA;

			ar& pointB;

			ar& pointC;

			ar& pointD;

			ar& speed;

			ar& dampen;

			ar& pad;
		}
	};

	/**
	 * \brief 파티클에 가해지는 힘
	 */
	struct ParticleForceOverLifeTime
	{
		ParticleForceOverLifeTime()
			: useModule(false)
			, force(0.f, 0.f, 0.f)
			, space(ParticleSpace::LOCAL)
		{
		}
		bool operator==(const ParticleForceOverLifeTime& other) const
		{
			if (memcmp(this, &other, sizeof(ParticleVelocityOverLifetime)) == 0)
				return true;
			else
				return false;
		}
		bool useModule;

		SimpleMath::Vector3 force;

		ParticleSpace space;

	protected:
		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& useModule;

			ar& force;

			ar& space;
		}
	};

	/**
	 * \brief 파티클의 컬러
	 */
	struct ParticleColorOverLifetime
	{
		/**
			@brief Cpu에서 보간 값을 미리 정해서 던지지 않는 이유는 파티클마다 라이프타임이 다르기 때문이다.
			gpu 안에서 계산할 수 있는 방법을 생각해보자.
		**/
		ParticleColorOverLifetime()
			: useModule(false)
		{
			for (int i = 0; i < 8; i++)
			{
				alphaRatio[i] = SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
				colorRatio[i] = SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
			}

		}
		bool operator==(const ParticleColorOverLifetime& other) const
		{
			for (int i = 0; i < 8; i++)
			{
				if (alphaRatio[i] != other.alphaRatio[i])
					return false;

				if (alphaRatio[i] != other.alphaRatio[i])
					return false;

			}
			if (useModule != other.useModule)
				return false;
			return true;
		}
		bool useModule;

		SimpleMath::Vector4 alphaRatio[8];

		SimpleMath::Vector4 colorRatio[8];

	protected:
		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& useModule;

			ar& alphaRatio[0];
			ar& alphaRatio[1];
			ar& alphaRatio[2];
			ar& alphaRatio[3];
			ar& alphaRatio[4];
			ar& alphaRatio[5];
			ar& alphaRatio[6];
			ar& alphaRatio[7];


			ar& colorRatio[0];
			ar& colorRatio[1];
			ar& colorRatio[2];
			ar& colorRatio[3];
			ar& colorRatio[4];
			ar& colorRatio[5];
			ar& colorRatio[6];
			ar& colorRatio[7];

		}
	};

	/**
	 * \brief 파티클의 사이즈
	 */
	struct ParticleSizeOverLifetime
	{
		ParticleSizeOverLifetime()
			: useModule(false)
			, pointA(0.f, 0.f)
			, pointB(0.333f, 0.333f)
			, pointC(0.666f, 0.666f)
			, pointD(1.0f, 1.0f)
		{
		}
		bool operator==(const ParticleSizeOverLifetime& other) const
		{
			if (memcmp(this, &other, sizeof(ParticleSizeOverLifetime)) == 0)
				return true;
			else
				return false;
		}
		bool useModule;

		SimpleMath::Vector2 pointA;
		SimpleMath::Vector2 pointB;
		SimpleMath::Vector2 pointC;
		SimpleMath::Vector2 pointD;

	protected:
		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& useModule;

			ar& pointA;

			ar& pointB;

			ar& pointC;

			ar& pointD;
		}
	};

	/**
	 * \brief 파티클 로테이션.
	 */
	struct ParticleRotationOverLifetime
	{
		ParticleRotationOverLifetime()
			: useModule(false)
			, angularVelocity(0)
			, pad(0.f, 0.f, 0.f)
		{
		}
		bool operator==(const ParticleRotationOverLifetime& other) const
		{
			if (memcmp(this, &other, sizeof(ParticleRotationOverLifetime)) == 0)
				return true;
			else
				return false;
		}
		bool useModule;

		float angularVelocity;

		SimpleMath::Vector3 pad;

		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& useModule;

			ar& angularVelocity;

			ar& pad;
		}
	};

	/**
	 * \brief 텍스쳐 시트의 애니메이션..
	 */
	struct ParticleTextureSheetAnimation
	{
		ParticleTextureSheetAnimation()
			: useModule(false)
			, gridXY{ 1, 1 }
			, timeMode(0)
			, pad(0.f)
		{
		}
		bool operator==(const ParticleTextureSheetAnimation& other) const
		{
			if (memcmp(this, &other, sizeof(ParticleTextureSheetAnimation)) == 0)
				return true;
			else
				return false;
		}
		bool useModule;

		int gridXY[2];

		int timeMode;

		float pad;

	protected:
		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& useModule;

			ar& gridXY[0];
			ar& gridXY[1];

			ar& timeMode;

			ar& pad;
		}
	};

	/**
	 * \brief 파티클의 노이즈..
	 */
	struct ParticleNoise
	{
		enum class Quality
		{
			LOW,
			MIDDLE,
			HIGH
		};

		ParticleNoise()
			: useModule(false)
			, strength(1.0f)
			, frequency(0.5f)
			, scrollSpeed(0)
			, damping(true)
			, octaves(1)
			, octaveMultiplier(0.5)
			, octaveScale(2)
			, quality(Quality::HIGH)
			, positionAmount(1)
			, rotationAmount(0)
			, sizeAmount(0)
		{}
		bool operator==(const ParticleNoise& other) const
		{
			if (memcmp(this, &other, sizeof(ParticleNoise)) == 0)
				return true;
			else
				return false;
		}
		bool useModule;

		float strength;
		float frequency;
		float scrollSpeed;
		bool damping;
		int octaves;
		float octaveMultiplier;
		float octaveScale;
		Quality quality;

		float positionAmount;
		float rotationAmount;
		float sizeAmount;

	protected:
		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& useModule;
			ar& strength;
			ar& frequency;
			ar& scrollSpeed;
			ar& damping;
			ar& octaves;
			ar& octaveMultiplier;
			ar& octaveScale;
			ar& quality;

			ar& positionAmount;
			ar& rotationAmount;
			ar& sizeAmount;
		}
	};

	/**
	 * \brief 파티클 충돌 관련 구조체
	 */
	struct ParticleCollision
	{
		ParticleCollision()
			: useModule(false)
			, planeCount(0)
			, bounce(1.f)
			, lifeTimeLoss(0.f)
			, pad(0.f)
		{

			for (int i = 0; i < 8; i++)
			{
				planePos[i] = SimpleMath::Vector4();
				planeNormalVec[i] = SimpleMath::Vector4(0, 1, 0, 0);
			}
		}
		bool operator==(const ParticleCollision& other) const
		{
			if (memcmp(this, &other, sizeof(ParticleCollision)) == 0)
				return true;
			else
				return false;
		}

		bool useModule;

		int planeCount; // 최대 갯수를 정해놓자. 8개? 이유는 팔면체를 생각했기 때문.

		float bounce;

		float lifeTimeLoss;

		float pad;

		SimpleMath::Vector4 planePos[8]; // 어째서 Vector4인가! 16바이트 정렬을 편하게 하기 위함.

		SimpleMath::Vector4 planeNormalVec[8];


	protected:
		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& useModule;

			ar& planeCount;

			ar& bounce;

			ar& lifeTimeLoss;

			ar& pad;

			ar& planePos[0];
			ar& planePos[1];
			ar& planePos[2];
			ar& planePos[3];
			ar& planePos[4];
			ar& planePos[5];
			ar& planePos[6];
			ar& planePos[7];

			ar& planeNormalVec[0];
			ar& planeNormalVec[1];
			ar& planeNormalVec[2];
			ar& planeNormalVec[3];
			ar& planeNormalVec[4];
			ar& planeNormalVec[5];
			ar& planeNormalVec[6];
			ar& planeNormalVec[7];

		}
	};

	/**
	 * \brief 특정 텍스쳐를 늘려 트레일을 만드는 트레일 관련 구조체
	 */
	struct ParticleTrails
	{
		enum class TextureMode
		{
			STRETCH,
			TILE,
			DISTRIBUTE_PER_SEGMENT,
			REPEAT_PER_SEGMENT

		};
		enum class Conditional
		{
			AND,
			OR
		};

		ParticleTrails()
			: useModule(false)
			, ratio(1.0f)
			, lifeTime(1.0f)
			, minimumVertexDistance(0.1f)
			, worldSpace(false)
			, dieWithParticle(false)
			, textureMode(TextureMode::STRETCH)
			, sizeAffectsWidth(false)
			, sizeAffectsLifeTime(false)
			, inheritParticleColor(true)
			, widthOverTrail{ 1.0f, 1.0f }
			, generateLightingData(false)
			, shadowBias(0)
			, trailVertexCount(15)
			, widthModifierOption(ParticleCommonInfo::OptionParticle::CONSTANT)
			, scrollXSpeed{ 0, 0 }
			, scrollYSpeed{ 0, 0 }
			, condition(Conditional::AND)
			, recordTime(0)
			, scrollModifierOption(ParticleCommonInfo::OptionParticle::CONSTANT)
		{
			for (int i = 0; i < 8; i++)
			{
				alphaRatioLifetime[i] = SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
				colorRatioLifetime[i] = SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);

				alphaRatioTrail[i] = SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
				colorRatioTrail[i] = SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
			}

		}
		bool operator==(const ParticleTrails& other) const
		{
			if (memcmp(this, &other, sizeof(ParticleTrails)) == 0)
				return true;
			else
				return false;
		}
		
		bool useModule;

		float ratio; // 0 ~ 1 값
		float lifeTime; // 0 ~ 1 값
		float minimumVertexDistance;
		bool worldSpace;

		bool dieWithParticle;
		TextureMode textureMode;
		bool sizeAffectsWidth;
		bool sizeAffectsLifeTime;

		bool inheritParticleColor;
		SimpleMath::Vector4 alphaRatioLifetime[8];
		SimpleMath::Vector4 colorRatioLifetime[8];

		ParticleCommonInfo::OptionParticle widthModifierOption;
		float widthOverTrail[2];

		SimpleMath::Vector4 alphaRatioTrail[8];
		SimpleMath::Vector4 colorRatioTrail[8];

		bool generateLightingData;
		float shadowBias;

		int trailVertexCount;

		ParticleCommonInfo::OptionParticle scrollModifierOption;
		float scrollXSpeed[2];
		float scrollYSpeed[2];

		Conditional condition;
		float recordTime;
	protected:
		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& useModule;

			ar& ratio;
			ar& lifeTime;
			ar& minimumVertexDistance;
			ar& worldSpace;

			ar& dieWithParticle;
			ar& textureMode;
			ar& sizeAffectsWidth;
			ar& sizeAffectsLifeTime;

			ar& inheritParticleColor;

			ar& alphaRatioLifetime[0];
			ar& alphaRatioLifetime[1];
			ar& alphaRatioLifetime[2];
			ar& alphaRatioLifetime[3];
			ar& alphaRatioLifetime[4];
			ar& alphaRatioLifetime[5];
			ar& alphaRatioLifetime[6];
			ar& alphaRatioLifetime[7];

			ar& colorRatioLifetime[0];
			ar& colorRatioLifetime[1];
			ar& colorRatioLifetime[2];
			ar& colorRatioLifetime[3];
			ar& colorRatioLifetime[4];
			ar& colorRatioLifetime[5];
			ar& colorRatioLifetime[6];
			ar& colorRatioLifetime[7];

			ar& widthModifierOption;

			ar& widthOverTrail[0];
			ar& widthOverTrail[1];

			ar& alphaRatioTrail[0];
			ar& alphaRatioTrail[1];
			ar& alphaRatioTrail[2];
			ar& alphaRatioTrail[3];
			ar& alphaRatioTrail[4];
			ar& alphaRatioTrail[5];
			ar& alphaRatioTrail[6];
			ar& alphaRatioTrail[7];

			ar& colorRatioTrail[0];
			ar& colorRatioTrail[1];
			ar& colorRatioTrail[2];
			ar& colorRatioTrail[3];
			ar& colorRatioTrail[4];
			ar& colorRatioTrail[5];
			ar& colorRatioTrail[6];
			ar& colorRatioTrail[7];

			ar& generateLightingData;
			ar& shadowBias;

			ar& trailVertexCount;

			ar& scrollModifierOption;
			ar& scrollXSpeed[0];
			ar& scrollXSpeed[1];

			ar& scrollYSpeed[0];
			ar& scrollYSpeed[1];

			ar& condition;
			ar& recordTime;
		}
	};

	/**
	 * \brief 텍스쳐 렌더러.
	 */
	 struct ParticleRenderer
	{
		 enum class RenderMode
		 {
			 BILLBOARD,
			 STRETCHED_BILLBOARD,
			 HORIZONTAL_BILLBOARD,
			 VERTICAL_BILLBOARD,
			 MESH,
			 NONE
		 };
		 enum class BlendState
		 {
			 OIT_DEFAULT,
			 OIT_ADDITIVE,
			 FOWARD
		 };
		 enum class RenderAlignment
		 {
			 VIEW,
			 WORLD,
			 LOCAL,
			 VELOCITY
		 };
		 enum class Masking
		 {
			 NO_MASKING,
			 VISIBLE_INSIDE_MASK,
			 OUTSIDE_MASK
		 };

		ParticleRenderer()
	 		: useModule(true)
	 		, renderMode(RenderMode::BILLBOARD)
	 		, blendState(BlendState::OIT_DEFAULT)
	 		, meshName(L"")
	 		, texturePath(L"")
	 		, trailTexturePath(L"")
	 		, renderAlignment(RenderAlignment::VIEW)
	 		, speedScale(0.f)
	 		, lengthScale(2.f)
		{
		}

		// 복사생성자
		ParticleRenderer(const ParticleRenderer& other)
		{
			useModule = other.useModule;
			renderMode = other.renderMode;
			blendState = other.blendState;

			// 이름들이 문제..
			meshName.assign(other.meshName.begin(), other.meshName.end());
			texturePath.assign(other.texturePath.begin(), other.texturePath.end());
			trailTexturePath.assign(other.trailTexturePath.begin(), other.trailTexturePath.end());

			renderAlignment = other.renderAlignment;

			speedScale = other.speedScale;
			lengthScale = other.lengthScale;

			masking = other.masking;
			
		}

		bool operator==(const ParticleRenderer& other) const
		{
			if (
				useModule != other.useModule ||
				renderMode != other.renderMode ||
				blendState != other.blendState ||
				meshName != other.meshName ||
				texturePath != other.texturePath ||
				trailTexturePath != other.trailTexturePath ||
				renderAlignment != other.renderAlignment ||
				speedScale != other.speedScale ||
				lengthScale != other.lengthScale ||
				masking != other.masking
				)
				return false;
			return true;
		}
		
		bool useModule;

		RenderMode renderMode;

		BlendState blendState;

		// Foot Graphics에서 기본적으로 오브젝트의 이름(mesh의 이름)은 string을 사용합니다s
		std::wstring meshName;

		// Foot Graphics에서 기본적으로 Path는 wstring을 활용합니다.
		std::wstring texturePath;			// 파티클 이펙트 텍스쳐의 주소.

		std::wstring trailTexturePath;			// 트레일 이펙트 텍스쳐의 주소.

		RenderAlignment renderAlignment;

		float speedScale;

		float lengthScale;

		Masking masking;

	protected:
		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& useModule;

			ar& renderMode;

			ar& blendState;

			ar& meshName;

			ar& texturePath;

			ar& trailTexturePath;

			ar& renderAlignment;

			ar& speedScale;

			ar& lengthScale;

			ar& masking;
		}
	};

	// 파티클 시스템을 사용하기 위한 인터페이스
	// 한 개의 입자 시스템을 묘사한다.
	struct RenderingDataParticle
	{
		RenderingDataParticle()
			: commonInfo()
			, emission()
			, shape()
			, velocityOverLifetime()
			, limitVelocityOverLifetime()
			, forceOverLifetime()
			, colorOverLifetime()
			, sizeOverLifetime()
			, rotationOverLifetime()
			, textureSheetAnimaition()
			, noise()
			, objectID(0)
			, isDelete(false)
			, shaderName()
			, childrenIDs()
			, isSolid(false)
			, particleCount(0)
		{}

		// 복사 생성자를 만들어봅시다..
		RenderingDataParticle(const RenderingDataParticle& other)
		{
			memcpy(&this->commonInfo, &other.commonInfo, sizeof(ParticleCommonInfo));
			memcpy(&this->emission, &other.emission, sizeof(ParticleEmission));
			memcpy(&this->shape, &other.shape, sizeof(ParticleShape));
			memcpy(&this->velocityOverLifetime, &other.velocityOverLifetime, sizeof(ParticleVelocityOverLifetime));
			memcpy(&this->limitVelocityOverLifetime, &other.limitVelocityOverLifetime, sizeof(ParticleLimitVelocityOverLifetime));
			memcpy(&this->forceOverLifetime, &other.forceOverLifetime, sizeof(ParticleForceOverLifeTime));
			memcpy(&this->colorOverLifetime, &other.colorOverLifetime, sizeof(ParticleColorOverLifetime));
			memcpy(&this->sizeOverLifetime, &other.sizeOverLifetime, sizeof(ParticleSizeOverLifetime));
			memcpy(&this->rotationOverLifetime, &other.rotationOverLifetime, sizeof(ParticleRotationOverLifetime));
			memcpy(&this->noise, &other.noise, sizeof(ParticleNoise));
			memcpy(&this->collision, &other.collision, sizeof(ParticleCollision));
			memcpy(&this->textureSheetAnimaition, &other.textureSheetAnimaition, sizeof(textureSheetAnimaition));
			memcpy(&this->trails, &other.trails, sizeof(ParticleTrails));
			// renderer의 wstring이 문제가 있기 때문에 renderer의 경우 복사 생성자를 호출하도록 하자..
			this->renderer = other.renderer;

			objectID = other.objectID;
			shaderName = other.shaderName;
			childrenIDs = other.childrenIDs;
			isDelete = other.isDelete;
			isSolid = other.isSolid;

			particleCount = other.particleCount;
		}

		bool operator==(GraphicsEngineSpace::RenderingDataParticle& other)
		{
			if (this->commonInfo != other.commonInfo ||
				this->emission != other.emission ||
				this->shape != other.shape ||
				this->velocityOverLifetime != other.velocityOverLifetime ||
				this->limitVelocityOverLifetime != other.limitVelocityOverLifetime ||
				this->forceOverLifetime != other.forceOverLifetime ||
				this->colorOverLifetime != other.colorOverLifetime ||
				this->sizeOverLifetime != other.sizeOverLifetime ||
				this->rotationOverLifetime != other.rotationOverLifetime ||
				this->textureSheetAnimaition != other.textureSheetAnimaition ||
				this->noise != other.noise ||
				this->collision != other.collision ||
				this->trails != other.trails ||
				this->renderer != other.renderer
				)
				return false;
			else
				return true;
		}
		//------------------------------------------------ 모듈 -------------------------------------------
		ParticleCommonInfo commonInfo;

		ParticleEmission emission;

		ParticleShape shape;

		ParticleVelocityOverLifetime velocityOverLifetime;

		ParticleLimitVelocityOverLifetime limitVelocityOverLifetime;

		ParticleForceOverLifeTime forceOverLifetime;

		ParticleColorOverLifetime colorOverLifetime;

		ParticleSizeOverLifetime sizeOverLifetime;

		ParticleRotationOverLifetime rotationOverLifetime;

		ParticleNoise noise;

		ParticleCollision collision;

		ParticleTextureSheetAnimation textureSheetAnimaition;

		ParticleTrails trails;

		ParticleRenderer renderer;
		//------------------------------------------------ 모듈 -------------------------------------------

		unsigned int objectID; // 파티클 ID 리소스 매니저에 맵핑한 아이디, 오브젝트 ID로 사용하자.

		std::vector<std::string> shaderName; // 어떤 쉐이더를 사용하는지. => Foot Engine에서는 쉐이더 매니저에서 가져와 줄 때 사용할 스트링

		std::vector<unsigned int> childrenIDs;

		bool isDelete; // 파티클을 다 사용했으면 할당 해제 하기 위함. 파티클을 내부에서 오브젝트 풀 등으로 관리 안하는 이유는 파티클마다 버퍼의 크기가 다르기 때문이다.

		bool isSolid;	// archive 받을 용도의 isSolid;

		unsigned int particleCount; // 현재 방출된 파티클의 숫자.
		/**
		* \brief 여기 값을 지표로
		* \brief RenderingData, Common.hlsli 를 수정하도록 하자..!
		*/
		unsigned int GetFlag()
		{
			unsigned int flag = 0;

			if (commonInfo.useModule) flag |= 1 << 0;
			if (emission.useModule) flag |= 1 << 1;
			if (shape.useModule) flag |= 1 << 2;
			if (velocityOverLifetime.useModule) flag |= 1 << 3;
			if (forceOverLifetime.useModule) flag |= 1 << 4;
			if (colorOverLifetime.useModule) flag |= 1 << 5;
			if (sizeOverLifetime.useModule) flag |= 1 << 6;
			if (rotationOverLifetime.useModule) flag |= 1 << 7;
			if (noise.useModule) flag |= 1 << 8;
			if (collision.useModule) flag |= 1 << 9;
			if (textureSheetAnimaition.useModule) flag |= 1 << 10;
			if (trails.useModule) flag |= 1 << 11;
			if (renderer.useModule) flag |= 1 << 12;

			if (shape.shape == ParticleShape::Shape::SPHERE) flag |= 1 << 13;
			if (shape.shape == ParticleShape::Shape::HEMISPHERE) flag |= 1 << 14;
			if (shape.shape == ParticleShape::Shape::CONE) flag |= 1 << 15;
			if (shape.shape == ParticleShape::Shape::DONUT) flag |= 1 << 16;
			if (shape.shape == ParticleShape::Shape::BOX) flag |= 1 << 17;
			if (shape.shape == ParticleShape::Shape::CIRCLE) flag |= 1 << 18;
			if (shape.shape == ParticleShape::Shape::RECTANGLE) flag |= 1 << 19;

			if (renderer.renderMode == ParticleRenderer::RenderMode::BILLBOARD) flag |= 1 << 20;
			if (renderer.renderMode == ParticleRenderer::RenderMode::STRETCHED_BILLBOARD) flag |= 1 << 21;
			if (renderer.renderMode == ParticleRenderer::RenderMode::HORIZONTAL_BILLBOARD) flag |= 1 << 22;
			if (renderer.renderMode == ParticleRenderer::RenderMode::VERTICAL_BILLBOARD) flag |= 1 << 23;
			if (renderer.renderMode == ParticleRenderer::RenderMode::MESH) flag |= 1 << 24;

			if (commonInfo.space == ParticleSpace::WORLD) flag |= 1 << 25;

			if (limitVelocityOverLifetime.useModule) flag |= 1 << 26;

			return flag;
		}

	protected:
		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& commonInfo;

			ar& emission;

			ar& shape;

			ar& velocityOverLifetime;

			ar& limitVelocityOverLifetime;

			ar& forceOverLifetime;

			ar& colorOverLifetime;

			ar& sizeOverLifetime;

			ar& rotationOverLifetime;

			ar& noise;

			ar& collision;

			ar& textureSheetAnimaition;

			ar& trails;

			ar& renderer;

			ar& objectID;

			ar& shaderName;

			ar& childrenIDs;

			ar& isDelete;

			ar& isSolid;

			ar& particleCount;
		}
	};

	BOOST_SERIALIZATION_ASSUME_ABSTRACT(ParticleCommonInfo);
	BOOST_SERIALIZATION_ASSUME_ABSTRACT(ParticleEmission);
	BOOST_SERIALIZATION_ASSUME_ABSTRACT(ParticleShape);
	BOOST_SERIALIZATION_ASSUME_ABSTRACT(ParticleVelocityOverLifetime);
	BOOST_SERIALIZATION_ASSUME_ABSTRACT(ParticleLimitVelocityOverLifetime);
	BOOST_SERIALIZATION_ASSUME_ABSTRACT(ParticleForceOverLifeTime);
	BOOST_SERIALIZATION_ASSUME_ABSTRACT(ParticleColorOverLifetime);
	BOOST_SERIALIZATION_ASSUME_ABSTRACT(ParticleSizeOverLifetime);
	BOOST_SERIALIZATION_ASSUME_ABSTRACT(ParticleRotationOverLifetime);
	BOOST_SERIALIZATION_ASSUME_ABSTRACT(ParticleNoise);
	BOOST_SERIALIZATION_ASSUME_ABSTRACT(ParticleCollision);
	BOOST_SERIALIZATION_ASSUME_ABSTRACT(ParticleTextureSheetAnimation);
	BOOST_SERIALIZATION_ASSUME_ABSTRACT(ParticleTrails);
	BOOST_SERIALIZATION_ASSUME_ABSTRACT(ParticleRenderer);
}

#pragma endregion