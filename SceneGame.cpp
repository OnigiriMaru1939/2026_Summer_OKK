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
	player_ = std::make_unique<Player>(fileMng, stage_.get());

  // ステージ固有のセットアップを実行
	const auto& stageConfigs = GetStageConfigs();
	const StageConfig& config = stageConfigs[selectedStageIndex_ - 1];
	stage_->Load(config.mapPath);
	config.setUpStage(*this);
	InputManager::GetInstance().SetTriggerCallback(ActionID::Decide, 
												   [this]()
												   {
													   SetNextScene(SceneID::RESULT);
													   isEnd = true;
												   });
}

SceneGame::~SceneGame()
{

}

void SceneGame::Update()
{
	player_->Update();
  if (!player_->GetAliveFlag())
	{
		sceneMng_.SetGameResult(false); // ゲームオーバー
		SetNextScene(SceneID::RESULT);
		isEnd = true;
	}

	//敵の更新
	UpdateEnemy();

	CheckPlayerEnemyCollision();

	// Stageの更新
	if (stage_) stage_->Update();
}

void SceneGame::Draw()
{
	DrawBox(0, 0, Application::SCREEN_WID, Application::SCREEN_HIG, 0x00aa00, true);

	// Stageを描画
	if (stage_) stage_->Draw();

	DrawString(0, 20, "GAME SCENE", 0xffffff);
	//プレイヤーを描画
	player_->Draw();

	//敵を描画
	for (auto& enemy : enemyList_)
	{
		enemy->Draw();
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
			if (std::dynamic_pointer_cast<IBoss>(enemy))
			{
				sceneMng_.SetGameResult(true); // クリア
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