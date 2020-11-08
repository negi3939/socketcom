#ifndef COMMINICATION_SCOK_H
#define COMMINICATION_SCOK_H

using namespace Eigen;
#define PRINT_MAT(X) std::cout << #X << ":\n" << X << std::endl << std::endl
#define MAX_SIZE    (1024)

class Structthis{
	public:
		void *num;
		void *send;
		void *instthis;
};

class Sockcom {
  protected:
    int *sock;
    int port;
    int max_clinum;
    int endfl;
    pthread_t *sockthread;
    pthread_mutex_t mutex;
    void connectToServer( int portnum , const char *ip_addr );
    void connectFromClient( int portnum , const char *ip_addr );
    static void* launchthread(void *pParam){
        	reinterpret_cast<Sockcom*>(reinterpret_cast<Structthis*>(pParam)->instthis)->sock_func(reinterpret_cast<Structthis*>(pParam)->send);
        	pthread_exit(NULL);
    	}
  public:
    Sockcom();
    ~Sockcom();
    void waittoend();
    void exitsock();
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
    int recvv(VectorXd &eigenV,int thrnum);
    int recvv(VectorXf &eigenV,int thrnum);
    int recvv(std::vector<VectorXd> &vectorV,int n,int an,int thrnum);
    int recvv(std::vector<VectorXf> &vectorV,int n,int an,int thrnum);
    int recvv(Sequence<double,VectorXd> &seq,int n,int an,int thrnum);
    int recvv(Sequence<float,VectorXf> &seq,int n,int an,int thrnum);
    virtual void sock_func(void *send);
};


class Sockcom_s : public Sockcom{
  protected:
    char *ip;
    void init();
  public:
    Sockcom_s();
    Sockcom_s(int p,char *ipp);
    ~Sockcom_s();
    void sock_func(void *send);
    void sock_functest(int thnum);
};

class Sockcom_c : public Sockcom{
  protected:
    char *host;
    void init();
  public:
    Sockcom_c();
    Sockcom_c(int p,char *h);
    ~Sockcom_c();
    void sock_func(void *send);
    void sock_functest(int thnum);
};

#endif