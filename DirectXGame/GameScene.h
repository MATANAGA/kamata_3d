#pragma once
#include "Block.h"
#include "CameraController.h"
#include "Enemy.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "MyMath.h"
#include "Player.h"
#include "Skydome.h"
#include <cassert>
#include <iostream>
#include <vector>
#include "DeathParticels.h"

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
	KamataEngine::Model* modelDeathParticle_ = nullptr;
	KamataEngine::Model* modelEnemy_ = nullptr; // モデルは共用

private:
	CameraController* cameraController_ = nullptr;
	KamataEngine::Camera camera_;
	Player* model_ = nullptr;
	BlockModel* block_ = nullptr;
	Skydome* skydome_ = nullptr;
	MapChipField* mapChipField_ = nullptr;
	DeathParticles* deathParticles_ = nullptr;


	std::vector<Enemy*> enemies_;               // 複数の敵
};
