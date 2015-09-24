#include<iostream>
#include<pthread.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<ctime>
/*
        This is a simple demo of mutex.
        The situation is a bank account spending money at the same time in many online shopping websites, and the money number should be thread safe.
        Author: ChiHai@13307130364
	Compile it with: g++ mutex.cpp -lpthread
*/
using namespace std;
class account{
private:
        int credit;
        pthread_mutex_t mutex;
public:
        account(int init_amount){
                credit=init_amount;
		pthread_mutex_init(&mutex, NULL);
        }
        void withdraw(int amount){
                pthread_mutex_lock(&mutex);
                if(credit>=amount&&amount>=0){
                        credit-=amount;
			cout<<"before: "<<credit+amount<<", withdraw "<<amount<<", "<<credit<<" left."<<endl;
                        pthread_mutex_unlock(&mutex);
                        return;
                }
                pthread_mutex_unlock(&mutex);
		cout<<"No enough money"<<endl;
                return;
        }
	static void *withdraw_helper(void *bank_account){
		srand((unsigned)time(0));
		for(int i=0;i<10;i++)
			((account *)bank_account)->withdraw(rand()%200);
	}
};
int main(){
	account bank_account(100000);
	pthread_t tids[10];
	void *retVal;
	for(int p=0;p<10;p++){
		pthread_create(&tids[p], NULL, &account::withdraw_helper, &bank_account);
		pthread_join(tids[p], &retVal);
	}
	return 0;
}
