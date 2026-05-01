#pragma once
#include <vector>

struct ParticleConfig;
struct ParticleInstance;

class ParticleEmitter
{
public:
	ParticleEmitter(const ParticleConfig* cfg);
	~ParticleEmitter();
	void Update();
	void Draw();

private:
	void ActivateParticle(); // 粒子を一つ生成

	const ParticleConfig* config;
	std::vector<ParticleInstance> particles;
	float emitCounter = 0.0f;
	int lastUsedIndex = 0;

	std::vector<float> GetColorHSL(float H, float S, float L);
};

