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
		int ticketsProcessed;
		int ticketsRemaining;
		int queue;
		int waitCount;
		int currentRuns;
		int queuePos; //Might not need this
		int runningTime; //Duration of buying tickets
		int waitingTime; //Duration of waiting
		int terminationTime; //Time when process bought all tickets
		int firstProcessTime;
		bool finishedProcess;
	Customer(){
		runningTime = 0;
		terminationTime = -1;
		waitingTime = -1;
		firstProcessTime = -1;
		ticketsProcessed = 0;
		finishedProcess = false;
	}

	int getCustID(){
		return custID;
	}

	//Called when processing a customer.
	//If we have had 3 currentRuns, decrease priority and check if we need to demote!
	void process(int currentTime){
		if(firstProcessTime == -1){
			firstProcessTime = currentTime;
		}
		currentRuns++;
		if(currentRuns == 3){
			currentRuns = 0;
			priority++;
		}
	}

	int getTicketQuantum(int prio){
		int temp = ((8-prio))*10;
		return temp/5;
	}

	void newRun(){
		ticketQuota=getTicketQuantum(priority);
	}

	//Called when we are increasing a customers priority
	//We then call promote to see if they need to be promoted.
	int increasePriority(){
		if(priority==1){
			return queue;
		} else {
			priority--;
			return promote(queue);
		}
	}

	//Check if a process needs to be demoted or not, given the queue they are in.
	//Use of function is:
	//if(demote(cust.queue)!=cust.queue)
	//{
	//	swapQueue(cust.queue,demote(cust.queue));
	//	cust.queue=demote(cust.queue);
	//}
	int checkForDemotion(int queueNum){
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
	//	cust.queue=promote(cust.queue);
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
	vector<Customer> promoted_customers;
	Customer demoted_customer;
	Customer quantum_customer;
	int finishedCustomers;

	CustomerQueue(){
		finishedCustomers = 0;
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

	// check for new arrivals, and promotions and demotions to the queues, returns true if a customer was added to queue one this tick
	bool checkForArrivals(vector<Customer> arrivingCustomers){
		bool res = false;
		// add new arrivals that go to queue one
		for(int i=0;i<arrivingCustomers.size();i++){
			if(arrivingCustomers[i].priority <= 3) {
				res = true;
				vector<Customer> * queueToAdd = getQueue(arrivingCustomers[i].priority);
				queueToAdd->push_back(arrivingCustomers[i]);
			}
		}
		// add completed time weight customer
		if(quantum_customer.custID != -1){
			vector<Customer> * queueToAdd = getQueue(quantum_customer.priority);
			queueToAdd->push_back(quantum_customer);
			quantum_customer.custID = -1;
		}
		// add promoted customers to sub_queue_three (queue one)
		for(int i=0;i<promoted_customers.size();i++){
			res = true;
			sub_queue_three.push_back(promoted_customers[i]);
		}
		// add new arrivals that go to queue two
		for(int i=0;i<arrivingCustomers.size();i++){
			if(arrivingCustomers[i].priority > 3){
				leaverbuster_queue.push_back(arrivingCustomers[i]);
			}
		}
		promoted_customers.clear();
		return res;
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
				move.queue=queue2;
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
				sub_queue_two[0].newRun();
				return &sub_queue_two[0];
			} else if(sub_queue_two.size()==0){
				if(sub_queue_three.size()!=0){
					sub_queue_three[0].newRun();
					return &sub_queue_three[0];
				} else if(sub_queue_three.size()==0){
					leaverbuster_queue[0].newRun();
					return &leaverbuster_queue[0];
				}
			}
		} else if (sub_queue_one.size()!=0){
			sub_queue_one[0].newRun();
			return &sub_queue_one[0];
		}
	}

	//FIX THIS FUNCITON ASAP
	void checkForPromotion(){
		for (int i = 0; i < leaverbuster_queue.size(); i++){

		}
	}

	void removeFrontCustomer(){
		finishedCustomers++;
		vector<Customer> * queueToRemove = getQueue(getFrontCustomer()->queue);
		queueToRemove->erase(queueToRemove->begin());
	}

	//Updates each customers variables and checks for promotions/demotions
	void updateCustomers(int processID){
		Customer* customerUpdate;
		// loop through each queue
		for(int i=0;i<sub_queue_one.size();i++){
			sub_queue_one[i].waitCount++;
			sub_queue_one[i].waitingTime++;
		}
		for(int i=0;i<sub_queue_two.size();i++){
			sub_queue_two[i].waitCount++;
			sub_queue_two[i].waitingTime++;
		}
		for(int i=0;i<sub_queue_three.size();i++){
			sub_queue_three[i].waitCount++;
			sub_queue_three[i].waitingTime++;
		}
		for(int i=0;i<leaverbuster_queue.size();i++){
			leaverbuster_queue[i].waitCount++;
			leaverbuster_queue[i].waitingTime++;
			if(leaverbuster_queue[i].waitCount%8==0){
				int temp=leaverbuster_queue[i].increasePriority();
				if(temp!=leaverbuster_queue[i].queue){
					// add to the promotion vector and delete from current one
					promoted_customers.push_back(leaverbuster_queue[i]);
					leaverbuster_queue.erase(leaverbuster_queue.begin()+i);
					i--;
					// changeQueue(leaverbuster_queue[i].queue,temp,leaverbuster_queue[i].custID);
				}
			}
		}
	}

	void deleteFromQueue(int custID, vector<Customer> * queueList){
	    for(int i=0;i<queueList->size();i++){
	        if(queueList->at(i).custID == custID){
	            queueList->erase(queueList->begin()+i);
	            break;
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
		vector<Customer> completedCustomers;
		CustomerQueue customerQueue;
		Customer* getCustomer = new Customer;
		getCustomer->custID=999;
		Customer* currentCustomer=getCustomer;
		int totalCustomers = 0;
		int counter=-1;
		string str;
		string token;
		string id;
		string delimiter = " ";
		size_t pos = 0;
		bool newCustomer = true;
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
					if(customer.priority > 3) {
						customer.queue = 0;
					} else {
						customer.queue = customer.priority;
					}
				} else if (counter==3){
					customer.age=StringToInt(token);
				}
				str.erase(0, pos + delimiter.length());
			}
			customer.ticketsRemaining=StringToInt(str);
			// add customer to parsed customer map
			parsedCustomers[customer.arrivalTime].push_back(customer);
			cout << "adding customer " << customer.custID << " to map with time " << customer.arrivalTime << endl;
			// customerQueue.addCustomer(customer);
			totalCustomers++;
		}

		map<int, vector<Customer> >::iterator it = parsedCustomers.begin();
		// process the queue
		int tick = 0;
		int ticket_processed = 0;
		while(completedCustomers.size() < totalCustomers){
			ticket_processed++;
			// cout << "Tick: " << tick << endl;
			vector<Customer> arrivingCustomers;
			// sort the list of arriving customers
			if(it->first == tick && it != parsedCustomers.end()) {
				// add each customer at this instance of time to a queue
				cout << "adding " << it->second.size() << " customers to the queue at time " << tick << endl;
				// sort it based on ID already, they wont be added to the same queue anyway
				sort(it->second.begin(),it->second.end(),IDCheck);
				arrivingCustomers = it->second;
				it++;
			}

			bool arrivedInQueueOne = customerQueue.checkForArrivals(arrivingCustomers);

			if(newCustomer){
				currentCustomer = customerQueue.getFrontCustomer();
				cout << "New customer has ID: " << currentCustomer->custID << endl;
				if(currentCustomer->queue==0){
					currentCustomer->newRun();currentCustomer->ticketQuota=20;
				}
				currentCustomer->process(tick);
				newCustomer=false;
			}

			if(currentCustomer->queue!=0){
				if (currentCustomer->ticketsRemaining==0){
					completedCustomers.push_back(*currentCustomer);
					// customerQueue.quantum_customer=*currentCustomer;
					customerQueue.deleteFromQueue(currentCustomer->custID,customerQueue.getQueue(currentCustomer->queue));
					newCustomer=true;
				} else if(currentCustomer->ticketQuota!=0 && ticket_processed%5==0){
					currentCustomer->ticketQuota--;
					currentCustomer->ticketsRemaining--;
					cout << "current customer ticketQuota is: " << currentCustomer->ticketQuota << endl;
					cout << "Current customer tickets remaining is: " << currentCustomer->ticketsRemaining << endl;
				} else if (currentCustomer->ticketQuota==0){
					currentCustomer->waitCount=0;
					if((currentCustomer->checkForDemotion(currentCustomer->queue)!=currentCustomer->queue)){
						// customerQueue.quantum_customer=*currentCustomer;
						customerQueue.deleteFromQueue(currentCustomer->custID,customerQueue.getQueue(currentCustomer->queue));
						newCustomer=true;
					}
					newCustomer=true;
				}
			} else if (currentCustomer->queue==0){
				if(!arrivedInQueueOne && currentCustomer->ticketQuota!=0 && currentCustomer->ticketsProcessed%5==0){
					currentCustomer->ticketsRemaining--;
					currentCustomer->ticketQuota--;
				} else if (currentCustomer->ticketsRemaining==0){
					cout << "Pushing back" << endl;
					completedCustomers.push_back(*currentCustomer);
					newCustomer=true;
				} else if (currentCustomer->ticketQuota==0){
					currentCustomer->waitCount=0;
				} else if (arrivedInQueueOne){
					currentCustomer->waitCount++;
 					newCustomer=true;
				}
			}

			customerQueue.updateCustomers(currentCustomer->custID);
			tick++;
		}

		cout << "completed customers size: " << completedCustomers.size() << endl;

		sort(completedCustomers.begin(),completedCustomers.end(),IDCheck);

		//output results
		cout << "name arrival end ready running waiting" << endl;
		for(int i=0;i<totalCustomers;i++){
			cout << "a" << completedCustomers[i].custID << " " << completedCustomers[i].arrivalTime << " " << completedCustomers[i].terminationTime << " " << completedCustomers[i].firstProcessTime << " " << completedCustomers[i].runningTime << " " << completedCustomers[i].waitingTime << endl;
		}

		return 0;
	}
