#pragma once
#include <memory>
#include "SceneSuper.h"
#include "Application.h"
#include "Stage.h"
#include "EnemyBase.h"
#include "Boss1.h"
#include "GimmickBase.h"
#include "ItemBase.h"

class Player;
class SceneManager;

class SceneGame :
    public SceneSuper
{
public:
	//ボスイベントの状態
	enum class BossEventState
	{
		NONE,
		WARNING,
		APPEAR,
		UIDRAW,
		BATTLE,
	};

	SceneGame(FileManager& fileMng, SceneManager& sceneMng);

	~SceneGame() override;

	void Update() override;
	void Draw() override;
	void UpdatePlayer();
	void UpdateEnemy();
	void UpdateStage();
	void UpdateItem();
	void UpdateDuringTransition() override; // トランジション中の更新処理
	void CheckPlayerEnemyCollision();
	void DrawGauge(int x,
				   int y,
				   int width,
				   int height,
				   float value,
				   float maxValue,
				   int color);
	void CheckPlayerGimmickCollision();									//プレイヤーとギミックの衝突判定
	void CheckPlayerItemCollision();									//プレイヤーとアイテムの衝突判定
	void CheckBossSpawn();												//ボスの生成判定
	void BossEvent();                                                   //ボスイベントの処理
	void BossEventDraw();                                               //ボスイベントの描画
	void SetBossArea(int left, int top, int right, int bottom);         //ボスエリアの矩形を設定
	RECT GetBossArea() const;                                           //ボスエリアの矩形を取得
	std::shared_ptr<EnemyBase> GetBoss();                                   //ボス1の取得

	//敵生成関数
	void AddEnemy(EnemyBase::ENEMY_TYPE type, float x, float y);
	void AddBoss(EnemyBase::ENEMY_TYPE type, float x, float y);
	void AddTeleport(float x, float y, float targetX, float targetY);
	void AddItem(ItemBase::ITEM_TYPE type, float x, float y);

	Stage* GetStage() { return stage_.get(); }
	Player* GetPlayer() { return player_.get(); }
	auto& GetEnemyList() { return enemyList_; }
	FileManager& GetFileManager() { return fileMng_; }

	static void SetSelectedStageIndex(int index) { selectedStageIndex_ = index; }
	static int selectedStageIndex_;

	void TransitionOut(float t) override;
	void TransitionIn(float t) override;
private:
	std::unique_ptr<Stage> stage_;
	std::unique_ptr<Player> player_;
	std::vector<std::shared_ptr<EnemyBase>> enemyList_;		//敵のリスト
	std::vector<std::shared_ptr<GimmickBase>> gimmickList_; //ギミックのリスト
	std::vector<std::shared_ptr<ItemBase>> itemList_;		//アイテムのリスト

	//シーンマネージャーへの弱い参照
	SceneManager& sceneMng_;

	float clearTime;		//クリアタイム
	int bossTimer;			//ボスイベントのタイマー
	float bossHpGauge;      //ボスのHP
	float bossHpGaugeMax;   //ボスのHPの最大値

	int _gameScreen;

	bool _isClear;
	bool IsClear() { return _isClear; }

	float _fadeAlpha;

	int bossNameFontHandle;
	int warningFontHandle;
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

	//ボスが出現したかどうかのフラグ
	bool isBossSpawned_;
	//ボスエリアの矩形
	RECT bossArea;
	//ボスイベントの状態
	BossEventState bossEventState;
};

