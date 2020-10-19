# socket通信class
ソケット通信が簡単に浸かるように抽象化

# make
makeするとserver.outとclient.outができる．
./server.outを実行後に別ターミナルで./client.outを実行すればSequence<float VectorXf>型の情報が送受信される．

# 送受信データ
全て一旦，double[]やfloat[]に変換して送受信する．関数に引数にEigen::Vectorやvector<Eigen::Vector>が使えるので，おおちゃくできる．
Sequenceは時間とEigen::Vectorを可変長さで入れるタイプ．中身はvectorでpush_back()が使える．