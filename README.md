# production2022
 2022/06 ~ 2023/01　個人製作物


/* --- 概要 ---- */

(デバッグ時) F3でデバッグメニュー F4で処理速度

ゲーム名：Shaze

開発期間：約６カ月（うち約４カ月はライブラリ改良）

開発環境：VisualStudio2022, DirectX11, C++

制作人数：１人

使用ライブラリ：
	TinyGLTF：GLTF形式のモデルを読み込むためのヘルパーライブラリ。
	Effekseer153a：対応したツールでエフェクトを作成し、そのエフェクトを描画するためのライブラリ
	ImGui：Debug時に使用。簡単な操作で、多くの便利な機能をもつGUIを描画できる。

利用したツール等：
	GitHubDesktop：バージョン管理、バックアップ。
	Blender：ダウンロードしたモデルの調整、GLTF形式への変換、アニメーションの追加等。
	Mixamo：人型のモデルの自動リギング、それに対応した多くのアニメーションを利用できるサイト
	Affinity2：不足しているテクスチャの作成、UIアセットの作成。
	Effekseer：上記ライブラリを利用したエフェクトの作成。
	RenderDoc：描画結果の詳細の確認や、シェーダー関連のエラーの原因の究明に利用。
	VLCMediaPlayer：利用するサウンドデータの変換に利用。
	golly-4.2-CellGame：セルオートマトンを利用した地形生成を模索していた際に利用。
	Geogebra：当たり判定の計算の確認等に使用。
	ChatGPT：関数、変数の命名や、技術研究の補助等に使用。

/* --- プログラムについて ---- */

実行開始時
	framework で複数のオブジェクトの初期化（DirectX, XInput, XAudio2, その他Graphics関連のクラス）を行った後、
	SceneGame を読み込む。（Debugの場合オーディオの確認を行うシーンを読み込む（そこからゲームシーンへ遷移可能））


グラフィック部分
	GLTF形式のモデルを利用したPBRによる描画。								GLTFModel.h
	Deferredレンダリングによるライティング。								DeferredLighting.h
	全方位シャドウによるシャドウイング。									OmnidirectionalShadow.h
	ブルームを利用するためHDRに対応。(川瀬式ブルーム) 							Bloom.h	  
	インスタンス描画を利用することで、ドローコールを減らし、処理負荷を軽減。				TerrainRenderer.h
	加えて、テクスチャ配列を作成し、マテリアルの切り替えにも対応。

ステージ
	区域分割法とセルオートマトンを利用して自動生成。							TerrainGenerator.h

ゲームシステム
	コンポーネント指向によるゲーム内オブジェクトの管理を行っている。						Actor.h
	StateMachine, BehaviorTreeによる行動遷移							StateMachine.h, BehaviorTree.h
	AnimationEvent(各Actor内に記載)								Actor*.h

※一部プログラム(GLTFの読み込み、スフィアキャスト、wavの読み込み、Component指向クラスのベース部分)は学内配布の物を導入
	
	
	
	
	
