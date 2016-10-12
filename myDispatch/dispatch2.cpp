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
int interrupt = 0;//�жϱ�־0Ϊû���жϷ�����

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
	ptr->round = ROUND;//������תʱ��Ƭ��ɵ�������
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
	cout<<"�������������"<<endl;
	char name[NAMELENGTH];
	cin>>name;
	cout<<"��������ɽ�������ʱ�䣺"<<endl;
	int needtime;
	cin>>needtime;
	cout<<"��������̵����ȼ���1�������ȼ���0�������ȼ�����"<<endl;
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
		printf("io�������У�\n");
		
		interrupt = 1;
	}
}//��30%�Ļ��ʷ���i/o����


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
	cout<<"\n���ȼ���Ķ��п�ʼ���У�\n";
	cout<<"\n������\t���ȼ�\t������תʱ��Ƭ\t����ռ�õ�CPUʱ��\t���̵���ɻ�Ҫ��ʱ��\t����״̬\n";
	while(!highQ.empty()){
		PCB* lPtr = (PCB*)malloc(sizeof(PCB));
		countTime = htime;
		ptr = &highQ.top();
		rQ.push(ptr);
		runtime = ptr->round > htime ? htime : ptr->round;

		while(countTime > 0){
			runtime = runtime > ptr->needtime ? ptr->needtime : runtime;//��������ʱ��
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
				}//�������ִ����ɣ�������ɶ���

			}else{
				strcpy(ptr->state, STATE[2]);
				print(ptr);
				wQ.push(ptr);
				rQ.pop();
				printf("io����������ϣ�\n");
				printf("----------------------------------------------\n");
				ptr = wQ.front();
				strcpy(ptr->state, STATE[3]);
				print(ptr);
				rQ.push(ptr);
				wQ.pop();//ִ���жϣ��ظ��ֳ�
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
	cout<<"\n���ȼ��͵Ķ��п�ʼ���У�\n";
	cout<<"\n������\t���ȼ�\t������תʱ��Ƭ\t����ռ�õ�CPUʱ��\t���̵���ɻ�Ҫ��ʱ��\t����״̬\n";
	while(!lowQ.empty()){
		PCB* ptr = lowQ.front();
		countTime = ltime > ptr->needtime ? ptr->needtime : ltime;
		lowQ.pop();
		rQ.push(ptr);
		runtime = ptr->round > ltime ? ltime : ptr->round;
		while(countTime > 0){
			runtime = runtime > ptr->needtime ? ptr->needtime : runtime;//��������ʱ��
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
				printf("io����������ϣ�\n");
				printf("----------------------------------------------\n\n");
				ptr = wQ.front();
				strcpy(ptr->state, STATE[3]);
				print(ptr);
				rQ.push(ptr);
				wQ.pop();//ִ���жϣ��ظ��ֳ�
				interrupt = 0;
			}

			if(ptr->needtime <= 0){
					strcpy(ptr->state, STATE[4]);
					print(ptr);
					fQ.push(ptr);
					rQ.pop();
					finish = 1;
					break;
				}//�������ִ����ɣ�������ɶ���

		}
		
		if(lowQ.size() == 0&&finish == 0){
				strcpy(ptr->state, STATE[3]);
				print(ptr);
				lowQ.push(ptr);//�����һ�����л�δ�������������
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
	cout<<"�����������תʱ��Ƭ���̶�����";
	cin>>ROUND;
	int N;
	cout<<"�������������"<<endl;
	cin>>N;
	for (int i = 0; i < N; i++) {
			cout<<i+1<<"��"<<endl;
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
		printf("��ӭʹ�ý��̵����㷨����ѡ������Ҫʹ�õ��㷨����0��ʱ��Ƭ��ת�����㷨��1.�˳���\n");
		scanf("%d",&i);
		printf("----------------------------------------------\n");
		switch(i){
			case 0 : 
				fPrio();
				break;			
			case 1:
				j = 1;
				printf("----------------------------------------------\n");
				printf("�������˳�����ӭ�ٴ�ʹ�ã�\n");
				printf("----------------------------------------------\n");
				break;
			default:break;
		}
		if(j == 1)
			break;
	}

	return 0;
}


