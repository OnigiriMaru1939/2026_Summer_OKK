#pragma once
#include "DxLib.h"
#include "ParticleEmitter.h"
#include "FileManager.h"
#include "ImageFile.h"
#include <vector>
#include <memory>
#include <string>
#include <map>

// JSONから読んでくるパーティクルの情報
// v:ばらつき
struct ParticleConfig
{
	int emitFrequecy;		// 発生個数(1sあたり)
	float x, y;				// 発生位置
	float vX, vY;			// 発生位置のばらつき
	float initDir;			// 初期方向
	float vInitDir;			// 初期方向のばらつき
	float initSpeed;		// 初期速度
	float vInitSpeed;		// 初期速度のばらつき
	float friction;			// 摩擦
	float acceleration;		// 重力
	float accelerationDir;  // 重力の方向
	float startScale;		// 最初のスケール
	float vStartScale;  	// 最初のスケールのばらつき
	float finishScale;		// 最後のスケール
	float vFinishScale;		// 最後のスケールのばらつき
	float lifeSpan;			// 寿命
	float vLifeSpan;		// 寿命のばらつき
	float startAlpha;		// 最初の透過度
	float vStartAlpha;		// 最初の透過度のばらつき
	float finishAlpha;		// 最初の透過度
	float vFinishAlpha;		// 最初の透過度のばらつき
	std::vector<std::string> shapeIDList; // シェイプリスト
	std::map<std::string, std::shared_ptr<ImageFile>> imageResouces;
	struct 
	{
		float hue, vHue;
		float saturation, vSaturation;
		float luminance, vLuminance;
	} startColor;
	bool blendMode;
	int alphaCurveType;
};

// 実際に動かす実体
struct ParticleInstance
{
	bool active = false;
	float x, y;
	float vx, vy;
	float ax, ay;
	float friction;
	float currentLife, maxLife;
	float scale, startScale, finishScale;
	float alpha, startAlpha, finishAlpha;
	std::vector<float> color;
	bool blendMode;
	int alphaCurveType;
	int imageHandle;
};

class ParticleManager
{
public:
	ParticleManager(FileManager& fileMng);
	~ParticleManager();
	
	// JSONデータ読み込み関数
	void RegisterConfig(const std::string& jsonPath);

	void UpdateAll();
	void DrawAll();

	std::string GetValue(const std::string& source, const std::string& key);

	void PlayParticle(const std::string& configName, float x, float y);
private:
	FileManager& _fileMng;

	std::vector<std::shared_ptr<ParticleEmitter>> emitters;
	std::map<std::string, ParticleConfig> configLibrary;
};