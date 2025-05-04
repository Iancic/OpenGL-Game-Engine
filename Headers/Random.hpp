#pragma once
#include <random>

class Random
{
public:
	inline static void Init()
	{
		randomEngine.seed(std::random_device()());
	}

	// TODO: make this lehmer like OLC video says
	inline static float RandomFloat()
	{
		return static_cast<float>(distribution(randomEngine)) / static_cast<float>(std::numeric_limits<uint32_t>::max());
	}

private:
	inline static std::mt19937 randomEngine;
	inline static std::uniform_int_distribution<std::mt19937::result_type> distribution;
};