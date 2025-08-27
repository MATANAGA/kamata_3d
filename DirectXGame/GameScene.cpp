#include "GameScene.h"
#include "MapChipField.h"
using namespace KamataEngine;

void GameScene::Initialize() {
	phase_ = Phase::kPlay;
	fade_ = new Fade();
	fade_->Initialize();

	debugCamera_ = new DebugCamera(1280, 720);

	skydome_ = new Skydome();
	skydome_->Initialize();

	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");
	GenerateBlocks();

	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(5, 8);

	model_ = new Player();
	modelPlayer_ = Model::CreateFromOBJ("player");
	modelDeathParticle_ = Model::CreateFromOBJ("deathParticle");

	model_->Initialize(modelPlayer_, &camera_, playerPosition);

	// 敌人初始化
	modelEnemy_ = Model::CreateFromOBJ("kunBall");
	std::vector<Vector3> enemyPositions = {
	    mapChipField_->GetMapChipPositionByIndex(10, 1),
	    mapChipField_->GetMapChipPositionByIndex(15, 1),
	    mapChipField_->GetMapChipPositionByIndex(12, 1),
	};
	for (const auto& pos : enemyPositions) {
		Enemy* enemy = new Enemy();
		enemy->Initialize(modelEnemy_, &camera_, pos);
		enemies_.push_back(enemy);
	}

	model_->SetMapChipField(mapChipField_);

	camera_.Initialize();
	block_ = new BlockModel();
	block_->Initialize();

	cameraController_ = new CameraController();
	cameraController_->Initialize();
	cameraController_->SetTarget(model_);
	cameraController_->Reset();
	CameraController::Rect cameraArea = {12.0f, 88.0f, 6.0f, 6.0f};
	cameraController_->SetMovableArea(cameraArea);

	deathParticles_ = nullptr;

	// 加载死亡音效
	deathSoundHandle_ = Audio::GetInstance()->LoadWave("go.wav");

	// 加载BGM
	bgmHandle_ = Audio::GetInstance()->LoadWave("game_bgm.mp3");
	bgmPlaying_ = false;
}

void GameScene::Update() {
	if (!bgmPlaying_) {
		Audio::GetInstance()->PlayWave(bgmHandle_, true);
		bgmPlaying_ = true;
	}

	switch (phase_) {
	case Phase::kPlay:
		if (model_ && model_->IsAlive()) {
			for (auto& enemy : enemies_) {
				if (enemy->CheckCollisionWithPlayer(*model_)) {
					model_->SetAlive(false);
					phase_ = Phase::kDeathWait;
					deathTimer_ = 0.0f;
					break;
				}
			}
		}
		break;

	case Phase::kDeathWait:
		deathTimer_ += 1.0f / 60.0f;
		if (deathTimer_ >= 2.0f) {
			phase_ = Phase::kFadeOutToTitle;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
		}
		break;

	case Phase::kFadeOutToTitle:
		fade_->Update();
		if (!fade_->IsFading()) {
			finished_ = true;
		}
		break;
	}

	if (fade_)
		fade_->Update();

	block_->Update();
	model_->Update();
	debugCamera_->Update();

	if (deathParticles_ && !deathParticles_->IsFinished())
		deathParticles_->Update();

	for (auto& enemy : enemies_)
		enemy->Update();

	for (const auto& line : worldTransformBlocks_) {
		for (WorldTransform* block : line) {
			if (!block)
				continue;
			block->matWorld_ = MakeAffineMatrrix(block->scale_, block->rotation_, block->translation_);
			block->TransferMatrix();
		}
	}

	Vector3 cameraPos;
	if (isDebugCameraActive_) {
		debugCamera_->Update();
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		camera_.TransferMatrix();
		cameraPos = debugCamera_->GetCamera().translation_;
	} else {
		camera_.UpdateMatrix();
		cameraPos = camera_.translation_;
	}

	if (skydome_)
		skydome_->Update(cameraPos);

	if (!isDebugCameraActive_) {
		cameraController_->Update();
		camera_.matView = cameraController_->GetViewProjection().matView;
		camera_.matProjection = cameraController_->GetViewProjection().matProjection;
		camera_.TransferMatrix();
	}

	ChangePhase();
}

void GameScene::ChangePhase() {
	if (phase_ == Phase::kDeathWait && !deathParticles_) {
		deathParticles_ = new DeathParticles();
		deathParticles_->Initialize(modelDeathParticle_, &camera_, model_->GetWorldTransform().translation_);
		std::cout << "死亡粒子生成\n";

		// 播放死亡音效
		if (!deathSoundPlayed_) {
			Audio::GetInstance()->PlayWave(deathSoundHandle_, false);
			deathSoundPlayed_ = true;
		}
	}
}

void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	KamataEngine::Model::PreDraw(dxCommon->GetCommandList());

	if (skydome_)
		skydome_->Draw(camera_);
	if (deathParticles_ && !deathParticles_->IsFinished())
		deathParticles_->Draw();
	for (auto& enemy : enemies_)
		enemy->Draw();
	for (const auto& line : worldTransformBlocks_)
		for (WorldTransform* blockTransform : line)
			if (blockTransform)
				block_->Draw(*blockTransform, camera_);

	if (model_ && model_->IsAlive())
		model_->Draw();
	if (fade_)
		fade_->Draw();

	KamataEngine::Model::PostDraw();
}

void GameScene::GenerateBlocks() {
	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirticcal();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; ++i)
		worldTransformBlocks_[i].resize(numBlockHorizontal, nullptr);

	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransform->scale_ = {1.0f, 1.0f, 1.0f};
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransform->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}

GameScene::~GameScene() {
	delete block_;
	delete debugCamera_;
	delete modelSkydome_;
	delete fade_;
	delete model_;
	delete mapChipField_;
	delete deathParticles_;

	for (Enemy* enemy : enemies_)
		delete enemy;
	enemies_.clear();

	delete modelEnemy_;

	for (auto& line : worldTransformBlocks_)
		for (WorldTransform* block : line)
			delete block;
	worldTransformBlocks_.clear();
}
