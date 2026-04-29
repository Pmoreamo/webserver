#ifndef UTILS_HPP
# define UTILS_HPP

#include <string>
#include <string>
#include <sys/stat.h>
#include
#include
#include
#include
#include
#include
#include
#include
#include
#include
#include
#include


void            RemoveComments(std::string &fullLine);
void            trimWhitepaces(std::string &fullLine);
size_t          digitCounter(int num);
bool            isAllDigits(const std::string &totest);
bool            isAllowedChars(const std::string &totest, const std::string &accepted);
bool            isAllowedChars(const std::string &totest);
void            findURL(const std::string &urlClient, const std::vector<LocationConfig> &urlLocs, std::string &urlWinner);
int             validMethods(const std::string  &methods, const LocationConfig *local, short &code);
bool            isDirectory(std::string &_urlDirect);
bool            fileExist(std::string &_urlFile);
std::string     codeToStr(short &code);
void            fillMapCode(std::map<short, std::string> &_mapCode);
std::string     statusCodeToString(short code);

#endif
