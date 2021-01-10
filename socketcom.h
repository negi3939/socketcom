#ifndef COMMINICATION_SCOK_H
#define COMMINICATION_SCOK_H

using namespace Eigen;
using namespace Mymath;
#define PRINT_MAT(X) std::cout << #X << ":\n" << X << std::endl << std::endl

namespace SOCK{
  enum FUNC{
    UNSET=0,
    SET=1,
  };
}

class Structthis{
	public:
		void *num;
		void *send;
		void *instthis;
};

class Structipport{
  public:
    int portnum;
    char *ip_addr;
};

class Sockcom {
  protected:
    int port;
    int max_clinum;
    int endfl;
    int endallfl;
    int maxsize;
    int maxstringsize;
    char *ip;
    char *host;
    int *sock;
    SOCK::FUNC fucflag;
    pthread_t *centerthread;
    pthread_t *sockthread;
    pthread_mutex_t mutex;
    pthread_mutex_t fucset_mutex;
    pthread_mutex_t return_mutex;
    void (*running_function)(Sockcom *so);
    void *funcreturn;
    void connectToServer( int portnum , const char *ip_addr );
    void connectFromClient( int portnum , const char *ip_addr );
    void connectFromClient(void *send);//client数に応じたmulti thread起動のためのthread
    static void* launchCfromS(void *pParam){
        	reinterpret_cast<Sockcom*>(reinterpret_cast<Structthis*>(pParam)->instthis)->connectFromClient(reinterpret_cast<Structthis*>(pParam)->send);
        	pthread_exit(NULL);
    }
    static void* launchthread(void *pParam){
        	reinterpret_cast<Sockcom*>(reinterpret_cast<Structthis*>(pParam)->instthis)->sock_func(reinterpret_cast<Structthis*>(pParam)->send);
        	pthread_exit(NULL);
    }
  public:
    Sockcom();
    ~Sockcom();
    void waittoend();
    void waittoallend();
    void exitsock();
    void exitallsock();
    int sendd(void *buf,int len,int thrnum);
    int sendd(std::string s,int thrnum);
    int sendd(double num,int thrnum);
    int sendd(float num,int thrnum);
    int sendd(int num,int thrnum);
    int sendd(VectorXd &eigenV,int thrnum);
    int sendd(VectorXf &eigenV,int thrnum);
    int sendd(std::vector<VectorXd> &vectorV,int thrnum);
    int sendd(std::vector<VectorXf> &vectorV,int thrnum);
    int sendd(Sequence<double,VectorXd> &seq,int thrnum);
    int sendd(Sequence<float,VectorXf> &seq,int thrnum);
    int recvv(void *buf,int thrnum);
    int recvv(std::string &s,int thrnum);
    int recvv(double *num,int thrnum);
    int recvv(float *num,int thrnum);
    int recvv(int *num,int thrnum);
    int recvv(VectorXd &eigenV,int thrnum);
    int recvv(VectorXf &eigenV,int thrnum);
    int recvv(std::vector<VectorXd> &vectorV,int n,int an,int thrnum);
    int recvv(std::vector<VectorXf> &vectorV,int n,int an,int thrnum);
    int recvv(Sequence<double,VectorXd> &seq,int n,int an,int thrnum);
    int recvv(Sequence<float,VectorXf> &seq,int n,int an,int thrnum);
    virtual void sock_func(void *send);
    void setrunnignfunctiuon(void(*f)(Sockcom *));
    void getfunc_return(void* p);
    void setfuncreturn(void* p);
};


class Sockcom_s : public Sockcom{
  protected:
    void init();
  public:
    Sockcom_s();
    Sockcom_s(int p,char *ipp);
    ~Sockcom_s();
    void sock_func(void *send);
    virtual void sock_funccontent(int thnum);
};

class Sockcom_c : public Sockcom{
  protected:
    void init();
  public:
    Sockcom_c();
    Sockcom_c(int p,char *h);
    ~Sockcom_c();
    void sock_func(void *send);
    virtual void sock_funccontent(int thnum);
};

#endif