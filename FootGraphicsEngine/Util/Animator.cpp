#include "GraphicsPch.h"
#include "Animator.h"

namespace GraphicsEngineSpace
{
	Animator::Animator(int boneSize)
		: nowAnimClip(nullptr)
		, nowKeyFrameIdx(0)
		, isLoop(true)
		, isPlay(true)
		, nowFrameTime(1000.f)
		, animDeltaTime(0.f)
	{
		// �ش� obj�� ������ �ִ� �� ũ�� ��ŭ�� Matrix vector�� �������ش�.
		modifiedBoneTM.assign(boneSize, SimpleMath::Matrix::Identity);

		blendBoneTM.assign(boneSize, SimpleMath::Matrix::Identity);

		// Parameter ����
		animatorParameter = std::make_shared<AnimatorParmeter>();

		// Any State ����
		anyAnimatorState = std::make_shared<AnimatorState>();
	}

	Animator::~Animator()
	{
		if (animResources != nullptr)
		{
			for (auto animClip : animResources->animationClipList)
			{
				SafeReset(animClip.second);
			}
		}

		SafeReset(animResources);
	}

	std::vector<SimpleMath::Matrix> Animator::UpdateBoneTM(float tick)
	{
		// ����� 02-20 ����
		// Aniamtor ���� & Aniamtion blending

		// Any State
		// ���Ǹ� �����ϸ� ���� State�� ���������� ���¸� �ٲٴ� ��.. 
		// ���� -> Idle Ʈ���� ���� -> currentStateName�� Idle�� �� �ٵ� cureentAniamtorState(Attack)�� �ٲ�������ä�� �ٽ� ���������� -> ���� Ʈ���� ����
		// -> AnyState�� ������ֱ⿡ currentStateName�� Attack�� �� �׷��� state��
		// ��ȯ�� �Ǿ��ٸ� �̶�� if���� �ȵ��� ���� �ִϸ��̼� + nowKeyFrameIdx�� �ʱ�ȭ�� �ȵ� (���� ���� �İ� ���� ���)
		currentStateName = anyAnimatorState->UpdateState(animatorParameter, this);

		if(currentStateName.empty())
			return std::vector<SimpleMath::Matrix>();

		// state�� ��ȯ�� �Ǿ��ٸ�
		if (currentStateName.compare(currentAnimatorState->GetStateName()) != 0)
		{
			// isPlay = true;

			// transitionDuration �� ���� blending �� �Ѵ�.
			if (currentTransitionTime > 0.0f)
			{
				anyAnimatorState->SetStateName(currentStateName);

				// ���� ��ȯ �ð��� ����
				currentTransitionTime -= tick;

				// blending �� �غ���..
				const float blendFactor = 1.0f - max(currentTransitionTime / currentTransitionDuration, 0.0f);

				// ������ bone ���
				auto curBoneTM = modifiedBoneTM;
				
				KeyFrame nextStateStartFrame;

				nowAnimClip = animResources->animationClipList[states[currentStateName]->GetMotion()];

				for (size_t i = 0; i < blendBoneTM.size(); i++)
				{
					// �ִϸ��̼� Ŭ���� �ش� Ű�����ӿ� ����
					nextStateStartFrame = nowAnimClip->keyFrames[i][0];

					// (SRT)
					SimpleMath::Vector3 curPos, nextPos, blendedPos;
					SimpleMath::Quaternion curRot, nextRot, blendedRot;
					SimpleMath::Vector3 curScale, nextScale, blendedScale;

					// ���� bone ��Ŀ��� pos, rot, scale ���� ����
					curBoneTM[i].Decompose(curScale, curRot, curPos);

					// (SRT)
					nextPos = nextStateStartFrame.framePos;
					nextRot = nextStateStartFrame.frameRot;
					nextScale = nextStateStartFrame.frameScale;

					// ������ pos, rot, scale ���� ���
					blendedPos = XMVectorLerp(curPos, nextPos, blendFactor);
					blendedRot = XMQuaternionSlerp(curRot, nextRot, blendFactor);
					blendedScale = XMVectorLerp(curScale, nextScale, blendFactor);

					// ������ pos, rot, scale ������ ���ο� bone ��� ����
					SimpleMath::Matrix nextStateStartFrameTM = SimpleMath::Matrix::CreateScale(blendedScale)
						* SimpleMath::Matrix::CreateFromQuaternion(blendedRot)
						* SimpleMath::Matrix::CreateTranslation(blendedPos);

					blendBoneTM[i] = nextStateStartFrameTM;
				}

				return blendBoneTM;
			}
			else
			{
				// ���� ���¸� �������ش�.
				currentAnimatorState = states[currentStateName];

				anyAnimatorState->SetStateName(currentStateName);

				nowKeyFrameIdx = 0;

				currentTransitionTime = 0.0f;

				// ���� �� ���� �ִϸ��̼� �ٷ� return ����
				nowFrameTime = 100.0f;
				
				return UpdateFrame(tick);
			}
		}
		// ��ȯ�� �ȵǾ������
		else
		{	
			currentStateName = currentAnimatorState->UpdateState(animatorParameter, this);

			anyAnimatorState->SetStateName(currentStateName);

			if (currentAnimatorState->operateTransition != nullptr)
			{
				currentTransitionTime = currentAnimatorState->operateTransition->GetTransitionDuration();

				currentTransitionDuration = currentTransitionTime;

				currentAnimatorState->operateTransition = nullptr;
			}

			if(currentStateName != currentAnimatorState->GetStateName())
				return modifiedBoneTM;

			return UpdateFrame(tick);
		}
	}

	// ���������� ���͸� ������ �־�� �ش� ������ ���� ������� �ʰ� ���� �ȴ�.
		// ���, ���� �� Matrix ������ �߰����ش�.

	// �ϴ� ���� ������ Animation�� �����غ���.
		// Frame Rate�� TickPerFrame ���� ���� �������� �ʴ´�.

	// �⺻������ �ִϸ��̼���
		// ���� FrameTime�� delta Tick�� ��� �����ָ鼭
		// FrameTime ���� ���� prev KeyFrame��
		// FrameTime ���� ū next KeyFrame�� �����ؼ� ���� TM�� ������.
	std::vector<SimpleMath::Matrix> Animator::UpdateFrame(float tick)
	{
		nowAnimClip = animResources->animationClipList[currentAnimatorState->GetMotion()];

		isLoop = currentAnimatorState->GetLoop();

		// ��� ���� �ľ�
		if (/*isPlay != true ||*/ nowAnimClip == nullptr)
			return modifiedBoneTM;

		nowFrameTime += tick * currentAnimatorState->GetSpeed();		

		// ���� �� ����� ���� idle->run �Ҷ� ��� idle ��¦�ϴ� ����..
		if (nowFrameTime < 1.0f / 60.f)
			return modifiedBoneTM;

		nowFrameTime = 0.f;
		animDeltaTime += tick * currentAnimatorState->GetSpeed();

		KeyFrame nowKeyFrame;
		KeyFrame nextKeyFrame;

		// ������ Ű������ üũ, ���� üũ
		if (nowKeyFrameIdx == nowAnimClip->endKeyFrame)
		{
			if (isLoop == true)
				nowKeyFrameIdx = nowAnimClip->startKeyFrame;
			else
			{
				// isPlay = false;
				nowKeyFrameIdx = nowAnimClip->endKeyFrame;
				return modifiedBoneTM;	// ������ �����ӿ��� ����.
			}
		}

		// ������ �ʿ��ϴ�.
		float currentTick = nowKeyFrameIdx * nowAnimClip->tickPerFrame;
		float t = 0.f;

		// ���� �ð��� ����ȭ�Ͽ� ���Ѵ�.
		currentAnimatorState->normalizedTime = currentTick / nowAnimClip->totalTime;

		// �ִϸ��̼� Ŭ���� ��� Ű�������� ���鼭..
			// cf)AnimationClip�� ����
			// ���ο� KeyFrames�� �ٽ�
			// n������ m��° Frame�� �������.
		// �� ��� ���� ������ �߰��ؾ� �ϱ� ������.. ��� ���� ���������
		for (size_t i = 0; i < modifiedBoneTM.size(); i++)
		{
			// �ִϸ��̼� Ŭ���� �ش� Ű�����ӿ� ����
			nowKeyFrame = nowAnimClip->keyFrames[i][nowKeyFrameIdx];
			
			// ������ ������ üũ
			if (nowKeyFrameIdx + 1 >= nowAnimClip->endKeyFrame)
			{
				if (isLoop == true)
					nextKeyFrame = nowAnimClip->keyFrames[i][nowAnimClip->startKeyFrame];
				else
					nextKeyFrame = nowAnimClip->keyFrames[i][nowKeyFrameIdx];
			}
			else
				nextKeyFrame = nowAnimClip->keyFrames[i][nowKeyFrameIdx + 1];

			// TODO ���� �� Matrix�� ���� �־��ش�.
			if (nextKeyFrame.time == nowKeyFrame.time)
				t = 1.f;
			else
			{
				//t = (currentTick - nowKeyFrame.time) / (nextKeyFrame.time - nowKeyFrame.time);

				t = (animDeltaTime - nowKeyFrame.time) / (nextKeyFrame.time - nowKeyFrame.time);

				if (t >= 1.f)
					t = 1.f;
			}

			// 0 ~ 1.0f �� �ǵ���..
			// now 0.4time next 0.6time �̶�� �������� tickperframe�� 0.04..
			// ��·�� ��ŸŸ���� �����༭ 0.6�� �Ǳ������� ������ �ִϸ��̼��� ���̰� 0.6�� �Ǹ� 
			// nowKeyFrameIdx++ �� ��������ݾ�? �ٵ� ���� tickPerFrame�� ���ʿ� ��û �۾Ƽ�.. 1�ۿ��ȳ����� ��

			// (SRT)
			SimpleMath::Vector3 pos = XMVectorLerp(nowKeyFrame.framePos, nextKeyFrame.framePos, t);
			SimpleMath::Quaternion rot = XMQuaternionSlerp(nowKeyFrame.frameRot, nextKeyFrame.frameRot, t);
			SimpleMath::Vector3 scale = XMVectorLerp(nowKeyFrame.frameScale, nextKeyFrame.frameScale, t);

			SimpleMath::Matrix nowTM = SimpleMath::Matrix::CreateScale(scale)
				* SimpleMath::Matrix::CreateFromQuaternion(rot)
				* SimpleMath::Matrix::CreateTranslation(pos);

			// i��° ���� �ش� TM�� �־��ش�.
			modifiedBoneTM[i] = nowTM;
		}

		// KeyFrame TimeCheck (������ next KeyFrame�� ���Դٰ� ����.)
			// ���� keyFrame�� Time���� nowFrameTime�� ũ�ٸ�.
			// ���� Ű���������� �Ѱ��ش�.
		if (animDeltaTime >= nextKeyFrame.time)
		{
			nowKeyFrameIdx++;
			animDeltaTime = nextKeyFrame.time;
		}

		//nowKeyFrameIdx++;

		return modifiedBoneTM;
	}

	void Animator::PlayAnim()
	{
		isPlay = true;
	}

	/// <summary>
	/// �ִϸ��̼� ���� ����� ���뼺 ����
	/// </summary>
	void Animator::SetAnimationClip(std::string animName)
	{
		nowAnimClip = animResources->animationClipList[animName];
	}

	void Animator::SetNowFrame(int val)
	{
		if (val > nowAnimClip->totalKeyFrame)
		{
			nowKeyFrameIdx = nowAnimClip->totalKeyFrame;
			return;
		}

		nowKeyFrameIdx = val;
	}

	int Animator::GetTotalFrame()
	{
		return nowAnimClip->totalKeyFrame;
	}

	std::string Animator::GetCurrentAnimatorStateName()
	{
		return currentAnimatorState->GetStateName();
	}

	void Animator::SetAnimatorState(const std::string& stateName, std::shared_ptr<AnimatorState> animatorState)
	{
		states.insert(std::make_pair(stateName, animatorState));
	}

	void Animator::ChangeAnimatorStateSpeed(const std::string& stateName, float speed)
	{
		states[stateName]->SetSpeed(speed);
	}

	void Animator::AddCondition(const std::string& transitionName, const std::string& parameterName, const std::string& conditionName, float val)
	{
		std::string delimiter = "->";
		std::string stateName = transitionName.substr(0, transitionName.find(delimiter));

		Condition condition;
		condition.parameterName = parameterName;
		condition.floatVal = val;
		
		if(conditionName == "Greater")
			condition.type = ConditionType::Greater;
		else if(conditionName == "Less")
			condition.type = ConditionType::Less;

		if(stateName == "AnyState")
			anyAnimatorState->GetTransition(transitionName)->AddCondition(condition);
		else
			states[stateName]->GetTransition(transitionName)->AddCondition(condition);
	}

	void Animator::EntryAnimatorState(const std::string& name)
	{
		currentStateName = name;

		// any���� ���� �̸��� �־��ش�.
		anyAnimatorState->SetStateName(name);

		currentAnimatorState = states[name];
	}

	void Animator::AddCondition(const std::string& transitionName, const std::string& parameterName, const std::string& conditionName, int val)
	{
		std::string delimiter = "->";
		std::string stateName = transitionName.substr(0, transitionName.find(delimiter));

		Condition condition;
		condition.parameterName = parameterName;
		condition.intVal = val;

		if (conditionName == "Greater")
			condition.type = ConditionType::Greater;
		else if (conditionName == "Less")
			condition.type = ConditionType::Less;
		else if(conditionName == "Equlals")
			condition.type = ConditionType::Equals;
		else if(conditionName == "NotEqual")
			condition.type = ConditionType::NotEqual;

		if (stateName == "AnyState")
			anyAnimatorState->GetTransition(transitionName)->AddCondition(condition);
		else
			states[stateName]->GetTransition(transitionName)->AddCondition(condition);
	}

	void Animator::AddCondition(const std::string& transitionName, const std::string& parameterName, const std::string& conditionName, bool val)
	{
		std::string delimiter = "->";
		std::string stateName = transitionName.substr(0, transitionName.find(delimiter));

		Condition condition;
		condition.parameterName = parameterName;
		condition.intVal = val;
		condition.type = ConditionType::None;

		if (stateName == "AnyState")
			anyAnimatorState->GetTransition(transitionName)->AddCondition(condition);
		else
			states[stateName]->GetTransition(transitionName)->AddCondition(condition);
	}

	void Animator::AddCondition(const std::string& transitionName, const std::string& parameterName, const std::string& conditionName)
	{
		std::string delimiter = "->";
		std::string stateName = transitionName.substr(0, transitionName.find(delimiter));

		Condition condition;
		condition.parameterName = parameterName;
		condition.type = ConditionType::None;

		if (stateName == "AnyState")
			anyAnimatorState->GetTransition(transitionName)->AddCondition(condition);
		else
			states[stateName]->GetTransition(transitionName)->AddCondition(condition);
	}

	void Animator::AddFloat(const std::string& name, float val)
	{
		animatorParameter->AddFloat(name, val);
	}

	void Animator::AddInteger(const std::string& name, int val)
	{
		animatorParameter->AddInteger(name, val);
	}

	void Animator::AddBool(const std::string& name, bool val)
	{
		animatorParameter->AddBool(name, val);
	}

	void Animator::AddTrigger(const std::string& name)
	{
		animatorParameter->AddTrigger(name);
	}

	void Animator::SetFloat(const std::string& name, float val)
	{
		animatorParameter->SetFloat(name, val);
	}

	void Animator::SetInteger(const std::string& name, int val)
	{
		animatorParameter->SetInteger(name, val);
	}

	void Animator::SetBool(const std::string& name, bool val)
	{
		animatorParameter->SetBool(name, val);
	}

	void Animator::SetTrigger(const std::string& name)
	{
		animatorParameter->SetTrigger(name);
	}
}
