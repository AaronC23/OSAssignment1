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

	int getCustID(){
		return custID;
	}

	void process(){
		currentRuns++;
		if(currentRuns == 3){
			currentRuns = 0;
			priority++;
		}
	}
};

// stores customers and processes the queue
class CustomerQueue{
private:
	vector<Customer> queue;
	vector<Customer> executingQueue;
public:
	CustomerQueue(){
	}

	int queueSize(){
		return queue.size();
	}

	void addCustomer(Customer cust){
		queue.push_back(cust);
	}

	void moveToExecutingQueue(int custID){
		for(int i=0;i<queue.size();i++){
			if(queue[i].getCustID == custID){
				Customer moveCustomer = queue[i];
				queue.erase(queue.begin()+i);
				executingQueue.push_back(moveCustomer);
				break;
			}
		}
		return;
	}

	void processQueue(){

	}
};

void changeQueue(CustomerQueue* q1, CustomerQueue* q2, int custID){
	
	for(int i=0;i<q1->queueSize();i++){
		if(q1->at(i)->getCustID == custID){
			Customer moveCustomer = queue->at(i);
			q1->erase(q1->begin()+i);
			q2->push_back(moveCustomer);
			break;
		}
	}
	return;
}

int main(int argc, char *argv[]){
	Customer customer;
	CustomerQueue customerQueue1;
	CustomerQUeue customerQueue2;
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
		if(customer.priority<=3){
			customerQueue1.addCustomer(customer);
		} else if (customer.priority>3){
			customerQueue2.addCustomer(customer);
		}
	}
	return 0;
}
