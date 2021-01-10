#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <Eigen/Dense>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/StdVector>
#include <vector>

#include "mymath.h"
#include "sequence.h"
#include "socketcom.h"


void sample_function(Sockcom *so){
  std::string st;
  so->recvv(st,0);
  std::cout << st << std::endl;
  VectorXd hoge(6);
  hoge(0) = 1;hoge(1) = 2;hoge(2) = 3;
  hoge(3) = 4;hoge(4) = 5;hoge(5) = 6;
  so->sendd(hoge,0);
  VectorXf hogef(6);
  so->recvv(hogef,0);
  showvec(hogef);
}

int main(){
  Sockcom_s *com;
  com = new Sockcom_s(2000,"127.0.0.1");
  com->setrunnignfunctiuon(&sample_function);
  com->waittoallend();
  std::cout << "end" << std::endl;
  delete com;
}
