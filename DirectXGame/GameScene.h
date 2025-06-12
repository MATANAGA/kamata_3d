#pragma once
#include <cassert>
#include <iostream>
#include <vector>
#include "KamataEngine.h"
#include "Block.h"
#include "MyMath.h"
#include "Skydome.h"
#include "Player.h"
#include "MapChipField.h"

class GameScene {
public:
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;
	~GameScene();
	void Initialize();
	void Update();
	void Draw();
	void GenerateBlocks();
	bool isDebugCameraActive_ = false;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
	KamataEngine::Model* modelSkydome_ = nullptr;
	KamataEngine::Model* modelPlayer_ = nullptr;


private:
	KamataEngine::Camera camera_;
	Player* model_ = nullptr;
	BlockModel* block_ = nullptr;
	Skydome* skydome_ = nullptr;
	MapChipField* mapChipField_;
};
