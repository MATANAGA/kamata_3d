#include "ClearScene.h"
#include "DeathScene.h"
#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"
#include <Windows.h>

enum class SceneType {
	kTitle,
	kGame,
	kDeath,
	kClear,
};

SceneType currentSceneType = SceneType::kTitle;

// 场景指针
TitleScene* titleScene = nullptr;
GameScene* gameScene = nullptr;
DeathScene* deathScene = nullptr;
ClearScene* clearScene = nullptr;

void ChangeScene() {
	switch (currentSceneType) {
	case SceneType::kTitle:
		if (titleScene && titleScene->IsFinished()) {
			delete titleScene;
			titleScene = nullptr;

			gameScene = new GameScene();
			gameScene->Initialize();

			currentSceneType = SceneType::kGame;
		}
		break;

	case SceneType::kGame:
		if (gameScene && gameScene->IsFinished()) {
			delete gameScene;
			gameScene = nullptr;

			clearScene = new ClearScene();
			clearScene->Initialize();

			currentSceneType = SceneType::kClear;
		} else if (gameScene && !gameScene->GetPlayer()->IsAlive() && gameScene->IsDeathWaitFinished()) {
			// 玩家死亡且等待时间结束
			delete gameScene;
			gameScene = nullptr;

			deathScene = new DeathScene();
			deathScene->Initialize();

			currentSceneType = SceneType::kDeath;
		}
		break;

	case SceneType::kDeath:
		if (deathScene && deathScene->IsFinished()) {
			delete deathScene;
			deathScene = nullptr;

			titleScene = new TitleScene();
			titleScene->Initialize();

			currentSceneType = SceneType::kTitle;
		}
		break;

	case SceneType::kClear:
		if (clearScene && clearScene->IsFinished()) {
			delete clearScene;
			clearScene = nullptr;

			titleScene = new TitleScene();
			titleScene->Initialize();

			currentSceneType = SceneType::kTitle;
		}
		break;
	}
}

void UpdateScene() {
	switch (currentSceneType) {
	case SceneType::kTitle:
		if (titleScene)
			titleScene->Update();
		break;
	case SceneType::kGame:
		if (gameScene)
			gameScene->Update();
		break;
	case SceneType::kDeath:
		if (deathScene)
			deathScene->Update();
		break;
	case SceneType::kClear:
		if (clearScene)
			clearScene->Update();
		break;
	}
}

void DrawScene() {
	switch (currentSceneType) {
	case SceneType::kTitle:
		if (titleScene)
			titleScene->Draw();
		break;
	case SceneType::kGame:
		if (gameScene)
			gameScene->Draw();
		break;
	case SceneType::kDeath:
		if (deathScene)
			deathScene->Draw();
		break;
	case SceneType::kClear:
		if (clearScene)
			clearScene->Draw();
		break;
	}
}

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	KamataEngine::Initialize(L"GC2C_01_エイハツ_AL3");
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 初始场景 Title
	titleScene = new TitleScene();
	titleScene->Initialize();

	// 主循环
	while (true) {
		if (KamataEngine::Update())
			break;

		UpdateScene();
		ChangeScene();

		dxCommon->PreDraw();
		DrawScene();
		dxCommon->PostDraw();
	}

	// 释放所有场景
	delete titleScene;
	delete gameScene;
	delete deathScene;
	delete clearScene;

	KamataEngine::Finalize();
	return 0;
}
