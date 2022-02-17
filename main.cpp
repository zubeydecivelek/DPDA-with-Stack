#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stack>
#include <string>

using namespace std;

vector<vector<string>> transitionRule;
stack<string> stackDPDA;
vector<string> statesVector;
string startState;
vector<string> finalStates;
vector<string> inputAlphabet;
vector<string> stackAlphabet;
vector<vector<string>> inputVector;

///Prints the stack
void print(string &output);

///Checks if the vector contains the element
bool contains(vector<string> &vector, string find);

///Checks the stack is empty
bool isEmpty();

///Splits the string
vector<string> split(string str, char delimeter);

///Splits the states
void splitFirstLine(string firstLine, vector<string> &states, string &startState, vector<string> &finalStates);

///Finds the next state
vector<string> findState(string &state, string &read, string &topOfStack, bool &isUsed);

///Applies the transition rules
void applyAutomata(vector<string> &state, vector<string> &currentRead, string &currentState, int &readIndex, bool &isUsed);

///Controls if dpda description is valid. If it's invalid program prints an error message and finishes the program.
void controlInputs();

int main(int argc, char *argv[]) {
    freopen(argv[3],"w",stdout);  // Using cout for writing in the output file.

    fstream files;
    files.open(argv[1],ios::in);

    string line;
    getline(files,line); //reading the first line of dpda file
    if (!line.empty() && (line[line.size() - 1] == '\r' || line[line.size() - 1] == '\n')) line.erase(line.size() - 1);
    splitFirstLine(line,statesVector,startState,finalStates);

    getline(files,line); //reading the second line of dpda file
    if (!line.empty() && (line[line.size() - 1] == '\r' || line[line.size() - 1] == '\n')) line.erase(line.size() - 1);
    inputAlphabet = split(line.substr(2),',');

    getline(files,line); //reading the third line of dpda file
    if (!line.empty() && (line[line.size() - 1] == '\r' || line[line.size() - 1] == '\n')) line.erase(line.size() - 1);
    stackAlphabet = split(line.substr(2),',');

    while (getline(files,line)){  //reading the transition rules
        if (!line.empty() && (line[line.size() - 1] == '\r' || line[line.size() - 1] == '\n')) line.erase(line.size() - 1);
        vector<string> appendVec = split(line.substr(2),',');
        transitionRule.push_back(appendVec);
    }
    files.close();

    files.open(argv[2],ios::in);
    while (getline(files,line)){  //reading dpda-input file
        if (!line.empty() && (line[line.size() - 1] == '\r' || line[line.size() - 1] == '\n')) line.erase(line.size() - 1);
        vector<string> appendVec = split(line,',');
        inputVector.push_back(appendVec);
    }
    files.close();

    controlInputs(); //Controls if dpda description.

    for (vector<string> read : inputVector){
        string currentState = startState;
        string top = "e", currentRead;
        bool isUsed = false;
        int readIndex = 0;
        if (read.empty()) currentRead = "e";
        else currentRead = read[0];
        vector<string> foundState = findState(currentState,currentRead,top,isUsed);
        applyAutomata(foundState,read,currentState,readIndex,isUsed);
        cout<<endl;
    }
    return 0;
}

void print(string &output){
    if (stackDPDA.empty()) return;
    string x = stackDPDA.top();
    stackDPDA.pop();
    print(output);
    output += x + ",";
    stackDPDA.push(x);
}

bool contains(vector<string> &vector, string find){
    for (string str:vector){
        if (str == find) return true;
    }
    return false;
}

bool isEmpty(){
    if (stackDPDA.empty())return true;
    vector<string> vector;
    while (!stackDPDA.empty()){
        vector.push_back(stackDPDA.top());
        stackDPDA.pop();
    }
    for (int i =0; i<vector.size();i++){
        if (vector[i] != "$") return false;
    }
    return true;
}

vector<string> split(string str, char delimeter){
    vector<string> returnVec;
    istringstream stream (str);
    string splitted;
    while (getline(stream,splitted,delimeter)){
        returnVec.push_back(splitted);
    }
    return returnVec;
}

void splitFirstLine(string firstLine, vector<string> &states, string &startState, vector<string> &finalStates){
    string stateString, finalString;
    int i;
    for (i = 2; firstLine[i] != '=' ; i++){ //all states
        if (firstLine[i] != ' ')stateString.push_back(firstLine[i]);
    }
    states = split(stateString,',');
    for (i = i+3; firstLine[i] != ')'; i++){ //start state
        if (firstLine[i] != '(' && firstLine[i] != ' ') startState.push_back(firstLine[i]);
    }
    for (i = i+2; i< firstLine.size(); i++){ //final states
        if (firstLine[i] != '[' && firstLine[i] != ']' && firstLine[i] != ' ') finalString.push_back(firstLine[i]);
    }
    finalStates = split(finalString,',');
}

vector<string> findState(string &state, string &read, string &topOfStack, bool &isUsed){
    vector<string> vec;
    for (vector<string> vec: transitionRule){
        if (vec[0] == state && vec[1] == read && vec[2] == topOfStack){
            isUsed = true;
            return vec;
        }
    }
    for (vector<string> vec: transitionRule){
        if (vec[0] == state && vec[1] == read && vec[2] == "e"){
            isUsed = true;
            return vec;
        }
    }
    for (vector<string> vec: transitionRule){
        if (vec[0] == state && vec[1] == "e" && vec[2]==topOfStack){
            return vec;
        }
    }
    for (vector<string> vec: transitionRule){
        if (vec[0] == state && vec[1] == "e" &&  vec[2] == "e"){
            return vec;
        }
    }
    return vec;
}

void applyAutomata(vector<string> &state, vector<string> &currentRead, string &currentState, int &readIndex, bool &isUsed){
    if (state.empty()){
        if (contains(stackAlphabet,"$")) {
            if (contains(finalStates,currentState) && isEmpty()) cout<<"ACCEPT\n";
            else cout<<"REJECT\n";
        }
        else{
            if (contains(finalStates,currentState) && stackDPDA.empty()) cout<<"ACCEPT\n";
            else cout<<"REJECT\n";
        }
        while (!stackDPDA.empty()) stackDPDA.pop();
        return;
    }
    if (state[2] != "e"){
        stackDPDA.pop();
    }
    currentState = state[3];
    if (state[4] != "e"){
        stackDPDA.push(state[4]);
    }
    string top, output;
    if (stackDPDA.empty()) top ="e";
    else top = stackDPDA.top();
    if (isUsed) readIndex++;
    isUsed = false;
    if (readIndex >= currentRead.size()) {
        string read = "e";
        cout<<state[0]<<","<<state[1]<<","<<state[2]<<" => "<<state[3]<<","<<state[4] <<" [STACK]:";
        print(output);
        if (!output.empty()) output = output.substr(0,output.size()-1);
        cout<<output<<endl;
        vector<string> newState = findState(currentState, read, top, isUsed);
        applyAutomata(newState,currentRead,currentState,readIndex,isUsed);
    }
    else {
        cout<<state[0]<<","<<state[1]<<","<<state[2]<<" => "<<state[3]<<","<<state[4] <<" [STACK]:";
        print(output);
        if (!output.empty()) output = output.substr(0,output.size()-1);
        cout<<output<<endl;
        vector<string> newState = findState(currentState, currentRead[readIndex], top, isUsed);
        applyAutomata(newState,currentRead,currentState,readIndex,isUsed);
    }
}

void controlInputs(){
    for (vector<string> vector : transitionRule){
        if (!contains(statesVector, vector[0]) || !contains(statesVector, vector[3])){
            cout<<"Error [1]:DPDA description is invalid!";
            exit(0);
        }
        else if (vector[1] != "e" && !contains(inputAlphabet,vector[1]) ){
            cout<<"Error [1]:DPDA description is invalid!";
            exit(0);
        }
        else if ((vector[2] != "e" && !contains(stackAlphabet,vector[2])) || (vector[4] != "e" && !contains(stackAlphabet,vector[4]))){
            cout<<"Error [1]:DPDA description is invalid!";
            exit(0);
        }
    }
}