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
	void UpdateEnemy();
	void CheckPlayerEnemyCollision();
	auto GetStage() { return stage_.get(); }
private:
	static int selectedStageIndex_;

	std::unique_ptr<Stage> stage_;
	std::unique_ptr<Player> player_;
	std::vector<std::shared_ptr<EnemyBase>> enemyList_;		//敵のリスト
};

