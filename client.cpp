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


int main(){
  Sockcom_c *comv;
  comv= new Sockcom_c(2000,"127.0.0.1");
  std::string st;
  st = "start";
  comv->sendd(st,0);
  
  VectorXd hogev(6);
  comv->recvv(hogev,0);
  showvec(hogev);
  VectorXf hogef(6);
  hogef(0) = 6;hogef(1) = 5;hogef(2) = 4;
  hogef(3) = 3;hogef(4) = 2;hogef(5) = 1;
  comv->sendd(hogef,0);
  comv->waittoallend();
  delete comv;
}

