#include "Apple.h"
#include "Player.h"


Apple::Apple(FileManager& fileMng, Stage* stage, float x, float y) :ItemBase(fileMng, stage, x, y)
{
	SetImage("Resource/Image/Gimmicks/Apple.png");
	itemTypp_ = ITEM_TYPE::APPLE;
	SetPosition(x, y);
}

Apple::~Apple()
{

}

void Apple::Update()
{

}

void Apple::OnGet(Player& player)
{
	if(player.GetPlayerHp() < player.GetPlayerHpMax())
	{
		if( player.GetPlayerHp() + 50 > player.GetPlayerHpMax())
		{
			player.SetPlayerHp(player.GetPlayerHpMax());
			alive_ = false;
		}
		else
		{
			player.SetPlayerHp(player.GetPlayerHp() + 50);
			alive_ = false;
		}
	}
	
}
