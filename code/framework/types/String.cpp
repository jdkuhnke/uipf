#include "String.hpp"

using namespace std;
using namespace uipf;

// gives the string
/*
*/
string String::getContent(){
	return str;
}

// sets the string
/*
s	string content
*/
void String::setContent(std::string s){
	str = s;
}

// returns the data type of this data object
Type String::getType(){
	return STRING;
}
