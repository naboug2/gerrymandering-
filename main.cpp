//  Gerrymandering
//  System: VSCode on MacOS
//  Author: Nuha Abougoash,  

#include "ourvector.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <math.h>
#include <string>

using namespace std;

//////////////// HELPER FUNCTIONS //////////////// 


// The splitString function takes a line and split it on the delimiter and push the parts into items
void splitString(string line, char delimiter,ourvector<string>& items ) {
	stringstream s_stream(line);
	while (s_stream.good()) {
		string item;
		getline(s_stream, item, delimiter);
		items.push_back(item);
	}
}
string trimWhiteSpace(const string s) {
    size_t index = 0;
    while(index < s.size() && isspace(s[index++]));
    return s.substr(index-1);
}

void splitFirstWord(const string &s, string &first, string &remains) {
    // first word or possibly the only word
    first = s.substr(0,s.find(" "));
 
    // skip all whitespace and get everything after the first word
    if (s.size() != first.size())
    {
        remains = trimWhiteSpace(s.substr(s.find(" ")));
    }
    else
    {
        remains = "";
    }

}


void tolower(string &s) {
    transform(s.begin(), s.end(), s.begin(), 
    [](unsigned char c) {return tolower(c);} );
}


// struct for districts data
struct districts {
    string state;
    ourvector<int> district_num;
    ourvector<int> dem_votes;
    ourvector<int> rep_votes;
};

// struct for eligible voters data
struct eligible_voters {
    string state;
    int voter_count;
};

// This function checks to see if the data was loaded and prints the statement accordingly
void check_loaded(bool loaded){
    if (loaded) {
        cout << "Data loaded? Yes" << endl;
        //return true;
    }
    else {
        cout << "Data loaded? No" << endl;
        //return false;
    }
}

// This function checks to see if the state was loaded and prints the statement accordingly
void check_state(bool state, string state_name) {
    if (state) {
        cout << "State: " << state_name << endl;
    }
    else {
        cout << "State: " << state_name << endl;
    }
}
// check state , if true, cout state name, else cout N/A

//////////////// LOAD FUNCTION //////////////// 

// This function reads in both inputted files and pushes the data from each file into a vector
void load(string file1, string file2, bool &check1, bool &check2, bool &loaded, ourvector<districts> &dist_data, ourvector<eligible_voters> &voter_data){
  
    // READ FILE 1    
    ifstream filein(file1);
    if(filein.is_open()) {
        string line;
        check1 = true;
        while(!filein.eof()){
            
            while (getline(filein, line)) {
                ourvector<string> items;
                splitString(line, ',', items);
                districts dist;

                dist.state = items.at(0); //set first index equal to state name

                for (int i = 1; i < items.size(); i += 3) {
                    if (items.at(i) == "AL") {
                        dist.district_num.push_back(1);
                    }
                    else {
                        dist.district_num.push_back(stoi(items.at(i)));
                    }

                    dist.dem_votes.push_back(stoi(items.at(i+1)));
                    dist.rep_votes.push_back(stoi(items.at(i+2)));
                    
                }
                dist_data.push_back(dist);
            }
        }

        filein.close();

        cout << "Reading: " << file1 << endl;
        for (int i = 0; i < dist_data.size(); i++) {
            cout << "..." << dist_data.at(i).state << "..." << dist_data.at(i).district_num.size() << " districts total" << endl;
        }

        // READ FILE 2
        ifstream file2in(file2);
        if (file2in.is_open()) {
            string line2;
            check2 = true;
            while (!file2in.eof()) {

                while (getline(file2in, line2)) {
                    ourvector<string> items2;
                    splitString(line2, ',', items2);
                    eligible_voters voters;


                    // TO DO: find a way to check if there is data in file2 that is not in file1
                    for (int i = 0; i < items2.size(); i += 2) {
                        if (items2.at(0) != "District of Columbia") {
                            voters.state = items2.at(0); //set first index equal to state name
                            voters.voter_count = stoi(items2.at(1));
                            voter_data.push_back(voters);
                        }
                    }
                }
            }

            file2in.close();

            cout << "Reading: " << file2 << endl;
            for (int i = 0; i < voter_data.size(); i++) {
                cout << "..." << voter_data.at(i).state << "..." << voter_data.at(i).voter_count << " eligible voters" << endl;
            }
        }

        else {
            check1 = false;
            loaded = false;
            cout << "Invalid second file, try again." << endl;
        }
    }
    
    else {
        check2 = false;
        loaded = false;
        cout << "Invalid first file, try again." << endl;
        
    }
}

//////////////// SEARCH FUNCTION //////////////// 

// This function searches through the data stored in the vector and if input matches one of the states, it return all the information for that matching state.
districts search(string state_input, string &state_name, bool &state, ourvector<districts> &dist_data) {
    districts dist;
    
    for(int i = 0; i < dist_data.size(); i++){
        tolower(state_input);
        tolower(dist_data.at(i).state);
        if (state_input == dist_data.at(i).state){ ///TO DO: check for lower & upper
            
            dist = dist_data.at(i);
            state_name = dist_data.at(i).state ;
            state = true;
            //cout << "State: " << state_name << endl;
            
        }
        else {
            state = false;
        }
    }
    return dist; // RETURNS THE ADDRESS FOR INPUTTED STATE 
}

//////////////// STATS FUNCTION ////////////////

void stats(int& num, ourvector<districts> &dist_data, ourvector<eligible_voters> &voter_data) {

    eligible_voters ev;
    string gerry; // true if efficiency gap >= 0.07
    int wasted_dem;
    int wasted_rep;
    int wasted_vote; //any vote not needed to win the election
    int total_votes;
    int over_half; // over_half = ceil((dem_votes + rep_votes)/2)+1
    int total_wasted;
    int total_wasted_dem; 
    int total_wasted_rep;
    int efficiency_gap; // efficiency_gap = (abs(total_wasted_dem - total_wasted_rep) / total_wasted) * 100;

    int voters = voter_data.at(num).voter_count;
    

    //TO DO: check if state Gerrymandered 
    for (int i = 0; i < voter_data.at(num).state.size(); i++)
    
    // if state has more than 3 districts, otherwise cannot calculate
        // total_votes = dem_votes + rep_votes
        //over_half = total_votes / 2; (rounded up)
        // if (dem_votes > rep_votes){
            // wasted_dem = dem_votes - overhalf;
            // wasted_rep = rep_votes;}
        // if (rep_votes > dem_votes) {
            // wasted_rep = rep_votes - overhalf;
            // wasted_dem = dem_votes; }

    cout << "Gerrymandered: " << gerry << endl;

    //print number of wasted dem votes & rep votes
    cout << "Wasted Democratic votes: " << wasted_dem << endl;
    cout << "Wasted Republican votes: " << wasted_rep << endl;


    //print eligible voter
    cout << "Eligible voters: " << ev.voter_count << endl;
    

    //TO DO: create bool to check if loaded
    cout << "Data loaded? No" << endl;
    cout << "State: TO DO" << endl;

}

//////////////// PLOT FUNCTION //////////////// 

// void plot(){

// }


int main( ){
    string userEntry;
    string command, remains;
    ourvector<districts> dist_data;
    ourvector<eligible_voters> voter_data;
    bool loaded = false;
    bool state = false;
    string state_name = "N/A";
    bool check1 = false, check2 = false;

    //cout out intro
    cout << "Welcome to the Gerrymandering App!" << endl << endl;

    // Stay in menu until exit command
    do {
        check_loaded(loaded);
        check_state(state, state_name );
        cout << endl << "Enter command: " ;
        getline(cin, userEntry);
        cout << endl << endl;
        cout << "-----------------------------" << endl << endl;
        // Split the user entry into two pieces
        splitFirstWord(userEntry, command, remains);
        tolower(command);

        // take an action, one per iteration, based on the command
        if (command == "load") {

            string file1, file2;
            splitFirstWord(remains, file1, file2);
            
            if (!check1 && !check2) { // if check1 and check2 true then loaded is true, and read files
                loaded = true;
                //run load
                load(file1, file2, check1, check2, loaded, dist_data , voter_data);
            }
            
            else { // otherwise loaded is true but already read in files
                loaded = true;
                cout << "Already read data in, exit and start over." << endl;
            }
            cout << endl;

        }

        else if (command == "search") {
            if (!state){
                search(remains, state_name, state, dist_data );
            }
            else if (state & loaded){
                cout << "No data loaded, please load data first." << endl;
            }
            else {
                state = false;
                cout <<"No state indicated, please search for state first." << endl;
            }
        }

        else if (command == "stats") {
            // run stats function
        }

        else if (command == "plot") {   
            // run plot function 
        }

    } while(command!="exit");        
    
    return 0;
}