------------------------------------------------------------------------------

               CAPCOM CPS2 Emulator for PSP 2.3.1 for PSP Slim

                このプログラムは PSP-2000 + CFW 3.71専用です。

------------------------------------------------------------------------------

<概要>

  PSP用のCAPCOM CPS2(Capcom Play System II)エミュレータです。

------------------------------------------------------------------------------
対応ROMセットについて

  MAME 0.120に準拠させているため、zipファイル名はMAME 0.120が要求するROM
  セット名と一致させる必要があります。

  全てのROMイメージファイルはzipファイルに圧縮する必要があります。
  フォルダに展開したファイルを扱えないことを除けば、MAMEと全く同じです。
  また、MAMEが対応していないROMセットには対応していません。

  zipファイル内のROMイメージファイル名はどんな名前でも構いませんが、CRCは
  MAME 0.120のROMセットのCRCと一致している必要があります。

  ファイルブラウザ上で白く表示されているゲームは全て動作します。
  動作しない場合はROMセットが要求するものと異なっているということです。
  ClrMame ProやRomCenter等のツールを使って、MAME 0.120以降のROMセットに一致
  しているか確認してください。

------------------------------------------------------------------------------
フォルダ設定

フォルダは全て初回起動時に自動的に作成されます。

 /PSP/GAME/  (または/PSP/GAME3xx/)
      |
      +- CPS2PSP/
         |  |
         |  +- EBOOT.PBP         CPS2PSP本体
         |  +- SystemButtons.prx システムボタン押下状態取得PRX
         |  +- cps2psp.ini       CPS2PSP設定情報記録ファイル (自動的に作成)
         |  +- rominfo.cps2      CPS2 ROMセットデータベース  (必須)
         |  +- zipname.cps2      CPS2 英語ゲーム名リスト     (必須)
         |  +- zipnamej.cps2     CPS2 日本語ゲーム名リスト   (任意)
         |  +- command.dat       MAME Plus!用command.dat (コマンドリスト/任意)
         |
         +- config/   ゲーム個別設定ファイル用フォルダ
         |
         +- nvram/    EEPROMデータ保存用フォルダ
         |
         +- snap/     スクリーンショット用フォルダ
         |
         +- state/    ステートデータ用フォルダ
         |
         +- roms/     ROMイメージファイル用フォルダ (ZIP形式で圧縮)
         |    |
         |    +- 19xx.zip   (例: 19xx)
         |    +- ssf2.zip   (例: Super Street Fighter II (parent))
         |    +- ssf2j.zip  (例: Super Street Fighter II (clone: Japanese ver.))
         |    +- ...

------------------------------------------------------------------------------
ユーザーインタフェースの表示言語について

・ver.2.2.2でバイナリを統合し、PSP本体の表示言語が日本語の場合は日本語で、
  それ以外の場合は英語で表示します。

・zipnamej.datは日本語を使用しない場合は不要ですので、削除してかまいません。

・旧バージョンでresource_jp.zipに含まれていた日本語フォントを使用していた場合
  は、不要になりましたので削除してください。

------------------------------------------------------------------------------
操作方法

・ゲーム実行中の画面とメインメニューを除き、全てのメニューで"Rトリガ"を押す
  ことで操作ヘルプが表示されるようになっています。
  わからなければとにかく"Rトリガ"を押してください。
  見ればわかると思うので、詳細は割愛します。

・ゲームの設定等を変更するメニューは、ゲーム実行中に"HOME"を押すことで表示
  されます。

・ゲーム中のボタン操作
  ボタンの割り当ては変更可能です。以下にデフォルトの設定を書いておきます。

  以下の場合には、自動的にボタン配置を画面に合わせて反転/回転しますので、
  特に意識して変更する必要はありません。
  ・サービスメニューでFlip Screenの項目を有効にした場合
  ・縦画面のゲームでRotate ScreenをYesにした場合

  共通
    Up    - Up or Analog Up
    Down  - Down or Analog Down
    Left  - Left or Analog Left
    Right - Right or Analog Right
    Start - Start
    Coin  - Select

  2ボタンのゲーム
    Button 1 - Square
    Button 2 - Triangle

  3ボタンのゲーム
    Button 1 - Square
    Button 2 - Triangle
    Button 3 - Cross

  6ボタンのゲーム
    Button 1 - Square
    Button 2 - Triangle
    Button 3 - L trigger
    Button 4 - Cross
    Button 5 - Circle
    Button 6 - R trigger

  クイズなないろドリームス専用 (方向ボタンは使用しません)
    Button 1 - Square
    Button 2 - Cross
    Button 3 - Triangle
    Button 4 - Circle
    プレイヤー切り替え - L trigger

  特殊操作
    HOME: メニューを開く
    L + R + SELECT: サービススイッチ (特定のボタンに割り当ても可能)
    L + R + START:  1P & 2P START (サービスメニューで使用)

  AdHoc時
  ※ファイルブラウザで□(Square)でゲームを選択するとAdHocモードで
    ゲームを起動します。
    HOME: ポーズ、切断ダイアログの表示
