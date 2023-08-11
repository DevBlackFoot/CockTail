#pragma once
#include <iostream>
#include <fstream>
#include <boost/serialization/access.hpp>

namespace fs = std::filesystem;

namespace ClientSpace
{
	struct PlayerData
	{
		friend boost::serialization::access;

		PlayerData(
			int _hp = 3,
			int _power = 4,
			float _attack1 = 1.0f,
			float _attack2 = 1.0f,
			float _attack3 = 2.0f,
			float _attack4 = 0.5f,
			float _moveSpeed = 1.0f, int _enemyHp = 5)
			: hp(std::move(_hp))
			, power(std::move(_power))
			, attack1(std::move(_attack1))
			, attack2(std::move(_attack2))
			, attack3(std::move(_attack3))
			, attack4(std::move(_attack4))
			, moveSpeed(std::move(_moveSpeed))
			, enemyHp(std::move(_enemyHp))
		{}

		int hp;
		int power;
		float attack1;
		float attack2;
		float attack3;
		float attack4;
		float moveSpeed;

		// 적의 체력도 넣어봅시다.
		int enemyHp;

		template <typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& hp;
			ar& power;
			ar& attack1;
			ar& attack2;
			ar& attack3;
			ar& attack4;
			ar& moveSpeed;
			ar& enemyHp;
		}
	};
}

