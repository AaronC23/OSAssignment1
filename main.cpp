#undef __STRICT_ANSI__
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <functional>
using namespace std;

class Customer{
public:
		int custID;
		int arrivalTime;
		int priority;
		int age;
		int ticketQuota;
		int ticketsRemaining;
		int queue;
		int waitCount;
		int currentRuns;
		int queuePos; //Might not need this
		int runningTime; //Duration of buying tickets
		int waitingTime; //Duration of waiting
		int terminationTime; //Time when process bought all tickets
		bool finishedProcess;
	Customer(){
		runningTime = 0;
		terminationTime = -1;
		waitingTime = -1;
		finishedProcess = false
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
			return queueNum;
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

	int getTicketQuantum(int prio){
		int temp = (8-prio)*10;
		return temp/5;
	}

};

bool IDCheck(const Customer &c1, const Customer &c2){
	return c1.custID < c2.custID;
}

// stores customers and processes the queue
class CustomerQueue{
public:
	vector<Customer> leaverbuster_queue;
	vector<Customer> sub_queue_one;
	vector<Customer> sub_queue_two;
	vector<Customer> sub_queue_three;
	int finishedCustomers;

	CustomerQueue(){
		finishedCustomers = 500;
	}

	//Public functions

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
			leaverbuster_queue.push_back(leaverbuster_queue[0]);
			leaverbuster_queue.erase(leaverbuster_queue.begin());
		}
	}

	int getIndex(Customer cust){
		for(int i = 0; i<getQueue(cust.queue)->size(); i++){
			if(getQueue(cust.queue)->at(i).custID==cust.custID){
				return i;
			}
		}
	}

	//Here you are John
	void sortArrivals(){
		// sort 1-3 sorts the subqueues 1-3, 4 sorts the 2nd queue
		map<int, vector<Customer> > customerSort1;
		map<int, vector<Customer> > customerSort2;
		map<int, vector<Customer> > customerSort3;
		map<int, vector<Customer> > customerSort4;

		// put all those customers into a map so we can put them in the subqueue again
		for(int i=0;i<sub_queue_one.size();i++){
			customerSort1[sub_queue_one.at(i).arrivalTime].push_back(sub_queue_one.at(i));
		}
		for(int i=0;i<sub_queue_two.size();i++){
			customerSort2[sub_queue_two.at(i).arrivalTime].push_back(sub_queue_two.at(i));
		}
		for(int i=0;i<sub_queue_three.size();i++){
			customerSort3[sub_queue_three.at(i).arrivalTime].push_back(sub_queue_three.at(i));
		}
		for(int i=0;i<leaverbuster_queue.size();i++){
			customerSort4[leaverbuster_queue.at(i).arrivalTime].push_back(leaverbuster_queue.at(i));
		}

		// sort each vector in the maps
		map<int, vector<Customer> >::iterator it;
		for(it = customerSort1.begin(); it!= customerSort1.end(); it++){
			sort(it->second.begin(),it->second.end(),IDCheck);
		}
		for(it = customerSort2.begin(); it!= customerSort2.end(); it++){
			sort(it->second.begin(),it->second.end(),IDCheck);
		}
		for(it = customerSort3.begin(); it!= customerSort3.end(); it++){
			sort(it->second.begin(),it->second.end(),IDCheck);
		}
		for(it = customerSort4.begin(); it!= customerSort4.end(); it++){
			sort(it->second.begin(),it->second.end(),IDCheck);
		}

		// replace the values in the queues with the ones stored in the maps in the correct order
		sub_queue_one.clear();
		sub_queue_two.clear();
		sub_queue_three.clear();
		leaverbuster_queue.clear();
		for(it = customerSort1.begin(); it!= customerSort1.end(); it++){
			for(int i=0;i<it->second.size();i++){
				Customer copy = it->second.at(i);
				sub_queue_one.push_back(copy);
			}
		}
		for(it = customerSort2.begin(); it!= customerSort2.end(); it++){
			for(int i=0;i<it->second.size();i++){
				Customer copy = it->second.at(i);
				sub_queue_two.push_back(copy);
			}
		}
		for(it = customerSort3.begin(); it!= customerSort3.end(); it++){
			for(int i=0;i<it->second.size();i++){
				Customer copy = it->second.at(i);
				sub_queue_three.push_back(copy);
			}
		}
		for(it = customerSort4.begin(); it!= customerSort4.end(); it++){
			for(int i=0;i<it->second.size();i++){
				Customer copy = it->second.at(i);
				leaverbuster_queue.push_back(copy);
			}
		}
	}

	//debug function
	void checkQueues(){
		cout << "subq 1" << endl;
		for(int i=0;i<sub_queue_one.size();i++){
			cout << "customer id: " << sub_queue_one[i].custID << " priority: " << sub_queue_one[i].priority << " arrivalTime: " << sub_queue_one[i].arrivalTime << " runningTime: " << sub_queue_one[i].runningTime << endl;
		}
		cout << "subq 2" << endl;
		for(int i=0;i<sub_queue_two.size();i++){
			cout << "customer id: " << sub_queue_two[i].custID << " priority: " << sub_queue_two[i].priority << " arrivalTime: " << sub_queue_two[i].arrivalTime << " runningTime: " << sub_queue_two[i].runningTime << endl;
		}
		cout << "subq 3" << endl;
		for(int i=0;i<sub_queue_three.size();i++){
			cout << "customer id: " << sub_queue_three[i].custID << " priority: " << sub_queue_three[i].priority << " arrivalTime: " << sub_queue_three[i].arrivalTime << " runningTime: " << sub_queue_three[i].runningTime << endl;
		}
		cout << "queue 2" << endl;
		for(int i=0;i<leaverbuster_queue.size();i++){
			cout << "customer id: " << leaverbuster_queue[i].custID << " priority: " << leaverbuster_queue[i].priority << " arrivalTime: " << leaverbuster_queue[i].arrivalTime << " runningTime: " << leaverbuster_queue[i].runningTime << endl;
		}
	}

	//
	void addCustomer(Customer cust){
		if(cust.priority==1){
			cust.queue=1;
			sub_queue_one.push_back(cust);
		} else if (cust.priority==2){
			cust.queue=2;
			sub_queue_two.push_back(cust);
		} else if (cust.priority==3){
			cust.queue=3;
			sub_queue_three.push_back(cust);
		} else {
			leaverbuster_queue.push_back(cust);
		}
	}

	vector<Customer> * getQueue(int queueNum){
		if(queueNum==1){
			return &sub_queue_one;
		} else if (queueNum==2){
			return &sub_queue_two;
		} else if (queueNum==3){
			return &sub_queue_three;
		} else if (queueNum==0){
			return &leaverbuster_queue;
		}
	}

	//Swap a customer from queue 1 to queue 2 (IDK HOW TO DO THIS)
	void changeQueue(int queue1, int queue2, int custID){
		vector<Customer> * q1 = getQueue(queue1);
		vector<Customer> * q2 = getQueue(queue2);

		// iterate through vector to find customer with ID
		for(int i=0;i<q1->size();i++){
			if(q1->at(i).custID == custID){
				Customer move = q1->at(i);
				q1->erase(q1->begin()+i);
				q2->push_back(move);
				break;
			}
		}
	}

	void printQueue(int queueNum){
		vector<Customer> * temp = getQueue(queueNum);
		for (int i = 0; i < temp->size(); i++){
			cout << temp->at(i).custID << endl;
		}
	}

	Customer* getFrontCustomer(){
		if(sub_queue_one.size()==0){
			if(sub_queue_two.size()!=0){
				return &sub_queue_two[0];
			} else if(sub_queue_two.size()==0){
				if(sub_queue_three.size()!=0){
					return &sub_queue_three[0];
				} else if(sub_queue_three.size()==0){
					return &leaverbuster_queue[0];
				}
			}
		} else if (sub_queue_one.size()!=0){
			return &sub_queue_one[0];
		}
	}

	void checkForLongWait(){
		for (int i = 0; i < leaverbuster_queue.size(); i++){
			if(leaverbuster_queue[i].waitCount%8==0){
				leaverbuster_queue[i].increasePriority();
			}
		}
	}

	//Queue 1 functions

	//Queue 2 functions

};


	int StringToInt(string input){
		stringstream output(input);
		int returnValue = 0;
		output >> returnValue;
		output.str("");
		return returnValue;
	}

	int main(int argc, char *argv[]){
		// stores all the customers that will be added to the queue
		map<int, vector<Customer> > parsedCustomers;

		CustomerQueue customerQueue;
		Customer* currentCustomer;
		int totalCustomers = 0;
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
			customer.ticketsRemaining=StringToInt(str);
			// add customer to parsed customer map
			parsedCustomers[customer.arrivalTime].push_back(customer);
			// customerQueue.addCustomer(customer);
			totalCustomers++;
		}

		map<int, vector<Customer> >::iterator it = parsedCustomers.begin();
		// process the queue
		int tick = 0;
		// while(customerQueue.finishedCustomers < totalCustomers)
		while (tick < 500)
			if(it->first == tick) {
				// add each customer at this instance of time to a queue
				cout << "adding " << it->second.size() << " customers to the queue at time " << tick << endl;
				// sort it based on ID already, they wont be added to the same queue anyway
				sort(it->second.begin(),it->second.end(),IDCheck);
				for(int i=0;i<it->second.size();i++){
					customerQueue.addCustomer(it->second.at(i));
				}
				// customerQueue.sortArrivals();
				customerQueue.checkQueues();
				it++;
			}
			tick++;
		}

		return 0;
	}
