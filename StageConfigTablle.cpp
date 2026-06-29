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
				scene.GetStage()->SetBgImage("Resource/Image/Game/Stage1_bg.png");
				scene.GetStage()->SetMchipImage("Resource/MapChip/Mapchip_neon.png");
				//プレイヤーの初期化
				scene.GetPlayer()->SetPosition(150.0f, 2050.0f);
				//ボスエリアの初期設定
				scene.SetBossArea(0, 2500, 1920, 5000 );
				//敵の生成
				//scene.AddEnemy(EnemyBase::ENEMY_TYPE::E_TYPE_1, 800.0f, 1700.0f);
				scene.AddEnemy(EnemyBase::ENEMY_TYPE::E_TYPE_1, 1200.0f,1500.0f);
				scene.AddEnemy(EnemyBase::ENEMY_TYPE::E_TYPE_1, 800.0f, 1200.0f);
				//アイテムの生成
				scene.AddItem(ItemBase::ITEM_TYPE::MENTOS, 1200.0f,1600.0f);
				//テレポートギミックの生成
				scene.AddTeleport(1700.0f, 500.0f, 100.0f,3300.0f);

			
			},
			

		},
		{
			2,
			"Resource/MapJson/stage_02.json",
			[](SceneGame& scene)
			{
			scene.GetStage()->SetBgImage("Resource/Image/Game/Stage1_bg.png");
			scene.GetStage()->SetMchipImage("Resource/MapChip/Mapchip_neon.png");
			//プレイヤーの初期化
			scene.GetPlayer()->SetPosition(100.0f, 3450.0f);
			//ボスエリアの初期設定
			scene.SetBossArea(80, 4600, 1900, 6500);
			//敵の生成
			scene.AddEnemy(EnemyBase::ENEMY_TYPE::E_TYPE_1, 500.0f, 200.0f);
			scene.AddEnemy(EnemyBase::ENEMY_TYPE::E_TYPE_1, 800.0f, 1700.0f);

			//テレポートギミックの生成
			scene.AddTeleport(960.0f, 250.0f, 960.0f, 5500.0f);
			
			},

		},
		{
			3,
			"Resource/MapJson/stage_03.json",
			[](SceneGame& scene)
			{
				scene.GetStage()->SetBgImage("Resource/Image/Game/Stage1_bg.png");
			},

		},
		{
			4,
			"Resource/MapJson/stage_04.json",
			[](SceneGame& scene)
			{
				scene.GetStage()->SetBgImage("Resource/Image/Game/Stage1_bg.png");
			},

		}
	};
	return configs;
}