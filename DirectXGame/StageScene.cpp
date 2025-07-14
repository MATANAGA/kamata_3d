#include "StageScene.h"
#include <iostream>

using namespace KamataEngine;

void StageScene::Initialize() {
	camera_.Initialize();
	std::cout << "StageScene: Initialize完了\n";
}

void StageScene::Update() {
	// デバッグ: Enterキーでゲーム本編に遷移
	if (Input::GetInstance()->PushKey(DIK_RETURN)) {
		finished_ = true;
	}
}

void StageScene::Draw() {
	// 仮描画：今後はここにモデルやUIを描画
	// 今は何もしないでもOK（背景色だけ表示される）
}
