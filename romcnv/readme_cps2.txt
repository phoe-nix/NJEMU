スプライトROM 変換ツール ver.2.2.0 (CPS2PSP用)

かなり手抜きですが、変換ツールです。

---------------------------------------------------------------------------
無圧縮キャッシュをまとめて作成する場合

1.cps2conv_all.batをダブルクリックして起動してください。

2.フォルダ選択ダイアログが表示されるので、ROMのフォルダを選択してください。

3.変換作業を開始します。
  ファイルが見つからなかい場合はスキップしますので、終わるまで待ちましょう。

4.cacheフォルダ内に ゲーム名.cacheというファイルが作成されますので、
  /PSP/GAMES/cps2psp/cache ディレクトリにコピーしてください。
  元のROMファイルも/PSP/GAMES/cps2psp/romsにコピーしてください。

5.ゲームを実行すれば、多分動作します。

---------------------------------------------------------------------------
無圧縮キャッシュをゲームを指定して作成する場合

1.cps2conv.batをダブルクリックして起動してください。

2.ファイル選択ダイアログが表示されるので、変換したいゲームのzipファイルを
  選択してください。

3.変換作業を開始します。
  なお、クローンセットの場合は、親ROMセットも同じフォルダ内にある必要が
  あります。

4.cacheフォルダ内に ゲーム名.cacheというファイルが作成されますので、
  /PSP/GAMES/cps2psp/cache ディレクトリにコピーしてください。
  元のROMファイルも/PSP/GAMES/cps2psp/romsにコピーしてください。

5.ゲームを実行すれば、多分動作します。

---------------------------------------------------------------------------
上記がうまくいかない場合

1.cps2conv.batに変換したいゲームのzipファイルをドラッグ＆ドロップして
  ください。

2.変換作業を開始します。
  なお、クローンセットの場合は、親ROMセットも同じフォルダ内にある必要が
  あります。

3.cacheフォルダ内に ゲーム名.cache(cps2conv.batの場合)というファイルが作成
  されますので、
  /PSP/GAMES/cps2psp/cache ディレクトリにコピーしてください。
  元のROMファイルも/PSP/GAMES/cps2psp/romsにコピーしてください。

4.ゲームを実行すれば、多分動作します。

---------------------------------------------------------------------------
romcnv_cps2.exeのコマンドライン引数について

·引数の順番は問いません。
·パスを渡す場合は、フルパスで指定してください。ダブルコーテーションで
  前後を挟んだほうが良いです。

-all: 起動後にダイアログで指定したフォルダにあるファイルで、対応するゲーム
      を全て変換します。指定されたパスは無視します。

-zip: ZIP圧縮キャッシュを作成します。

-batch: バッチ処理用のオプションです。変換後に一時停止しなくなります。


記述例) avspとqndreamはZIP圧縮し、mvscとvsavは無圧縮で変換。
        vsavが終わるまで一時停止しない。

romcnv_cps2.exe "D:\roms\avsp.zip" -zip -batch
romcnv_cps2.exe "D:\roms\qndream.zip" -zip -batch
romcnv_cps2.exe "D:\roms\mvsc.zip" -batch
romcnv_cps2.exe "D:\roms\vsav.zip"

---------------------------------------------------------------------------

Linux等で使用する場合
---------------------

ver.2.0.6よりLinux等でも一応変換できるようにしました。
環境に依存するため、バイナリでの提供はありません。

ソースコードを適当なディレクトリに展開し、

make -f makefile.cps2 UNIX=1

とすることで、Unix系OS用のバイナリを作成します。

使用方法は基本的にWindows版と同じですが、ダイアログでの指定は出来ないので、
コマンドラインで引数を指定する必要があります。

以下にLinuxのbash上で使用する例を書いておきます。

例) /home/username/romcnv/roms内のssf2.zipを変換する場合。

./romcnv_cps2 /home/username/romcnv/roms/ssf2.zip

例) /home/username/romcnv/roms内のssf2.zipをzip圧縮変換する場合。

./romcnv_cps2 /home/username/romcnv/roms/ssf2.zip -zip

例) /home/username/romcnv/roms内の全てのromを変換する場合。

./romcnv_cps2 /home/username/romcnv/roms -all

例) /home/username/romcnv/roms内の全てのromをzip圧縮変換する場合。

./romcnv_cps2 /home/username/romcnv/roms -all -zip


romのディレクトリがromcnv_mvs本体より下の階層にある場合は以下のようにして
指定することも可能です。

romcnv_cps2は /home/username/romcnv-2.0.6にあるとする。

例) /home/username/romcnv-2.0.6/roms内のssf2.zipを変換する場合。

./romcnv_cps2 ./roms/ssf2.zip

例) /home/username/romcnv-2.0.6/roms内の全てのromを変換する場合。

./romcnv_cps2 ./roms -all


---------------------------------------------------------------------------
更新履歴

- ver.2.2.0 -
 ·rominfo.cps2のフォーマットを変更。
 ·キャッシュファイル自体には変更はありませんが、エミュレータのバージョン
   更新に伴い、キャッシュファイルのバージョンもV22に更新されているため、
   再度変換が必要です。

- ver.2.0.6 (3) -
 ·メモリに全て読み込めるゲームは常にzip圧縮を行うよう変更。
 ·rominfo.cps2を少し修正。

- ver.2.0.6 (2) -
 ·ドラッグ&ドロップで変換できなくなっていた不具合の修正。

- ver.2.0.6 -
 ·フォーラム等で動作しないという報告が目に付いたので、作り直してみました。
 ·ついでにMAME 0.112u2で追加されたゲームに対応。
 ·Linux等のUNIX系OSのコンソールで動作するバイナリをコンパイルできるように
   しました。

- ver.2.0 -
 ·バージョン表記を本体に合わせました。
 ·zip圧縮は非推奨なので説明を除去。
 ·zip圧縮キャッシュにもバージョン情報を埋め込むように変更。
 ·mpangとmpangjを分離。
   ごくわずかにデータが異なりますが、おそらく日本語版がBAD DUMPかROMを焼く時に
   ミスったかのどちらかだと思われます。(日本語版は一部にスプライトの欠けあり)
