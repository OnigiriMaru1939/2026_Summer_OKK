#pragma once
#include "SceneSuper.h"
#include "Application.h"
#include "Stage.h"
#include "EnemyBase.h"

class Player;
class SceneManager;

class SceneGame :
    public SceneSuper
{
public:
	SceneGame(FileManager& fileMng, SceneManager& sceneMng);

	~SceneGame() override;

	void Update() override;
	void Draw() override;
	void UpdateEnemy();
	void CheckPlayerEnemyCollision();
	Stage* GetStage() { return stage_.get(); }
	Player* GetPlayer() { return player_.get(); }
	auto& GetEnemyList() { return enemyList_; }
	FileManager& GetFileManager() { return fileMng_; }

	static void SetSelectedStageIndex(int index) { selectedStageIndex_ = index; }
private:
	static int selectedStageIndex_;

	std::unique_ptr<Stage> stage_;
	std::unique_ptr<Player> player_;
	std::vector<std::shared_ptr<EnemyBase>> enemyList_;		//敵のリスト

	SceneManager& sceneMng_; // シーンマネージャーへの弱い参照

	float clearTime; // クリアタイム
};

