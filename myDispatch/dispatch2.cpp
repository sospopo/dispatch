#include <iostream>
#include <string>
#include <queue>
#include <ctime>
#include <cstring>
#include <stdlib.h>
#include <time.h>
using namespace std;

const int NAMELENGTH = 9;
char* STATE[5]={"Ready1","Ready2","waitting","Running","Finishing"};
int ROUND;
int htime = 100,ltime = 500;
int interrupt = 0;//中断标志0为没有中断发生；

struct PCB {
	char name[NAMELENGTH];
	int prio;
	int round;
	int cputime;
	int needtime;
	char state[10];
};


struct PCBCmp{
	bool operator()(const PCB &na, const PCB &nb)  
    {  
        if (na.prio != nb.prio)  
            return na.prio <= nb.prio;  
		else
			return na.prio > nb.prio;

    }  
}; 

queue<PCB*> lowQ,wQ,rQ,fQ;
priority_queue<PCB,vector<PCB>,PCBCmp> highQ;

PCB* getPCB(char* name,int needtime,int prio) {
	PCB* ptr = (PCB*)malloc(sizeof(PCB));
	strcpy(ptr->name, name);
	ptr->needtime = needtime;
	ptr->round = ROUND;//进程轮转时间片亦可单独输入
	ptr->prio = prio;
	ptr->cputime = 0;
	strcpy(ptr->state, STATE[0]);
	return ptr;
}

void randomGet(int N) {
	for (int i = 0; i < N; i++) {
		int nameLength = rand()%(NAMELENGTH-1)+1;
		char name[NAMELENGTH] = "";
		for (int j = 0; j < nameLength; j++) {
			name[j] = rand()%26+'a';
		}
		name[nameLength] = '\0';
		int needtime = rand()%100+1;
		int prio = 0;
		highQ.push(*getPCB(name,needtime,prio));
	}
}

void insertPCB() {
	cout<<"请输入进程名："<<endl;
	char name[NAMELENGTH];
	cin>>name;
	cout<<"请输入完成进程所需时间："<<endl;
	int needtime;
	cin>>needtime;
	cout<<"请输入进程的优先级（1：高优先级，0：低优先级）："<<endl;
	int prio;
	cin>>prio;
	highQ.push(*getPCB(name,needtime,prio));
}

void listen(){
	int i=0;
	i=rand()%10;
	if(i>7){
		
		interrupt = 0;
	}else{
		printf("\n----------------------------------------------\n");
		printf("io进程运行！\n");
		
		interrupt = 1;
	}
}//有30%的机率发生i/o请求；


void print(PCB* ptr) {
	cout<<ptr->name<<"\t";
	cout<<ptr->prio<<"\t";
	cout<<ptr->round<<"\t";
	cout<<ptr->cputime<<"\t";
	cout<<ptr->needtime<<"\t";
	cout<<ptr->state<<"\n";
}


void hRun(){
	PCB* ptr;
	int countTime;
	int runtime;
	cout<<"\n优先级搞的队列开始运行！\n";
	cout<<"\n进程名\t优先级\t进程轮转时间片\t进程占用的CPU时间\t进程到完成还要的时间\t进程状态\n";
	while(!highQ.empty()){
		PCB* lPtr = (PCB*)malloc(sizeof(PCB));
		countTime = htime;
		ptr = &highQ.top();
		rQ.push(ptr);
		runtime = ptr->round > htime ? htime : ptr->round;

		while(countTime > 0){
			runtime = runtime > ptr->needtime ? ptr->needtime : runtime;//计算运行时间
			runtime = runtime > countTime ? countTime : runtime;
			countTime -= runtime;
			strcpy(ptr->state, STATE[3]);
			print(ptr);
			ptr->needtime -= runtime;
			ptr->cputime += runtime;

			listen();

			if(interrupt == 0){

				if(ptr->needtime <= 0){
					strcpy(ptr->state, STATE[4]);
					print(ptr);
					fQ.push(ptr);
					rQ.pop();
					highQ.pop();
					continue;
				}//如果进程执行完成，加入完成队列

			}else{
				strcpy(ptr->state, STATE[2]);
				print(ptr);
				wQ.push(ptr);
				rQ.pop();
				printf("io进程运行完毕！\n");
				printf("----------------------------------------------\n");
				ptr = wQ.front();
				strcpy(ptr->state, STATE[3]);
				print(ptr);
				rQ.push(ptr);
				wQ.pop();//执行中断，回复现场
				interrupt = 0;
			}
		}
			ptr->prio = 0;
			strcpy(ptr->state, STATE[1]);
			print(ptr);
			
			strcpy(lPtr->name, ptr->name);
			lPtr->cputime = ptr->cputime;
			lPtr->needtime = ptr->needtime;
			lPtr->prio = ptr->prio;
			lPtr->round = ptr->round;
			strcpy(lPtr->state, ptr->state);
			lowQ.push(lPtr);
			
			rQ.pop();
			highQ.pop();
	}

}

void lRun(){
	int countTime;
	int runtime;
	int finish = 0;
	cout<<"\n优先级低的队列开始运行！\n";
	cout<<"\n进程名\t优先级\t进程轮转时间片\t进程占用的CPU时间\t进程到完成还要的时间\t进程状态\n";
	while(!lowQ.empty()){
		PCB* ptr = lowQ.front();
		countTime = ltime > ptr->needtime ? ptr->needtime : ltime;
		lowQ.pop();
		rQ.push(ptr);
		runtime = ptr->round > ltime ? ltime : ptr->round;
		while(countTime > 0){
			runtime = runtime > ptr->needtime ? ptr->needtime : runtime;//计算运行时间
			runtime = runtime > countTime ? countTime : runtime;
			countTime -= runtime;
			strcpy(ptr->state, STATE[3]);
			print(ptr);
			ptr->needtime -= runtime;
			ptr->cputime += runtime;

			listen();

			if(interrupt != 0){
				strcpy(ptr->state, STATE[2]);
				print(ptr);
				wQ.push(ptr);
				rQ.pop();
				printf("io进程运行完毕！\n");
				printf("----------------------------------------------\n\n");
				ptr = wQ.front();
				strcpy(ptr->state, STATE[3]);
				print(ptr);
				rQ.push(ptr);
				wQ.pop();//执行中断，回复现场
				interrupt = 0;
			}

			if(ptr->needtime <= 0){
					strcpy(ptr->state, STATE[4]);
					print(ptr);
					fQ.push(ptr);
					rQ.pop();
					finish = 1;
					break;
				}//如果进程执行完成，加入完成队列

		}
		
		if(lowQ.size() == 0&&finish == 0){
				strcpy(ptr->state, STATE[3]);
				print(ptr);
				lowQ.push(ptr);//到最后一个运行还未结束则继续计算
			}else{
				if(finish != 0){
					finish = 0;
					continue;
				}else{
					ptr->prio = 0;
					strcpy(ptr->state, STATE[1]);
					print(ptr);
					lowQ.push(ptr);
					rQ.pop();
			}
		}
	}


}

void fPrio(){
	cout<<"请输入进程轮转时间片（固定）：";
	cin>>ROUND;
	int N;
	cout<<"请输入进程数："<<endl;
	cin>>N;
	for (int i = 0; i < N; i++) {
			cout<<i+1<<"："<<endl;
			insertPCB();
			cout<<endl;
	}
	hRun();
	lRun();
	system("pause");
}

int main(){
	srand((unsigned)time(NULL));
	int i,j=0;
	while(1){
		printf("----------------------------------------------\n");
		printf("欢迎使用进程调度算法！请选择您需要使用的算法！（0：时间片轮转调度算法，1.退出）\n");
		scanf("%d",&i);
		printf("----------------------------------------------\n");
		switch(i){
			case 0 : 
				fPrio();
				break;			
			case 1:
				j = 1;
				printf("----------------------------------------------\n");
				printf("程序已退出！欢迎再次使用！\n");
				printf("----------------------------------------------\n");
				break;
			default:break;
		}
		if(j == 1)
			break;
	}

	return 0;
}


