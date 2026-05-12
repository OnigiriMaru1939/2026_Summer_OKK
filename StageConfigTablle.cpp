#include "StageConfigTablle.h"
#include "Stage.h"

const std::vector<StageConfig>& GetStageConfigs()
{
	static std::vector<StageConfig> configs = {
		{
			1,
			"Resource/MapJson/stage_01.json",
			[](SceneGame& scene)
			{

			},
			

		},
		{
			2,
			"Resource/Map/json/stage_02.json",
			[](SceneGame& scene)
			{

			},

		},
		{
			3,
			"Resource/Map/json/stage_03.json",
			[](SceneGame& scene)
			{
			
			},

		},
		{
			4,
			"Resource/Map/json/stage_04.json",
			[](SceneGame& scene)
			{

			},

		}
	};
	return configs;
}