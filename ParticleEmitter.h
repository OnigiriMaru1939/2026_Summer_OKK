#pragma once
#include <vector>
#include "ParticleManager.h"

struct ParticleInstance;

class ParticleEmitter
{
public:
	ParticleEmitter(const ParticleConfig& cfg, float x, float y);
	~ParticleEmitter();

	void Update();
	void Draw();

	bool IsDead() const { return isDead; }

private:
	void ActivateParticle(); // 粒子を一つ生成

	ParticleConfig config; // 実体で保持
	std::vector<ParticleInstance> particles;
	float base_x;
	float base_y;
	float emitCounter = 0.0f;
	int lastUsedIndex = 0;
	bool isDead = false;
	float emitterLife; // エミッター自体の稼働時間

	std::vector<float> GetColorHSL(float H, float S, float L);
};

