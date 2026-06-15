#include "SceneGame.h"
#include "InputManager.h"
#include "Player.h"
#include "EnemyBase.h"
#include "Enemy1.h"
#include "Boss1.h"
#include "Stage.h"
#include "FileManager.h"
#include "StageConfig.h"
#include "StageConfigTablle.h"
#include "SceneManager.h"

// 静的変数の定義
int SceneGame::selectedStageIndex_ = 1;

SceneGame::SceneGame(FileManager& fileMng, SceneManager& sceneMng) : SceneSuper(fileMng), sceneMng_(sceneMng)
{
	AddFontResourceExA("Resource/fonts/DotGothic16-Regular.ttf", FR_PRIVATE, NULL);
	bossNameFontHandle = CreateFontToHandle("DotGothic16", 60, -1, DX_FONTTYPE_NORMAL);
	warningFontHandle = CreateFontToHandle("DotGothic16", 160, -1, DX_FONTTYPE_NORMAL);
	stage_ = std::make_unique<Stage>(fileMng);
	player_ = std::make_unique<Player>(fileMng, stage_.get(), *this);

	//ステージ固有のセットアップを実行
	const auto& stageConfigs = GetStageConfigs();
	const StageConfig& config = stageConfigs[selectedStageIndex_ - 1];
	stage_->Load(config.mapPath);
	config.setUpStage(*this);

	bossEventState = BossEventState::NONE;
	isBossSpawned_ = false;

	bossTimer = 0;
	clearTime = 0.0f;

	_isClear = false;

	_gameScreen = MakeScreen(Application::SCREEN_WID, Application::SCREEN_HIG);

	highBrightScreen = MakeScreen(Application::SCREEN_WID, Application::SCREEN_HIG, false);
	downScaleScreen = MakeScreen(DOWN_SCALE_SCREEN_W, DOWN_SCALE_SCREEN_H, FALSE);
	gaussScreen = MakeScreen(DOWN_SCALE_SCREEN_W, DOWN_SCALE_SCREEN_H, FALSE);

	gaussRatio = 0;
	filterRatio = 200;
	_fadeAlpha = 255.0f;

	// デバッグ
	InputManager::GetInstance().SetTriggerCallback(ActionID::Cancel, 
												   [this]()
												   {
													   if (!isTransition)
													   {
														   _isClear = false;
														   ClearResult result;
														   result.time = 0.0f;
														   result.stageIndex = selectedStageIndex_;
														   sceneMng_.SetClearResult(result);
														   sceneMng_.SetGameResult(_isClear); // ゲームオーバー
														   SetNextScene(SceneID::RESULT);
														   isEnd = true;
													   }
												   });
	InputManager::GetInstance().SetTriggerCallback(ActionID::Pause,
												   [this]()
												   {
													   if (!isTransition)
													   {
														   sceneMng_.PushScene(SceneID::PAUSE);
													   }
												   });
}

SceneGame::~SceneGame()
{
	DeleteGraph(_gameScreen);
	DeleteGraph(highBrightScreen);
	DeleteGraph(downScaleScreen);
	DeleteGraph(gaussScreen);
	DeleteFontToHandle(bossNameFontHandle);
	DeleteFontToHandle(warningFontHandle);
	RemoveFontResourceExA("Resource/fonts/DotGothic16-Regular.ttf", FR_PRIVATE, NULL);
}

void SceneGame::Update()
{
	clearTime += 1.0f / 60.0f; // クリアタイムの更新

	// プレイヤーの更新
	UpdatePlayer();

	//敵の更新
	UpdateEnemy();

	//ボスの生成判定
	CheckBossSpawn();

	//ボスイベントの処理
	BossEvent();

	//プレイヤーと敵の衝突判定
	CheckPlayerEnemyCollision();

	// Stageの更新
	UpdateStage();
}

void SceneGame::Draw()
{
	SetDrawScreen(_gameScreen);
	ClearDrawScreen();

	DrawBox(0, 0, Application::SCREEN_WID, Application::SCREEN_HIG, 0x00aa00, true);

	// Stageを描画
	if (stage_) stage_->Draw();

	//ボスエリアを描画
	RECT area = bossArea;

	DrawBox(area.left - stage_->GetScrollX(),
			area.top - stage_->GetScrollY(),
			area.right - stage_->GetScrollX(),
			area.bottom - stage_->GetScrollY(),
			0xff0000, false);

	DrawString(0, 20, "GAME SCENE", 0xffffff);

	//敵を描画
	for (auto& enemy : enemyList_)
	{
		enemy->Draw();
	}

	//プレイヤーを描画
	player_->Draw();

	//ボスイベントの描画
	BossEventDraw();

	SetDrawScreen(DX_SCREEN_BACK);
	DrawGraph(0, 0, _gameScreen, false);

	DrawClearTransition();

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(_fadeAlpha));
	if (IsClear())
	{
		DrawBox(0, 0, Application::SCREEN_WID, Application::SCREEN_HIG, 0xffffff, true);
	}
	else
	{
		DrawBox(0, 0, Application::SCREEN_WID, Application::SCREEN_HIG, 0x000000, true);
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

}

//ボスイベントの描画
void SceneGame::BossEventDraw()
{
	auto boss = GetBoss();

	int strWidth = 0;

	if (boss)
	{
		strWidth = GetDrawStringWidthToHandle(
			boss->GetName().c_str(),
			strlen(boss->GetName().c_str()),
			bossNameFontHandle
		);
	}

	if (bossEventState == BossEventState::UIDRAW)
	{
		DrawStringToHandle(
			(Application::SCREEN_WID - strWidth) / 2,
			20,
			boss->GetName().c_str(),
			GetColor(255, 255, 255),
			bossNameFontHandle
		);

		DrawGauge(100, 90, 1720, 50, bossHpGauge, bossHpGaugeMax, GetColor(255, 0, 0));

	}

	if (bossEventState == BossEventState::BATTLE)
	{
		if (boss)
		{
			DrawStringToHandle(
				(Application::SCREEN_WID - strWidth) / 2,
				20,
				boss->GetName().c_str(),
				GetColor(255, 255, 255),
				bossNameFontHandle
			);

			DrawGauge(100, 90, 1720, 50, boss->GetHp(), boss->GetHpMax(), GetColor(255, 0, 0));
		}
	}

	if (bossEventState == BossEventState::WARNING)
	{
		if (bossTimer > 0)
		{
			if ((bossTimer / 10) % 6 == 0) return;
		}

		DrawBox(
			0,
			Application::SCREEN_HIG / 2 - 100,
			Application::SCREEN_WID,
			Application::SCREEN_HIG / 2 + 100,
			GetColor(255, 0, 0),
			true);

		strWidth = GetDrawStringWidthToHandle(
			"WARNING !!",
			strlen("WARNING !!"),
			warningFontHandle
		);
		DrawStringToHandle(
			(Application::SCREEN_WID - strWidth) / 2,
			(Application::SCREEN_HIG - GetFontSizeToHandle(warningFontHandle)) / 2,
			"WARNING !!",
			GetColor(255, 255, 255),
			warningFontHandle
			);
	}


}

//ゲージの描画
void SceneGame::DrawGauge(
	int x,
	int y,
	int width,
	int height,
	float value,
	float maxValue,
	int color)
{
	//ゲージの枠
	DrawBox(
		x - 1,
		y - 1,
		x + width + 1,
		y + height + 1,
		GetColor(255, 255, 255),
		FALSE
	);
	//横ゲージ割合
	int barWidth = (int)((value / maxValue) * width);

	//中身
	DrawBox(
		x,
		y,
		x + barWidth,
		y + height,
		color,
		TRUE
	);
}

void SceneGame::DrawClearTransition()
{
	if (IsClear())
	{
		// 描画結果から高輝度部分のみを抜き出した画像を得る
		GraphFilterBlt(_gameScreen, highBrightScreen, DX_GRAPH_FILTER_BRIGHT_CLIP, DX_CMP_LESS, filterRatio, TRUE, GetColor(0, 0, 0), 255);
		// 高輝度部分を８分の１に縮小した画像を得る
		GraphFilterBlt(highBrightScreen, downScaleScreen, DX_GRAPH_FILTER_DOWN_SCALE, DOWN_SCALE);
		// ８分の１に縮小した画像をガウスフィルタでぼかす
		GraphFilterBlt(downScaleScreen, gaussScreen, DX_GRAPH_FILTER_GAUSS, 16, gaussRatio);
		// 描画モードをバイリニアフィルタリングにする(拡大したときにドットがぼやけるようにする)
		SetDrawMode(DX_DRAWMODE_BILINEAR);
		// 描画ブレンドモードを加算にする
		SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
		// 高輝度部分を縮小してぼかした画像を画面いっぱいに数回描画する( 数回描画するのはより明るくみえるようにするため )
		DrawExtendGraph(0, 0, Application::SCREEN_WID, Application::SCREEN_HIG, gaussScreen, FALSE);
		DrawExtendGraph(0, 0, Application::SCREEN_WID, Application::SCREEN_HIG, gaussScreen, FALSE);
		// 描画ブレンドモードをブレンド無しに戻す
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		// 描画モードを二アレストに戻す
		SetDrawMode(DX_DRAWMODE_NEAREST);
	}
}

//プレイヤーの更新
void SceneGame::UpdatePlayer()
{
	player_->Update();
	if (!player_->GetAliveFlag())
	{
		_isClear = false;
		ClearResult result;
		result.time = 0.0f;
		result.stageIndex = selectedStageIndex_;
		sceneMng_.SetClearResult(result);
		sceneMng_.SetGameResult(_isClear); // ゲームオーバー
		SetNextScene(SceneID::RESULT);
		isEnd = true;
	}
}

//敵の更新
void SceneGame::UpdateEnemy()
{
	for (auto& enemy : enemyList_)
	{
		if (enemy->IsAlive())
		{
			enemy->Update();
		}
		if (!enemy->IsAlive())
		{
			// 敵が死んでいる場合はリストから削除
			// ボスを倒したらトランジション
			if (std::dynamic_pointer_cast<IBoss>(enemy))
			{
				_isClear = true;
				ClearResult result;
				result.time = clearTime;
				result.stageIndex = selectedStageIndex_;
				sceneMng_.SetClearResult(result);
				sceneMng_.SetGameResult(_isClear); // クリア
				SetNextScene(SceneID::RESULT);
				isEnd = true;
			}
			enemy = nullptr; // shared_ptrをnullptrに設定して削除
		}
	}

	enemyList_.erase(
		std::remove(enemyList_.begin(), enemyList_.end(), nullptr),
		enemyList_.end()
	);
 }

//ステージの更新
void SceneGame::UpdateStage()
{
	if (stage_) stage_->Update();
}

void SceneGame::UpdateDuringTransition()
{
	// プレイヤーの更新
	player_->UpdateStageScroll();
	//UpdatePlayer();

	// Stageの更新
	UpdateStage();

}

//プレイヤーと敵の衝突判定
void SceneGame::CheckPlayerEnemyCollision()
{
	for (auto& enemy : enemyList_)
	{
		if (!enemy->IsAlive()) continue;
		RECT p = player_->GetRect();
		RECT e = enemy->GetRect();

		bool hit =
			p.right > e.left &&
			p.left < e.right &&
			p.bottom > e.top &&
			p.top < e.bottom;

		if (hit && !player_->GetAttakFlag())
		{
			//プレイヤーにダメージを与える
			player_->Damage(10.0f);
			//プレイヤーをノックバックさせる
			player_->PlayerKnockBack(enemy->GetX(), enemy->GetY(), 10.0f);
		}
		else if (hit && player_->GetAttakFlag())
		{
			enemy->ApplyDamage(static_cast<int>(player_->GetAttackDamage()));
			if (enemy->IsAlive())
			{
				//プレイヤーをノックバックさせる
				player_->PlayerKnockBack(enemy->GetX(), enemy->GetY(), 10.0f);
			}
		}
	}
}

//ボスエリアの矩形を設定
void SceneGame::SetBossArea(int left, int top, int right, int bottom)
{
	// ボスエリアの矩形を定義
	bossArea.left = left;
	bossArea.top = top;
	bossArea.right = right;
	bossArea.bottom = bottom;
}

//ボスエリアの矩形を取得
RECT SceneGame::GetBossArea() const
{
	return bossArea;
}

//ボスの生成判定
void SceneGame::CheckBossSpawn()
{
	if (isBossSpawned_) return;

	bossArea = GetBossArea();

	float x = player_->GetX();
	float y = player_->GetY();

	if (x >= bossArea.left - stage_->GetScrollX() &&
		x <= bossArea.right - stage_->GetScrollX() &&
		y >= bossArea.top - stage_->GetScrollY() &&
		y <= bossArea.bottom - stage_->GetScrollY())
	{
		bossEventState = BossEventState::WARNING;
		bossTimer = 0;

		player_->SetCanMoveFlag(false); // プレイヤーの移動を制限

		isBossSpawned_ = true;
	}
}

std::shared_ptr<Boss1> SceneGame::GetBoss()
{
	for (auto& enemy : enemyList_)
	{
		auto boss = std::dynamic_pointer_cast<Boss1>(enemy);

		if (boss)
		{
			return boss;
		}
	}

	return nullptr;
}

void SceneGame::BossEvent()
{
	auto boss = GetBoss();

	switch (bossEventState)
	{
		case SceneGame::BossEventState::NONE:
			break;

		case SceneGame::BossEventState::WARNING:
			bossTimer++;
			if (bossTimer > 180)
			{
				AddBoss(
					EnemyBase::ENEMY_TYPE::E_TYPE_BOSS_1,
					1800.0f,
					200.0f
				);
				bossEventState = BossEventState::APPEAR;
				bossTimer = 0;
			}
			break;

		case SceneGame::BossEventState::APPEAR:
			bossTimer++;
			//2秒後にUIを表示
			if (bossTimer > 120)
			{
				bossEventState = BossEventState::UIDRAW;

				if (boss)
				{
					bossHpGauge = 0;
					bossHpGaugeMax = boss->GetHpMax();
				}
				bossTimer = 0;
			}
			break;

		case SceneGame::BossEventState::UIDRAW:
			if (bossHpGauge < bossHpGaugeMax)
			{
				bossHpGauge += 2.0f;
			}

			if (bossHpGauge >= bossHpGaugeMax)
			{
				bossHpGauge = bossHpGaugeMax;

				player_->SetCanMoveFlag(true);

				if (boss)
				{
					boss->SetAppearFlag(false);
				}

				bossEventState = BossEventState::BATTLE;
			}

			break;

		case SceneGame::BossEventState::BATTLE:
			break;

		default:
			break;
	}
}

//敵生成関数(雑魚敵)
void SceneGame::AddEnemy(EnemyBase::ENEMY_TYPE type, float x, float y)
{
	switch (type)
	{
	case EnemyBase::ENEMY_TYPE::E_TYPE_1:
		enemyList_.push_back(std::make_shared<Enemy1>(fileMng_, stage_.get(), x, y));
		break;
	default:
		break;
	}
}

//敵生成関数(ボス敵)
void SceneGame::AddBoss(EnemyBase::ENEMY_TYPE type, float x, float y)
{
	switch (type)
	{
	case EnemyBase::ENEMY_TYPE::E_TYPE_BOSS_1:
		enemyList_.push_back(std::make_shared<Boss1>(fileMng_, stage_.get(), x, y));
		break;
	default:
		break;
	}
}

void SceneGame::TransitionOut(float t)
{
	if (IsClear())
	{
		// クリア時の遷移アニメーション
		_fadeAlpha = EaseOutCubic(t) * 255.0f;
		gaussRatio = static_cast<int>(t * 1000);
	}
	else
	{
		_fadeAlpha = EaseOutCubic(t) * 255.0f;
	}
}

void SceneGame::TransitionIn(float t)
{
	float e = EaseInCubic(1 - t);
	_fadeAlpha = e * 255.0f;
}
