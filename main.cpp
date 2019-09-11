#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
using namespace std;

class Customer{  
public:
		int custID;
		int arrivalTime;
		int priority;
		int age;
		int tickets;
		int queue;
		int interruptCount;
		int currentRuns;
		int queuePos; //Might not need this
		int runningTime; //Duration of buying tickets
		int waitingTime; //Duration of waiting
		int terminationTime; //Time when process bought all tickets

	Customer(){
		
	}


};

int main(int argc, char *argv[]){
	Customer customer;
	int counter=-1;
	string str;
	string token;
	string id;
	string delimiter = " ";
	size_t pos = 0;
	ifstream file(argv[1]);
	while (getline(file, str)) {
		counter=-1;
		while((pos = str.find(delimiter)) != string::npos){
			counter++;
			token = str.substr(0, pos);
			if(counter==0){
				for (unsigned int i = 1; i < token.length(); i++){
					id+=token.at(i);
				}
				customer.custID=stoi(id);
			} else if (counter==1){
				customer.arrivalTime=stoi(token);
			} else if (counter==2){
				customer.priority=stoi(token);
			} else if (counter==3){
				customer.age=stoi(token);
			}
			str.erase(0, pos + delimiter.length());
		}
		customer.tickets=stoi(str);
	}
	return 0;
}
