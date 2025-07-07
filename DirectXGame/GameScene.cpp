#include "GameScene.h"
#include "MapChipField.h"
using namespace KamataEngine;

void GameScene::Initialize() {


	debugCamera_ = new DebugCamera(1280, 720);

	skydome_ = new Skydome();
	skydome_->Initialize();

	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");
	GenerateBlocks();

	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, 1);

	model_ = new Player();
	modelPlayer_ = Model::CreateFromOBJ("player"); 
	model_->Initialize(modelPlayer_, &camera_, playerPosition);

	// ↓↓↓ 敵の初期化 ↓↓↓
	Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(10, 1);
	enemy_ = new Enemy();
	modelEnemy_ = Model::CreateFromOBJ("enemy"); // モデルファイル名に注意
	enemy_->Initialize(modelEnemy_, &camera_, enemyPosition);
	// ↑↑↑ 敵の初期化 ↑↑↑

	model_->SetMapChipField(mapChipField_);  // ← この行をプレイヤー初期化後に追加

	camera_.Initialize();
	block_ = new BlockModel();
	block_->Initialize();

	// カメラコントローラ生成
	cameraController_ = new CameraController();
	cameraController_->Initialize();
	cameraController_->SetTarget(model_);
	cameraController_->Reset();
	CameraController::Rect cameraArea = {12.0f, 88.0f, 6.0f, 6.0f};
	cameraController_->SetMovableArea(cameraArea);
}
void GameScene::Update() {

#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_0)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif
	block_->Update();
	model_->Update();
	debugCamera_->Update();
	if (enemy_) {
		enemy_->Update();
	}
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

	if (skydome_) {
		skydome_->Update(cameraPos);
	}
	
	if (isDebugCameraActive_) {
		// デバッグカメラ更新処理
	} else {
		// カメラコントローラの更新
		cameraController_->Update();

		// GameSceneのカメラにView/Projectionを転送
		camera_.matView = cameraController_->GetViewProjection().matView;
		camera_.matProjection = cameraController_->GetViewProjection().matProjection;

		// 実カメラへ転送
		camera_.TransferMatrix();
	}
}

void GameScene::Draw() {

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	KamataEngine::Model::PreDraw(dxCommon->GetCommandList());

	Model::PreDraw(dxCommon->GetCommandList());

	if (skydome_) {
		skydome_->Draw(camera_);
	}
	if (enemy_) {
		enemy_->Draw();
	}
	for (const auto& line : worldTransformBlocks_) {
		for (WorldTransform* blockTransform : line) {
			if (!blockTransform)
				continue;
			block_->Draw(*blockTransform, camera_);
		}
	}

	model_->Draw(); //////////////

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
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransform->scale_ = {1.0f, 1.0f, 1.0f}; // 增加这一行来放大墙壁

				// 指定インデックスの座標を取得して設置
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
	delete model_;
	delete mapChipField_;
	delete modelEnemy_; // ← モデルがあれば削除
	delete enemy_;

	for (auto& line : worldTransformBlocks_) {
		for (WorldTransform* block : line) {
			delete block;
		}
	}
	worldTransformBlocks_.clear();
}
