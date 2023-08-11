// 파티클에 사용되는 각종 함수 및 구조체들을 모아둔 Helper 파일입니다.
	// 기본적으로 구조체 및 변수의 이름을 해당 cb의 이름과 동일하게 맞춰줍니다.
#include "ParticleCommon.hlsli"

#define PT_EMITTER 0
#define PT_FLARE 1
#define PT_TRAIL 2

#define TrailCount 30

struct CommonInfo // 144
{
    float4x4 TransformMatrix;
    float4x4 DeltaMatrix;

    float2 StartDelay; // 몇 초 뒤에 파티클이 재생될 지.					
    float2 StartLifeTime; // 한 파티클의 생존 시간.						

    float2 StartSpeed; // 파티클 생성시 시작 속도.
    float2 StartRotation; // 파티클의 시작 회전.							

    float4 StartSize; // 파티클의 시작 크기.							

    float4 StartColor[2]; // 파티클의 시작 색상									

    float2 GravityModifier; // 파티클에 가해지는 중력.
    float ParticlePlayTime; // 파티클 재생 후 흐른 시간.
    float ObjectID; // 파티클이 가지고 있는 오브젝트 ID					

    int isLooping; // 반복여부.
    float SimulationSpeed;
    float Duration; // 몇 초 동안 파티클 객체가 재생될 지.					
    int MaxParticles; // 파티클 최대 출력 사이즈.							

};

struct Emission // 16
{
    int EmissiveCount; // 한번에 몇개를 방출 시킬지.

    float EmissiveTime; // 다음 방출까지 걸리는 시간.

    int IsRateOverDistance;
    
    float pad;
};

struct Shape // 64
{
    float Angle;
    float Radius;
    float DonutRadius;
    float Arc;

    float3 Position;
    float RadiusThickness;

    float3 Rotation;
    float pad1;

    float3 Scale;
    float pad2;
    
    int EdgeMode;
    float Speed;
    float Spread; //
    float pad3;
};

struct VelocityOverLifeTime // 16
{
    float3 Velocity; // 시간에 따른 파티클 속력.
    float pad1;
    float3 Orbital; // Orbital 궤도의 영향을 얼마나 받을 것 인가.
    uint IsGravity;
    float3 Offset; // 궤도 중심의 Offset 값
    float pad2;
};

struct LimitVelocityOverLifetime // 14 // 4
{
    float2 PointA;
    float2 PointB;

    float2 PointC;
    float2 PointD;

    float Speed;
    float Dampen;
    float2 pad;
};

struct ForceOverLifeTime // 16
{
    float3 Force;
    float pad;
};

struct ColorOverLifeTime // 48
{
    float4 AlphaRatio[8]; // 알파 값 비율.	16

    float4 ColorRatio[8]; // 컬러 비율.	32
};

struct SizeOverLifetime // 16
{
    float2 PointA;
    float2 PointB;
    float2 PointC;
    float2 PointD;
};

struct RotationOverLifetime // 16
{
    float AngularVelocity;
    float3 pad;
};

struct TextureSheetAnimation // 16
{
    int2 GridXY;
    int TimeMode;
    float pad;
};

struct Noise
{
    float Strength;
    float ScrollSpeed;
    float PositionAmount;
    float RotationAmount;

    float SizeAmount;
    float3 pad;
};

struct Collision
{
    int PlaneCount;
    float Bounce;
    float LifeTimeLoss;
    float pad;

    float4 PlanePosition[8];
    float4 PlaneNormalVec[8];
};


struct Trails // 16
{
    float Ratio;
    float LifeTime;
    float MinimumVertexDistance;
    int pad1;

    int TrailsFlag;
    int TrailVertexCount;
    float2 WidthOverTrail;

    float4 AlphaRatioLifetime[8];
    float4 ColorRatioLifetime[8];
    float4 AlphaRatioTrail[8];
    float4 ColorRatioTrail[8];
    
    float2 ScrollXSpeed;
    float2 ScrollYSpeed;
    
    uint Condition;
    float RecordTime;
    float2 pad2;
};

struct ParticleRenderer // 16
{
    float SpeedScale;
    float LengthScale;
    uint BlendType;
    uint RenderAlignment;
};

cbuffer cbPerObjectParticle : register(b1)
{
    CommonInfo commonInfo;

    Emission emission;

    Shape shape;

    VelocityOverLifeTime velocityOverLifetime;

    LimitVelocityOverLifetime limitVelocityOverLifetime;
    
    ForceOverLifeTime forceOverLifeTime;

    ColorOverLifeTime colorOverLifetime;

    SizeOverLifetime sizeOverLifetime;

    RotationOverLifetime rotationOverLifetime;

    Noise noise;

    Collision collision;

    TextureSheetAnimation textureSheetAnimation;

    Trails trails;

    ParticleRenderer particleRenderer;

    int ParticleFlag;

    float3 pad;
};

struct ParticleStruct // 16 바이트 정렬 필 수.
{
    float Type;
    float3 PosW;

    float4 VelW; // Start speed

    float4 SizeWStartSize; // Start size
    float4 AgeLifeTimeRotationGravity;

    float4 StartColor;
    float4 Color; // Start Color

    float2 QuadTexC[4];
    float4 InitEmitterPos;
    float4 PrevPos[TrailCount]; // Trail을 그리기 위해 일정 거리마다 위치를 기록한다. 

    float4 LatestPrevPos;
    
    float TrailWidth;
    float TrailRecordTime;
    float2 TrailScrollSpeed;
};

struct StreamOutParticle
{
    uint Type : TYPE; // 방출기인가
    uint VertexID : VERTEXID;

    float3 PosW : POSITION;
    float3 VelW : VELOCITY; // Start speed

    float4 SizeWStartSize : SIZE_STARTSIZE; // Start size
    float4 AgeLifeTimeRotationGravity : AGE_LIFETIME_ROTATION_GRAVITY;

    float4 StartColor : STARTCOLOR;
    float4 Color : COLOR; // Start Color

    float2 QuadTexC[4] : QUADTEX;
    float3 InitEmitterPos : EMITTERPOS;
    float3 PrevPos[TrailCount] : PREVPOS; // Trail을 그리기 위해 일정 거리마다 위치를 기록한다. 

    float3 LatestPrevPos : LASTESTPREVPOS;
};

struct ParticleVertexOut
{
    float3 PosW : POSITIONT;
    uint Type : TYPE;

    float3 LatestPrevPos : LASTEST_PREV_POS;

    float2 SizeW : SIZE;
    float4 Color : COLOR;

    float3 AgeLifeTimeRotation : AGE_LIFETIME_ROTATION;

    float2 QuadTexC[4] : QUADTEX;
    float3 PrevPos[TrailCount] : PREVPOS;
};

struct VertexIDOut
{
    uint index : VERTEXID; // 방출기인가
};

void SetTextureSheetAnimation(float4 vunsignedRandom4, out float2 QuadTexC[4]);
void SetColorOverLifeTime(float ratio, float4 alpha_Ratio[8], float4 color_Ratio[8],
	out float4 color);
void PushBackPrevPos(float3 prevPosIn[TrailCount], float3 posW, out float3 prevPosOut[TrailCount]);

void Orbital(float3 posW, float3 InitEmitterPos, float3 velW, float deltaTime, out float3 velW_Out, out float3 posW_Out);
void SetBillBoard(
	float3 cameraPos, ParticleStruct particle,
	out float3 look, out float3 right, out float3 up);
void CollisionCheck(float3 posW, float3 velW, float deltaTime, float age, float lifeTime, out float3 posWout, out float3 velWout, out float ageOut);
// -------------------------------------------------- 전방 선언 -------------------------------
float3 TextureSample(Texture2D tex, SamplerState sam, float2 uv)
{
    return tex.SampleLevel(sam, uv, 0).xyz;
}

// 각종 모듈들의 인터페이스를 선언합니다.
interface IParticleInterFace_Shape
{
    void Shape(float4 vRandom1, float4 vRandom2, float4 vRandom5, float4 vunsignedRandom2, float4 vUnUnitRandom2
		, out float3 InitialPosW, out float3 InitialVelW);
};

interface IParticleInterFace_VelocityOverLifeTime
{
    void VelocityOverLifeTime(StreamOutParticle particleIn, float deltaTime, out StreamOutParticle particelOut);
};

interface IParticleInterFace_ForceOverLifeTime
{
    void ForceOverLifeTime(float3 posW, float ratio, float deltaTime, out float3 posWOut);
};

interface IParticleInterFace_ColorOverLifeTime
{
    void ColorOverLifeTime(float ratio, float4 alpha_Ratio[8], float4 color_Ratio[8], out float4 color);
};

interface IParticleInterFace_SizeOverLifeTime
{
    void SizeOverLifeTime(float ratio, out float2 size);
};

interface IParticleInterFace_RoationOverLifeTime
{
    void RoationOverLifeTime(float originRotate, float angularVelocity, float lifetime, float deltaTime, out float rotateOut);
};

interface IParticleInterFace_Noise
{
    void Noise(float2 sizeW, float3 posW, float rotW,
		Texture2D noiseMap, SamplerState sam, float deltaTime, float gamePlayTime,
		out float2 size, out float3 pos, out float rot);
};

interface IParticleInterFace_Collision
{
    void Collision(float3 posW, float3 velW, float deltaTime, float age, float lifeTime, out float3 posWout, out float3 velWout, out float ageOut);
};

interface IParticleInterFace_TextureSheetAnimation
{
    void TextureSheetAnimation(float4 vunsignedRandom4, out float2 QuadTexC[4]);
};

interface IParticleInterFace_Trails
{
    void Trails(float3 prevPosIn[TrailCount], float3 posW, out float3 prevPosOut[TrailCount]);
};

// 모듈의 여부에 따라 실제적인 구현이 달라집니다.
class CShape : IParticleInterFace_Shape
{
    void Shape(float4 vRandom1, float4 vRandom2, float4 vRandom5, float4 vunsignedRandom2, float4 vUnUnitRandom2
		, out float3 InitialPosW, out float3 InitialVelW)
    {
        float speed = lerp(commonInfo.StartSpeed[0], commonInfo.StartSpeed[1], vunsignedRandom2.x);

        InitialPosW = float3(commonInfo.TransformMatrix[3][0], commonInfo.TransformMatrix[3][1], commonInfo.TransformMatrix[3][2]); /*gCommonInfo.gEmitPosW.xyz;*/
        InitialVelW = speed * float3(commonInfo.TransformMatrix[1][0], commonInfo.TransformMatrix[1][1], commonInfo.TransformMatrix[1][2]); /* gCommonInfo.gEmitDirW;*/

        if (ParticleFlag & Use_Shape_Sphere)
        {
            float4x4 temp = commonInfo.TransformMatrix;

            temp[3].xyz = float3(0, 0, 0);

            float3 vel = mul(float4((vRandom1.xyz).xyz, 1), temp).xyz;
            InitialVelW = speed * vel;

            float3 pos = mul(float4((vRandom1.xyz).xyz * shape.Radius, 1), commonInfo.TransformMatrix).xyz;
            InitialPosW = pos.xyz;
        }
        else if (ParticleFlag & Use_Shape_Hemisphere)
        {
            float4x4 temp = commonInfo.TransformMatrix;

            temp[3].xyz = float3(0, 0, 0);

            float3 vel = mul(float4(vRandom1.x, abs(vRandom1.y), vRandom1.z, 1), temp).xyz;
            InitialVelW = speed * vel;

            float3 pos = mul(float4(vRandom1.x, abs(vRandom1.y), vRandom1.z, 1), commonInfo.TransformMatrix).xyz;
            InitialPosW = pos.xyz * shape.Radius;
        }
        else if (ParticleFlag & Use_Shape_Cone)
        {
            float a = cos(shape.Angle) / speed;
            float y = abs(sin(shape.Angle) / a);

            float TopRadius = (y + shape.Radius);

            float3 topPosition = mul(float4(((float3(0, speed, 0) + (float3(vRandom1.x, 0, vRandom1.z) * TopRadius))).xyz, 1), commonInfo.TransformMatrix).xyz;
            InitialPosW = mul(float4((float3(vRandom1.x, 0, vRandom1.z) * shape.Radius).xyz, 1), commonInfo.TransformMatrix).xyz;
            InitialVelW = (topPosition - InitialPosW) /** gCommonInfo.gSimulationSpeed*/;

        }
        else if (ParticleFlag & Use_Shape_Donut)
        {
            float2 radiusVec = vRandom1.xy;
            radiusVec = normalize(radiusVec);

            InitialPosW = mul(float4((float3(radiusVec.x, 0, radiusVec.y) * shape.Radius + vRandom2.xyz * shape.DonutRadius).xyz, 1), commonInfo.TransformMatrix).xyz;
            InitialVelW = speed * vRandom5.xyz;
        }
        else if (ParticleFlag & Use_Shape_Box)
        {
            InitialPosW = mul(float4((vUnUnitRandom2.xyz).xyz, 1), commonInfo.TransformMatrix).xyz;
        }
        else if (ParticleFlag & Use_Shape_Circle)
        {
            InitialPosW = mul(float4((float3(vRandom1.x, 0, vRandom1.z) * shape.Radius).xyz, 1), commonInfo.TransformMatrix).xyz;

            float4x4 temp = commonInfo.TransformMatrix;
            temp[3] = float4(0, 0, 0, 1.0f);
            InitialVelW = mul(float4(vRandom1.x, 0, vRandom1.z, 1.0f), temp).xyz * speed;
        }
        else if (ParticleFlag & Use_Shape_Rectangle)
        {
            InitialPosW = mul(float4((float3(vUnUnitRandom2.x, 0, vUnUnitRandom2.z)).xyz, 1), commonInfo.TransformMatrix).xyz;
        }

    }
};
class CNullShape : IParticleInterFace_Shape
{
    void Shape(float4 vRandom1, float4 vRandom2, float4 vRandom5, float4 vunsignedRandom2, float4 vUnUnitRandom2
		, out float3 InitialPosW, out float3 InitialVelW)
    {
        float speed = lerp(commonInfo.StartSpeed[0], commonInfo.StartSpeed[1], vunsignedRandom2.x);

        InitialPosW = float3(commonInfo.TransformMatrix[3][0], commonInfo.TransformMatrix[3][1], commonInfo.TransformMatrix[3][2]); /*gCommonInfo.gEmitPosW.xyz;*/
        InitialVelW = speed * float3(commonInfo.TransformMatrix[1][0], commonInfo.TransformMatrix[1][1], commonInfo.TransformMatrix[1][2]); /* gCommonInfo.gEmitDirW;*/
    }
};

class CVelocityOverLifeTime : IParticleInterFace_VelocityOverLifeTime
{
    void VelocityOverLifeTime(StreamOutParticle particleIn, float deltaTime, out StreamOutParticle particelOut)
    {
        float ratio = particleIn.AgeLifeTimeRotationGravity.x / particleIn.AgeLifeTimeRotationGravity.y;

        float3 velocity = float3(0, 0, 0);

        velocity = lerp(0, velocityOverLifetime.Velocity, ratio);

        particleIn.PosW += velocity * deltaTime;

        Orbital(particleIn.PosW, particleIn.InitEmitterPos, particleIn.VelW, deltaTime, particleIn.VelW, particleIn.PosW);

        particelOut = particleIn;

    }
};
class CNullVelocityOverLifeTime : IParticleInterFace_VelocityOverLifeTime
{
    void VelocityOverLifeTime(StreamOutParticle particleIn, float deltaTime, out StreamOutParticle particelOut)
    {
        particelOut = particleIn;
    }
};

class CForceOverLifeTime : IParticleInterFace_ForceOverLifeTime
{
    void ForceOverLifeTime(float3 posW, float ratio, float deltaTime, out float3 posWOut)
    {
        float3 force = float3(0, 0, 0);

        force = lerp(0, forceOverLifeTime.Force, ratio);

        posWOut = posW + force * deltaTime;
    }
};
class CNullForceOverLifeTime : IParticleInterFace_ForceOverLifeTime
{
    void ForceOverLifeTime(float3 posW, float ratio, float deltaTime, out float3 posWOut)
    {
        posWOut = posW;
    }
};

class CColorOverLifeTime : IParticleInterFace_ColorOverLifeTime
{
    void ColorOverLifeTime(float ratio, float4 alphaRatio[8], float4 colorRatio[8], out float4 color)
    {
        SetColorOverLifeTime(ratio, colorOverLifetime.AlphaRatio, colorOverLifetime.ColorRatio, color);
    }
};
class CNullColorOverLifeTime : IParticleInterFace_ColorOverLifeTime
{
    void ColorOverLifeTime(float ratio, float4 alphaRatio[8], float4 colorRatio[8], out float4 color)
    {
        color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
};

class CSizeOverLifeTime : IParticleInterFace_SizeOverLifeTime
{
    void SizeOverLifeTime(float ratio, out float2 size)
    {
        float t = ratio;
        float s = 1.0f - t;

        size = pow(s, 3) * sizeOverLifetime.PointA
			+ 3 * pow(s, 2) * t * sizeOverLifetime.PointB
			+ 3 * s * pow(t, 2) * sizeOverLifetime.PointC
			+ pow(t, 3) * sizeOverLifetime.PointD;

        size.x = size.y;
        size.y = size.y;
        
        size = clamp(size, 0, 1);
    }
};
class CNullSizeOverLifeTime : IParticleInterFace_SizeOverLifeTime
{
    void SizeOverLifeTime(float ratio, out float2 size)
    {
        size = float2(1.0f, 1.0f);
    }
};

class CRoationOverLifeTime : IParticleInterFace_RoationOverLifeTime
{
    void RoationOverLifeTime(float originRotate, float angularVelocity, float lifetime, float deltaTime, out float rotateOut)
    {
        rotateOut = originRotate + angularVelocity / lifetime * deltaTime;
    }
};
class CNullRoationOverLifeTime : IParticleInterFace_RoationOverLifeTime
{
    void RoationOverLifeTime(float originRotate, float angularVelocity, float lifetime, float deltaTime, out float rotateOut)
    {
        rotateOut = originRotate;
    }
};

class CNoise : IParticleInterFace_Noise
{
    void Noise(float2 sizeW, float3 posW, float rotW,
		Texture2D noiseMap, SamplerState sam, float deltaTime, float gamePlayTime,
		out float2 size, out float3 pos, out float rot)
    {
        float eps = 0.01f;
        float3 curl0;
        float3 curl1;
        float3 curl2;

        float noiseVal;

        float scrollOffset = noise.ScrollSpeed * gamePlayTime;

		{
            float2 uv = float2(posW.x, posW.y);

            noiseVal = TextureSample(noiseMap, sam, float2(uv.x, uv.y)).x;

            uv.y += scrollOffset;

            float3 n1 = TextureSample(noiseMap, sam, float2(uv.x + eps, uv.y)).x;
            float3 n2 = TextureSample(noiseMap, sam, float2(uv.x - eps, uv.y)).x;

            float3 a = (n1 - n2) / (2 * eps);

            n1 = TextureSample(noiseMap, sam, float2(uv.x, uv.y + eps)).x;
            n2 = TextureSample(noiseMap, sam, float2(uv.x, uv.y - eps)).x;

            float3 b = (n1 - n2) / (2 * eps);


            curl0 = float3(b.x, -a.x, 0);
        }
		{
            float2 uv = float2(posW.y, posW.z);

            uv.y += scrollOffset;

            float3 n1 = TextureSample(noiseMap, sam, float2(uv.x + eps, uv.y)).y;
            float3 n2 = TextureSample(noiseMap, sam, float2(uv.x - eps, uv.y)).y;

            float3 a = (n1 - n2) / (2 * eps);

            n1 = TextureSample(noiseMap, sam, float2(uv.x, uv.y + eps)).y;
            n2 = TextureSample(noiseMap, sam, float2(uv.x, uv.y - eps)).y;

            float3 b = (n1 - n2) / (2 * eps);


            curl1 = float3(0, b.x, -a.x);
        }
		{
            float2 uv = float2(posW.x, posW.z) / 100.0f;

            uv.y += scrollOffset;

            float3 n1 = TextureSample(noiseMap, sam, float2(uv.x + eps, uv.y)).z;
            float3 n2 = TextureSample(noiseMap, sam, float2(uv.x - eps, uv.y)).z;

            float3 a = (n1 - n2) / (2 * eps);

            n1 = TextureSample(noiseMap, sam, float2(uv.x, uv.y + eps)).z;
            n2 = TextureSample(noiseMap, sam, float2(uv.x, uv.y - eps)).z;

            float3 b = (n1 - n2) / (2 * eps);


            curl2 = float3(b.x, 0, -a.x);
        }


        float3 curl = (curl0 + curl1 + curl2) / 2.0f;

        pos = posW + curl * deltaTime * noise.Strength * noise.PositionAmount;

        size = sizeW + ((noiseVal * 2) - 1) * deltaTime * noise.SizeAmount * noise.Strength * noise.SizeAmount;

        rot = rotW + ((noiseVal * 2) - 1) * deltaTime * noise.RotationAmount * noise.Strength * noise.RotationAmount;
    }
};
class CNullNoise : IParticleInterFace_Noise
{
    void Noise(float2 sizeW, float3 posW, float rotW,
		Texture2D noiseMap, SamplerState sam, float deltaTime, float gamePlayTime,
		out float2 size, out float3 pos, out float rot)
    {
        size = sizeW;
        pos = posW;
        rot = rotW;
    }
};

class CCollision : IParticleInterFace_Collision
{
    void Collision(float3 posW, float3 velW, float deltaTime, float age, float lifeTime, out float3 posWout, out float3 velWout, out float ageOut)
    {
        ageOut = age;
		[unroll]
        for (int i = 0; i < collision.PlaneCount; i++)
        {
            float3 planeToPos = posW - collision.PlanePosition[i].xyz;

            float t = dot(planeToPos, collision.PlaneNormalVec[i].xyz);

            if (t <= 0) // 충돌!
            {

                velW = collision.Bounce * 2 * dot(-velW, collision.PlaneNormalVec[i].xyz) * collision.PlaneNormalVec[i].xyz + velW;
				//velW = reflect(velW, (gCollision.gPlanNormalVec[i].xyz * gCollision.gBoundce));
                posW += velW * deltaTime;

                planeToPos = posW - collision.PlanePosition[i].xyz;

                t = dot(planeToPos, collision.PlaneNormalVec[i].xyz);

                if (t <= 0)
                {
                    posW += dot(-planeToPos, collision.PlaneNormalVec[i].xyz) * collision.PlaneNormalVec[i].xyz;
                }

                ageOut = age + lifeTime * collision.LifeTimeLoss;

            }
        }

        velWout = velW;
        posWout = posW;
    }
};
class CNullCollision : IParticleInterFace_Collision
{
    void Collision(float3 posW, float3 velW, float deltaTime, float age, float lifeTime, out float3 posWout, out float3 velWout, out float ageOut)
    {
        velWout = velW;
        posWout = posW;
        ageOut = age;
    }
};

class CTextureSheetAnimation : IParticleInterFace_TextureSheetAnimation
{
    void TextureSheetAnimation(float4 vunsignedRandom4, out float2 QuadTexC[4])
    {
        int2 texIndex;
        texIndex.x = lerp(1, textureSheetAnimation.GridXY[0] + 1, vunsignedRandom4.x);
        texIndex.y = lerp(1, textureSheetAnimation.GridXY[1] + 1, vunsignedRandom4.y);

        float xStep = 1.0f / (float) textureSheetAnimation.GridXY.x;
        float yStep = 1.0f / (float) textureSheetAnimation.GridXY.y;
        float xMax = xStep * texIndex.x;
        float yMax = yStep * texIndex.y;
        float xMin = xStep * (texIndex.x - 1);
        float yMin = yStep * (texIndex.y - 1);

        QuadTexC[0] = float2(xMin, yMax); // lefttop
        QuadTexC[1] = float2(xMin, yMin); //leftbottom
        QuadTexC[2] = float2(xMax, yMax); // righttop
        QuadTexC[3] = float2(xMax, yMin); // ritghtbottom

    }
};
class CNullTextureSheetAnimation : IParticleInterFace_TextureSheetAnimation
{
    void TextureSheetAnimation(float4 vunsignedRandom4, out float2 QuadTexC[4])
    {
        QuadTexC[0] = float2(0.0f, 1.0f); // lefttop
        QuadTexC[1] = float2(0.0f, 0.0f); //leftbottom
        QuadTexC[2] = float2(1.0f, 1.0f); // righttop
        QuadTexC[3] = float2(1.0f, 0.0f); // ritghtbottom
    }
};

class CTrails : IParticleInterFace_Trails
{
    void Trails(float3 prevPosIn[TrailCount], float3 posW, out float3 prevPosOut[TrailCount])
    {

		[unroll]
        for (int i = 0; i < TrailCount; i++)
        {
            if (!(trails.TrailsFlag & Use_TrailFlag_WorldSpace) && !(ParticleFlag & Use_Commoninfo_WorldSpace))
                prevPosIn[i] = mul(float4(prevPosIn[i], 1.0f), commonInfo.DeltaMatrix).xyz;
        }

        if (distance(prevPosIn[0], posW) >= trails.MinimumVertexDistance)
        {
			[unroll]
            for (int i = TrailCount - 2; i >= 0; i--)
            {
                prevPosIn[i + 1] = prevPosIn[i];
            }
            prevPosIn[0] = posW;

        }
        prevPosOut = prevPosIn;
    }
};
class CNullTrails : IParticleInterFace_Trails
{
    void Trails(float3 prevPosIn[TrailCount], float3 posW, out float3 prevPosOut[TrailCount])
    {
        prevPosIn = prevPosOut;
    }
};

void ManualShape(float4 vRandom1, float4 vRandom2, float4 vRandom5, float4 vunsignedRandom2, float4 vUnUnitRandom2
	, out float3 InitialPosW, out float3 InitialVelW)
{
    if (ParticleFlag & Use_Shape)
    {
        float speed = lerp(commonInfo.StartSpeed[0], commonInfo.StartSpeed[1], vunsignedRandom2.x);

        InitialPosW = float3(commonInfo.TransformMatrix[3][0], commonInfo.TransformMatrix[3][1], commonInfo.TransformMatrix[3][2]); /*gCommonInfo.gEmitPosW.xyz;*/
        InitialVelW = speed * float3(commonInfo.TransformMatrix[1][0], commonInfo.TransformMatrix[1][1], commonInfo.TransformMatrix[1][2]); /* gCommonInfo.gEmitDirW;*/

        if (ParticleFlag & Use_Shape_Sphere)
        {
            float4x4 temp = commonInfo.TransformMatrix;

            float3 nRandom = normalize(vRandom1.xyz); // xz 평면 기준으로 Arc를 적용시키자.
            
            float _length = length(nRandom.xz); // 벡터의 길이를 구한다.
            
            float cosTheata = nRandom.xz.x / _length; // 벡터의 길이 = 빗변, \\  밑면 / 빗변 = cosTheata

            float _radian = nRandom.z > 0 ? acos(cosTheata) : 2 * PI - acos(cosTheata); // 파티클의 방출 각도. 
            
            _radian = _radian * (shape.Arc / (2 * PI)); // 변환.
            
            nRandom.x = cos(_radian) * _length;
            nRandom.z = sin(_radian) * _length;
            
            temp[3].xyz = float3(0, 0, 0);

            float3 vel = mul(float4(nRandom, 1), temp).xyz;
            
            InitialVelW = speed * vel;

			// gShape.gRadiusThickness == 0 모든 위치에서 방출
			// gShape.gRadiusThickness == 1 최대 지점에서만 방출
			
            float posLength = length(nRandom * shape.Radius);
			
            posLength = shape.RadiusThickness * shape.Radius + (1.0f - shape.RadiusThickness) * posLength;
			
            float3 pos = nRandom * posLength;
			
            pos = mul(float4(pos, 1), commonInfo.TransformMatrix).xyz;
            
            InitialPosW = pos.xyz;
        }
        else if (ParticleFlag & Use_Shape_Hemisphere)
        {
            float4x4 temp = commonInfo.TransformMatrix;
            
            float3 nRandom = normalize(vRandom1.xyz); // xz 평면 기준으로 Arc를 적용시키자.
            
            float _length = length(nRandom.xz); // 벡터의 길이를 구한다.
            
            float cosTheata = nRandom.xz.x / _length; // 벡터의 길이 = 빗변, \\  밑면 / 빗변 = cosTheata

            float _radian = nRandom.z > 0 ? acos(cosTheata) : 2 * PI - acos(cosTheata); // 파티클의 방출 각도. 
            
            _radian = _radian * (shape.Arc / (2 * PI)); // 변환.
            
            nRandom.x = cos(_radian) * _length;
            nRandom.z = sin(_radian) * _length;
            
            temp[3].xyz = float3(0, 0, 0);

            float3 vel = mul(float4(nRandom.x, abs(nRandom.y), nRandom.z, 1), temp).xyz;
            
            InitialVelW = speed * vel;
            
            float posLength = length(nRandom.xyz * shape.Radius);
			
            posLength = shape.RadiusThickness * shape.Radius + (1.0f - shape.RadiusThickness) * posLength;
			
            float3 pos = normalize(nRandom.xyz) * posLength;
			
            pos = mul(float4(pos.x, abs(pos.y), pos.z, 1), commonInfo.TransformMatrix).xyz;
            
            InitialPosW = pos.xyz;
        }
        else if (ParticleFlag & Use_Shape_Cone)
        {
            float a = cos(shape.Angle) / speed;
            float y = abs(sin(shape.Angle) / a);

            float TopRadius = (y + shape.Radius);
            
            float3 nRandom = normalize(vRandom1.xyz); // xz 평면 기준으로 Arc를 적용시키자.
            
            float _length = length(nRandom.xz); // 벡터의 길이를 구한다.
            
            float cosTheata = nRandom.xz.x / _length; // 벡터의 길이 = 빗변, \\  밑면 / 빗변 = cosTheata

            float _radian = nRandom.z > 0 ? acos(cosTheata) : 2 * PI - acos(cosTheata); // 파티클의 방출 각도. 
            
            _radian = _radian * (shape.Arc / (2 * PI)); // 변환.
            
            nRandom.x = cos(_radian) * _length;
            nRandom.z = sin(_radian) * _length;
            
            float posLength = length(nRandom.xz * shape.Radius);
			
            posLength = shape.RadiusThickness * shape.Radius + (1.0f - shape.RadiusThickness) * posLength;
			
            float2 pos = normalize(nRandom.xz) * posLength;
            
            float3 topPosition = mul(float4(((float3(0, speed, 0) + (float3(pos.x, 0, pos.y) * TopRadius))).xyz, 1), commonInfo.TransformMatrix).xyz;
            
            InitialPosW = mul(float4(float3(pos.x, 0, pos.y), 1), commonInfo.TransformMatrix).xyz;
            InitialVelW = (topPosition - InitialPosW) /** gCommonInfo.gSimulationSpeed*/;

        }
        else if (ParticleFlag & Use_Shape_Donut)
        {
            float3 nRandom = normalize(vRandom1.xyz); // xz 평면 기준으로 Arc를 적용시키자.
            
            float _length = length(nRandom.xz); // 벡터의 길이를 구한다.
            
            float cosTheata = nRandom.xz.x / _length; // 벡터의 길이 = 빗변, \\  밑면 / 빗변 = cosTheata

            float _radian = nRandom.z > 0 ? acos(cosTheata) : 2 * PI - acos(cosTheata); // 파티클의 방출 각도. 
            
            _radian = _radian * (shape.Arc / (2 * PI)); // 변환.
            
            nRandom.x = cos(_radian) * _length;
            nRandom.z = sin(_radian) * _length;
            
            float2 radiusVec = nRandom.xz;
            
            radiusVec = normalize(radiusVec);
            
            // 도넛은 회전 행렬을 곱해야 할 것 같은데 어떡할까? 추가할까?
            float posLength = length(vRandom2.xy * shape.DonutRadius);
			
            posLength = shape.RadiusThickness * shape.DonutRadius + (1.0f - shape.RadiusThickness) * posLength;
			
            float3 pos = float3(normalize(vRandom2.xy) * posLength, 0.0f);
            
            float4x4 rotateTM =
            {
                radiusVec.x, 0, radiusVec.y, 0,
                0, 1, 0, 0,
                -radiusVec.y, 0, radiusVec.x, 0,
                0, 0, 0, 1,
                
            };
            
            pos = mul(float4(pos, 1.0f), rotateTM).xyz;
            // 도넛은 회전 행렬을 곱해야 할 것 같은데 어떡할까? 추가할까?
            
            InitialPosW = mul(float4((float3(radiusVec.x, 0, radiusVec.y) * shape.Radius + pos).xyz, 1), commonInfo.TransformMatrix).xyz;
            
            InitialVelW = speed * vRandom5.xyz;
        }
        else if (ParticleFlag & Use_Shape_Box)
        {
            InitialPosW = mul(float4((vUnUnitRandom2.xyz).xyz, 1), commonInfo.TransformMatrix).xyz;
        }
        else if (ParticleFlag & Use_Shape_Circle)
        {
            float3 nRandom = normalize(vRandom1.xyz); // xz 평면 기준으로 Arc를 적용시키자.
            
            float _length = length(nRandom.xz); // 벡터의 길이를 구한다.
            
            float cosTheata = nRandom.xz.x / _length; // 벡터의 길이 = 빗변, \\  밑면 / 빗변 = cosTheata

            float _radian = nRandom.z > 0 ? acos(cosTheata) : 2 * PI - acos(cosTheata); // 파티클의 방출 각도. 
            
            _radian = _radian * (shape.Arc / (2 * PI)); // 변환.
            
            nRandom.x = cos(_radian) * _length;
            nRandom.z = sin(_radian) * _length;
            
            float2 radiusVec = nRandom.xz;
            
            float posLength = length(nRandom.xz * shape.Radius);
			
            posLength = shape.RadiusThickness * shape.Radius + (1.0f - shape.RadiusThickness) * posLength;
			
            float2 pos = normalize(nRandom.xz) * posLength;
            
            InitialPosW = mul(float4(pos.x, 0, pos.y, 1), commonInfo.TransformMatrix).xyz;

            float4x4 temp = commonInfo.TransformMatrix;
            temp[3] = float4(0, 0, 0, 1.0f);
            InitialVelW = mul(float4(nRandom.x, 0, nRandom.z, 1.0f), temp).xyz * speed;
        }
        else if (ParticleFlag & Use_Shape_Rectangle)
        {
            InitialPosW = mul(float4((float3(vUnUnitRandom2.x, 0, vUnUnitRandom2.z)).xyz, 1), commonInfo.TransformMatrix).xyz;
        }
        else // Edge
        {
            if (shape.EdgeMode & 1 << 0) // Random
            {
                InitialPosW = mul(float4(vRandom1.x * shape.Radius, 0, 0, 1.0f), commonInfo.TransformMatrix).xyz;
            }
            else if (shape.EdgeMode & 1 << 1) // Loop
            {
                float x = fmod(commonInfo.ParticlePlayTime * shape.Speed, shape.Radius * 2);
                
                x -= shape.Radius;
                
                InitialPosW = mul(float4(x, 0, 0, 1.0f), commonInfo.TransformMatrix).xyz;
            }
            else if (shape.EdgeMode & 1 << 2) // PingPong
            {
                float time = commonInfo.ParticlePlayTime * shape.Speed;
                
                float x;
                
                if ((int) fmod(time / shape.Radius * 2, 2) == 0)
                {
                    x = fmod(time, shape.Radius * 2);
                    x -= shape.Radius;
                }
                else
                {
                    x = fmod(time, shape.Radius * 2);
                    
                    x = shape.Radius - x;
                }
                
                InitialPosW = mul(float4(x, 0, 0, 1.0f), commonInfo.TransformMatrix).xyz;
            }
            else if (shape.EdgeMode & 1 << 3) // BurstSpread
            {
                InitialPosW = mul(float4(shape.Radius, 0, 0, 1.0f), commonInfo.TransformMatrix).xyz;
            }
            else
            {
                // except
            }
        }

    }
    else
    {
        float speed = lerp(commonInfo.StartSpeed[0], commonInfo.StartSpeed[1], vunsignedRandom2.x);

        InitialPosW = float3(commonInfo.TransformMatrix[3][0], commonInfo.TransformMatrix[3][1], commonInfo.TransformMatrix[3][2]); /*gCommonInfo.gEmitPosW.xyz;*/
        InitialVelW = speed * float3(commonInfo.TransformMatrix[1][0], commonInfo.TransformMatrix[1][1], commonInfo.TransformMatrix[1][2]); /* gCommonInfo.gEmitDirW;*/
    }

}

void ManualVelocityOverLifeTime(ParticleStruct particleIn, float deltaTime, out ParticleStruct particelOut)
{
    if (ParticleFlag & Use_VelocityOverLifetime)
    {
        float ratio = particleIn.AgeLifeTimeRotationGravity.x / particleIn.AgeLifeTimeRotationGravity.y;

        float3 velocity = float3(0, 0, 0);

        velocity = lerp(0, velocityOverLifetime.Velocity, ratio);

        particleIn.PosW += velocity * deltaTime;

        Orbital(particleIn.PosW, particleIn.InitEmitterPos.xyz, particleIn.VelW.xyz, deltaTime, particleIn.VelW.xyz, particleIn.PosW);
    }

    particelOut = particleIn;
}

void ManualLimitVelocityOverLifeTime(float ratio, float3 VelWIn, out float3 VelWOut)
{
    if (ParticleFlag & Use_LimitVelocityOverLifetiem)
    {
        float t = ratio;
        float s = 1.0f - t;

        float2 speed = pow(s, 3) * limitVelocityOverLifetime.PointA
			+ 3 * pow(s, 2) * t * limitVelocityOverLifetime.PointB
			+ 3 * s * pow(t, 2) * limitVelocityOverLifetime.PointC
			+ pow(t, 3) * limitVelocityOverLifetime.PointD;
        
        speed = clamp(speed, 0, 1);
        
        speed.y *= limitVelocityOverLifetime.Speed;
        
        float nowSpeed = length(VelWIn);
        
        if (nowSpeed > speed.y)
        {
            float overSpeed = nowSpeed - speed.y;
            nowSpeed -= overSpeed * limitVelocityOverLifetime.Dampen;
            VelWIn = nowSpeed * normalize(VelWIn);
        }
    }

    VelWOut = VelWIn;
}

void ManualForceOverLifeTime(float3 velW, float deltaTime, out float3 velWOut)
{
    if (ParticleFlag & Use_ForceOverLifetime)
    {
        float3 force = float3(0, 0, 0);

        velWOut = velW + forceOverLifeTime.Force * deltaTime;
    }
    else
    {
        velWOut = velW;
    }
}

void ManualColorOverLifeTime(float ratio, float4 alpha_Ratio[8], float4 color_Ratio[8], out float4 color)
{
    if (ParticleFlag & Use_ColorOverLifetime)
    {
        SetColorOverLifeTime(ratio, colorOverLifetime.AlphaRatio, colorOverLifetime.ColorRatio, color);
    }
    else
    {
        color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    }

}

void ManualSizeOverLifeTime(float ratio, out float2 size)
{
    if (ParticleFlag & Use_SizeOverLifetime)
    {
        float t = ratio;
        float s = 1.0f - t;

        size = pow(s, 3) * sizeOverLifetime.PointA
			+ 3 * pow(s, 2) * t * sizeOverLifetime.PointB
			+ 3 * s * pow(t, 2) * sizeOverLifetime.PointC
			+ pow(t, 3) * sizeOverLifetime.PointD;

        size.x = size.y;
        size.y = size.y;
        
        size = clamp(size, 0, 1);
    }
    else
    {
        size = float2(1.0f, 1.0f);
    }

}

void ManualRoationOverLifeTime(float originRotate, float angularVelocity, float lifetime, float deltaTime, out float rotateOut)
{
    if (ParticleFlag & Use_RotationOverLifetime)
    {
        rotateOut = originRotate + angularVelocity / lifetime * deltaTime;
    }
    else
    {
        rotateOut = originRotate;
    }
}

void ManualNoise(float2 sizeW, float3 posW, float rotW,
	Texture2D noiseMap, SamplerState sam, float deltaTime, float gamePlayTime,
	out float2 size, out float3 pos, out float rot)
{
    if (ParticleFlag & Use_Noise)
    {
        float eps = 0.01f;
        float3 curl0;
        float3 curl1;
        float3 curl2;

        float noiseVal;

        float scrollOffset = noise.ScrollSpeed * gamePlayTime;

		{
            float3 uv = float3(posW.x * 0.333f, posW.y * 0.333f, posW.z * 0.333f);

            uv.y += scrollOffset;

            noiseVal = TextureSample(noiseMap, sam, float2(uv.x, uv.y)).x;

            float a = 0;
            float b = 0;
            float c = 0;
            int count = 1;

            while (count < 10 && abs(a) < 0.0001f)
            {
                float n1 = TextureSample(noiseMap, sam, float2(uv.x + eps * count, uv.y)).x;
                float n2 = TextureSample(noiseMap, sam, float2(uv.x - eps * count, uv.y)).x;

                a = (n1 - n2);
                count++;
            }

            count = 1;
            while (count < 10 && abs(b) < 0.0001f)
            {
                float n1 = TextureSample(noiseMap, sam, float2(uv.x, uv.y + eps * count)).x;
                float n2 = TextureSample(noiseMap, sam, float2(uv.x, uv.y - eps * count)).x;

                b = (n1 - n2);
                count++;
            }

            count = 1;
            while (count < 10 && abs(c) < 0.0001f)
            {
                float n1 = TextureSample(noiseMap, sam, float2(uv.x + eps * count, uv.z + eps * count)).x;
                float n2 = TextureSample(noiseMap, sam, float2(uv.x - eps * count, uv.z - eps * count)).x;

                c = (n1 - n2);
                count++;
            }

            curl0 = float3(b, -a, c);

            curl0 = normalize(curl0);
        }


        float3 curl = curl0;

        pos = posW + curl * deltaTime * noise.Strength * noise.PositionAmount;

        size = sizeW + ((noiseVal * 2) - 1) * deltaTime * noise.SizeAmount * noise.Strength * noise.SizeAmount;

        rot = rotW + ((noiseVal * 2) - 1) * deltaTime * noise.RotationAmount * noise.Strength * noise.RotationAmount;

    }
    else
    {
        size = sizeW;
        pos = posW;
        rot = rotW;
    }
}

void ManualCollision(float3 posW, float3 velW, float deltaTime, float age, float lifeTime, out float3 posWout, out float3 velWout, out float ageOut)
{
    if (ParticleFlag & Use_Collision)
    {
        ageOut = age;
		[unroll]
        for (int i = 0; i < collision.PlaneCount; i++)
        {
            float3 planeToPos = posW - collision.PlanePosition[i].xyz;

            float t = dot(planeToPos, collision.PlaneNormalVec[i].xyz);

            if (t <= 0) // 충돌!
            {

                velW = collision.Bounce * 2 * dot(-velW, collision.PlaneNormalVec[i].xyz) * collision.PlaneNormalVec[i].xyz + velW;
				//velW = reflect(velW, (gCollision.gPlanNormalVec[i].xyz * gCollision.gBoundce));
                posW += velW * deltaTime;

                planeToPos = posW - collision.PlanePosition[i].xyz;

                t = dot(planeToPos, collision.PlaneNormalVec[i].xyz);

                if (t <= 0)
                {
                    posW += dot(-planeToPos, collision.PlaneNormalVec[i].xyz) * collision.PlaneNormalVec[i].xyz;
                }

                ageOut = age + lifeTime * collision.LifeTimeLoss;

            }
        }

        velWout = velW;
        posWout = posW;
    }
    else
    {
        velWout = velW;
        posWout = posW;
        ageOut = age;
    }

}

void ManualTextureSheetAnimation(float4 vunsignedRandom4, out float2 QuadTexC[4])
{
    if (ParticleFlag & Use_TextureSheetAnimation)
    {
        int2 texIndex;
        float xStep = 1.0f / (float) textureSheetAnimation.GridXY.x; // 0.5f
        float yStep = 1.0f / (float) textureSheetAnimation.GridXY.y; // 0.5f
        if (textureSheetAnimation.TimeMode == 1)
        {
            int totalCount = textureSheetAnimation.GridXY.x * textureSheetAnimation.GridXY.y;
        
            int nowIndex = 0;
        
            texIndex.x = 0;
            texIndex.y = 0;
        }
        else
        {
            texIndex.x = lerp(1, textureSheetAnimation.GridXY[0] + 1, vunsignedRandom4.x);
            texIndex.y = lerp(1, textureSheetAnimation.GridXY[1] + 1, vunsignedRandom4.y);
        }
        float xMax = xStep * texIndex.x;
        float yMax = yStep * texIndex.y;
        float xMin = xStep * (texIndex.x - 1);
        float yMin = yStep * (texIndex.y - 1);

        QuadTexC[0] = float2(xMin, yMax); // lefttop
        QuadTexC[1] = float2(xMin, yMin); //leftbottom
        QuadTexC[2] = float2(xMax, yMax); // righttop
        QuadTexC[3] = float2(xMax, yMin); // ritghtbottom
    }
    else
    {
        QuadTexC[0] = float2(0.0f, 1.0f); // lefttop
        QuadTexC[1] = float2(0.0f, 0.0f); //leftbottom
        QuadTexC[2] = float2(1.0f, 1.0f); // righttop
        QuadTexC[3] = float2(1.0f, 0.0f); // ritghtbottom
    }

}

void ManualTextureSheetAnimationForLifetime(float2 QuadTexCIn[4], float4 ratio, out float2 QuadTexCOut[4])
{
    if (ParticleFlag & Use_TextureSheetAnimation && textureSheetAnimation.TimeMode == 1)
    {
        
        float xStep = 1.0f / (float) textureSheetAnimation.GridXY.x; // 0.5f
        float yStep = 1.0f / (float) textureSheetAnimation.GridXY.y; // 0.5f
        
        int totalCount = textureSheetAnimation.GridXY.x * textureSheetAnimation.GridXY.y;
        
        int nowIndex = (int) (ratio * (float) totalCount);
        
        uint2 texIndex;
        
        texIndex.x = nowIndex % textureSheetAnimation.GridXY.x + 1;
        texIndex.y = nowIndex / textureSheetAnimation.GridXY.x + 1;


        float xMax = xStep * texIndex.x;
        float yMax = yStep * texIndex.y;
        float xMin = xStep * (texIndex.x - 1);
        float yMin = yStep * (texIndex.y - 1);

        QuadTexCIn[0] = float2(xMin, yMax); // lefttop
        QuadTexCIn[1] = float2(xMin, yMin); //leftbottom
        QuadTexCIn[2] = float2(xMax, yMax); // righttop
        QuadTexCIn[3] = float2(xMax, yMin); // ritghtbottom
    }
    QuadTexCOut = QuadTexCIn; // lefttop
}

void ManualTrail(float4 prevPosIn[TrailCount], float recordTime, float3 posW,
out float recordTimeOut,
out float4 prevPosOut[TrailCount])
{
    if (ParticleFlag & Use_Trails)
    {

        if (!(trails.TrailsFlag & Use_TrailFlag_WorldSpace) && !(ParticleFlag & Use_Commoninfo_WorldSpace))
        {
			[unroll]
            for (int i = 0; i < TrailCount; i++)
            {
                prevPosIn[i].w = 1.0f;
                prevPosIn[i] = mul(prevPosIn[i], commonInfo.DeltaMatrix);
            }
        }

        if (trails.Condition & 1 << 0) // and
        {
            if (distance(prevPosIn[0].xyz, posW) >= trails.MinimumVertexDistance && recordTime >= trails.RecordTime)
            {
			[unroll]
                for (int i = TrailCount - 2; i >= 0; i--)
                {
                    prevPosIn[i + 1] = prevPosIn[i];
                }
                prevPosIn[0] = float4(posW, 1.0f);
                recordTime = 0;
            }
        }
        else if (trails.Condition & 1 << 1) // or
        {
            if (distance(prevPosIn[0].xyz, posW) >= trails.MinimumVertexDistance || recordTime >= trails.RecordTime)
            {
			[unroll]
                for (int i = TrailCount - 2; i >= 0; i--)
                {
                    prevPosIn[i + 1] = prevPosIn[i];
                }
                prevPosIn[0] = float4(posW, 1.0f);
                recordTime = 0;
            }
        }
        prevPosOut = prevPosIn;
        recordTimeOut = recordTime;
    }
    else
    {
        recordTimeOut = recordTime;
        prevPosIn = prevPosOut;
    }
}


// 각종 인터페이스의 전역 변수 => 해당부분은 이름을 바꾸지 않겠습니다..
IParticleInterFace_Shape g_shapeInstance; // 0
IParticleInterFace_VelocityOverLifeTime g_velocityOverLifeTimeInstance; // 1
IParticleInterFace_ForceOverLifeTime g_forceOverLifeTimeInstance; // 2
IParticleInterFace_ColorOverLifeTime g_colorOverLifeTimeInstance; // 3
IParticleInterFace_SizeOverLifeTime g_sizeOverLifeTimeInstance; // 4
IParticleInterFace_RoationOverLifeTime g_roationOverLifeTimeInstance; // 5
IParticleInterFace_Noise g_noiseInstance; // 6
IParticleInterFace_Collision g_collisionInstance; // 7
IParticleInterFace_TextureSheetAnimation g_textureSheetAnimationInstance; // 8
IParticleInterFace_Trails g_trails; // 9

void SetColorOverLifeTime(float ratio, float4 alpha_Ratio[8], float4 color_Ratio[8],
	out float4 color)
{
    float4 startColor = float4(color_Ratio[0].xyz, alpha_Ratio[0].x);
    float4 endColor = float4(color_Ratio[0].xyz, alpha_Ratio[0].x);
    float startTime = 0.0f;
    float endTime = 1.0f;

    color = float4(1.0f, 1.0f, 1.0f, 1.0f);

	[unroll]
    for (int i = 0; i < 8; i++)
    {
        if (ratio < alpha_Ratio[i].w)
        {
            endColor.a = alpha_Ratio[i].x;
            endTime = alpha_Ratio[i].w;

            if (i != 0)
            {
                startColor.a = alpha_Ratio[i - 1].x;
                startTime = alpha_Ratio[i - 1].w;
            }
            else
            {
                startColor.a = alpha_Ratio[i].x;
                startTime = 0.0f;
            }
            break;
        }
        else if (1.0f <= alpha_Ratio[i].w)
        {
            startColor.a = alpha_Ratio[i - 1].x;
            startTime = alpha_Ratio[i - 1].w;
            endColor.a = alpha_Ratio[i].x;
            endTime = 1.0f + 0.00001f;
            break;
        }
        else if (i == 7)
        {
            startColor.a = alpha_Ratio[i].x;
            startTime = alpha_Ratio[i].w;
            endColor.a = alpha_Ratio[i].x;
            endTime = 1.0f + 0.00001f;
        }
    }
    float factor = 1.0f / (endTime - startTime);
    factor = (ratio - startTime) * factor;
    color.a = lerp(startColor.a, endColor.a, factor);

	[unroll]
    for (i = 0; i < 8; i++)
    {
        if (ratio < color_Ratio[i].w)
        {
            endColor.xyz = color_Ratio[i].xyz;
            endTime = color_Ratio[i].w;

            if (i != 0)
            {
                startColor.xyz = color_Ratio[i - 1].xyz;
                startTime = color_Ratio[i - 1].w;
            }
            else
            {
                startColor.xyz = color_Ratio[i].xyz;
                startTime = 0.0f;
            }
            break;
        }
        else if (1.0f <= color_Ratio[i].w)
        {
            startColor.xyz = color_Ratio[i - 1].xyz;
            startTime = color_Ratio[i - 1].w;
            endColor.xyz = color_Ratio[i].xyz;
            endTime = 1.0f + 0.00001f;
            break;
        }
        else if (i == 7)
        {
            startColor.xyz = color_Ratio[i].xyz;
            startTime = color_Ratio[i].w;
            endColor.xyz = color_Ratio[i].xyz;
            endTime = 1.0f + 0.00001f;
        }
    }
    factor = 1.0f / (endTime - startTime);
    factor = (ratio - startTime) * factor;
    color.xyz = lerp(startColor.xyz, endColor.xyz, factor);
}

void Orbital(float3 posW, float3 InitEmitterPos, float3 velW, float deltaTime, out float3 velW_Out, out float3 posW_Out)
{
    float3 axis = velocityOverLifetime.Orbital;

    if (length(axis) == 0 || !(ParticleFlag & Use_VelocityOverLifetime))
    {
    }
    else
    {
        float3 n_axis = normalize(axis);

        float3 orbitalCenterPos = dot(posW, n_axis) * n_axis + InitEmitterPos; // 파티클이 향하는 방향!

        float3 offset = posW - InitEmitterPos;

        float power = length(axis);

        float angle = deltaTime * power;

        float4 rotateQuat = rotate_angle_axis(angle, n_axis);

        float3 rotated = rotate_vector(offset, rotateQuat);

        velW = rotate_vector(velW, rotateQuat);

        posW = rotated + InitEmitterPos;

		// angle == t;
        if (velocityOverLifetime.IsGravity != 0)
        {
            float3 graivtDir = orbitalCenterPos - posW;

            float3 n_graivtDir = normalize(graivtDir) * power;

            velW += n_graivtDir * deltaTime;
        }
            
    }
    velW_Out = velW;
    posW_Out = posW;

}

void SetBillBoard(
	float3 cameraPos, ParticleStruct particle,
	out float3 look, out float3 right, out float3 up)
{
    
    if (particleRenderer.RenderAlignment & 1 << 0) // View
    {
        look = normalize(cameraPos - particle.PosW);
    }
    else if (particleRenderer.RenderAlignment & 1 << 1) // World
    {
        look = float3(1.0f, 0, 0);
    }
    else if (particleRenderer.RenderAlignment & 1 << 2) // Local
    {
        look = commonInfo.TransformMatrix[0].xyz;
    }
    else if (particleRenderer.RenderAlignment & 1 << 3) // Velocity
    {
        float3 direction = particle.LatestPrevPos.xyz - particle.PosW;
        look = normalize(direction);
    }
    else
    {
        look = normalize(cameraPos - particle.PosW);
    }
    
    if (ParticleFlag & Use_Renderer_HorizontalBillBoard)
    {
        look = float3(0, 1, 0);
    }
    else if (ParticleFlag & Use_Renderer_VerticalBillBoard)
    {
        look.y = 0;
        look = normalize(look);
    }
    
    if (0 > dot(look, cameraPos - particle.PosW))
    {
        look = -look;
    }
    
    if (ParticleFlag & Use_Renderer_BillBoard)
    {
		// 카메라 방향으로 룩엣
        right = normalize(cross(float3(0, 1, 0), look));
        up = normalize(cross(look, right));
    }
    else if (ParticleFlag & Use_Renderer_StretchedBillBoard)
    {
        float3 direction = particle.PosW - particle.LatestPrevPos.xyz;

        up = normalize(direction) * (particleRenderer.LengthScale + length(direction) * particleRenderer.SpeedScale);
        right = normalize(cross(up, look));
    }
    else if (ParticleFlag & Use_Renderer_HorizontalBillBoard)
    {
        right = float3(-1, 0, 0);
        up = float3(0, 0, 1);
    }
    else if (ParticleFlag & Use_Renderer_VerticalBillBoard)
    {
        right = normalize(cross(float3(0, 1, 0), look));
        up = float3(0, 1, 0);
    }
    else if (ParticleFlag & Use_Renderer_Mesh)
    {
		// 어떻게 구현하는걸까...
    }
    else
    {
		// 카메라 방향으로 룩엣
        right = normalize(cross(float3(0, 1, 0), look));
        up = normalize(cross(look, right));
    }

    const float costheta = cos(particle.AgeLifeTimeRotationGravity.z);
    const float sintheta = sin(particle.AgeLifeTimeRotationGravity.z);
    const float OneMinusCos = 1.0f - costheta;

    const float X2 = pow(look.x, 2);
    const float Y2 = pow(look.y, 2);
    const float Z2 = pow(look.z, 2);

    float4x4 RotationTM;

    RotationTM[0] = float4(X2 * OneMinusCos + costheta, look.x * look.y * OneMinusCos + look.z * sintheta, look.x * look.z * OneMinusCos - look.y * sintheta, 0);
    RotationTM[1] = float4(look.x * look.y * OneMinusCos - look.z * sintheta, Y2 * OneMinusCos + costheta, look.y * look.z * OneMinusCos + look.x * sintheta, 0);
    RotationTM[2] = float4(look.x * look.z * OneMinusCos + look.y * sintheta, look.y * look.z * OneMinusCos - look.x * sintheta, Z2 * OneMinusCos + costheta, 0);
    RotationTM[3] = float4(0, 0, 0, 1);

    right = mul(float4(right, 1.0f), RotationTM).xyz;
    up = mul(float4(up, 1.0f), RotationTM).xyz;
}
