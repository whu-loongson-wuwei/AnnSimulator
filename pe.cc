#include"common.h"
#include<iostream>
using namespace std;


class Register:public AtomOperation
{
    // fan_in  is value in wire which is connected to this register from others
    // fan_out is the actual value of this  register 
public:

    Register(int a,int b):AtomOperation(a,b)
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
class Mul:public Register
{
public:
        Mul():Register(1,1)
        {

        }
};
template<short cycle>
class Add:public Register
{
public:
    Add():Register(1,1)
    {

    }
};
template<>
class Mul<1>:public AtomOperation
{
public:
    Mul():AtomOperation(2,1)
    {
        type=MUL;
    }
    virtual void Process() override
    {
        fan_out[0]=fan_in[0]*fan_in[1];
    }
};
template<>
class Add<1>:public AtomOperation
{
public:
    Add():AtomOperation(2,1)
    {
        type=ADD;
    }
    virtual void Process() override
    {
        fan_out[0]=fan_in[0]+fan_in[1];
    }
};


class Bobble:public Register
{
public:
    Bobble():Register(1,1)
    {
        type=BOBBLE;
    }
};


class PE:public AtomOperation
{
private:
     Modules modules;
     Wires wires;
public:
    PE():AtomOperation(2,1)
    { 
        modules.push_back(new Register(1,1));
        modules.push_back(new Register(1,1));
        //modules.push_back(new Register(1,1));
        modules.push_back(new Mul<2>);
        modules.push_back(new Register(1,1));
        modules.push_back(new Register(1,1));
        modules.push_back(new Register(1,2));

        SET_ALIAS(modules[0],mul_src_reg1)
        SET_ALIAS(modules[1],mul_src_reg2)
        SET_ALIAS(modules[2],mul_cycle_2)
        SET_ALIAS(modules[3],mul_dst_reg)
        SET_ALIAS(modules[4],add_acc_reg)
        SET_ALIAS(modules[5],add_dst_reg)

        Mul<1>* mul=new Mul<1>;
        Add<1>* add=new Add<1>;
        for(int i=0;i<8;i++)
        {
            wires.push_back(Wire());
        }
        wires[0](mul_src_reg1,mul);
        wires[1](mul_src_reg2,mul);
        wires[2](mul,mul_cycle_2);
        wires[3](mul_cycle_2,mul_dst_reg);
        wires[4](mul_dst_reg,add);
        wires[5](add_acc_reg,add);
        wires[6](add,add_dst_reg);
        wires[7](add_dst_reg,add_acc_reg);
    }
    virtual void Process()
    {
        modules[0]->Input(0,fan_in[0]);
        modules[1]->Input(0,fan_in[1]);
        EXECUTE;
        fan_out[0]= modules[5]->Output(0);
        cout<<endl;
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
