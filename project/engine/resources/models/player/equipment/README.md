# Player用の剣と盾

青い体・オレンジ色のくちばし・ローポリの形状を持つ既存のPlayerに合わせた装備です。
剣は羽をイメージした鍔と青いグリップ、盾は青い面と羽の紋章を組み合わせています。

## ファイル

| フォルダー | Blender保存データ | 見た目 | UVテクスチャ | UV展開図 |
|---|---|---|---|---|
| sword | sword.blend | sword_preview.png | sword_basecolor.png | sword_uv_layout.png |
| shield | shield.blend | shield_preview.png / shield_back.png | shield_basecolor.png | shield_uv_layout.png |

- Blender 5.1で作成しています。
- 各モデルの外形を囲むボックスの中心と、オブジェクトの原点をワールド原点 `(0, 0, 0)` に合わせています。持ち手中心や重心ではありません。
- Z軸が上下、装飾の正面が+Y方向です。回転は0、スケールは1です。
- 剣と盾はそれぞれ1つのメッシュです。編集モードでは部品単位で選択できます。
- カメラとライトは `Preview Studio` コレクションにまとめています。
- UVは展開済みです。`*_basecolor.png` がモデルに実際に貼られている1024×1024のカラーテクスチャです。
- `*_uv_layout.png` は塗り直し用の透明背景のUV線画です。カラーテクスチャの上に重ねて使えます。
- テクスチャは外部PNGとして書き出し、Blenderファイルにも同じ画像をパックしています。
- プレビューPNGは透明背景です。盾には裏面の持ち手も作ってあります。
- 寸法は `Player_Animation_Ready.blend` の高さ約2.006を基準にしています。既存の `player.gltf` にはルートの0.5倍スケールがあるため、そちらと組み合わせる場合は装備にも同じ倍率を適用してください。

各フォルダーの `validation.json` に寸法・面数・中心座標を記録しています。
既存のPlayerデータは変更していません。
