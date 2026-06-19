#include "Mentos.h"
#include "Player.h"


Mentos::Mentos(FileManager& fileMng, Stage* stage, float x, float y):ItemBase(fileMng, stage, x, y)
{
	SetImage("Resource/Image/Cider_Player_NoWeapon.png");
	itemTypp_ = ITEM_TYPE::MENTOS;
	SetPosition(x, y);
}

Mentos::~Mentos()
{

}

void Mentos::Update()
{

}

void Mentos::OnGet(Player& player)
{
	player.SetPlayerJumpMag(2.0f);
	alive_ = false;
}
