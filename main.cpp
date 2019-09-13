#undef __STRICT_ANSI__ 
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

	//Called when processing a customer.
	//If we have had 3 currentRuns, decrease priority and check if we need to demote!
	void process(){
		currentRuns++;
		if(currentRuns == 3){
			currentRuns = 0;
			priority++;
		}
	}

	//Called when we are increasing a customers priority
	//We then call promote to see if they need to be promoted.
	void increasePriority(){
		if(priority==1){
			return;
		} else {
			priority--;
			promote(queue);
		}
	}

	//Check if a process needs to be demoted or not, given the queue they are in. 
	//Use of function is:
	//if(demote(cust.queue)!=cust.queue) 
	//{
	//	swapQueue(cust.queue,demote(cust.queue));
	//}
	int demote(int queueNum){
		if(queueNum==1 && priority>1){
			return 2;
		} else if (queueNum==2 && priority>2){
			return 3;
		} else if (queueNum==3 && priority>3){
			return 0;
		} else {
			//If we are in Queue2 it doesn't matter
			return -1;
		}
	}

	//Check if their priority is low enough to swap queues
	//This occurs when a customer gets interrupted/has waited 8 times.
	//Use of function is:
	//if(promote(cust.queue)!=cust.queue)
	//{
	//	swapQueue(cust.queue,promote(cust.queue));
	//}

	int promote(int queueNum){
		if(queueNum==0 && priority==3){
			return 3;
		} else if (queueNum==3 && priority==2){
			return 2;
		} else if (queueNum==2 && priority==1){
			return 1;
		} else {
			return 0;
		}
	}
};

// stores customers and processes the queue
class CustomerQueue{
public:
	vector<Customer> queue_two;
	vector<Customer> sub_queue_one;
	vector<Customer> sub_queue_two;
	vector<Customer> sub_queue_three;

	CustomerQueue(){ 

	}

	//Public functions

	//I'M NOT SURE IF WE NEED THIS FUNCTION, lemme know if we do
	// void moveToExecutingQueue(int custID){
	// 	for(int i=0;i<queue.size();i++){
	// 		if(queue[i].getCustID() == custID){
	// 			Customer moveCustomer = queue[i];
	// 			queue.erase(queue.begin()+i);
	// 			executingQueue.push_back(moveCustomer);
	// 			break;
	// 		}
	// 	}
	// 	return;
	// }

	void processQueue(){

	}

	//Send customer to end of specific queue.
	//Not sure if this handles interrupt cases yet..
	void sendToBack(Customer cust, int subQueue=0){
		if(subQueue==1){
			sub_queue_one.push_back(sub_queue_one[0]);
			sub_queue_one.erase(sub_queue_one.begin());
		} else if (subQueue==2){
			sub_queue_two.push_back(sub_queue_two[0]);
			sub_queue_two.erase(sub_queue_two.begin());
		} else if (subQueue==3){
			sub_queue_three.push_back(sub_queue_three[0]);
			sub_queue_three.erase(sub_queue_three.begin());
		} else {
			queue_two.push_back(queue_two[0]);
			queue_two.erase(queue_two.begin());
		}
	}

	int getIndex(Customer cust){
		for(int i = 0; i<getQueue(cust.queue).size(); i++){
			if(getQueue(cust.queue).at(i).custID==cust.custID){
				return i;
			}
		}
	}

	//Here you are John 
	void sortArrivals(){

	}


	//
	void addCustomer(Customer cust){
		if(cust.priority==1){
			sub_queue_one.push_back(cust);
		} else if (cust.priority==2){
			sub_queue_two.push_back(cust);
		} else if (cust.priority==3){
			sub_queue_three.push_back(cust);
		} else {
			queue_two.push_back(cust);
		}
	}

	vector<Customer> getQueue(int queueNum){
		if(queueNum==1){
			return sub_queue_one;
		} else if (queueNum==2){
			return sub_queue_two;
		} else if (queueNum==3){
			return sub_queue_three;
		} else if (queueNum==0){
			return queue_two;
		}
	}

	//Swap a customer from queue 1 to queue 2 (IDK HOW TO DO THIS)
	void changeQueue(int queue1, int queue2, int custID){

	}

	void printQueue(int queueNum){
		vector<Customer> temp = getQueue(queueNum);
		for (int i = 0; i < temp.size(); i++){
			cout << temp[i].custID << endl;
		}
	}

	//Queue 1 functions

	//Queue 2 functions

};


	int StringToInt(string input){
		stringstream output(input);
		int returnValue = 0;
		output >> returnValue;
		cout << "Return Value is: " << returnValue << endl;
		output.str("");
		return returnValue;
	}

	int main(int argc, char *argv[]){
		CustomerQueue customerQueue;
		int counter=-1;
		string str;
		string token;
		string id;
		string delimiter = " ";
		size_t pos = 0;
		ifstream file(argv[1]);
		while (getline(file, str)) {
			Customer customer;
			counter=-1;
			id="";
			while((pos = str.find(delimiter)) != string::npos){
				counter++;
				token = str.substr(0, pos);
				if(counter==0){
					for (unsigned int i = 1; i < token.length(); i++){
						id+=token.at(i);
					}
					customer.custID=StringToInt(id);
				} else if (counter==1){
					customer.arrivalTime=StringToInt(token);
				} else if (counter==2){
					customer.priority=StringToInt(token);
				} else if (counter==3){
					customer.age=StringToInt(token);
				}
				str.erase(0, pos + delimiter.length());
			}
			customer.tickets=StringToInt(str);
			customerQueue.addCustomer(customer);
		}
		customerQueue.getIndex()



		//At the end of each "tick" we need to call several functions for each customer.
		//

		return 0;
	}
