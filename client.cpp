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
  comv= new Sockcom_c();
  //comv= new Sockcom_c(2000,"192.168.4.3");//newserver
  comv->waittoallend();
  delete comv;
}

