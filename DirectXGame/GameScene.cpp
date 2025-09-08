#include "GameScene.h"
#include "MapChipField.h"
using namespace KamataEngine;

void GameScene::Initialize() {

	phase_ = Phase::kPlay;
	fade_ = new Fade();
	fade_->Initialize();

	debugCamera_ = new DebugCamera(1280, 720);

	// 天球模型初始化
	modelSkydomeNormal_ = KamataEngine::Model::CreateFromOBJ("SkyDome");
	modelSkydomeSpecial_ = KamataEngine::Model::CreateFromOBJ("SkyDomeSpecial"); // 另一种天球
	skydomeTransform_.Initialize();

	// 根据模式选择当前天球
	currentSkydomeModel_ = skydomeSpecialMode_ ? modelSkydomeSpecial_ : modelSkydomeNormal_;
	skydomeTransform_.Initialize();

	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");
	GenerateBlocks();

	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(5, 8);

	model_ = new Player();
	modelPlayerNormal_ = Model::CreateFromOBJ("player");
	modelPlayerDead_ = Model::CreateFromOBJ("player2");

	if (deadMode) {
		model_->Initialize(modelPlayerDead_, &camera_, playerPosition);
	} else {
		model_->Initialize(modelPlayerNormal_, &camera_, playerPosition);
	}
	modelDeathParticle_ = Model::CreateFromOBJ("playerDead");

	// 敌人 A 死亡特效模型
	modelEnemyADeath_ = Model::CreateFromOBJ("zonbiface");

	// 敌人 B 死亡特效模型
	modelEnemyBDeath_ = Model::CreateFromOBJ("gost");

	// 敌人初始化
	modelEnemy_ = Model::CreateFromOBJ("zonbiface");
	std::vector<Vector3> enemyPositions = {
		mapChipField_->GetMapChipPositionByIndex(12, 1),
	    mapChipField_->GetMapChipPositionByIndex(25, 8), 
		mapChipField_->GetMapChipPositionByIndex(12, 6),
	    mapChipField_->GetMapChipPositionByIndex(32, 8), 
		mapChipField_->GetMapChipPositionByIndex(40, 2),

	};
	for (const auto& pos : enemyPositions) {
		Enemy* enemy = new Enemy();
		enemy->Initialize(modelEnemy_, &camera_, pos);
		enemies_.push_back(enemy);
	}
	remainingEnemies_ += static_cast<int>(enemyPositions.size());

	// EnemyB 初始化
	modelEnemyB_ = Model::CreateFromOBJ("gost"); // OBJ 文件名

	std::vector<Vector3> enemyBPositions = {
	    mapChipField_->GetMapChipPositionByIndex(10, 5),
		mapChipField_->GetMapChipPositionByIndex(15, 1), 
		mapChipField_->GetMapChipPositionByIndex(18, 3),
	    mapChipField_->GetMapChipPositionByIndex(21, 4), 
		mapChipField_->GetMapChipPositionByIndex(23, 5), 
	    // 根据地图位置添加
	};

	for (const auto& pos : enemyBPositions) {
		EnemyB* enemyB = new EnemyB();
		enemyB->Initialize(modelEnemyB_, &camera_, pos);
		enemiesB_.push_back(enemyB);
	}
	remainingEnemies_ += static_cast<int>(enemyBPositions.size());

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
	deathSoundHandle_ = Audio::GetInstance()->LoadWave("dead_bgm.wav");
	deathSoundPlayed_ = false;
	// 加载死亡音效
	hitSoundHandle_ = Audio::GetInstance()->LoadWave("2f.wav");
	hitSoundPlayed_ = false;

	// 加载 BGM
	bgmHandle_ = Audio::GetInstance()->LoadWave("title_bgm.wav");
	bgmPlaying_ = false;

	// 加载 BGM
	haSoundHandle_ = Audio::GetInstance()->LoadWave("clear_bgm.wav");
	haSoundPlayed_ = false;
}

void GameScene::Update() {
	// 游戏 BGM 循环播放（只播放一次）
	if (!bgmPlaying_) {
		bgmHandle_ = Audio::GetInstance()->PlayWave(bgmHandle_, true);
		bgmPlaying_ = true;
	}
	// GameScene.cpp Update()
	// 按键切换玩家模型
	// 按键请求切换（触发淡出）
	// 模式切换按键
	if (!fade_->IsFading() && switchPhase_ == SwitchPhase::None) {
		if (Input::GetInstance()->TriggerKey(DIK_1) && skydomeSpecialMode_) {
			// 当前是特殊模式 → 切换到普通模式
			nextSpecialMode_ = false;
			Audio::GetInstance()->PlayWave(haSoundHandle_, false);

			fade_->Start(Fade::Status::FadeOut, 0.5f);
			switchPhase_ = SwitchPhase::FadingOut;
		}
		if (Input::GetInstance()->TriggerKey(DIK_2) && !skydomeSpecialMode_) {
			// 当前是普通模式 → 切换到特殊模式
			nextSpecialMode_ = true;
			Audio::GetInstance()->PlayWave(haSoundHandle_, false);

			fade_->Start(Fade::Status::FadeOut, 0.5f);
			switchPhase_ = SwitchPhase::FadingOut;
		}
	}


	// Fade 切换逻辑
	if (switchPhase_ == SwitchPhase::FadingOut && !fade_->IsFading()) {
		// 淡出完成 → 切换模型
		if (nextSpecialMode_) {
			model_->SetModel(modelPlayerDead_);
			currentSkydomeModel_ = modelSkydomeSpecial_;
			skydomeSpecialMode_ = true;
		} else {
			model_->SetModel(modelPlayerNormal_);
			currentSkydomeModel_ = modelSkydomeNormal_;
			skydomeSpecialMode_ = false;
		}
		// 切换完毕 → 淡入
		fade_->Start(Fade::Status::FadeIn, 2.5f);
		switchPhase_ = SwitchPhase::FadingIn;
	} else if (switchPhase_ == SwitchPhase::FadingIn && !fade_->IsFading()) {
		// 淡入完成 → 切换结束
		switchPhase_ = SwitchPhase::None;
	}

#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_0)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif

	switch (phase_) {
	case Phase::kPlay: {
		// EnemyA 碰撞
		for (auto& enemy : enemies_) {
			if (!enemy->isAlive_)
				continue;

			if (enemy->CheckCollisionWithPlayer(*model_)) {
				const auto& playerPos = model_->GetWorldTransform().translation_;
				const auto& enemyPos = enemy->GetWorldTransform().translation_;

				// 模式判断：只有普通模式才能击败 EnemyA
				if (!skydomeSpecialMode_) {
					bool stomped = (model_->velocity_.y < 0) && (playerPos.y > enemyPos.y + Enemy::kHeight / 2.0f);
					if (stomped) {
						Audio::GetInstance()->PlayWave(hitSoundHandle_, false);

						DeathParticles* enemyDeath = new DeathParticles();
						enemyDeath->Initialize(modelEnemyADeath_, &camera_, enemy->GetWorldTransform().translation_);
						enemyDeathParticles_.push_back(enemyDeath);

						enemy->isAlive_ = false;
						remainingEnemies_--; // 敌人数量 -1

						model_->velocity_.y = Player::kJumpAcceleration * 0.7f;
					} else {
						// 玩家死亡
						model_->SetAlive(false);
						playerLives_--;
						phase_ = Phase::kDeathWait;
						deathTimer_ = 0.0f;
						if (playerLives_ <= 0) {
							isGameOverDelayed_ = true;
							gameOverTimer_ = 0.0f;
						}
					}
				} else {
					// 模式不对，踩敌人直接死亡
					model_->SetAlive(false);
					playerLives_--;
					phase_ = Phase::kDeathWait;
					deathTimer_ = 0.0f;
					if (playerLives_ <= 0) {
						isGameOverDelayed_ = true;
						gameOverTimer_ = 0.0f;
					}
				}
				break;
			}
		}

		// EnemyB 碰撞
		for (auto& enemyB : enemiesB_) {
			if (!enemyB->isAlive_)
				continue;

			if (enemyB->CheckCollisionWithPlayer(*model_)) {
				const auto& playerPos = model_->GetWorldTransform().translation_;
				const auto& enemyPos = enemyB->GetWorldTransform().translation_;

				// 模式判断：只有特殊模式才能击败 EnemyB
				if (skydomeSpecialMode_) {
					bool stomped = (model_->velocity_.y < 0) && (playerPos.y > enemyPos.y + EnemyB::kHeight / 2.0f);
					if (stomped) {
						Audio::GetInstance()->PlayWave(hitSoundHandle_, false);

						DeathParticles* enemyDeath = new DeathParticles();
						enemyDeath->Initialize(modelEnemyBDeath_, &camera_, enemyB->GetWorldTransform().translation_);
						enemyDeathParticles_.push_back(enemyDeath);

						enemyB->isAlive_ = false;
						remainingEnemies_--; // 敌人数量 -1

						model_->velocity_.y = Player::kJumpAcceleration * 0.7f;
					} else {
						// 玩家死亡
						model_->SetAlive(false);
						playerLives_--;
						phase_ = Phase::kDeathWait;
						deathTimer_ = 0.0f;
						if (playerLives_ <= 0) {
							isGameOverDelayed_ = true;
							gameOverTimer_ = 0.0f;
						}
					}
				} else {
					// 模式不对，碰敌人直接死亡
					model_->SetAlive(false);
					playerLives_--;
					phase_ = Phase::kDeathWait;
					deathTimer_ = 0.0f;
					if (playerLives_ <= 0) {
						isGameOverDelayed_ = true;
						gameOverTimer_ = 0.0f;
					}
				}
				break;
			}
		}

	} break;
	case Phase::kDeathWait:
		deathTimer_ += 1.0f / 60.0f;
		if (deathTimer_ >= 2.0f) { // 死亡动画播放 2 秒
			if (playerLives_ > 0) {
				// 玩家复活
				Vector3 respawnPos = mapChipField_->GetMapChipPositionByIndex(5, 8);
				model_->Reset(respawnPos);
				model_->SetAlive(true);
				phase_ = Phase::kPlay;
				deathSoundPlayed_ = false;
				deathParticles_ = nullptr;
			} else {
				// 残机为0 → Game Over
				phase_ = Phase::kFadeOutToTitle;
				fade_->Start(Fade::Status::FadeOut, 1.0f);
				deathParticles_ = nullptr;
				std::cout << "Game Over" << std::endl;
			}
		}
		break;

	case Phase::kFadeOutToTitle:
		fade_->Update();
		if (!fade_->IsFading())
			finished_ = true;
		break;
	}

	if (fade_)
		fade_->Update();

	block_->Update();
	model_->Update();
	debugCamera_->Update();

	if (deathParticles_ && !deathParticles_->IsFinished())
		deathParticles_->Update();
	for (auto it = enemyDeathParticles_.begin(); it != enemyDeathParticles_.end();) {
		DeathParticles* dp = *it;
		dp->Update();
		if (dp->IsFinished()) {
			delete dp;
			it = enemyDeathParticles_.erase(it);
		} else {
			++it;
		}
	}

	for (auto& enemy : enemies_) {
		if (enemy->isAlive_) {
			enemy->Update();
		}
	}
	for (auto& enemyB : enemiesB_) {
		if (enemyB->isAlive_) {
			enemyB->Update();
		}
	}

	for (const auto& line : worldTransformBlocks_) {
		for (WorldTransform* block : line) {
			if (!block)
				continue;
			block->matWorld_ = MakeAffineMatrrix(block->scale_, block->rotation_, block->translation_);
			block->TransferMatrix();
		}
	}
	if (playerLives_ <= 0) {
		// 没残机 → 延迟进入 GameOver
		isGameOverDelayed_ = true; // 标记延迟状态
		gameOverTimer_ = 0.0f;     // 重置计时
	}
	if (isGameOverDelayed_) {
		gameOverTimer_ += 1.0f / 60.0f; // 每帧累加
		if (gameOverTimer_ >= 3.0f) {   // 3秒后正式进入 Game Over
			phase_ = Phase::kFadeOutToTitle;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
			isGameOverDelayed_ = false;
			finished_ = true; // 标记场景结束
			std::cout << "Game Over" << std::endl;
		}
	}

	Vector3 cameraPos;
	if (isDebugCameraActive_) {
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		camera_.TransferMatrix();
		cameraPos = debugCamera_->GetCamera().translation_;
	} else {
		cameraController_->Update();
		camera_.matView = cameraController_->GetViewProjection().matView;
		camera_.matProjection = cameraController_->GetViewProjection().matProjection;
		camera_.TransferMatrix();
		cameraPos = camera_.translation_;
	}

	skydomeTransform_.translation_ = camera_.translation_; // 跟随相机
	skydomeTransform_.matWorld_ = MakeAffineMatrrix(skydomeTransform_.scale_, skydomeTransform_.rotation_, skydomeTransform_.translation_);
	skydomeTransform_.TransferMatrix();

	if (phase_ == Phase::kPlay && remainingEnemies_ == 0) {
		phase_ = Phase::kFadeOutToTitle;
		fadeTimer_ = 0.0f;
		fade_->Start(Fade::Status::FadeOut, kFadeOutToClear);
	}


	ChangePhase();
}

void GameScene::ChangePhase() {
	if (phase_ == Phase::kDeathWait && !deathParticles_) {
		deathParticles_ = new DeathParticles();
		deathParticles_->Initialize(modelDeathParticle_, &camera_, model_->GetWorldTransform().translation_);

		if (!deathSoundPlayed_) {
			Audio::GetInstance()->PlayWave(deathSoundHandle_, false);
			deathSoundPlayed_ = true;
		}
	}

	if (phase_ == Phase::kFadeOutToTitle) {
		fadeTimer_ += 1.0f / 60.0f; // 每帧累加
		if (fadeTimer_ >= kFadeOutToClear) {
			finished_ = true;
			std::cout << "通关！" << std::endl;
		}
	}
}

void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	KamataEngine::Model::PreDraw(dxCommon->GetCommandList());

	if (currentSkydomeModel_)
		currentSkydomeModel_->Draw(skydomeTransform_, camera_);

	if (deathParticles_ && !deathParticles_->IsFinished())
		deathParticles_->Draw();
	for (auto& dp : enemyDeathParticles_) {
		if (dp && !dp->IsFinished()) {
			dp->Draw();
		}
	}

	for (auto& enemy : enemies_) {
		if (enemy->isAlive_) {
			enemy->Draw();
		}
	}
	for (auto& enemyB : enemiesB_) {
		if (enemyB->isAlive_) {
			enemyB->Draw();
		}
	}

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
	if (bgmPlaying_)
		Audio::GetInstance()->StopWave(bgmHandle_);
	delete block_;
	delete debugCamera_;
	delete modelSkydome_;
	delete fade_;
	delete model_;
	delete mapChipField_;
	delete deathParticles_;
	delete modelPlayerNormal_;
	delete modelPlayerDead_;
	delete modelSkydomeNormal_;
	delete modelSkydomeSpecial_;

	for (Enemy* enemy : enemies_)
		delete enemy;
	enemies_.clear();
	delete modelEnemy_;
	for (EnemyB* enemyB : enemiesB_)
		delete enemyB;
	enemiesB_.clear();
	delete modelEnemyB_;

	for (DeathParticles* dp : enemyDeathParticles_) {
		delete dp;
	}
	enemyDeathParticles_.clear();

	for (auto& line : worldTransformBlocks_)
		for (WorldTransform* block : line)
			delete block;
	worldTransformBlocks_.clear();
}
