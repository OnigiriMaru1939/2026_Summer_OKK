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

// 静的変数の定義
int SceneGame::selectedStageIndex_ = 1;

SceneGame::SceneGame(FileManager& fileMng) : SceneSuper(fileMng)
{

	stage_ = std::make_unique<Stage>(fileMng);
	player_ = std::make_unique<Player>(fileMng, stage_.get());
	//敵の生成
	enemyList_.push_back(std::make_shared<Enemy1>(
			fileMng,
		    stage_.get(),
			800.0f,
			160.0f
		)
	);
	enemyList_.push_back(std::make_shared<Boss1>(
			fileMng,
			stage_.get(),
			1200.0f,
			160.0f
		)
	);

	player_->SetImage("Resource/Image/Monster.png");

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

	//デバッグ用に敵のHPを表示
	int y = 1000;

	for (auto& enemy : enemyList_)
	{
		DrawFormatString(
			1200,
			y,
			GetColor(255, 0, 0),
			"%s HP: %d",
			enemy->GetEnemyName(),
			enemy->GetHp()
		);

		y += 20;
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
	}
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
			enemy->ApplyDamage(
				static_cast<int>(player_->GetAttackDamage())
			);
			if (enemy->IsAlive())
			{
				//プレイヤーをノックバックさせる
				player_->PlayerKnockBack(enemy->GetX(), 10.0f);
			}
		}
	}
}