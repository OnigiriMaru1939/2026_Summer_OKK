#pragma once
#include <memory>
#include "SceneSuper.h"
#include "Application.h"
#include "Stage.h"
#include "EnemyBase.h"
#include "BulletBase.h"
#include "Bullet.h"
#include "Boss1.h"
#include "FontFile.h"
#include "ScreenFile.h"
#include "GimmickBase.h"
#include "ItemBase.h"
#include "NetworkManager.h"
#include "RemotePlayer.h"
#include "SceneManager.h"

class Player;
class ParticleManager;

class SceneGame :
    public SceneSuper
{
public:
	struct tutorialPanel
	{
		std::shared_ptr<ImageFile> path;
		float x;
		float y;
	};
	//ボスイベントの状態
	enum class BossEventState
	{
		NONE,
		WARNING,
		APPEAR,
		UIDRAW,
		BATTLE,
	};

	SceneGame(FileManager& fileMng, SceneManager& sceneMng, bool isHost = true, const std::string& ip = "127.0.0.1");

	~SceneGame() override;

	void Update() override;
	void UpdatePlayer();
	void UpdateEnemy();
	void UpdateEnemyShot();
	void UpdateStage();
	void UpdateGimmick();
	void UpdateItem();
	void UpdateDuringTransition() override; // トランジション中の更新処理

	void Draw() override;
	void DrawGauge(int x,
				   int y,
				   int width,
				   int height,
				   float value,
				   float maxValue,
				   int color);

	void CheckPlayerEnemyCollision();
	void CheckPlayerGimmickCollision();									//プレイヤーとギミックの衝突判定
	void CheckPlayerItemCollision();									//プレイヤーとアイテムの衝突判定
	void CheckPlayerEnemyShotCollision();								//プレイヤーと敵の弾の衝突判定

	void CheckBossSpawn();												//ボスの生成判定
	void StartBossEvent();
	void BossEvent();                                                   //ボスイベントの処理
	void BossEventDraw();                                               //ボスイベントの描画
	void SetBossArea(int left, int top, int right, int bottom);         //ボスエリアの矩形を設定
	void ApplyDamageToEnemy(int enemyID, int damage);

	// 生成関数
	void AddEnemy(EnemyBase::ENEMY_TYPE type, float x, float y);
	void AddBoss(EnemyBase::ENEMY_TYPE type, float x, float y);
	void AddEnemyShot(BulletBase::BULLET_TYPE type, float x, float y, float vx, float vy, float scale);
	void AddTeleport(float x, float y, float targetX, float targetY);
	void AddItem(ItemBase::ITEM_TYPE type, float x, float y);
	void SetTutorial(std::string path, float x, float y);

	Stage* GetStage() { return stage_.get(); }
	Player* GetPlayer() { return player_.get(); }
	auto& GetEnemyList() { return enemyList_; }
	FileManager& GetFileManager() { return fileMng_; }
	RECT GetBossArea() const;                                           //ボスエリアの矩形を取得
	std::shared_ptr<EnemyBase> GetBoss();                               //ボスの取得


	void RequestPause() { sceneMng_.PushScene(SceneID::PAUSE); }
	static void SetSelectedStageIndex(int index) { selectedStageIndex_ = index; }
	static int selectedStageIndex_;

	void TransitionOut(float t) override;
	void TransitionIn(float t) override;

	bool IsHost() const { return isHost_; }

	void Teleport2BossArea();
private:
	std::unique_ptr<Stage> stage_;
	std::unique_ptr<Player> player_;

	std::unique_ptr<ParticleManager> _pMng;

	std::vector<tutorialPanel> _tutorialPanelList; // チュートリアルの画像リスト

	std::shared_ptr<ImageFile> _PauseImg;
	bool CollisionPauseImg();

	std::vector<std::shared_ptr<EnemyBase>> enemyList_;			//敵のリスト
	std::vector<std::shared_ptr<GimmickBase>> gimmickList_;		//ギミックのリスト
	std::vector<std::shared_ptr<ItemBase>> itemList_;			//アイテムのリスト
	std::vector<std::shared_ptr<BulletBase>> bulletList_;		//敵の弾のリスト

	NetworkManager& networkManager_;
	std::shared_ptr<RemotePlayer> remotePlayer_;

	bool isHost_;

	float clearTime;		//クリアタイム
	int bossTimer;			//ボスイベントのタイマー
	float bossHpGauge;      //ボスのHP
	float bossHpGaugeMax;   //ボスのHPの最大値

	std::shared_ptr<ScreenFile> _gameScreen;

	bool _isClear;
	bool IsClear() { return _isClear; }

	float _fadeAlpha;

	int _nextEnemyId = 0;

	std::shared_ptr<FontFile> _bossNameFont;
	std::shared_ptr<FontFile> _warningFont;
	// ステージクリア時演出用 ------------------
	static constexpr int DOWN_SCALE = 8;
	static constexpr int DOWN_SCALE_SCREEN_W = (Application::SCREEN_WID / DOWN_SCALE);	// ガウスフィルタを掛ける画像の横幅
	static constexpr int DOWN_SCALE_SCREEN_H = (Application::SCREEN_HIG / DOWN_SCALE);	// ガウスフィルタを掛ける画像の縦幅
	std::shared_ptr<ScreenFile> highBrightScreen;	// 普通の描画結果から高輝度部分を抜き出した結果を書き込むスクリーン
	std::shared_ptr<ScreenFile> downScaleScreen;	// 高輝度部分を縮小した結果を書き込むスクリーン
	std::shared_ptr<ScreenFile> gaussScreen;	// 縮小画像をガウスフィルタでぼかした結果を書き込むスクリーン
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

	bool _isBossDefeatedSequence = false;
	int _sequenceTimer = 0;

	std::shared_ptr<ScreenFile> _offScreen;
};

