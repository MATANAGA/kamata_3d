#include "GameScene.h"

using namespace KamataEngine;

void GameScene::Initialize() {
	const uint32_t kNumBlockVirtical = 10;
	const uint32_t kNumBlockHorizontal = 20;
	const float kBlockWidth = 2.0f;
	const float kBlockHeight = 2.0f;

	debugCamera_ = new DebugCamera(1280, 720);
	modelSkydome_ = Model::CreateFromOBJ("SkyDome", true);

	// 先按行数 resize
	worldTransformBlocks_.resize(kNumBlockVirtical);
	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);

		for (uint32_t j = 0; j < kNumBlockHorizontal; j++) {
			// 棋盘格模式：仅在 (i + j) 是偶数时放砖块
			if ((i + j) % 2 == 0) {
				worldTransformBlocks_[i][j] = new WorldTransform();
				worldTransformBlocks_[i][j]->Initialize();
				worldTransformBlocks_[i][j]->translation_.x = kBlockWidth * j;
				worldTransformBlocks_[i][j]->translation_.y = kBlockHeight * i;
			} else {
				worldTransformBlocks_[i][j] = nullptr; // 空位
			}
		}
	}

	skydome_ = new Skydome();
	skydome_->Initialize();

	camera_.Initialize();
	model_ = new BlockModel();
	model_->Initialize();
}

void GameScene::Update() {
#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_0)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif
	model_->Update();
	debugCamera_->Update();
	// modelSkydome_->Update();
	for (const auto& line : worldTransformBlocks_) {
		for (WorldTransform* block : line) {
			if (!block)
				continue;
			block->matWorld_ = MakeAffineMatrrix(block->scale_, block->rotation_, block->translation_);
			block->TransferMatrix();
		}
	}
	if (isDebugCameraActive_) {
		debugCamera_->Update();
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		camera_.TransferMatrix();
	} else {
		camera_.UpdateMatrix();
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

	if (skydome_) {
		skydome_->Update(cameraPos);
	}
}

void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	KamataEngine::Model::PreDraw(dxCommon->GetCommandList());

	if (skydome_) {
		skydome_->Draw(camera_);
	}

	for (const auto& line : worldTransformBlocks_) {
		for (WorldTransform* block : line) {
			if (!block)
				continue;
			model_->Draw(*block, camera_);
		}
	}

	KamataEngine::Model::PostDraw();
}


GameScene::~GameScene() {
	delete model_;
	delete debugCamera_;
	delete modelSkydome_;
	for (auto& line : worldTransformBlocks_) {
		for (WorldTransform* block : line) {
			delete block; // delete nullptr is safe
		}
	}
	worldTransformBlocks_.clear();
}
