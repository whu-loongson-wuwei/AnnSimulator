
/*****************************************************************************
*  Copyright (C) 2014 Liwenqing ,Wuwei  wuwei@loongson.cn                    *
*                                                                            *
*  @file     common.h                                                        *
*  @brief    common definitions which will be used in most files             *
*                                                                            *
*  @author   Shaqing ShaWei                                                  *
*  @email    liwenqing@loongson.cn                                           *
*  @version  0.0.0.1                                                         *
*  @date     2017-11-24                                                      *
*  @license  GNU General Public License (GPL)                                *
*                                                                            *
*                                                                            *
*****************************************************************************/

#include<cassert>
#include<functional>
#include<algorithm>
#include<cstring>
#include<vector>
#include<iostream>
//#define TEST
#define IN
#define OUT
#define EXECUTE for_each(modules.begin(),modules.end(),[](AtomOperation* e){e->Process();});\
                for_each(wires.begin(),wires.end(),[](Connect e){e._set_data();});

enum HW_TYPE {
    UNDEFINE,
    REGISTER,
    MUL,
    BOBBLE,
    ASSIGN,
    ADD
};
#define TEST

/**
 * @brief the basic class of all hardware modules/n
 * all hardware parts must derive from this class
 * and you should never change this class
 */
class AtomOperation {
#ifndef TEST
protected:
#else
public:
#endif
    int *fan_out = nullptr; ///< place the results of operation
    int count_fan_out; ///< total of fan_out
    int *fan_in = nullptr; ///<place the operands and controls of operation
    int count_fan_in;///< total of fan_in
    int current_connect_in=-1;///<denote the current port connected from other
    int current_connect_out=-1;///<denote the current port connected to other
    HW_TYPE type;///< hardware type
public:
    /**
     * @brief make room for fan_in and fan_out
     * @param a  the value of count_fan_in
     * @param b  the value of count_fan_out
     */
    AtomOperation(int a,int b)
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
 * @brief actual operation of this module\n
 * all class should override this function
 */
    virtual void Process(){} 
};

/**
 * @brief connect two AtomOperations/n
 * denote the logical wire of two hardware modules
 */
class Connect {
#ifndef TEST
protected:
#else
public:
#endif
    AtomOperation *m1;
    AtomOperation *m2;
    int num_port_in;
    int num_port_out;
public:
    /**
     * @brief update the value from fan_out to fan_in
     */
    void _set_data()
    {    
         m2->fan_in[num_port_in]=m1->fan_out[num_port_out];
        if(m2->GetType()==MUL||m2->GetType()==ADD)
            m2->Process();
    }
    AtomOperation * Getm2()
    {
        return m2;
    }
    /**
     * @brief connect n1 and n2
     * @param n1 source module
     * @param n2 destination module
     */
    void operator()(AtomOperation *n1,AtomOperation *n2)
    {   
       
        m1=n1;
        m2=n2;
        num_port_out=++m1->GetCurrentOut();
        num_port_in=++m2->GetCurrentIn();
        assert(m1->GetCountFanOut()>m1->GetCurrentOut());
        assert(m2->GetCountFanIn()>m2->GetCurrentIn());
    }
};
using Modules = std::vector<AtomOperation*>;
using Wires = std::vector<Connect>;
using Wire = Connect;
#define SET_ALIAS(src,alias)\
    auto &alias = src;
