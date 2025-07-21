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

	// ↓↓↓ 敵の初期化 ↓↓↓
	modelEnemy_ = Model::CreateFromOBJ("enemy"); // 模型只生成一次，多个敌人共用

	// 设定多个生成位置
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
	// ↑↑↑ 敵の初期化 ↑↑↑

	model_->SetMapChipField(mapChipField_); // ← この行をプレイヤー初期化後に追加

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

	//deathParticles_ = new DeathParticles();
	//deathParticles_->Initialize(modelDeathParticle_, &camera_, playerPosition);
	deathParticles_ = nullptr; // 不提前生成，等死亡时再生成
}
void GameScene::Update() {
	switch (phase_) {
	case Phase::kPlay:
		// プレイヤー死亡判定
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
		deathTimer_ += 1.0f / 60.0f; // 2秒待つ
		if (deathTimer_ >= 2.0f) {
			phase_ = Phase::kFadeOutToTitle;
			fade_->Start(Fade::Status::FadeOut, 1.0f); // 1秒フェードアウト開始
		}
		break;

	case Phase::kFadeOutToTitle:
		fade_->Update();
		if (!fade_->IsFading()) {
			finished_ = true; // シーン終了（タイトルに戻る準備完了）
		}
		break;
	}

	// フェードはフェーズによって描画するか制御
	if (fade_) {
		fade_->Update();
	}
#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_0)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif
	block_->Update();
	model_->Update();
	debugCamera_->Update();

	if (deathParticles_ && !deathParticles_->IsFinished()) {
		deathParticles_->Update();
	}

	for (auto& enemy : enemies_) {
		enemy->Update();
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
	
	ChangePhase();
}

void GameScene::Draw() {

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	KamataEngine::Model::PreDraw(dxCommon->GetCommandList());

	Model::PreDraw(dxCommon->GetCommandList());

	if (skydome_) {
		skydome_->Draw(camera_);
	}
	if (deathParticles_ && !deathParticles_->IsFinished()) {
		deathParticles_->Draw();
	}

	for (auto& enemy : enemies_) {
		enemy->Draw();
	}

	for (const auto& line : worldTransformBlocks_) {
		for (WorldTransform* blockTransform : line) {
			if (!blockTransform)
				continue;
			block_->Draw(*blockTransform, camera_);
		}
	}
	if (model_ && model_->IsAlive()) {
		model_->Draw();
	}
	if (fade_) {
		fade_->Draw();
	}
	// model_->Draw(); //////////////

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

void GameScene::ChangePhase() {
	switch (phase_) {
	case GameScene::Phase::kPlay:
		// 通常プレイ状態
		break;

	case GameScene::Phase::kDeathWait:
		// 粒子未生成时才生成
		if (!deathParticles_) {
			deathParticles_ = new DeathParticles();
			deathParticles_->Initialize(modelDeathParticle_, &camera_, model_->GetWorldTransform().translation_);
			std::cout << "死亡パーティクル生成\n";

		}
		break;

	default:
		break;
	}
}


GameScene::~GameScene() {
	delete block_;
	delete debugCamera_;
	delete modelSkydome_;  // 如果没赋值，删掉这行
	delete fade_;
	delete model_;
	delete mapChipField_;
	delete deathParticles_;
	delete modelDeathParticle_;

	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear();

	delete modelEnemy_; // 只删一次

	for (auto& line : worldTransformBlocks_) {
		for (WorldTransform* block : line) {
			delete block;
		}
	}
	worldTransformBlocks_.clear();
}
