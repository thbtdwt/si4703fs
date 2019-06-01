#include <iostream> 
#include <sstream> 
#include <exception> 
#include <cstdarg>

#include "si4703_exception.h"


void raise_Si4703_exception_c_style_impl(const char* function, const char* format, ...) {

    char buffer[4096];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer,sizeof(buffer),format, args);
    va_end(args);

    throw  Si4703_exception(function, std::string(buffer));
}


Si4703_exception::Si4703_exception(const char* function, const char* msg) { 
    std::ostringstream oss; 
    oss << "Exception [" << function << "]: " 
        << msg; 
    this->msg = oss.str(); 
}

Si4703_exception::Si4703_exception(const char* function, std::string msg) { 
    std::ostringstream oss; 
    oss << "Exception [" << function << "]: " 
        << msg; 
    this->msg = oss.str(); 
}

Si4703_exception::~Si4703_exception() throw() {} 

const char* Si4703_exception::what() const throw() { 
    return this->msg.c_str(); 
} 