#include"common.h"
#include<iostream>
using namespace std;

class Register:public Operation
{
    // fan_in  is value in wire which is connected to this register from others
    // fan_out is the actual value of this  register 
public:
    Register(int b):Operation(1,b)
    {  
        type = REGISTER;
    }
    virtual void Process() override
    {   
        for(int i=0;i<count_fan_out;i++)
        {
            fan_out[i] = fan_in[0];
        }
    }
};

template<short cycle>
class Allocator:public Register
{
public:
    Allocator():Register(1)
    {

    }
};

/**
* @brief an allocator
* 2 input,the first one is data_in,the another is signal of control 
*/
template<>
class Allocator<1>:public Operation
{
public:
/**
* @brief initialize an allocator
* @param b output number
*/
    Allocator(int b):Operation(2,b)
    {
        type=ALLOCATOR;
    }
    virtual void Process() override
    {
        assert(fan_in[1]<count_fan_out);
        fan_out[fan_in[1]]=fan_in[0];
    }
};

template<short cycle>
class Selector:public Register
{
public:
    Selector():Register(1)
    {

    }
};
template<>
class Selector<1>:public Operation
{
public:
    Selector(int a):Operation(a+1,1)
    {
        type=SELECTOR;
    }
    virtual void Process() override
    {
        assert(fan_in[count_fan_in-1]<count_fan_in-1);
        fan_out[0]=fan_in[fan_in[count_fan_in-1]];
    }
};

template<short cycle>
class Mul:public Register
{
public:
    Mul():Register(1)
    {

    }
};
template<>
class Mul<1>:public Operation
{
public:
    Mul():Operation(2,1)
    {
        type=MUL;
    }
    virtual void Process() override
    {
        fan_out[0]=fan_in[0]*fan_in[1];
    }
};
template<short cycle>
class Add:public Register
{
public:
    Add():Register(1)
    {

    }
};
template<>
class Add<1>:public Operation
{
public:
    Add():Operation(2,1)
    {
        type=ADD;
    }
    virtual void Process() override
    {
        fan_out[0]=fan_in[0]+fan_in[1];
    }
};

class PE:public Operation
{
private:
     Modules modules;
     Wires wires;
public:
    PE():Operation(2,1)
    { 
        modules.push_back(new Register(1));
        modules.push_back(new Register(1));
        modules.push_back(new Mul<2>);
        modules.push_back(new Register(1));
        modules.push_back(new Register(1));
        modules.push_back(new Register(2));
        modules.push_back(new Register(2));
        modules.push_back(new Register(1));
        modules.push_back(new Register(1));
        
        SET_ALIAS(modules[0],mul_src_reg1)
        SET_ALIAS(modules[1],mul_src_reg2)
        SET_ALIAS(modules[2],mul_cycle_1)
        SET_ALIAS(modules[3],mul_dst_reg)
        SET_ALIAS(modules[4],add_acc_reg)
        SET_ALIAS(modules[5],add_dst_reg1)
        SET_ALIAS(modules[6],add_dst_reg2)
        SET_ALIAS(modules[7],pes_share_reg1)
        SET_ALIAS(modules[8],pes_share_reg2)

        Mul<1>* mul=new Mul<1>;
        Add<1>* add=new Add<1>;
        Allocator<1>* allocator=new Allocator<1>(2);
        Selector<1>* selector=new Selector<1>(2);

        for(int i=0;i<14;i++)
        {
            wires.push_back(Wire());
        }
        wires[0](mul_src_reg1,mul);
        wires[1](mul_src_reg2,mul);
        wires[2](mul,mul_cycle_1);
        wires[3](mul_cycle_1,mul_dst_reg);
        wires[4](mul_dst_reg,add);
        wires[5](add_acc_reg,add);
        wires[6](add,allocator);
        wires[7](allocator,add_dst_reg1);
        wires[8](allocator,add_dst_reg2);
        wires[9](add_dst_reg1,selector);
        wires[10](add_dst_reg2,selector);
        wires[11](selector,add_acc_reg);
        wires[12](add_dst_reg1,pes_share_reg1);
        wires[13](add_dst_reg2,pes_share_reg2);
    }
    virtual void Process()
    {
        modules[0]->Input(0,fan_in[0]);
        modules[1]->Input(0,fan_in[1]);
        EXECUTE;
        fan_out[0]= modules[5]->Output(0);
 /*     
        for(int i=0;i<5;i++)
            cout<<modules[i]->fan_in[0]<<'-'<<modules[i]->fan_out[0]<<' ';
        cout<<endl;
 */
     }
};

int main()
{  
    PE *pe=new PE;
    for(int i=1;i<15;i++)
    {
        pe->Input(0,i);
        pe->Input(1,i);
        pe->Process();
         cout<<pe->Output(0)<<endl;
    }
     
    return 0;
}