#ifndef LIBUIPF_UTIL_HPP
#define LIBUIPF_UTIL_HPP

#include <string>
#include <sstream>
#include <vector>

// a macro to simply simple for loops
#define uipf_cforeach(i, set) for(auto i = set.cbegin(); i != set.end(); ++i)
#define uipf_foreach(i, set) for(auto i = set.begin(); i != set.end(); ++i)


// This is a collection of help-methods

extern "C" {

// TODO refactor these to "split()"
// gets the first part of the string, which is divided by : , e.g. source:../ball.png -> source
std::string uipf_split_first(const std::string &s);

// gets the second part of the string, which is divided by : , e.g. source:../ball.png -> ../ball.png
std::string uipf_split_second(const std::string &s);

// renames a filename by adding _result before the end, eg. ball.png -> ball_result.png
std::string uipf_rename(const std::string &s);

std::string uipf_rename_postfix(const std::string &s, const std::string &pf);

bool uipf_str_begins_with(const std::string &s, const std::string &with);

bool uipf_str_ends_with(const std::string &s, const std::string &with);

std::string uipf_str_to_lower(const std::string &st);

std::string uipf_str_to_upper(const std::string &st);

// http://stackoverflow.com/a/236803/1106908
void split_string_into(const std::string &s, char delim, std::back_insert_iterator<std::vector<std::string>> result);
std::vector<std::string> split_string(const std::string &s, char delim);

// http://stackoverflow.com/a/29892589/1106908
std::string & ltrim(std::string & str);
std::string & rtrim(std::string & str);
std::string & trim(std::string & str);
std::string trim_copy(std::string const & str);

// execute external program and return stdout
std::string uipf_exec_stdout(const char *cmd);

}

#endif //LIBUIPF_UTIL_HPP
