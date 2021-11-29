# GROVE-MIDI-UNIT

Case-3D-data
ケース用3Dデータ(STL)です
3Dプリントする場合はどちらも一番広い面が下になるように配置し、
サポートなしでプリントをお試しください。
サポートをつける場合は側面の文字部分はサポートを除外することをお勧めします。

example
M5ATOM Lite用のサンプルコードです。Matrixでも同様に動くと思います

MIDI-TX-RX-Test
MIDI INコネクタとMIDI OUTコネクタを接続するとループバックしてM5ATOM LiteのLEDを点滅させます

MIDI-SerialLED
MIDI INで受けたMIDI信号によりM5ATOM Liteに接続されたシリアルLED（動作確認はWS2812B）を制御します
受信チャンネルはデフォルトで16ch、どのノートを押しても同じように光ります。
色の制御はHSV方式です。
ノートオンのベロシティ：明るさ(V)
Control Change2：光の減衰早さ
Control Change3：色相(H)
Control Change4：彩度(S)
彩度を下げるとどの色相でも白になります。