#include "GameScene.h"
#include "MapChipField.h"
using namespace KamataEngine;

void GameScene::Initialize() {

	debugCamera_ = new DebugCamera(1280, 720);

	skydome_ = new Skydome();
	skydome_->Initialize();

	mapChipField_ = new MapChipField;
	mapChipField_ -> LoadMapChipCsv("Resources/blocks.csv");
	GenerateBlocks();

	player_ = new Player();
	player_->Initialize();

	camera_.Initialize();
	block_ = new BlockModel();
	block_->Initialize();
}
void GameScene::Update() {
#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_0)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif
	block_->Update();
	player_->Update();
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

	Model::PreDraw(dxCommon->GetCommandList());

	if (skydome_) {
		skydome_->Draw(camera_);
	}

	for (const auto& line : worldTransformBlocks_) {
		for (WorldTransform* blockTransform : line) {
			if (!blockTransform)
				continue;
			block_->Draw(*blockTransform, camera_);
		}
	}

	player_->Draw(camera_); //////////////

	KamataEngine::Model::PostDraw();
}

void GameScene::GenerateBlocks() {
	// 获取地图的行列数
	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirticcal();    // 行数（纵向）
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal(); // 列数（横向）

	// 先 resize 行数
	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		// 再为每一行 resize 列数
		worldTransformBlocks_[i].resize(numBlockHorizontal, nullptr);
	}

	// チップの種類に応じて WorldTransform を生成
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::KBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				// 指定インデックスの座標を取得して設置
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransform->translation_ = mapChipField_->GetMapChipPositionByIndex(i, j);
			}
		}
	}
}


GameScene::~GameScene() {
	delete block_;
	delete debugCamera_;
	delete modelSkydome_;
	delete player_;
	delete mapChipField_;
	for (auto& line : worldTransformBlocks_) {
		for (WorldTransform* block : line) {
			delete block;
		}
	}
	worldTransformBlocks_.clear();
}
