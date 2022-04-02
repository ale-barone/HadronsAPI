#include <iomanip>
#include <iostream>
#include <algorithm> 
#include <Hadrons/Modules.hpp>

using namespace Grid;
using namespace Hadrons;


#ifndef Hadrons_API_Utilities_hpp_
#define Hadrons_API_Utilities_hpp_

#define BEGIN_API_NAMESPACE \
namespace API {

#define END_API_NAMESPACE }

#define BEGIN_APIMODULE_NAMESPACE(name)\
BEGIN_API_NAMESPACE \
namespace name {

#define END_APIMODULE_NAMESPACE \
END_API_NAMESPACE \
}


////////////////////////////////////////////////////////////////////////////////
// STRING MANIPUALTION
////////////////////////////////////////////////////////////////////////////////


// remove string "toErase" from "string"
std::string remove_str(std::string string, std::string toErase){ 
    size_t pos = string.find(toErase);
    if (pos != std::string::npos){
        // If found then erase it from string
        string.erase(pos, toErase.length());
    }
    return string; 
}

// count the number of negative signs "-" from the string "string"
int count_negative(std::string string){
    int neg = 0;
    size_t pos_neg = string.find("-");
    if (pos_neg!=std::string::npos)
    {
        neg +=1;
        pos_neg = string.find("-", pos_neg+1);
        while (pos_neg!=std::string::npos){
            neg +=1;
            pos_neg = string.find("-", pos_neg+1);
        }         
    }
    return neg;
}

// remove "_mom_" string and the value of momentum in the form "x_x_x_x"
std::string remove_mom_str(std::string string){ 
    std::string toErase = "_mom_";
    size_t pos = string.find(toErase);
    if (pos != std::string::npos)
    {   
        int neg = count_negative(string);
        // If found then erase it from string
        string.erase(pos, toErase.length()+7+neg);
    }
    return string; 
}

// copy substring in "string" that starts from "begin" (included)
// for lenght len (until the end if nothing is specified)
std::string copy_from(std::string string, std::string begin, size_t len = std::string::npos){
    size_t pos = string.find(begin);  
    std::string out_str;
    if (pos != std::string::npos){
        out_str = string.substr(pos, len);
    }
    return out_str;
}

// find the 4th underscore. The string is supposed to start with the
// momentum info "(mom)_x_x_x_x_*" and the 4th underscore is the last one after
// the last component. If it's not there it must mean that the string is terminated
size_t pos_5th_underscore(std::string string){
	size_t pos = string.size();
	char underscore = '_';
	int num_underscore = 0;
	for (int i=0; i<string.size(); i++){
		if (string[i] == underscore)
			num_underscore++;
		if (num_underscore == 5){
			pos = i;
			break;
		}
	}
	return pos;
}

size_t pos_4th_underscore(std::string string){
	size_t pos = string.size();
	char underscore = '_';
	int num_underscore = 0;
	for (int i=0; i<string.size(); i++){
		if (string[i] == underscore)
			num_underscore++;
		if (num_underscore == 4){
			pos = i;
			break;
		}
	}
	return pos;
}

// number of decimals (max is 6)
int num_decimals(float num){
    num = std::abs(num-(int)num);
    std::ostringstream num_stream;
    num_stream << std::abs(num);
    std::string num_string = num_stream.str();

    // erase 
    std::string dot = ".";
    size_t pos = num_string.find(dot);
    num_string.erase(0, pos+1);
    
    int num_digits = num_string.length();
    return num_digits;
}

// string manipulation
std::string space_to_underscore(std::string text){
    std::replace(text.begin(), text.end(), ' ', '_');
    return text;
}
std::string underscore_to_space(std::string text){
    std::replace(text.begin(), text.end(), '_', ' ');
    return text;
}


////////////////////////////////////////////////////////////////////////////////
// MOMENTUM (FROM SINK)
////////////////////////////////////////////////////////////////////////////////

// make momentum parameter in the form "x x x x" 
std::string make_mom_par(std::array<int, 4> mom){
    std::string mom_par = std::to_string(mom[0]);
    for (int d=1; d<mom.size(); d++ ){
        mom_par += " " + std::to_string(mom[d]);
    }
    return mom_par;
}

// make 3momentum parameter in the form "x x x"
std::string make_3mom_par(std::array<int, 3> mom){
    std::string mom_par = std::to_string(mom[0]);
    for (int d=1; d<mom.size(); d++ ){
        mom_par += " " + std::to_string(mom[d]);
    }
    return mom_par;
}

// make momentum string in the form "x_x_x_x"
std::string make_mom_name(std::array<int, 4> mom){
    std::string mom_str = space_to_underscore(make_mom_par(mom));
    return mom_str;
}

// make 3momentum string in the form "x_x_x" 
std::string make_3mom_name(std::array<int, 3> mom){
    std::string mom_str = space_to_underscore(make_3mom_par(mom));
    return mom_str;
}

// get momentum in a string with the "mom_" keyword
std::string get_snkmom(std::string string){
    std::string mom_full = copy_from(string, "mom_");
	std::string mom_string = copy_from(mom_full, "mom_", pos_4th_underscore(mom_full));
	std::string mom = remove_str(mom_string, "mom_");
	return mom;
}

// get momentum in a string with the "mom_" keyword
std::string get_mom(std::string string){
    std::string mom_full = copy_from(string, "mom_");
	std::string mom_string = copy_from(mom_full, "mom_", pos_5th_underscore(mom_full));
	std::string mom = remove_str(mom_string, "mom_");
	return mom;
}


////////////////////////////////////////////////////////////////////////////////
// TWIST
////////////////////////////////////////////////////////////////////////////////

// make twist parameter in the form "x x x x" 
std::string make_twist_par(std::array<float, 4> twist){
    // Create an output string stream
    std::ostringstream streamObj;
    // Set fixed-point Notation and precision
    streamObj << std::fixed << std::setprecision(num_decimals(twist[0])) << twist[0];
    // Get string from output string stream
    std::string mom_par = streamObj.str();

    //std::string mom_par = std::to_string(twist[0]);
    for (int d=1; d<twist.size(); d++){
        std::ostringstream streamObj_d;
        streamObj_d << std::fixed << std::setprecision(num_decimals(twist[d])) << twist[d];
        std::string mom_par_d = streamObj_d.str();        
        mom_par += " " + mom_par_d;
    }
    return mom_par;
}

// make twist name in the form "x_x_x_x"
std::string make_twist_name(std::array<float, 4> twist){
    std::string twist_str = space_to_underscore(make_twist_par(twist));
    return twist_str;
}

// get twist in a string with the "tw_" keyword
std::string get_twist(std::string string){
    std::string tw_string_full = copy_from(string, "tw_");
	std::string tw_string = copy_from(tw_string_full, "tw_", pos_5th_underscore(tw_string_full));
	std::string twist = remove_str(tw_string, "tw_");
	return twist;
}

// remove "_tw_" string and the value of momentum in the form "x_x_x_x"
std::string remove_twist_str(std::string string){ 
    std::string toErase = copy_from(string, "_tw_", pos_5th_underscore(string));
    size_t pos = string.find(toErase);
    if (pos != std::string::npos){   
        string.erase(pos, toErase.length());
    }
    return string; 
}

#endif