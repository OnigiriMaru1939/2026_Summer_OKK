#include "StageConfigTablle.h"
#include "Stage.h"
#include "SceneGame.h"
#include "Player.h"
#include "Enemy1.h"
#include "Boss1.h"
const std::vector<StageConfig>& GetStageConfigs()
{
	static std::vector<StageConfig> configs = {
		{
			1,
			"Resource/MapJson/stage_01.json",
			[](SceneGame& scene)
			{
				scene.GetStage()->SetBgImage("Resource/Image/Stage1_bg.png");
				scene.GetStage()->SetMchipImage("Resource/MapChip/Mapchip_neon.png");
				//プレイヤーの初期化
				scene.GetPlayer()->SetPosition(100.0f, 1840.0f);
				//敵の生成
				scene.AddEnemy(EnemyBase::ENEMY_TYPE::E_TYPE_1, 500.0f, 200.0f);
				scene.AddEnemy(EnemyBase::ENEMY_TYPE::E_TYPE_1, 800.0f, 1700.0f);
				scene.AddEnemy(EnemyBase::ENEMY_TYPE::E_TYPE_1, 1200.0f,1500.0f);
				scene.AddEnemy(EnemyBase::ENEMY_TYPE::E_TYPE_1, 800.0f, 1200.0f);
				scene.AddBoss(EnemyBase::ENEMY_TYPE::E_TYPE_BOSS_1, 1200.0f, 200.0f);
			},
			

		},
		{
			2,
			"Resource/MapJson/stage_02.json",
			[](SceneGame& scene)
			{
			scene.GetStage()->SetMchipImage("Resource/MapChip/Mapchip_neon.png");
			//プレイヤーの初期化
      scene.GetPlayer()->SetPosition(100.0f, 3480.0f);
			//敵の生成
			scene.AddEnemy(EnemyBase::ENEMY_TYPE::E_TYPE_1, 500.0f, 200.0f);
			scene.AddEnemy(EnemyBase::ENEMY_TYPE::E_TYPE_1, 800.0f, 1700.0f);
			
			},

		},
		{
			3,
			"Resource/MapJson/stage_03.json",
			[](SceneGame& scene)
			{
			
			},

		},
		{
			4,
			"Resource/MapJson/stage_04.json",
			[](SceneGame& scene)
			{

			},

		}
	};
	return configs;
}