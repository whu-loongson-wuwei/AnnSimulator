
/************************************************
class derived from AtomOperation is a harware part;
class Connect connect two AtomOperations;
modules run at the same time 
************************************************/
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
                for_each(wires.begin(),wires.end(),[](Connect e){\
                    e._set_data();\
                    if(e.Getm2()->GetType()==MUL||e.Getm2()->GetType()==ADD) e.Getm2()->Process();});                

enum HW_TYPE {
    UNDEFINE,
    REGISTER,
    MUL,
    BOBBLE,
    ASSIGN,
    ADD
};
#define TEST
class AtomOperation {
#ifndef TEST
protected:
#else
public:
#endif
    AtomOperation* attached_func_unit = nullptr;
    int *fan_out = nullptr;
    int count_fan_out;
    int *fan_in = nullptr;
    int count_fan_in;
    int current_connect_in=-1;
    int current_connect_out=-1;
    HW_TYPE type;
public:
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
    virtual void Process(){} 
};

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
    void _set_data()
    {    
         m2->fan_in[num_port_in]=m1->fan_out[num_port_out];
    }
    AtomOperation * Getm2()
    {
        return m2;
    }
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
