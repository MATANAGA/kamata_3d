#pragma once
#include <cassert>
#include <iostream>
#include <vector>
#include "KamataEngine.h"
#include "Block.h"
#include "MyMath.h"
#include "Skydome.h"
#include "Player.h"

class GameScene {
public:
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;
	~GameScene();
	void Initialize();
	void Update();
	void Draw();
	bool isDebugCameraActive_ = false;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
	KamataEngine::Model* modelSkydome_ = nullptr;

private:
	KamataEngine::Camera camera_;
	Player* player_ = nullptr;
	BlockModel* block_ = nullptr;
	Skydome* skydome_ = nullptr;
};
