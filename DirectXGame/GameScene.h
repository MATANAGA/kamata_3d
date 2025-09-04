#pragma once
#include "Block.h"
#include "CameraController.h"
#include "DeathParticels.h"
#include "Enemy.h"
#include "EnemyB.h"
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
	enum class Phase { kPlay, kDeathWait, kFadeOutToTitle };

	Phase phase_;

	static constexpr float kFadeOutToClear = 1.0f; // 通关淡出持续时间 1 秒
	float fadeTimer_ = 0.0f;                       // 通关淡出计时器

public:
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;
	~GameScene();
	void Initialize();
	void Update();
	void Draw();
	void GenerateBlocks();
	void ChangePhase();

	bool IsFinished() const { return finished_; }
	bool IsDeathWaitFinished() const { return phase_ == Phase::kFadeOutToTitle; }
	Player* GetPlayer() const { return model_; } // 访问玩家

	bool isDebugCameraActive_ = false;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
	KamataEngine::Model* modelSkydome_ = nullptr;
	KamataEngine::Model* modelPlayer_ = nullptr;
	KamataEngine::Model* modelDeathParticle_ = nullptr;
	KamataEngine::Model* modelEnemy_ = nullptr;

private:
	bool finished_ = false;
	float deathTimer_ = 0.0f;
	std::vector<EnemyB*> enemiesB_; // EnemyB 容器
	KamataEngine::Model* modelEnemyB_ = nullptr;
	Model* modelPlayerNormal_ = nullptr;
	Model* modelPlayerDead_ = nullptr;
	CameraController* cameraController_ = nullptr;
	KamataEngine::Camera camera_;
	Player* model_ = nullptr;
	BlockModel* block_ = nullptr;
	Skydome* skydome_ = nullptr;
	MapChipField* mapChipField_ = nullptr;
	DeathParticles* deathParticles_ = nullptr;
	Fade* fade_ = nullptr;
	std::vector<DeathParticles*> enemyDeathParticles_;

	std::vector<Enemy*> enemies_;

	// 音声
	uint32_t bgmHandle_ = 0; // 游戏 BGM
	bool bgmPlaying_ = false;

	uint32_t deathSoundHandle_ = 0; // 死亡音效
	bool deathSoundPlayed_ = false;
	uint32_t hitSoundHandle_ = 0; // 死亡音效
	bool hitSoundPlayed_ = false;

	bool deadMode = false;
};
