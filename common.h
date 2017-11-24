
/***************************************************
 * @file     common.h                              *
 * @brief hardware execute engine                  *
 * @author wuwei                                   *
 * @email    wuwei@loongson.cn                     *
 * @date 2017-11-24                                *
 * @version 0.0.1                                  *
 * @license  GNU General Public License (GPL)      *
 ***************************************************/
#include<cassert>
#include<functional>
#include<algorithm>
#include<cstring>
#include<vector>
#include<iostream>
#define TEST
#define IN
#define OUT
#define EXECUTE for_each(modules.begin(),modules.end(),[](Operation* e){e->Process();});\
                for_each(wires.begin(),wires.end(),[](Connect e){\
                    e._set_data();});  
#define SET_ALIAS(src,alias) auto &alias = src;              


/**
 * @brief hardware type
 */
enum HW_TYPE {
    UNDEFINE,
    REGISTER,
    MUL,
    ADD,
    ALLOCATOR,
    SELECTOR
};
/**
 * @brief the base class of all hardware modules
 */
class Operation {
#ifndef TEST
protected:
#else
public:
#endif
    int *fan_out = nullptr; ///< place the result of this operation
    int count_fan_out;      ///<total of fan_out
    int *fan_in = nullptr;  ///<place operands and controls of this module
    int count_fan_in;       ///<total of fan_int
    int current_connect_in=-1; /// the current connect to this module from others
    int current_connect_out=-1;/// the current connect from this module to others
    HW_TYPE type;
public:
    /**
     * @brief initialize a module with a inputs and b outputs
     * @param a the value of count_fan_int
     * @param b the valur of count_fan_out
     */
    Operation(int a,int b)
    {
        fan_in = new int[a];
        fan_out = new int[b];
        count_fan_in = a;
        count_fan_out = b;
        memset(fan_in,0,a*sizeof(int));
        memset(fan_out,0,b*sizeof(int));
    }
    HW_TYPE GetType()
    {
        return type;
    }
    int Output(int index)const
    {
        assert(count_fan_out > index);
        return fan_out[index];
    }
    void Input(int index, int value)
    {
        assert(count_fan_in>index);
        fan_in[index]=value;
    }
    int& GetCurrentIn()
    {
        return current_connect_in;
    }
    int & GetCurrentOut()
    {
        return current_connect_out;
    }
    int GetCountFanIn()const
    {
        return count_fan_in;
    }
    int GetCountFanOut()const
    {
        return count_fan_out;
    }
//    virtual void Initialize()=0;
/**
 * @brief  the actual operation of this module/n
 * @note all derived class must implement this method
 */
    virtual void Process(){} 
};

/**
 * @brief connect two operations
 */
class Connect {
#ifndef TEST
protected:
#else
public:
#endif
    Operation *m1;
    Operation *m2;
    int num_port_in; ///< this wire connect to fan_ont[num_port_out] in m1
    int num_port_out;///< this wire connect to fan_in[num_port_in] in m2
public:
    /**
     * @brief update fan_out to fan in between two adjacent register
     * @note you should not use this function explicitly.\n
     * instead,use macros EXECUTE
     */
    void _set_data()
    {    
         m2->fan_in[num_port_in]=m1->fan_out[num_port_out];
        //execute a function unit when update
         if(m2->GetType()!=REGISTER) m2->Process();
    }
    /**
     * @brief connect n1 with n2
     */
    void operator()(Operation *n1,Operation *n2)
    {   
        m1=n1;
        m2=n2;
        num_port_out=++m1->GetCurrentOut();
        num_port_in=++m2->GetCurrentIn();
        assert(m1->GetCountFanOut()>m1->GetCurrentOut());
        assert(m2->GetCountFanIn()>m2->GetCurrentIn());
    }
};
using Modules = std::vector<Operation*>;
//using FunctionUnitSet = std::vector<AtomOperation*>;
using Wires = std::vector<Connect>;
using Wire = Connect;