#include "ItemBase.h"


class Apple : public ItemBase
{
public:
	Apple(FileManager& fileMng, Stage* stage, float x, float y);
	~Apple();
	void Update() override;
	void OnGet(Player& player) override;
};


