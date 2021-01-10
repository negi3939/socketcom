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
  so->setfuncreturn((void*)&st);
}

void read_string(Sockcom *so,std::string &s){
  void *p;
  std::string nullst = "";
  so->getfunc_return(p);
  if(p!=NULL){
    s = static_cast<std::string*>(p);
  }
  s =  nullst;
}

int main(){
  Sockcom_s *com;
  com = new Sockcom_s(2000,"127.0.0.1");
  com->setrunnignfunctiuon(&sample_function);
  com->waittoallend();
  std::string command= "";
  while(1){
    read_string(com,command);
    std::cout << "recv :" << command << std::endl; 
  }
  std::cout << "end" << std::endl;
  delete com;
}
