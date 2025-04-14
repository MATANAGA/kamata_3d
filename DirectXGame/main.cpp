#include "GameScene.h"
#include "KamataEngine.h"
#include "StageScene.h"
#include "TitleScene.h"
#include <Windows.h>

using namespace KamataEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	// エンジンの初期化

	KamataEngine::Initialize(L"GC1C_01_エイハツ_AL3");
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	// ゲームシーンのインスタンス作成
	GameScene* gameScene = new GameScene();
	TitleScene* titleScene = new TitleScene();
	StageScene* stageScene = new StageScene();

	// ゲームシーンの初期化
	gameScene->Initialize();
	titleScene->Initialize();
	stageScene->Initialize();

	// メインループ
	while (true) {
		// エンジンの更新
		if (KamataEngine::Update()) {
			break;
		}
		// 描画
		dxCommon->PreDraw();

		// 描画終了
		dxCommon->PostDraw();
	}

	// エンジン終了処理
	KamataEngine::Finalize();

	return 0;
}
