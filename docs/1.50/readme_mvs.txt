------------------------------------------------------------------------------

                NEOGEO  Emulator for PSP 2.3.1 for 1.5 kernel

                  このプログラムは PSP-2000では動作しません。

------------------------------------------------------------------------------

<概要>

  PSP用のNEOGEO(MVS/AES)エミュレータです。

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

  ファイルブラウザ上でグレーで表示されているゲームは、起動できません。

  BIOSはneogeo.zipというファイルにまとめ、romsフォルダに置いてください。

------------------------------------------------------------------------------
フォルダ設定

フォルダは全て初回起動時に自動的に作成されます。

 /PSP/GAME/  (GAMEフォルダが3.xx Kernelの場合は/PSP/GAME150/)
      |
      +- MVSPSP/
         |  |
         |  +- EBOOT.PBP         MVSPSP本体
         |  +- SystemButtons.prx システムボタン押下状態取得PRX
         |  +- mvspsp.ini        MVSPSP設定情報記録ファイル (自動的に作成)
         |  +- rominfo.mvs       MVS ROMセットデータベース  (必須)
         |  +- zipname.mvs       MVS 英語ゲーム名リスト     (必須)
         |  +- zipnamej.mvs      MVS 日本語ゲーム名リスト   (任意)
         |  +- command.dat       MAME Plus!用command.dat (コマンドリスト/任意)
         |
         +- cache/    キャッシュファイル用フォルダ(romcnv_mvs.exeで作成)
         |  |
         |  +- mslug_cache/   (例: Metal Slug) ※作成されたフォルダをコピー
         |
         +- config/   ゲーム個別設定ファイル用フォルダ
         |
         +- memcard/  メモリカード用フォルダ
         |
         +- nvram/    SRAMデータ保存用フォルダ
         |
         +- snap/     スクリーンショット用フォルダ
         |
         +- state/    ステートデータ用フォルダ
         |
         +- roms/     ROMイメージファイル用フォルダ (ZIP形式で圧縮)
         |    |
         |    +- neogeo.zip   (NEOGEO BIOS)
         |    +- samsho.zip   (例: サムライスピリッツ)
         |    +- ...

------------------------------------------------------------------------------
ユーザーインタフェースの表示言語について

・ver.2.2.2でバイナリを統合し、PSP本体の表示言語が日本語の場合は日本語で、
  それ以外の場合は英語で表示します。

・zipnamej.datは日本語を使用しない場合は不要ですので、削除してかまいません。

・旧バージョンでresource_jp.zipに含まれていた日本語フォントを使用していた場合
  は、不要になりましたので削除してください。

------------------------------------------------------------------------------
ROMセットについての補足

・以下のROMセットは、MAMEのROMセットと親子関係が異なるため注意してください。
  (※いずれもbootleg対応版のみ動作可能)

   親セット    クローンセット

   garoup   --- garoubl

   svcboot  -+- svcplus
             +- svcplusa
             +- svcsplus

   kof2k4se --- kf2k4pls

   kof10th  -+- kf10thep
             +- kf2k5uni

   kf2k3bl  -+- kf2k3bla
             +- kf2k3pl
             +- kf2k3upl

キャッシュファイルの作成

・ROM読み込み時に"メモリが足りない"というエラーが表示される場合は、
  グラフィックデータのキャッシュを作成する必要があります。
  付属のromcnv_mvs.exeで作成してください。使い方はromcnv_mvs.exeの
  readme_mvs.txtを参照してください。

・以下のクローンセットは親セットのキャッシュがあればそちらを使用するので、
  親セットのみ作成すればOKです。
  (括弧内が親セット名、bootlegはbootleg対応版でのみ動作)

   aof2a     (aof2)
   fatfursa  (fatfursp)
   kof95a    (kof95)
   samsho3a  (samsho3)
   fswords   (samsho3)
   aof3k     (aof3)
   kof96h    (kof96)
   kof96ep   (kof96)      ※bootleg
   kof97a    (kof97)
   kof97pls  (kof97)      ※bootleg
   lastbldh  (lastblad)
   lastsold  (lastblad)
   shocktra  (shocktro)
   rbff2h    (rbff2)
   rbff2k    (rbff2)
   kof98k    (kof98)
   kof98n    (kof98)
   kof99a    (kof99)
   kof99e    (kof99)
   kof99n    (kof99)
   garouo    (garou)
   mslug3n   (mslug3)
   kof2000n  (kof2000)
   kof2001h  (kof2001)
   kf2k1pls  (kof2001)    ※bootleg
   kf2k1pa   (kof2001)    ※bootleg
   ms4plus   (mslug4)     ※bootleg
   kf2k2pls  (kof2002)    ※bootleg
   kf2k2pla  (kof2002)    ※bootleg
   kf2k2plb  (kof2002)    ※bootleg
   mslug5b   (mslug5)     ※bootleg
   svcpcba   (svcpcb)
   samsho5h  (samsho5)
   samsh5sh  (samsh5sp)
   samsh5sn  (samsh5sp)
   kf2k5uni  (kof10th)    ※bootleg / MAMEと親セットが異なる
   svcplusa  (svcboot)    ※bootleg / MAMEと親セットが異なる
   kf2k3bla  (kf2k3bl)    ※bootleg / MAMEと親セットが異なる

・以下のクローンセットは一部のキャッシュファイルを親セットの
  キャッシュと共有します。[]内のファイルが親セットのキャッシュに
  あればそちらを使用します。
  (括弧内が親セット名、bootkegはbootleg対応版でのみ動作)

   kof97pla  (kof97)    [crom/vrom] ※bootleg
   kog       (kof97)    [vrom]      ※bootleg
   kof99p    (kof99)    [vrom]
   garoubl   (garoup)   [crom]      ※bootleg / MAMEと親セットが異なる
   mslug3b6  (mslug3)   [crom/vrom] ※bootleg
   cthd2003  (kof2001)  [vrom]      ※bootleg
   ct2k3sp   (kof2001)  [vrom]      ※bootleg
   ct2k3sa   (kof2001)  [vrom]      ※bootleg
   kof2002b  (kof2002)  [srom/vrom] ※bootleg
   kf2k2plc  (kof2002)  [crom/vrom] ※bootleg
   kf2k2mp   (kof2002)  [crom/vrom] ※bootleg
   kf2k2mp2  (kof2002)  [crom/vrom] ※bootleg
   matrimbl  (matrim)   [crom/srom] ※bootleg
   ms5plus   (mslug5)   [crom/srom] ※bootleg
   kf2k4pls  (kof2k4se) [crom/srom] ※bootleg / MAMEと親セットが異なる
   kf10thep  (kof10th)  [vrom]      ※bootleg / MAMEと親セットが異なる
   svcplus   (svcboot)  [crom/vrom] ※bootleg / MAMEと親セットが異なる
   svcsplus  (svcboot)  [crom/vrom] ※bootleg / MAMEと親セットが異なる
   kf2k3pl   (kf2k3bl)  [crom/vrom] ※bootleg / MAMEと親セットが異なる
   kf2k3upl  (kf2k3bl)  [crom/vrom] ※bootleg / MAMEと親セットが異なる

------------------------------------------------------------------------------
操作方法

※BIOSの設定画面はファイルブラウザ実行中に"START"を押すことで表示されます。
  "必ずここで設定してから"ゲームを起動してください。

・ゲーム実行中の画面とメインメニューを除き、全てのメニューで"Rトリガ"を押す
  ことで操作ヘルプが表示されるようになっています。
  わからなければとにかく"Rトリガ"を押してください。
  見ればわかると思うので、詳細は割愛します。

・ゲームの設定等を変更するメニューは、ゲーム実行中に"HOME"を押すことで表示
  されます。

・ゲーム中のボタン操作
  ボタンの割り当ては変更可能です。以下にデフォルトの設定を書いておきます。

    Up       - Up or Analog Up
    Down     - Down or Analog Down
    Left     - Left or Analog Left
    Right    - Right or Analog Right
    Start    - Start
    Coin     - Select
    Button A - Cross
    Button B - Circle
    Button C - Square
    Button D - Triangle

  特殊操作
    HOME: メニューを開く
    L + R + SELECT: サービススイッチ (特定のボタンに割り当ても可能)

  AdHoc時
  ※ファイルブラウザで□(Square)でゲームを選択するとAdHocモードで
    ゲームを起動します。
    HOME: ポーズ、切断ダイアログの表示

------------------------------------------------------------------------------
BIOSのRegion/Machine Modeの変更について

・ゲーム設定メニュー内で変更できるようにしていますが、完全に動作するわけでは
  ありません。後期のゲームの場合、この設定を変更するとプロテクトに引っかかり
  動作しないものがあります。
  また、AESのBIOSでMVSのゲームを動作させようとした場合も、同様にプロテクトに
  引っかかって動作しない場合があります。

・確実に変更したいのであれば、uni-biosを使用してください。

------------------------------------------------------------------------------
その他

・メモリカードのファイルはゲームごとに作成されます。
  また、メモリカードは常に認識した状態になっています。

・以下のゲームはMAMEでは動作しますが、このエミュレータではメモリ不足に
  より起動できないため未対応です。

  svcpcb    SvC Chaos - SNK vs Capcom (JAMMA PCB)
