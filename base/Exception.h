/******************************************************************************************
 * Read And Write Muduo(RAWMD)
 * wzhe, ahuwang@163.com
 ******************************************************************************************/

#pragma once

#include <base/Types.h>

#include <exception>

namespace rawmd
{

class Exception : public std::exception
{
public:
    explicit Exception(const char* what);
    explicit Exception(const string& what);
    virtual ~Exception() throw();
    virtual const char* what() const throw();
    const char* stackTrace() const throw();

private:
    void fillStackTrace();
    string demangle(const char* symbol);
    string message_;
    string stack_;

};

}