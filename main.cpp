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
		int currentRuns;
		int queuePos; //Might not need this
		int runningTime; //Duration of buying tickets
		int waitingTime; //Duration of waiting
		int terminationTime; //Time when process bought all tickets
public:
	Customer(){
		
	}

};

int main(int argc, char *argv[]){
	char str[256];
	string delimiter = " ";
	size_t pos = 0;
	string token;
	ifstream infile(argv[1]);
	while(infile) {
		Customer customer;
	   	infile.getline(str, 256);  // delim defaults to '\n'
	   	while((pos = s.find(delimiter)) != string:npos){
			token = s.substr(0,pos);
			cout << token << endl;
			s.erase(0,pos+delimiter.length());
		}
	   	if(priority<=3){
	   		queueOne.addCustomer(customer);
	   	} else {
	   		queueTwo.addCustomer(customer);
	   	}
	   	if(infile) cout << str << endl;
	}
	return 0;
}

std::string s = "scott>=tiger>=mushroom";
std::string delimiter = ">=";

size_t pos = 0;
std::string token;
while ((pos = s.find(delimiter)) != std::string::npos) {
    token = s.substr(0, pos);
    std::cout << token << std::endl;
    s.erase(0, pos + delimiter.length());
}
std::cout << s << std::endl;