#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <Eigen/Dense>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/StdVector>
#include <vector>

#include "sequence.h"
#include "socketcom.h"

int main(){
  Sockcom_s *com;
  com = new Sockcom_s;
  std::string st;
  com->recvv(st);
  std::cout << st << std::endl;
  VectorXf hoge(6);
  std::vector<VectorXf> hogevector;
  hoge(0) = 1;
  hoge(1) = 2;
  hoge(2) = 3;
  hoge(3) = 4;
  hoge(4) = 5;
  hoge(5) = 6;
  float timmm;
  VectorXf hov(6);
  Sequence<float,VectorXf> hogeseq;
  for(int ii=0;ii<1000;ii++){
    timmm = 0.1*(float)ii;
    for(int jj=0;jj<6;jj++){
      hoge(jj) = ii*jj;
      hov(jj) = hoge(jj);
    }
    hogeseq.push_back(timmm,hov);
    hogevector.push_back(hov);
  }
  //com->sendd(hogevector);
  com->sendd(hogeseq);
  delete com;
}