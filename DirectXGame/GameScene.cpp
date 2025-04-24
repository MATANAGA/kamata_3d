#include "GameScene.h"

using namespace KamataEngine;

void GameScene::Initialize() {
	camera_.Initialize();
	player_ = new Player();
	player_->Initialize();
}

void GameScene::Update() { player_->Update(); }

void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxCommon->GetCommandList());

	player_->Draw(camera_);

	Model::PostDraw();
}

GameScene::~GameScene() { delete player_; }
