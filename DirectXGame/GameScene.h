#pragma once
#include "KamataEngine.h"
#include "Player.h"

class GameScene {
public:
	~GameScene();
	void Initialize();
	void Update();
	void Draw();

private:
	KamataEngine::Camera camera_;
	Player* player_ = nullptr;
};
