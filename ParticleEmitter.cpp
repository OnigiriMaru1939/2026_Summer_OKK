#include "ParticleEmitter.h"
#include "ParticleManager.h"

ParticleEmitter::ParticleEmitter(const ParticleConfig* cfg) : config(cfg)
{
	emitCounter = 0.0f;
	particles.resize(1000);
}

ParticleEmitter::~ParticleEmitter()
{
}

void ParticleEmitter::Update()
{
	// フレームごとの発生カウンタを計算
	float emitParFrame = static_cast<float>(config->emitFrequecy / 60.0f);
	emitCounter += emitParFrame;

	// 計算値から生成
	while (emitCounter >= 1.0f)
	{
		ActivateParticle();
		emitCounter -= 1.0f;
	}

	for (auto& p : particles)
	{
		if (!p.active) continue;

		// (速度 + 重力) * 摩擦
		p.vx = (p.vx + p.ax) * (1.0f - p.friction);
		p.vy = (p.vy + p.ay) * (1.0f - p.friction);
		p.x += p.vx;
		p.y += p.vy;

		p.currentLife -= 1.0f;
		if (p.currentLife <= 0)
		{
			p.active = false;
			continue;
		}

		float t = 1.0f - (p.currentLife / p.maxLife);
		p.scale = p.startScale + (p.finishScale - p.startScale) * t;
		p.alpha = p.startAlpha + (p.finishAlpha - p.startAlpha) * t;
	}
}

void ParticleEmitter::ActivateParticle()
{
	auto it = std::find_if(
		particles.begin(),
		particles.end(),
		[](const ParticleInstance& p) { return !p.active; }
	);

	if (it == particles.end()) return;

	ParticleInstance& p = *it;
	p.active = true;

	// ヘルパー(ラムダ): -0.5~0.5の乱数生成
	auto getVar = []() { return ((float)GetRand(1000) / 1000.0f) - 0.5f; };

	// パラメータ初期化
	// 位置
	p.x = config->x + getVar() * config->vX;
	p.y = config->y + getVar() * config->vY;

	// 方向・速度
	float angle = (config->initDir + getVar() * config->vInitDir) * (DX_PI_F / 180.0f);
	float speed = config->initSpeed + getVar() * config->vInitSpeed;
	p.vx = cosf(angle) * speed;
	p.vy = sinf(angle) * speed;

	// 加速度
	float accAngle = config->accelerationDir * (DX_PI_F / 180.0f);
	float acc = config->acceleration;
	p.ax = cosf(accAngle) * acc;
	p.ay = sinf(accAngle) * acc;

	// 摩擦
	p.friction = config->friction;

	// 寿命
	p.maxLife = config->lifeSpan + getVar() * config->vLifeSpan;
	if (p.maxLife <= 2.0f) p.maxLife = 2.0f;
	p.currentLife = p.maxLife;

	// スケール
	p.startScale = config->startScale + getVar() * config->vStartScale;
	p.finishScale = config->finishScale + getVar() * config->vFinishScale;
	p.scale = p.startScale;

	// アルファ
	p.startAlpha = config->startAlpha + getVar() * config->vStartAlpha;
	p.finishAlpha = config->finishAlpha + getVar() * config->vFinishAlpha;
	p.alpha = p.startAlpha;

	// 色
	float h = config->startColor.hue + getVar() * config->startColor.vHue; // 基本0.0～360.0
	float s = (config->startColor.saturation + getVar() * config->startColor.vSaturation) / 100; // 基本0.0～1.0
	float l = (config->startColor.luminance + getVar() * config->startColor.vLuminance) / 100; // 基本0.0～1.0
	p.color = GetColorHSL(h, s, l); // HSLをRGBに変換

	// ブレンドモードの設定
	p.blendMode = config->blendMode;

	// アルファカーブの設定
	p.alphaCurveType = config->alphaCurveType;

	// 画像のランダム選択
	if (!config->shapeIDList.empty())
	{
		int index = GetRand(static_cast<int>(config->shapeIDList.size()) - 1);
		std::string shapeID = config->shapeIDList[index];

		p.imageHandle = config->imageResouces.at(shapeID)->GetHandle();
	}
}

// H : 色相　有効範囲 0.0f ～ 360.0f
// S : 彩度　有効範囲 0.0f ～ 1.0f
// L : 輝度　有効範囲 0.0f ～ 1.0f
std::vector<float> ParticleEmitter::GetColorHSL(float H, float S, float L)
{
	// Hを 0.0 ～ 360.0 の範囲に収める
	while (H < 0.0f) H += 360.0f;
	while (H >= 360.0f) H -= 360.0f;
	// Sを 0.0 ～ 1.0 の範囲に収める
	if (S < 0.0f) { S = 0.0f; }
	else if (S >= 1.0f) { S = 1.0f; }
	// Lを 0.0 ～ 1,0 の範囲に収める
	if (L < 0.0f) { L = 1.0f; }
	else if (L >= 1.0f) { L = 1.0f; }

	float r = 0, g = 0, b = 0;

	// C (Chroma): 最大値と最小値の差
	float C = (1.0f - fabsf(2.0f * L - 1.0f)) * S;

	// H' (H / 60)
	float hPrime = H / 60.0f;

	// X: 中間的な色成分
	float X = C * (1.0f - fabsf(fmodf(hPrime, 2.0f) - 1.0f));

	// H'の値に応じてRGBを割り振る
	if (0.0f <= hPrime && hPrime < 1.0f) { r = C; g = X; b = 0; }
	else if (1.0f <= hPrime && hPrime < 2.0f) { r = X; g = C; b = 0; }
	else if (2.0f <= hPrime && hPrime < 3.0f) { r = 0; g = C; b = X; }
	else if (3.0f <= hPrime && hPrime < 4.0f) { r = 0; g = X; b = C; }
	else if (4.0f <= hPrime && hPrime < 5.0f) { r = X; g = 0; b = C; }
	else if (5.0f <= hPrime && hPrime < 6.0f) { r = C; g = 0; b = X; }

	// m: 最小値を足して全体を底上げする
	float m = L - C / 2.0f;

	// 0-255の範囲に変換し、四捨五入(+0.5f)を行う
	auto to255 = [m](float val)
		{
			float res = (val + m) * 255.0f + 0.5f;
			if (res > 255.0f) return 255.0f;
			if (res < 0.0f) return 0.0f;
			return res;
		};

	// std::vector<int> で要素3（R, G, B）を返す
	return { to255(r), to255(g), to255(b) };
}

void ParticleEmitter::Draw()
{
	// ブレンドモード設定(true:加算合成 false:通常)
	int blendMode = config->blendMode ? DX_BLENDMODE_ADD : DX_BLENDMODE_ALPHA;

	for (auto& p : particles)
	{
		if (!p.active) continue;

		SetDrawBlendMode(blendMode, static_cast<int>(p.alpha * 255));

		// p.colorの反映

		SetDrawBright((int)p.color[0], (int)p.color[1], (int)p.color[2]);

		DrawRotaGraphF(p.x, p.y, static_cast<double>(p.scale), 0.0, p.imageHandle, true);
	}
	SetDrawBright(255, 255, 255);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}