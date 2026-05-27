#include "StageConfigTablle.h"
#include "Stage.h"
#include "SceneGame.h"
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
				//敵の生成
				scene.GetEnemyList().push_back(std::make_shared<Enemy1>(
					scene.GetFileManager(),
					scene.GetStage(),
					800.0f,
					160.0f
				)
				);
				scene.GetEnemyList().push_back(std::make_shared<Boss1>(
					scene.GetFileManager(),
					scene.GetStage(),
					1200.0f,
					160.0f
				)
				);
			},
			

		},
		{
			2,
			"Resource/MapJson/stage_02.json",
			[](SceneGame& scene)
			{

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