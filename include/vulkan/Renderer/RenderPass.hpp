#pragma once
#include <string>


class IRenderPass
{
private:
    std::string name;    
public:
    IRenderPass();
    ~IRenderPass();


    virtual ~IRenderPass() = default;
};
