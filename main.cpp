#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
using namespace std;

class Customer{  
private:
		int custID;
		int arrivalTime;
		int priority;
		int age;
		int tickets;
		int queue;
		int interruptCount;
		int queuePos; //Might not need this
		int runningTime; //Duration of buying tickets
		int waitingTime; //Duration of waiting
		int terminationTime; //Time when process bought all tickets
		int currentRuns;
public:
	Customer(){
			
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

	}

	void processQueue(){

	}
};

void changeQueue(CustomerQueue* q1, CustomerQueue* q2, int custID){
	for(int i=0;i<q1.queueSize();i++){
		
	}
}

int main(int argc, char *argv[]){
	Customer customers;
	if(!infile) {
    	cout << "Cannot open input file.\n";
   		return 1;
  	}
	customers.getCustomers(argv[1]);

	CustomerQueue queueOne;
	CustomerQueue queueTwo;

	return 0;
}