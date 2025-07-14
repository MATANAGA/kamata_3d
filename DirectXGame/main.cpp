#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"
#include <Windows.h>

using namespace KamataEngine;

// ==== シーンタイプ ====
enum class Scene {
	kTitle,
	kGame,
	kUnknown = 0,
};

// ==== グローバル変数 ====
Scene scene = Scene::kTitle;

TitleScene* titleScene = nullptr;
GameScene* gameScene = nullptr;

// ==== シーン切り替え ====
void ChangeScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene && titleScene->IsFinished()) {
			delete titleScene;
			titleScene = nullptr;

			gameScene = new GameScene();
			gameScene->Initialize();

			scene = Scene::kGame;
		}
		break;

	case Scene::kGame:
		if (gameScene && gameScene->IsFinished()) {
			delete gameScene;
			gameScene = nullptr;

			titleScene = new TitleScene();
			titleScene->Initialize();

			scene = Scene::kTitle;
		}
		break;
	}
}

// ==== 毎フレーム更新 ====
void UpdateScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene)
			titleScene->Update();
		break;
	case Scene::kGame:
		if (gameScene)
			gameScene->Update();
		break;
	}
}

// ==== 毎フレーム描画 ====
void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene)
			titleScene->Draw();
		break;
	case Scene::kGame:
		if (gameScene)
			gameScene->Draw();
		break;
	}
}

// ==== WinMainエントリーポイント ====
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// エンジン初期化
	KamataEngine::Initialize(L"GC2C_01_エイハツ_AL3");
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 初期シーン（タイトル）
	titleScene = new TitleScene();
	titleScene->Initialize();

	// メインループ
	while (true) {
		// エンジンの更新（×ボタンでtrueになる）
		if (KamataEngine::Update()) {
			break;
		}

		// シーン更新＆切り替え
		UpdateScene();
		ChangeScene();

		// 描画開始
		dxCommon->PreDraw();

		DrawScene();

		// 描画終了
		dxCommon->PostDraw();
	}

	// 最後のシーン解放（nullptrでも安全）
	delete titleScene;
	titleScene = nullptr;

	delete gameScene;
	gameScene = nullptr;

	// エンジン終了処理
	KamataEngine::Finalize();

	return 0;
}
