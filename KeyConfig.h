#pragma once
#include <map>
#include <string>
#include <vector>
enum class InputType
{
	Keyboard,
	GamepadButton,
	GamepadAxis,
	Mouse
};

struct KeyMapping
{
	InputType inputType;
	int code; // キーコード、ゲームパッドのボタン番号、マウスボタン番号など
	float scale = 1.0f; // ゲームパッドの軸など、値にスケールをかけたい場合に使用
};

using KeyConfigMap = std::map<std::wstring, std::vector<KeyMapping>>;

class KeyConfig
{
public:
	// シングルトン
	static KeyConfig& GetInstance()
	{
		static KeyConfig instance;
		return instance;
	}
	bool LoadFromFile(const std::wstring& filename);
	const std::vector<KeyMapping>& GetMappings(const std::wstring& action) const;

private:
	KeyConfig();
	~KeyConfig();
	KeyConfigMap keyConfigMap_;
};

