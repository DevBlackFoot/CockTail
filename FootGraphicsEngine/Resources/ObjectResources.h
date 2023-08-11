#pragma once
#include "Define/d3d11Utils.h"
#include "Mesh.h"
#include "RenderingMaterial.h"

namespace GraphicsEngineSpace
{
	class Texture;

	// �� �ϳ��� ������
	struct Bone
	{
		SimpleMath::Matrix offsetTransform;
		SimpleMath::Matrix nodeTransform;
		int parentIdx;

		Bone()
			: offsetTransform(SimpleMath::Matrix::Identity)
			, nodeTransform(SimpleMath::Matrix::Identity)
			, parentIdx(-1)
		{
			
		}

		~Bone() = default;
	};

	// ��Ű�� ���� �� ������.
	struct BoneResources
	{
		bool isSkinnedObj;
		std::vector<Bone> bones;

		// ��� TM�� �̹� ������°�..
		bool setNode;

		BoneResources()
			: isSkinnedObj(false)
			, setNode(false)
		{
			
		}

		~BoneResources() = default;
	};

	// �ִϸ��̼ǿ� ���� ���� �������� KeyFrame ����
		// ������ ���� ���� �Ǿ�� �ϴ� �ִϸ��̼� ����.
	struct KeyFrame
	{
		// Local ������.
		SimpleMath::Vector3 framePos;
		SimpleMath::Quaternion frameRot;
		SimpleMath::Vector3 frameScale;

		float time;

		KeyFrame()
			: framePos(0.f, 0.f, 0.f)
			, frameRot(0.f, 0.f, 0.f, 0.f)
			, frameScale(0.f, 0.f, 0.f)
			, time(0.f)
		{

		}

		~KeyFrame() = default;
	};

	// �ִϸ��̼� Ŭ�� �ϳ��ϳ��� �����ϴ� Struct
	struct AnimationClipResource
	{
		std::string animationName;

		// �ش� Ŭ���� ��� KeyFrame�� �����ص� Vector
		// BoneResources�� ���� �� ������ �ش� ���ҽ��� ���� Ű ������ ������ ��ġ�Ѵ�.
		std::vector<std::vector<KeyFrame>> keyFrames;

		// ������ Rate
		float frameRate;

		// �ִϸ��̼� ù ��° ������
		int startKeyFrame;

		// �ִϸ��̼� �� ������ ��
		int totalKeyFrame;

		// �ִϸ��̼� ������ ������
		int endKeyFrame;

		// �����Ӵ� �� Tick�� ����������
		float tickPerFrame;

		// �ִϸ��̼� �� �ð� (�ϴ� (totalKeyFrame - startKeyFrame) * tickPerFrame) �̷��� ����
		float totalTime;

		AnimationClipResource(const AnimationClipResource& other)
			: animationName(other.animationName)
			, keyFrames(other.keyFrames)
			, frameRate(other.frameRate)
			, startKeyFrame(other.startKeyFrame)
			, totalKeyFrame(other.totalKeyFrame)
			, endKeyFrame(other.endKeyFrame)
			, tickPerFrame(other.tickPerFrame)
			, totalTime(other.totalTime)
		{}

		AnimationClipResource&operator=(const AnimationClipResource& other)
		{
			if (this != &other)
			{
				animationName = other.animationName;
				keyFrames = other.keyFrames;
				frameRate = other.frameRate;
				startKeyFrame = other.startKeyFrame;
				totalKeyFrame = other.totalKeyFrame;
				endKeyFrame = other.endKeyFrame;
				tickPerFrame = other.tickPerFrame;
				totalTime = other.totalTime;
			}
			return *this;
		}

		AnimationClipResource()
			: animationName("NONE")
			, frameRate(0.f)
			, startKeyFrame(0)
			, totalKeyFrame(0)
			, endKeyFrame(0)
			, tickPerFrame(0.f)
			, totalTime(0.f)
		{
			
		}

		~AnimationClipResource() = default;
	};

	// ��Ű�� ���� �ִϸ��̼� ������
		// �ش� Object�� ������ �ִ� ��� �ִϸ��̼� ������ ��Ƶ� Resources
		// Animation Clip���� List�� ������ �ִ�.
		// (�� �ִϸ��̼� Clip�� �̸� ���� ������ ���� Animator���� ���� �����ϵ��� �����.)
	struct AnimationResources
	{
		std::unordered_map<std::string, std::shared_ptr<AnimationClipResource>> animationClipList;

		AnimationResources(const AnimationResources& other)
			: animationClipList(other.animationClipList)
		{}

		AnimationResources& operator=(const AnimationResources& other)
		{
			if (this != &other)
			{
				animationClipList = other.animationClipList;
			}
			return *this;
		}

		AnimationResources()
		{}

		~AnimationResources() = default;
	};

	/**
	* \brief ������Ʈ�� ������ �ִ� ���� ���ҽ���
	*
	* // �ش� ������ ������ ������ ����, ������Ʈ�� ������ ����. �������� �����ؼ� ������Ʈ�� �־��ش�.
	*
	* 2022.07.11(��) Dev.BlackFoot
	*/
	struct MeshResources
	{
		bool setMesh;

		bool setMaterial;

		std::shared_ptr<Mesh> mesh;

		std::vector<std::shared_ptr<RenderingMaterial>> material;	

		// ������Ʈ �̸�
		std::string ObjName;

		// Skinned ���� ���� ���⼭ ������ �ִ´�.
		bool isSkinnedMesh;

		// Shadow Render�� �� obj ����.
		bool isShadow;

		// �� �޽��� ������ ���� ������..
		bool isRender = true;

		bool isLight = true;

		// ���� ������
		MeshResources(const MeshResources& other)
		{
			setMesh = other.setMesh;
			setMaterial = other.setMaterial;
			mesh = other.mesh ? std::make_shared<Mesh>(*other.mesh) : nullptr;
			for (const auto& mat : other.material)
			{
				material.push_back(std::make_shared<RenderingMaterial>(*mat));
			}
			ObjName = other.ObjName;
			isSkinnedMesh = other.isSkinnedMesh;
			isShadow = other.isShadow;
			isRender = other.isRender;
			isLight = other.isLight;
		}

		// ���� ������
		MeshResources& operator=(const MeshResources& other)
		{
			if (this != &other) {
				setMesh = other.setMesh;
				setMaterial = other.setMaterial;
				mesh = other.mesh;
				material.clear();
				for (const auto& mat : other.material)
				{
					material.push_back(std::make_shared<RenderingMaterial>(*mat));
				}
				ObjName = other.ObjName;
				isSkinnedMesh = other.isSkinnedMesh;
				isShadow = other.isShadow;
				isRender = other.isRender;
				isLight = other.isLight;
			}
			return *this;
		}

		MeshResources()
			: setMesh(false)
			, setMaterial(false)
			, ObjName("NULL")
			, isSkinnedMesh(false)
			, isShadow(false)
		{}

		~MeshResources()
		{
			
		}
	};
}