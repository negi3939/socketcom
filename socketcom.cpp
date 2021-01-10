#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <Eigen/Dense>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/StdVector>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include <stdio.h>

#include "mymath.h"
#include "sequence.h"
#include "socketcom.h"


Sockcom::Sockcom(){
  max_clinum = 30;
  endfl = 0;
  endallfl = 0;
  maxsize = 1024;
  maxstringsize = 10;
  fucflag = SOCK::UNSET;
  pthread_mutex_init(&fucset_mutex,NULL);
  pthread_mutex_init(&return_mutex,NULL);
}

Sockcom::~Sockcom(){}

void Sockcom::connectToServer( int portnum , const char *ip_addr ){
  struct sockaddr_in   addr;
  unsigned short       port;
  int                  sock2=0 , flag=0;
  pthread_mutex_init(&mutex,NULL);
  long l_count =0;
  while(sock2<=0){
    if( portnum < 1024 || portnum > 65535 ){
      fprintf( stderr , "error: invarid port number %d\n" , portnum );
      sock2 = -2;
    }else{
      sock2             = socket( PF_INET , SOCK_STREAM , 0 );
      port             = (unsigned short)portnum;
      addr.sin_port    = htons( port );
      inet_aton( ip_addr , &(addr.sin_addr) );
      addr.sin_family  = PF_INET;
      flag             = connect( sock2 , (struct sockaddr *)&addr , sizeof( struct sockaddr_in ) );
      if( flag < 0 ){
        sock2 = flag;
      }
    }
    l_count++;
    if(l_count%10000==0){std::cout << "wait for connecting.   sock is " << sock2 << " count " << l_count <<  std::endl;}
  }
  
  int thnum = 0;
  sock[thnum] = sock2;
  Structthis *sockksend = new Structthis;
	sockksend->instthis = (void*)this;
  sockksend->send = (void*)thnum;
  pthread_create(&sockthread[0],NULL,this->launchthread,sockksend);
  sleep(1);
  delete sockksend;
}

void Sockcom::connectFromClient( int portnum , const char *ip_addr ){
  pthread_mutex_init(&mutex,NULL);
  centerthread = new pthread_t;
  //threadに渡す情報の準備
  Structipport *sendipport = new Structipport;//ipとportnameを登録インスタンス
  sendipport->portnum = portnum;
  sendipport->ip_addr = ip_addr;
  Structthis *sockksend = new Structthis;//このSockcomそのもののインスタンスを登録
  sockksend->instthis = (void*)this;
  sockksend->send = (void*)sendipport;//ipとportname登録インスタンスを格納
  pthread_create(centerthread,NULL,this->launchCfromS,sockksend);
  sleep(1);
}

//client数に応じたmulti thread起動のためのthread
void Sockcom::connectFromClient(void *send){
  int portnum; char *ip_addr;
  Structipport *sendipport = (Structipport *)send;
  portnum = sendipport->portnum;
  ip_addr = sendipport->ip_addr;
  std::cout << "portnum : " << portnum << "  ip_addr : " << ip_addr <<std::endl;

  struct sockaddr_in saddr;
  unsigned short     port;
  int                sock1 , L;
  if( portnum < 1024 || portnum > 65535 ){
    fprintf( stderr , "error: invarid port number %d\n" , portnum );
    sock1 = -2;
  }else{
    sock1            = socket( PF_INET , SOCK_STREAM , 0 );
    saddr.sin_family = PF_INET;
    port             = (unsigned short)portnum;
    saddr.sin_port   = htons( port );
    inet_aton( ip_addr , &(saddr.sin_addr) );
    bind( sock1 , (struct sockaddr *)&saddr , sizeof( struct sockaddr_in ) );
    L                = listen( sock1 , 1 );
  }
  struct sockaddr_in caddr;
  socklen_t          length;
  int                sock2;
  length = sizeof( struct sockaddr_in );
  for(int ii=0;ii<max_clinum;ii++){
    sock2  = accept( sock1 , (struct sockaddr *)&caddr , &length );
    std::cout << "accepted as num : " << ii <<std::endl;
    sock[ii] = sock2;
    Structthis *sockksend = new Structthis;
	  sockksend->instthis = (void*)this;
    sockksend->send = (void*)ii;
    pthread_create(&sockthread[ii],NULL,this->launchthread,sockksend);
    sleep(1);
    delete sockksend;
  }
  close( sock1 );
}

void Sockcom::waittoend(){
  int l_endfl=0;
  while(1){
    pthread_mutex_lock(&mutex);
    l_endfl = endfl;
    pthread_mutex_unlock(&mutex);
    if(l_endfl!=0){break;}
  }
}

void Sockcom::waittoallend(){
  int l_endallfl=0;
  while(1){
    pthread_mutex_lock(&mutex);
    l_endallfl = endallfl;
    pthread_mutex_unlock(&mutex);
    if(l_endallfl!=0){break;}
  }
}

void Sockcom::exitsock(){
  pthread_mutex_lock(&mutex);
  endfl = 1;
  pthread_mutex_unlock(&mutex);
  std::cout << "exit a socket com" << std::endl;
}

void Sockcom::exitallsock(){
  pthread_mutex_lock(&mutex);
  endallfl = 1;
  pthread_mutex_unlock(&mutex);
  std::cout << "exit all socket coms" << std::endl;
}

int Sockcom::sendd(void *buf,int len,int thrnum){
  int ret;
  ret = send( sock[thrnum] , buf , len , 0 );
  return ret;
}
int Sockcom::sendd(std::string s,int thrnum){
  int ret;
  char *sendch = new char[maxstringsize];
  std::strcpy(sendch,s.c_str());
  ret = send( sock[thrnum] , sendch , maxstringsize*sizeof(char), 0 );
  delete[] sendch;
  return ret;
}

int Sockcom::sendd(double num,int thrnum){
  return send( sock[thrnum] , &num , sizeof(double) , 0 );
}

int Sockcom::sendd(float num,int thrnum){
  return send( sock[thrnum] , &num , sizeof(float) , 0 );
}


int Sockcom::sendd(int num,int thrnum){
  int ret;
  ret = send( sock[thrnum] , &num , sizeof(int) , 0 );
  return ret;
}

int Sockcom::sendd(VectorXd &eigenV,int thrnum){
  double *senddouble = new double[eigenV.size()];
  for(int ii = 0;ii<eigenV.size();ii++){
    senddouble[ii] = eigenV(ii);
  }
  int ret = 0;
  while(1){
    if(eigenV.size()*sizeof(double) - ret > maxsize){
      ret += send( sock[thrnum] , senddouble + ret/sizeof(double) ,maxsize , 0 );
    }else{
      ret += send( sock[thrnum] , senddouble + ret/sizeof(double) , eigenV.size()*sizeof(double) - ret , 0 );
    }
    if(ret>=eigenV.size()*sizeof(double)){break;}
  }
  delete[] senddouble;
  return ret;
}

int Sockcom::sendd(VectorXf &eigenV,int thrnum){
  float *sendfloat = new float[eigenV.size()];
  for(int ii = 0;ii<eigenV.size();ii++){
    sendfloat[ii] = eigenV(ii);
  }
  int ret = 0;
  while(1){
    if(eigenV.size()*sizeof(float) - ret > maxsize){
      ret += send( sock[thrnum] , sendfloat + ret/sizeof(float) ,maxsize , 0 );
    }else{
      ret += send( sock[thrnum] , sendfloat + ret/sizeof(float) , eigenV.size()*sizeof(float) - ret , 0 );
    }
    if(ret>=eigenV.size()*sizeof(float)){break;}
  }
  delete[] sendfloat;
  return ret;
}

int Sockcom::sendd(std::vector<VectorXd> &vectorV,int thrnum){
  double *senddouble = new double[vectorV.size()*vectorV[0].size()];
  for(int ii = 0;ii<vectorV.size();ii++){
    for(int jj = 0;jj<vectorV[0].size();jj++){
      senddouble[ii*vectorV[0].size()+jj] = vectorV[ii](jj);
    }
  }
  int ret = 0;
  while(1){
    if(vectorV.size()*vectorV[0].size()*sizeof(double) - ret > maxsize){
      ret += send( sock[thrnum] , senddouble + ret/sizeof(double) ,maxsize , 0 );
    }else{
      ret += send( sock[thrnum] , senddouble + ret/sizeof(double) , vectorV.size()*vectorV[0].size()*sizeof(double) - ret , 0 );
    }
    if(ret>=vectorV.size()*vectorV[0].size()*sizeof(double)){break;}
  }
  delete[] senddouble;
  return ret;
}
int Sockcom::sendd(std::vector<VectorXf> &vectorV,int thrnum){
  float *sendfloat = new float[vectorV.size()*vectorV[0].size()];
  for(int ii = 0;ii<vectorV.size();ii++){
    for(int jj = 0;jj<vectorV[0].size();jj++){
      sendfloat[ii*vectorV[0].size()+jj] = vectorV[ii](jj);
    }
  }
  int ret = 0;
  while(1){
    if(vectorV.size()*vectorV[0].size()*sizeof(float) - ret > maxsize){
      ret += send( sock[thrnum] , sendfloat + ret/sizeof(float) ,maxsize , 0 );
    }else{
      ret += send( sock[thrnum] , sendfloat + ret/sizeof(float) , vectorV.size()*vectorV[0].size()*sizeof(float) - ret , 0 );
    }
    if(ret>=vectorV.size()*vectorV[0].size()*sizeof(float)){break;}
  }
  delete[] sendfloat;
  return ret;
}

int Sockcom::sendd(Sequence<double,VectorXd> &seq,int thrnum){
  double *senddouble = new double[seq.size()*(seq.vecsize()+1)];
  double timm;
  VectorXd vvv(seq.vecsize());
  for(int ii = 0;ii<seq.size();ii++){
    seq.get(ii,timm,vvv);
    senddouble[ii*(seq.vecsize()+1)] = timm; 
    for(int jj = 0;jj<seq.vecsize();jj++){
      senddouble[ii*(seq.vecsize()+1) + jj+1] = vvv(jj);
    }
  }
  int ret = 0;
  while(1){
    if(seq.size()*(seq.vecsize()+1)*sizeof(double) - ret > maxsize){
      ret += send( sock[thrnum] , senddouble + ret/sizeof(double) , maxsize , 0 );
    }else{
      ret += send( sock[thrnum] , senddouble + ret/sizeof(double) , seq.size()*(seq.vecsize()+1)*sizeof(double) - ret , 0 );
    }
    if(ret >= seq.size()*(seq.vecsize()+1)*sizeof(double)){break;}
  }
  delete[] senddouble;
  return ret;
}

int Sockcom::sendd(Sequence<float,VectorXf> &seq,int thrnum){
  float *sendfloat = new float[seq.size()*(seq.vecsize()+1)];
  float timm;
  VectorXf vvv(seq.vecsize());
  for(int ii = 0;ii<seq.size();ii++){
    seq.get(ii,timm,vvv);
    sendfloat[ii*(seq.vecsize()+1)] = timm; 
    for(int jj = 0;jj<seq.vecsize();jj++){
      sendfloat[ii*(seq.vecsize()+1) + jj+1] = vvv(jj);
    }
  }
  int ret = 0;
  while(1){
    if(seq.size()*(seq.vecsize()+1)*sizeof(float) - ret > maxsize){
      ret += send( sock[thrnum] , sendfloat + ret/sizeof(float) , maxsize , 0 );
    }else{
      ret += send( sock[thrnum] , sendfloat + ret/sizeof(float) , seq.size()*(seq.vecsize()+1)*sizeof(float) - ret , 0 );
    }
    if(ret >= seq.size()*(seq.vecsize()+1)*sizeof(float)){break;}
  }
  delete[] sendfloat;
  return ret;
  return ret;
}

int Sockcom::recvv(void *buf,int thrnum){
  int ret = recv( sock[thrnum] , buf , maxsize , 0 );
  return ret;
}

int Sockcom::recvv(std::string &s,int thrnum){
  char *buf = new char[maxstringsize];
  //int ret = recv( sock[thrnum] , buf , maxstringsize*sizeof(char) , 0 );
  int ret = 0;
  while(1){
    if(maxstringsize*sizeof(char) - ret > maxsize){
      ret += recv( sock[thrnum] , buf , maxsize , 0 );
    }else{
      ret += recv( sock[thrnum] , buf , maxstringsize*sizeof(char) - ret , 0 );
    }
    if(ret >= maxstringsize*sizeof(char)){break;}
  }
  char *buff = new char[ret/sizeof(char)];

  for(int ii=0;ii<ret;ii++){
    buff[ii] = buf[ii];
  }
  std::string bs(buff);
  delete[] buf;
  delete[] buff;
  s = bs;
  return ret;
}

int Sockcom::recvv(double *num,int thrnum){
  int ret = recv( sock[thrnum] , num , sizeof(double) , 0 );
  return ret;
}

int Sockcom::recvv(float *num,int thrnum){
  int ret = recv( sock[thrnum] , num , sizeof(float) , 0 );
  return ret;
}

int Sockcom::recvv(int *num,int thrnum){
  int ret = recv( sock[thrnum] , num , sizeof(int) , 0 );
  return ret;
}

int Sockcom::recvv(VectorXd &eigenV,int thrnum){
 double *revd = new double[eigenV.size()];
 int ret=0;
 while(1){
   if(eigenV.size()*sizeof(double) - ret > maxsize){
     ret += recv( sock[thrnum] , revd + ret/sizeof(double) , maxsize , 0 );
   }else{
    ret += recv( sock[thrnum] , revd + ret/sizeof(double) , eigenV.size()*sizeof(double) - ret , 0 );
   }
   if(ret>=eigenV.size()*sizeof(double)){break;}
 }
  for(int ii = 0;ii<eigenV.size();ii++){
    eigenV(ii) = revd[ii];
  }
  delete[] revd;
  return ret;
}

int Sockcom::recvv(VectorXf &eigenV,int thrnum){
float *revd = new float[eigenV.size()];
int ret=0;
 while(1){
   if(eigenV.size()*sizeof(float) - ret > maxsize){
     ret += recv( sock[thrnum] , revd + ret/sizeof(float) , maxsize , 0 );
   }else{
    ret += recv( sock[thrnum] , revd + ret/sizeof(float) , eigenV.size()*sizeof(float) - ret , 0 );
   }
   if(ret>=eigenV.size()*sizeof(float)){break;}
 } 
  for(int ii = 0;ii<eigenV.size();ii++){
    eigenV(ii) = revd[ii];
  }
  delete[] revd;
  return ret;
}

int Sockcom::recvv(std::vector<VectorXd> &vectorV,int n,int an,int thrnum){
  double *revd = new double[n*an];
  int ret=0;
  while(1){
   if(an*(n+1)*sizeof(double) - ret > maxsize){
     ret += recv( sock[thrnum] , revd + ret/sizeof(double) , maxsize , 0 );
   }else{
    ret += recv( sock[thrnum] , revd + ret/sizeof(double) , an*(n+1)*sizeof(double) - ret , 0 );
   }
   if(ret>=an*(n+1)*sizeof(double)){break;}
 }
  int ii = 0;
  int retd = ret/sizeof(double);
  VectorXd bufv(n);
  while(1){
    if(ii>retd){break;}
    for(int jj=0;jj<n;jj++){
      bufv(jj) = revd[ii];
      ii++;
    }
    vectorV.push_back(bufv);
  }
  delete[] revd;
  return ret;
}

int Sockcom::recvv(std::vector<VectorXf> &vectorV,int n,int an,int thrnum){
  float *revd = new float[n*an];
  int ret = 0;
  while(1){
   if(an*(n+1)*sizeof(float) - ret > maxsize){
     ret += recv( sock[thrnum] , revd + ret/sizeof(float) , maxsize , 0 );
   }else{
    ret += recv( sock[thrnum] , revd + ret/sizeof(float) , an*(n+1)*sizeof(float) - ret , 0 );
   }
   if(ret>=an*(n+1)*sizeof(float)){break;}
 }
  int ii = 0;
  int retd = ret/sizeof(float);
  VectorXf bufv(n);
  while(1){
    if(ii>retd){break;}
    for(int jj=0;jj<n;jj++){
      bufv(jj) = revd[ii];
      ii++;
    }
    vectorV.push_back(bufv);
  }
  delete[] revd;
  return ret;
}

int Sockcom::recvv(Sequence<double,VectorXd> &seq,int n,int an,int thrnum){
  int ret=0;
  double timm;
  double *revd;
  VectorXd bufv(n);
  revd = new double[an*(n+1)];
  while(1){
    if(an*(n+1)*sizeof(double)- ret > maxsize){
      ret += recv( sock[thrnum] , revd + ret/sizeof(double) , maxsize , 0 );
    }else{
      ret += recv( sock[thrnum] , revd + ret/sizeof(double) , an*(n+1)*sizeof(double)- ret , 0 );
    }
    if(ret>=an*(n+1)*sizeof(double)){break;}
  }
  for(int ii=0;ii<an;ii++){
    timm = (double)revd[ii*(n+1)];
    for(int jj=1;jj<n+1;jj++){
      bufv(jj-1) = (double)revd[ii*(n+1) + jj];
    }
    seq.push_back(timm,bufv);
  }
  delete[] revd;
  return ret;
}

int Sockcom::recvv(Sequence<float,VectorXf> &seq,int n,int an,int thrnum){
  int ret=0;
  float timm;
  float *revd;
  VectorXf bufv(n);
  revd = new float[an*(n+1)];
  while(1){
    if(an*(n+1)*sizeof(float)- ret > maxsize){
      ret += recv( sock[thrnum] , revd + ret/sizeof(float) , maxsize , 0 );
    }else{
      ret += recv( sock[thrnum] , revd + ret/sizeof(float) , an*(n+1)*sizeof(float)- ret , 0 );
    }
    if(ret>=an*(n+1)*sizeof(float)){break;}
  }
  for(int ii=0;ii<an;ii++){
    timm = (float)revd[ii*(n+1)];
    for(int jj=1;jj<n+1;jj++){
      bufv(jj-1) = (float)revd[ii*(n+1) + jj];
    }
    seq.push_back(timm,bufv);
  }
  delete[] revd;
  return ret;
}

void Sockcom::sock_func(void *send){
  std::cout << "Override this function! " << std::endl;
}

void Sockcom::setrunnignfunctiuon(void(*f)(Sockcom *)){
  running_function = f;
  pthread_mutex_lock(&fucset_mutex);
  fucflag = SOCK::SET;
  pthread_mutex_unlock(&fucset_mutex);
}

void Sockcom::getfunc_return(void* p){
  pthread_mutex_lock(&return_mutex);
  p = funcreturn;
  pthread_mutex_unlock(&return_mutex);
}

void Sockcom::setfuncreturn(void* p){
  pthread_mutex_lock(&return_mutex);
  funcreturn = p;
  pthread_mutex_unlock(&return_mutex);
}

Sockcom_s::Sockcom_s(){
  std::cout << "this is server" << std::endl;
  port = 2000;
  char *ipp = "127.0.0.1";
  ip = ipp;
  init();
}

Sockcom_s::Sockcom_s(int p,char *ipp){
  std::cout << "this is server" << std::endl;
  port = p;
  ip = ipp;
  init();
}

Sockcom_s::~Sockcom_s(){
  close(sock);
  delete[] sock;
  delete[] sockthread;
}

void Sockcom_s::init(){
  sock = new int[max_clinum];
  sockthread = new pthread_t[max_clinum];
  connectFromClient( port ,ip);
}

void Sockcom_s::sock_func(void *send){
  int thnum = (int) send;
  std::string buffer;
  recvv(buffer,thnum);
  std::cout << buffer << std::endl;
  buffer = "connectted";
  sendd(buffer,thnum);
  sock_funccontent(thnum);
}

void Sockcom_s::sock_funccontent(int thnum){
  /*std::string st;
  recvv(st,thnum);
  std::cout << st << std::endl;
  VectorXd hoge(6);
  hoge(0) = 1;hoge(1) = 2;hoge(2) = 3;
  hoge(3) = 4;hoge(4) = 5;hoge(5) = 6;
  sendd(hoge,thnum);
  VectorXf hogef(6);
  recvv(hogef,thnum);
  showvec(hogef);
  exitsock();*/

  SOCK::FUNC l_fucflag;
  while(1){
    pthread_mutex_lock(&fucset_mutex);
    l_fucflag = fucflag;
    pthread_mutex_unlock(&fucset_mutex);
    if(l_fucflag==SOCK::SET){break;}
  }
  while(1){
    pthread_mutex_lock(&return_mutex);
    running_function(this);
    pthread_mutex_unlock(&return_mutex);
    usleep(10000);
  }
}

Sockcom_c::Sockcom_c() : Sockcom(){
  std::cout << "this is client" << std::endl;
  port=2000;
  char *h ="127.0.0.1";
  host = h;
  init();
}

Sockcom_c::Sockcom_c(int p,char *h) : Sockcom(){
  std::cout << "this is client" << std::endl;
  port = p;
  host = h;
  init();
}

Sockcom_c::~Sockcom_c(){
  close(sock);
  delete[] sock;
  delete[] sockthread;
}

void Sockcom_c::init(){
  sock = new int[1];
  sockthread = new pthread_t[1];
  std::cout << "portnum : " << port << "  host : " << host <<std::endl;
  connectToServer( port , host );
}

void Sockcom_c::sock_func(void *send){
  std::cout << "thread launched" << std::endl;
  int thnum = (int) send;
  if( sock[0] < 0 ){
    perror( "Couldn't connect to server\n" );
    exit( 1 );
  }
  std::string buffer;
  buffer = "00/99+";
  sendd(buffer,thnum); 
  recvv(buffer,thnum);
  std::cout << buffer << std::endl;
  sock_funccontent(thnum);
}

void Sockcom_c::sock_funccontent(int thnum){
  /*please override this function*/
  /*std::vector<VectorXf> hogevector;
  std::string st;
  st = "start";
  sendd(st,thnum);
  
  VectorXd hogev(6);
  recvv(hogev,thnum);
  showvec(hogev);
  VectorXf hogef(6);
  hogef(0) = 6;hogef(1) = 5;hogef(2) = 4;
  hogef(3) = 3;hogef(4) = 2;hogef(5) = 1;
  sendd(hogef,thnum);
  exitsock();
  exitallsock();*/
  /*
  SOCK::FUNC l_fucflag;
  while(1){
    pthread_mutex_lock(&fucset_mutex);
    l_fucflag = fucflag;
    pthread_mutex_unlock(&fucset_mutex);
    if(l_fucflag==SOCK::SET){break;}
  }
    pthread_mutex_lock(&return_mutex);
    running_function();
    pthread_mutex_unlock(&return_mutex);
  */
}

#if defined(COMMCLIENT_IS_MAIN)
int main(){
  Sockcom_c *comv;
  comv= new Sockcom_c(2000,"192.168.4.200");
  comv->waittoallend();
  delete comv;
}
#endif

#if defined(COMMSERVER_IS_MAIN)
int main(){
  Sockcom_s *com;
  com = new Sockcom_s(2000,"192.168.4.200");
  com->waittoallend();
  delete com;
}
#endif