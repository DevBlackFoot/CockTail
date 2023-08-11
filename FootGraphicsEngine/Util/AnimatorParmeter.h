#pragma once
namespace GraphicsEngineSpace
{
	enum class ParameterType
	{
		Float,
		Int,
		Bool,
		Trigger
	};

	struct Parameter
	{
		std::string name;

		ParameterType parameterType;

		float floatValue;
		int intValue;
		bool boolValue;
		bool isTrigger;
	};

	class AnimatorParmeter
	{
	public:
		// AnimatorParameter 복사 생성자
		AnimatorParmeter(const AnimatorParmeter& other)
		{
			this->parameters = other.parameters;
		}

		// AnimatorParameter 대입 연산자
		AnimatorParmeter& operator=(const AnimatorParmeter& other)
		{
			this->parameters = other.parameters;

			return *this;
		}

	public:
		AnimatorParmeter();
		~AnimatorParmeter();

		std::unordered_map<std::string, Parameter> parameters;

	public:
		float GetFloat(const std::string& name) { return parameters[name].floatValue; }
			
		int GetInteger(const std::string& name) { return parameters[name].intValue; }

		bool GetBool(const std::string& name) { return parameters[name].boolValue; }
		
		void SetFloat(const std::string& name, float value);

		void SetInteger(const std::string& name, int value);

		void SetBool(const std::string& name, bool value);

		void SetTrigger(const std::string& name);
		
		void AddFloat(const std::string& name, float value);

		void AddInteger(const std::string& name, int value);

		void AddBool(const std::string& name, bool value);

		void AddTrigger(const std::string& name);
	};
}

