#include "StageConfigTablle.h"
#include "Stage.h"
#include "SceneGame.h"

const std::vector<StageConfig>& GetStageConfigs()
{
	static std::vector<StageConfig> configs = {
		{
			1,
			"Resource/MapJson/stage_01.json",
			[](SceneGame& scene)
			{
				scene.GetStage()->SetBgImage("Resource/Image/Stage1_bg.png");
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