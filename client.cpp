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
  Sockcom_c *comv;
  comv= new Sockcom_c;
  Sequence<float,VectorXf> hogeseq;
  std::vector<VectorXf> hogevector;
  std::string st;
  st = "start";
  comv->sendd(st);
  //comv->recvv(hogevector,6,1000);
  //PRINT_MAT(hogevector[999]);
  comv->recvv(hogeseq,6,100);
  hogeseq.show(99);
  delete comv;
}