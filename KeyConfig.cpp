#include "KeyConfig.h"
#include <fstream>
#include <sstream>
#include <DxLib.h>
#include "InputManager.h"

KeyConfig::KeyConfig()
{
	// デフォルトのキー設定をここで定義してもいいかも
	keyConfigMap_ = {
		{ L"MoveLR",    { 
			{InputType::Keyboard, KEY_INPUT_D, 1.0f},
			{InputType::Keyboard, KEY_INPUT_A, -1.0f},
			{InputType::GamepadAxis, (int)PadAxis::L_X, 1.0f}
		}	},
		{ L"MoveUD",    { 
			{InputType::Keyboard, KEY_INPUT_W, -1.0f},
			{InputType::Keyboard, KEY_INPUT_S, 1.0f},
			{InputType::GamepadAxis, (int)PadAxis::L_Y, 1.0f}
		}	},
		{ L"Jump",		{ 
			{InputType::Keyboard, KEY_INPUT_SPACE, 1.0f},
			{InputType::GamepadButton, (int)PadButton::Face_Down, 1.0f}
		}	} };
}


KeyConfig::~KeyConfig()
{
}

bool KeyConfig::LoadFromFile(const std::wstring& filename)
{
	std::wifstream file(filename);
	if (!file.is_open()) return false;

	keyConfigMap_.clear();
	std::wstring line;
	while (std::getline(file, line))
	{
		if (line.empty() || line[0] == L'#') continue; // 空行やコメントをスキップ

		std::wstringstream ss(line);
		std::wstring action, typeStr, codeStr;

		// 形式例:Jump,Keyboard,32
		if (std::getline(ss, action, L',') && std::getline(ss, typeStr, L',') && std::getline(ss, codeStr, L','))
		{
			KeyMapping mapping;
			mapping.code = std::stoi(codeStr);

			if (typeStr == L"Keyboard") mapping.inputType = InputType::Keyboard;
			else if (typeStr == L"GamepadButton") mapping.inputType = InputType::GamepadButton;
			else if (typeStr == L"GamepadAxis") mapping.inputType = InputType::GamepadAxis;
			else if (typeStr == L"Mouse") mapping.inputType = InputType::Mouse;

			keyConfigMap_[action].push_back(mapping);
		}
	}
	return true;
}

const std::vector<KeyMapping>& KeyConfig::GetMappings(const std::wstring& action) const
{
	static const std::vector<KeyMapping> empty; // アクションが見つからない場合の空のベクター
	auto it = keyConfigMap_.find(action);
	return (it != keyConfigMap_.end()) ? it->second : empty;
}