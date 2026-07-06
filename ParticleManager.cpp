#include "ParticleManager.h"
#include "ParticleEmitter.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>


ParticleManager::ParticleManager(FileManager& fileMng) : _fileMng(fileMng)
{
	emitters.clear();
}

ParticleManager::~ParticleManager()
{
}

// Particle Developからのデータを輸入するために強引にロード
void ParticleManager::RegisterConfig(const std::string& jsonPath)
{
	// 既に登録済みなら何もしない
	if (configLibrary.find(jsonPath) != configLibrary.end()) return;

	std::ifstream ifs(jsonPath);
	if (!ifs.is_open()) return;

	// ファイル全体を一つの文字列に読み込む
	std::stringstream ss;
	ss << ifs.rdbuf();
	std::string content = ss.str();

	ParticleConfig cfg;

	cfg.emitFrequecy = std::stoi(GetValue(content, "emitFrequency"));
	cfg.x = std::stof(GetValue(content, "startX"));
	cfg.y = std::stof(GetValue(content, "startY"));
	cfg.vX = std::stof(GetValue(content, "startXVariance"));
	cfg.vY = std::stof(GetValue(content, "startYVariance"));

	cfg.initDir = std::stof(GetValue(content, "initialDirection"));
	cfg.vInitDir = std::stof(GetValue(content, "initialDirectionVariance"));
	cfg.initSpeed = std::stof(GetValue(content, "initialSpeed"));
	cfg.vInitSpeed = std::stof(GetValue(content, "initialSpeedVariance"));

	cfg.friction = std::stof(GetValue(content, "friction"));

	cfg.acceleration = std::stof(GetValue(content, "accelerationSpeed"));
	cfg.accelerationDir = std::stof(GetValue(content, "accelerationDirection"));

	cfg.startScale = std::stof(GetValue(content, "startScale"));
	cfg.vStartScale = std::stof(GetValue(content, "startScaleVariance"));
	cfg.finishScale = std::stof(GetValue(content, "finishScale"));
	cfg.vFinishScale = std::stof(GetValue(content, "finishScaleVariance"));

	cfg.lifeSpan = std::stof(GetValue(content, "lifeSpan"));
	cfg.vLifeSpan = std::stof(GetValue(content, "lifeSpanVariance"));

	cfg.startAlpha = std::stof(GetValue(content, "startAlpha"));
	cfg.vStartAlpha = std::stof(GetValue(content, "startAlphaVariance"));
	cfg.finishAlpha = std::stof(GetValue(content, "finishAlpha"));
	cfg.vFinishAlpha = std::stof(GetValue(content, "finishAlphaVariance"));

	// startColor (階層構造になっているが強引に取得)
	cfg.startColor.hue = std::stof(GetValue(content, "hue"));
	cfg.startColor.vHue = std::stof(GetValue(content, "hueVariance"));
	cfg.startColor.saturation = std::stof(GetValue(content, "saturation"));
	cfg.startColor.vSaturation = std::stof(GetValue(content, "saturationVariance"));
	cfg.startColor.luminance = std::stof(GetValue(content, "luminance"));
	cfg.startColor.vLuminance = std::stof(GetValue(content, "luminanceVariance"));

	cfg.blendMode = (GetValue(content, "blendMode") == "true");

	size_t listStart = content.find("\"shapeIdList\"");
	if (listStart != std::string::npos) // shapeIdListが存在する場合
	{
		size_t arrayStart = content.find("[", listStart);
		size_t arrayEnd = content.find("]", arrayStart);

		if (arrayStart != std::string::npos && arrayEnd != std::string::npos)
		{
			// []の中身を取り出す
			std::string arrayContent = content.substr(arrayStart + 1, arrayEnd - arrayStart - 1);
			std::stringstream ss(arrayContent);
			std::string item;

			// カンマで区切って取り出し
			while (std::getline(ss, item, ','))
			{
				item.erase(std::remove(item.begin(), item.end(), ' '), item.end());
				item.erase(std::remove(item.begin(), item.end(), '\"'), item.end());
				item.erase(std::remove(item.begin(), item.end(), '\n'), item.end());
				item.erase(std::remove(item.begin(), item.end(), '\r'), item.end());

				if (!item.empty())
				{
					cfg.shapeIDList.push_back(item);

					std::string path = "Resource/Particles/" + item + ".png";
					cfg.imageResouces[item] = _fileMng.LoadImageFM(path);
				}
			}
		}
	}

	configLibrary[jsonPath] = cfg;
}

void ParticleManager::UpdateAll()
{
	// 各エミッターの更新
	for (auto it = emitters.begin(); it != emitters.end();)
	{
		(*it)->Update();

		if ((*it)->IsDead())
		{
			it = emitters.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void ParticleManager::DrawAll(int scrollX, int scrollY)
{
	for (auto& emitter : emitters)
	{
		emitter->Draw(scrollX, scrollY);
	}
}

std::string ParticleManager::GetValue(const std::string& source, const std::string& key)
{
	std::string searchKey = "\"" + key + "\"";
	size_t pos = source.find(searchKey);
	if (pos == std::string::npos) return "0";

	size_t start = source.find(":", pos) + 1;
	// 次のカンマか閉じかっこまでを値とする
	size_t end = source.find_first_of(",}", start);

	std::string val = source.substr(start, end - start);
	// いらない空白やダブルクォーテーションを取り除く
	val.erase(std::remove(val.begin(), val.end(), ' '), val.end());
	val.erase(std::remove(val.begin(), val.end(), '\"'), val.end());
	val.erase(std::remove(val.begin(), val.end(), '\n'), val.end());
	val.erase(std::remove(val.begin(), val.end(), '\r'), val.end());

	return val;
}

std::weak_ptr<ParticleEmitter> ParticleManager::PlayParticle(const std::string& configName, float x, float y, float life)
{
	if (configLibrary.find(configName) == configLibrary.end()) return std::weak_ptr<ParticleEmitter>();

	const auto& cfg = configLibrary[configName];

	std::shared_ptr<ParticleEmitter> newEmitter = std::make_shared<ParticleEmitter>(cfg, x, y, life);
	emitters.push_back(newEmitter);
	return newEmitter;
}

std::weak_ptr<ParticleEmitter> ParticleManager::PlayParticle(const ParticleConfig& customConfig, float x, float y, float life)
{
	std::shared_ptr<ParticleEmitter> newEmitter = std::make_shared<ParticleEmitter>(customConfig, x, y, life);
	emitters.push_back(newEmitter);
	return newEmitter;
}

const ParticleConfig* ParticleManager::GetConfig(const std::string& configName)
{
	auto it = configLibrary.find(configName);
	if (it == configLibrary.end()) return nullptr; // ない場合NULLを返す
	return &(it->second);
}
