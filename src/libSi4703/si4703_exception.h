#ifndef SI4703_EXCEPTION_H
#define SI4703_EXCEPTION_H

#include <exception> 
#include <string>


#define raise_Si4703_exception(format) \
do { \
        throw  Si4703_exception(__FUNCTION__, format); \
} while(0)

#define raise_Si4703_exception_c_style(format,...) \
do { \
        raise_Si4703_exception_c_style_impl(__FUNCTION__, format, ##__VA_ARGS__); \
} while(0)
void raise_Si4703_exception_c_style_impl(const char* function, const char* format, ...) __attribute__ ((format (printf, 2, 3)));

class Si4703_exception : public std::exception 
{ 
public: 
    Si4703_exception(const char* function, const char* msg);
    Si4703_exception(const char* function, std::string msg);
    virtual ~Si4703_exception() throw();
    virtual const char * what() const throw();
  
private: 
    std::string msg; 
}; 

#endif //SI4703_EXCEPTION_H