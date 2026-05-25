#pragma once
#include "SceneSuper.h"
#include "Application.h"
#include "Stage.h"
#include "EnemyBase.h"

class Player;

class SceneGame :
    public SceneSuper
{
public:
	SceneGame(FileManager& fileMng);

	~SceneGame() override;

	void Update() override;
	void Draw() override;
	auto GetStage() { return stage_.get(); }
	auto GetPlayer() { return player_.get(); }
	auto& GetEnemyList() { return enemyList_; }
	auto& GetFileManager() { return fileMng_; }
private:
	static int selectedStageIndex_;

	std::unique_ptr<Stage> stage_;
	std::unique_ptr<Player> player_;
	std::vector<std::shared_ptr<EnemyBase>> enemyList_;		//敵のリスト
};

