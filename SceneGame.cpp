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
	stage_ = std::make_unique<Stage>(fileMng);
	player_ = std::make_unique<Player>(fileMng, stage_.get(), *this);

  // ステージ固有のセットアップを実行
	const auto& stageConfigs = GetStageConfigs();
	const StageConfig& config = stageConfigs[selectedStageIndex_ - 1];
	stage_->Load(config.mapPath);
	config.setUpStage(*this);

	clearTime = 0.0f;

	_isClear = false;

	_gameScreen = MakeScreen(Application::SCREEN_WID, Application::SCREEN_HIG);

	highBrightScreen = MakeScreen(Application::SCREEN_WID, Application::SCREEN_HIG, false);
	downScaleScreen = MakeScreen(DOWN_SCALE_SCREEN_W, DOWN_SCALE_SCREEN_H, FALSE);
	gaussScreen = MakeScreen(DOWN_SCALE_SCREEN_W, DOWN_SCALE_SCREEN_H, FALSE);

	gaussRatio = 0;
	filterRatio = 200;

	// デバッグ
	InputManager::GetInstance().SetTriggerCallback(ActionID::Cancel, 
												   [this]()
												   {
													   if (!isTransition)
													   {
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
}

void SceneGame::Update()
{
	clearTime += 1.0f / 60.0f; // クリアタイムの更新

	// プレイヤーの更新
	UpdatePlayer();

	//敵の更新
	UpdateEnemy();

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

	//プレイヤーを描画
	player_->Draw();

	//敵を描画
	for (auto& enemy : enemyList_)
	{
		enemy->Draw();
	}
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
			player_->Damage(10.0f); // プレイヤーにダメージを与える
			//プレイヤーをノックバックさせる
			player_->PlayerKnockBack(enemy->GetX(), 10.0f);
		}
		else if (hit && player_->GetAttakFlag())
		{
			enemy->ApplyDamage(static_cast<int>(player_->GetAttackDamage()));
			if (enemy->IsAlive())
			{
				//プレイヤーをノックバックさせる
				player_->PlayerKnockBack(enemy->GetX(), 10.0f);
			}
		}
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
}
