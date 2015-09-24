#include<iostream>
#include<pthread.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<ctime>
/*
        This is a simple demo of condition variables.
        The situation is a bank account spending money at the same time in many online shopping websites, and the money number should be thread safe.When your money is below 2000, all threads will stop and wait for your choice whether to continue shopping or not.
        Author: ChiHai@13307130364
	Compile it with: g++ condition_var.cpp -lpthread
*/
using namespace std;
class account{
private:
        int credit;
        pthread_mutex_t money_lock;
	pthread_mutex_t msg_sent_lock;
	pthread_mutex_t no_money_lock;
	pthread_cond_t cond;
	bool no_money;
public:
	int can_below;
	bool msg_sent;
	bool send_msg(){
		//send message only once
		pthread_mutex_lock(&msg_sent_lock);
		if(!msg_sent){
			cout<<"Money is below 2000."<<endl;
			cout<<"Continue shopping?Type y or other:"<<endl;
			msg_sent=true;
			pthread_mutex_unlock(&msg_sent_lock);
			string x;
			cin>>x;
			sleep(1);//wait for all other threads
			if(x=="y"){
				cout<<"Allow money below 2000."<<endl;
				can_below=1;
				}
			else{
				cout<<"Don't allow money below 2000."<<endl;
				can_below=-1;
			}
			pthread_cond_signal(&cond);
			return true;
		}
		else
			pthread_mutex_unlock(&msg_sent_lock);
		return false;
	}
        account(int init_amount){
		msg_sent=false;
		no_money=false;
                credit=init_amount;
		pthread_mutex_init(&money_lock, NULL);
		pthread_mutex_init(&msg_sent_lock, NULL);
		pthread_mutex_init(&no_money_lock, NULL);
		pthread_cond_init(&cond,NULL);
        	can_below=0;
	}
        void withdraw(int amount){
                pthread_mutex_lock(&money_lock);
                if(credit>=amount&&amount>=0){
			if(can_below==0&&credit-amount<2000){
				if(!send_msg())
					pthread_cond_wait(&cond, &money_lock);
			}
			if(can_below==-1){
				pthread_mutex_unlock(&money_lock);
				return;
			}
                        credit-=amount;
			cout<<"before: "<<credit+amount<<", withdraw "<<amount<<", "<<credit<<" left."<<endl;
                        pthread_mutex_unlock(&money_lock);
                        return;
                }
                pthread_mutex_unlock(&money_lock);
		pthread_mutex_lock(&no_money_lock);
		if(!no_money){
			cout<<"No enough money to buy this one."<<endl;
			no_money=true;
		}
		pthread_mutex_unlock(&no_money_lock);
                return;
        }
	static void *withdraw_helper(void *bank_account){
		srand((unsigned)time(0));
		for(int i=0;i<10;i++){
			((account *)bank_account)->withdraw(rand()%200);
			//sleep(1);//Sleep 1 second
		}
	}
};
int main(){
	account bank_account(5000);
	pthread_t tids[10];
	void *retVal;
	for(int p=0;p<10;p++){
		pthread_create(&tids[p], NULL, &account::withdraw_helper, &bank_account);
		pthread_join(tids[p], &retVal);
	}
	return 0;
}
