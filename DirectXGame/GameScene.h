#pragma once
#include "Block.h"
#include "CameraController.h"
#include "DeathParticels.h"
#include "Enemy.h"
#include "Fade.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "MyMath.h"
#include "Player.h"
#include "Skydome.h"
#include <cassert>
#include <iostream>
#include <vector>

class GameScene {
	
	enum class Phase {
		kPlay,
		kDeathWait,     // 死亡判定後、2秒待機フェーズ
		kFadeOutToTitle // フェードアウト演出フェーズ
	};

	Phase phase_;

public:
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;
	~GameScene();
	void Initialize();
	void Update();
	void Draw();
	void GenerateBlocks();
	void ChangePhase();
	bool isDebugCameraActive_ = false;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
	KamataEngine::Model* modelSkydome_ = nullptr;
	KamataEngine::Model* modelPlayer_ = nullptr;
	KamataEngine::Model* modelDeathParticle_ = nullptr;
	KamataEngine::Model* modelEnemy_ = nullptr;   // モデルは共用
	bool IsFinished() const { return finished_; } // ← 添加这个

private:
	bool finished_ = false; // ← 添加这个
	float deathTimer_ = 0.0f;

	CameraController* cameraController_ = nullptr;
	KamataEngine::Camera camera_;
	Player* model_ = nullptr;
	BlockModel* block_ = nullptr;
	Skydome* skydome_ = nullptr;
	MapChipField* mapChipField_ = nullptr;
	DeathParticles* deathParticles_ = nullptr;
	Fade* fade_ = nullptr;

	std::vector<Enemy*> enemies_; // 複数の敵
};
