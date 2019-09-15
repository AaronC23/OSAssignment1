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
#include <cmath>
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
		int totalProcessTime;
		int currentWaitingTime;
		bool finishedProcess;
	Customer(){
		runningTime = 0;
		terminationTime = -1;
		waitingTime = 0;
		firstProcessTime = -1;
		ticketsProcessed = 0;
		finishedProcess = false;
		totalProcessTime = 0;
		currentWaitingTime = 0;
		ticketQuota = 0;
	}

	int getCustID(){
		return custID;
	}

	void fixPriority(){
		priority -= floor(waitCount/8);
		waitCount = 0;
	}

	//Called when processing a customer.
	//If we have had 3 currentRuns, decrease priority and check if we need to demote!
	void process(int currentTime){
		if(firstProcessTime == -1){
			firstProcessTime = currentTime;
		}
		totalProcessTime++;
		runningTime++;
		if(totalProcessTime%5==0){
			cout << "decreasing tickets remaining and ticket quota for customer " << custID << endl;
			ticketsRemaining--;
			ticketQuota--;
		}
	}

	void finishRun(){
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
	bool checkForDemotion(){
		bool res = false;
		int currentQueue = queue;
		int afterQueue = priority;
		if(priority > 3) afterQueue = 0;
		if(currentQueue != afterQueue) res = true;
		queue = afterQueue;
		return res;
	}

	void fixQueue(){
		queue = priority;
		if(priority > 3) queue = 0;
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

bool TerminationTimeCheck(const Customer &c1, const Customer &c2){
	return c1.terminationTime < c2.terminationTime;
}

// stores customers and processes the queue
class CustomerQueue{
public:
	vector<Customer> leaverbuster_queue;
	vector<Customer> sub_queue_one;
	vector<Customer> sub_queue_two;
	vector<Customer> sub_queue_three;
	vector<Customer> promoted_customers;
	Customer quantum_customer;
	int totalRuns;

	CustomerQueue(){
		totalRuns = 0;
		leaverbuster_queue.clear();
		promoted_customers.clear();
		sub_queue_one.clear();
		sub_queue_two.clear();
		sub_queue_three.clear();
		quantum_customer.custID = -1;
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
				vector<Customer> * queueToAdd = getQueue(arrivingCustomers[i].queue);
				cout << "1 adding customer " << arrivingCustomers[i].custID << " to queue " << arrivingCustomers[i].queue << endl;
				queueToAdd->push_back(arrivingCustomers[i]);
			}
		}
		// add completed time weight customer
		if(quantum_customer.custID != -1){
			cout << "2 adding customer " << quantum_customer.custID << " to queue " << quantum_customer.queue << endl;
			vector<Customer> * queueToAdd = getQueue(quantum_customer.queue);
			queueToAdd->push_back(quantum_customer);
			quantum_customer.custID = -1;
		}
		// add promoted customers to sub_queue_three (queue one)
		for(int i=0;i<promoted_customers.size();i++){
			res = true;
			cout << "3 adding customer " << promoted_customers[i].custID << " to queue " << promoted_customers[i].queue << endl;
			sub_queue_three.push_back(promoted_customers[i]);
		}
		// add new arrivals that go to queue two
		for(int i=0;i<arrivingCustomers.size();i++){
			if(arrivingCustomers[i].priority > 3){
				cout << "4 adding customer " << arrivingCustomers[i].custID << " to queue " << arrivingCustomers[i].queue << endl;
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

	//FIX THIS FUNCITON ASAP
	void checkForPromotion(){
		for (int i = 0; i < leaverbuster_queue.size(); i++){

		}
	}

	void removeFrontCustomer(){
		vector<Customer> * queueToRemove = getQueue(getFrontCustomer()->queue);
		queueToRemove->erase(queueToRemove->begin());
	}

	//Updates each customers variables
	void updateCustomers(int processID){
		// loop through each queue
		for(int i=0;i<sub_queue_one.size();i++){
			sub_queue_one[i].waitingTime++;
			if(sub_queue_one[i].custID == processID) {
				sub_queue_one[i].waitingTime--;
			}
		}
		for(int i=0;i<sub_queue_two.size();i++){
			sub_queue_two[i].waitingTime++;
			if(sub_queue_two[i].custID == processID) {
				sub_queue_two[i].waitingTime--;
			}
		}
		for(int i=0;i<sub_queue_three.size();i++){
			sub_queue_three[i].waitingTime++;
			if(sub_queue_three[i].custID == processID) {
				sub_queue_three[i].waitingTime--;
			}
		}
		for(int i=0;i<leaverbuster_queue.size();i++){
			leaverbuster_queue[i].waitingTime++;
			if(leaverbuster_queue[i].custID == processID) {
				leaverbuster_queue[i].waitingTime--;
			}
		}
	}

	int customersLeft(){
		return sub_queue_one.size() + sub_queue_two.size() + sub_queue_three.size() + leaverbuster_queue.size();
	}

	void deleteFromQueue(int custID, vector<Customer> * queueList){
	    for(int i=0;i<queueList->size();i++){
	        if(queueList->at(i).custID == custID){
						cout << "deleting " << custID << " from queue" << endl;
            queueList->erase(queueList->begin()+i);
            break;
	        }
	    }
	}

	void finishRun(int processID){
		totalRuns++;
		for(int i=0;i<sub_queue_one.size();i++){
			sub_queue_one[i].waitCount++;
			if(sub_queue_one[i].custID == processID) {
				sub_queue_one[i].waitCount--;
			}
		}
		for(int i=0;i<sub_queue_two.size();i++){
			sub_queue_two[i].waitCount++;
			if(sub_queue_two[i].custID == processID) {
				sub_queue_two[i].waitCount--;
			}
		}
		for(int i=0;i<sub_queue_three.size();i++){
			sub_queue_three[i].waitCount++;
			if(sub_queue_three[i].custID == processID) {
				sub_queue_three[i].waitCount--;
			}
		}
		for(int i=0;i<leaverbuster_queue.size();i++){
			leaverbuster_queue[i].waitCount++;
			if(leaverbuster_queue[i].custID == processID) {
				leaverbuster_queue[i].waitCount--;
			}
			if(leaverbuster_queue[i].waitCount%8==0){
				int currentQueue = leaverbuster_queue[i].queue;
				leaverbuster_queue[i].priority--;
				leaverbuster_queue[i].waitCount=0;
				leaverbuster_queue[i].fixQueue();
				if(currentQueue!=leaverbuster_queue[i].queue){
					// add to the promotion vector and delete from current one
					cout << "customer " << leaverbuster_queue[i].custID << " has been promoted" << endl;
					promoted_customers.push_back(leaverbuster_queue[i]);
					leaverbuster_queue.erase(leaverbuster_queue.begin()+i);
					i--;
				}
			}
		}
	}
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
					customer.waitCount=StringToInt(token);
				}
				str.erase(0, pos + delimiter.length());
			}
			cout << "parsed customer priority is " << customer.priority << endl;
			customer.ticketsRemaining=StringToInt(str);
			customer.fixPriority();
			// add customer to parsed customer map
			parsedCustomers[customer.arrivalTime].push_back(customer);
			cout << "adding customer " << customer.custID << " p: " << customer.priority << " to map with time " << customer.arrivalTime << endl;
			cout << "a" << customer.custID << " " << customer.arrivalTime << " " << customer.priority << " " << customer.age << " " << customer.ticketsRemaining << endl;
			// customerQueue.addCustomer(customer);
			totalCustomers++;
		}

		cout << "total customers is " << totalCustomers << endl;

		customerQueue.checkQueues();

		map<int, vector<Customer> >::iterator it = parsedCustomers.begin();
		// process the queue
		int tick = 0;
		int check = 0;
		while(completedCustomers.size() < totalCustomers){
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

			if(tick == 140 || tick == 0) {
				customerQueue.checkQueues();
			}

			bool arrivedInQueueOne = customerQueue.checkForArrivals(arrivingCustomers);
			if(customerQueue.customersLeft()==0){
				tick++;
				cout << "----------------------------------------" << endl;
				continue;
			}
			// get the customer at the front of the queue if the queue is queue two
			if((customerQueue.getFrontCustomer()->priority <= 3 && currentCustomer->queue == 0) || newCustomer){
				int newCustID = customerQueue.getFrontCustomer()->custID;
				cout << "getting customer " << newCustID << endl;
				// interrupted customer counts as a run
				if (customerQueue.getFrontCustomer()->priority <= 3 && currentCustomer->queue == 0) {
					customerQueue.finishRun(newCustID);
				}
				currentCustomer = customerQueue.getFrontCustomer();
				newCustomer = false;
				// start a new run, but set the quota to 20 if in leaverbuster_queue
				currentCustomer->newRun();
				if(currentCustomer->queue==0){
					currentCustomer->ticketQuota=20;
				}
			}
			cout << "we are processing customer " << currentCustomer->custID << " at tick " << tick << endl;
			cout << "he has " << currentCustomer->ticketsRemaining << " tickets remaining and a quota of " << currentCustomer->ticketQuota << endl;
			currentCustomer->process(tick);
			// customer finished completely or customer finished their quota run
			if(currentCustomer->ticketsRemaining==0){
				customerQueue.finishRun(currentCustomer->custID);
				newCustomer = true;
				currentCustomer->terminationTime = tick+1;
				completedCustomers.push_back(*currentCustomer);
				customerQueue.deleteFromQueue(currentCustomer->custID,customerQueue.getQueue(currentCustomer->queue));
			} else if(currentCustomer->ticketQuota==0){
				cout << "customer " << currentCustomer->custID << " finished its quota for this run" << endl;
				customerQueue.finishRun(currentCustomer->custID);
				newCustomer = true;
				vector<Customer> * queueToDelFrom = customerQueue.getQueue(currentCustomer->queue);
				currentCustomer->waitCount=0;
				currentCustomer->finishRun();
				currentCustomer->checkForDemotion();
				customerQueue.quantum_customer = *currentCustomer;
				customerQueue.deleteFromQueue(currentCustomer->custID,queueToDelFrom);
			}
			// make everyone else wait
			customerQueue.updateCustomers(currentCustomer->custID);
			// cout << "total runs so far is " << customerQueue.totalRuns << endl;
			tick++;
		}

		cout << "completed customers size: " << completedCustomers.size() << endl;

		sort(completedCustomers.begin(),completedCustomers.end(),TerminationTimeCheck);
		//output results
		cout << "name arrival end ready running waiting" << endl;
		for(int i=0;i<totalCustomers;i++){
			if(completedCustomers[i].custID<1000){
				completedCustomers[i].waitingTime=(completedCustomers[i].terminationTime-completedCustomers[i].firstProcessTime)-completedCustomers[i].runningTime;
				cout << "a" << completedCustomers[i].custID << " " << completedCustomers[i].arrivalTime << " " << completedCustomers[i].terminationTime << " " << completedCustomers[i].firstProcessTime << " " << completedCustomers[i].runningTime << " " << completedCustomers[i].waitingTime << endl;
			}
		}


		return 0;
	}
