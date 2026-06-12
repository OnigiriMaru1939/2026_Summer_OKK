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
	void UpdatePlayer();
	void UpdateEnemy();
	void UpdateStage();
	void UpdateDuringTransition() override; // トランジション中の更新処理
	void CheckPlayerEnemyCollision();
	//敵生成関数
	void AddEnemy(EnemyBase::ENEMY_TYPE type, float x, float y);
	void AddBoss(EnemyBase::ENEMY_TYPE type, float x, float y);
	Stage* GetStage() { return stage_.get(); }
	Player* GetPlayer() { return player_.get(); }
	auto& GetEnemyList() { return enemyList_; }
	FileManager& GetFileManager() { return fileMng_; }

	static void SetSelectedStageIndex(int index) { selectedStageIndex_ = index; }
	static int selectedStageIndex_;

	void TransitionOut(float t) override;
private:
	std::unique_ptr<Stage> stage_;
	std::unique_ptr<Player> player_;
	std::vector<std::shared_ptr<EnemyBase>> enemyList_;		//敵のリスト

	SceneManager& sceneMng_; // シーンマネージャーへの弱い参照

	float clearTime; // クリアタイム

	int _gameScreen;

	bool _isClear;
	bool IsClear() { return _isClear; }

	float _fadeAlpha;
	// ステージクリア時演出用 ------------------
	static constexpr int DOWN_SCALE = 8;
	static constexpr int DOWN_SCALE_SCREEN_W = (Application::SCREEN_WID / DOWN_SCALE);	// ガウスフィルタを掛ける画像の横幅
	static constexpr int DOWN_SCALE_SCREEN_H = (Application::SCREEN_HIG / DOWN_SCALE);	// ガウスフィルタを掛ける画像の縦幅
	int highBrightScreen;	// 普通の描画結果から高輝度部分を抜き出した結果を書き込むスクリーン
	int downScaleScreen;	// 高輝度部分を縮小した結果を書き込むスクリーン
	int gaussScreen;	// 縮小画像をガウスフィルタでぼかした結果を書き込むスクリーン
	int gaussRatio;
	int filterRatio;

	void DrawClearTransition();
	// -------------------------------------
};

