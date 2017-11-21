#include<iostream>
#include<queue>
#include<algorithm>
#include<vector>
#include<cstdio>
#include<cstdlib>
#include<string.h>
#include<stdarg.h>
using namespace std;
#define IN
#define OUT
#define  REG int

enum HW_TYPE
{
    UNDEFINE,
    MUL,
    BOBBLE,
    ASSIGN,
    ADD
};
template<typename T1,typename T2>
class Connect
{
public:
    void operator()(T1 m1,T2 m2)
    {

    }
};

template<typename T2>
class Connect<REG>
{
public:
    void operator()(REG m1,T2 m2)
    {

    }
};

template<>
class Connect<REG,REG>
{
public:
    void operator()(REG m1,T2 m2)
    {

    }
};
class Event{
protected:
    int *results;
    int  result;
    HW_TYPE type;
public:
    Event()
    {
        type=UNDEFINE;
    }
    HW_TYPE GetType()
    {
        return type;
    }
    int GetResult()
    {
        return result;
    }
    int *GetResults()
    {
        return results;
    }
    virtual void Input(int count,...)
    {

    }
    virtual void Process()
    {

    }
};
class Mul:public Event{
private:
    int op1;
    int op2;
    
public: 
    Mul()
    {
        type = MUL;
    }
    void Input(int count ,...)
    {
        va_list ap;
        va_start(ap,count);
        op1 = va_arg(ap,int);
        op2 = va_arg(ap,int);
        va_end(ap);
    } 
   
    void Process()
    {
        result = op1 * op2;
    }
};
class Bobble:public Event{
public:
    Bobble()
    {
        type = BOBBLE;
    }
    void Input(int count ,...)
    {
        va_list ap;
        va_start(ap,count);
        result = va_arg(ap,int);
        va_end(ap);
    } 
};
class Add:public Event{
private:
    int op1;
    int op2;
public:
    Add()
    {
        type = ADD;
    }
    void Input(int count ,...)
    {
        va_list ap;
        va_start(ap,count);
        op1 = va_arg(ap,int);
        op2 = va_arg(ap,int);
        va_end(ap);
    } 
    void Process(){result= op1+op2;}
};
class Assign:public Event{
private:
    int op1;
    int op2;
    int op3;
public:
    Assign()
    {
        type = ASSIGN;
    }
    void Input(int count,...)
    {
        va_list ap;
        va_start(ap,count);
        op1 = va_arg(ap,int);
        op2 = va_arg(ap,int);
        op3 = va_arg(ap,int);
        va_end(ap);
    } 
    void Process()
    {   
        results=new int[3];
        results[0]=op1;
        results[1]=op2;
        results[2]=op3;
    }
};
class PE{
public:
    std::vector<Event*> modules;
    IN REG op_mul_1;
    IN REG op_mul_2;
    REG op_add;
    REG temporary;
    OUT queue<REG>op_dest;
    static const REG const0=0;
    static const REG const1=1;
    static const REG const2=2;
    static const REG const_1=-1; 
    int list[8];
public:
    PE(){
        op_mul_1=0;
        op_mul_2=0;
        op_add=0;
        memset(list,0,sizeof(list));
        modules.push_back(new Assign);
        modules.push_back(new Mul);
        modules.push_back(new Bobble);
        modules.push_back(new Add);  
        for(int i=0;i<2;i++)
            op_dest.push(0);
    }
    ~PE(){}
    void pipeline(int buffer_a,int buff_b);
};

#define ASSIGN_OP_1 list[0]
#define ASSIGN_OP_2 list[1]
#define ASSIGN_OP_3 list[2]
#define MUL_OP_1 list[3]
#define MUL_OP_2 list[4]
#define MUL2_OP list[5]
#define ADD_OP_1 list[6]
#define ADD_OP_2 list[7]

#define EXECUTE \
    for_each(modules.begin(),modules.end(),[](Event* e){e->Process();})
void PE::pipeline(int buffer_a,int buffer_b)
{
    ASSIGN_OP_1=buffer_a;
    ASSIGN_OP_2=buffer_b;
    ASSIGN_OP_3=op_dest.front();
    op_dest.pop();
    
    modules[0]->Input(3,ASSIGN_OP_1,ASSIGN_OP_2,ASSIGN_OP_3);
    modules[1]->Input(2,MUL_OP_1,MUL_OP_2);
    modules[2]->Input(1,MUL2_OP);
    modules[3]->Input(2,ADD_OP_1,ADD_OP_2);


    EXECUTE;

    MUL_OP_1=modules[0]->GetResults()[0];
    MUL_OP_2=modules[0]->GetResults()[1];
    MUL2_OP= modules[1]->GetResult();
    ADD_OP_1=modules[2]->GetResult();
    ADD_OP_2=modules[0]->GetResults()[2];


    op_dest.push(modules[3]->GetResult());
    
    //ASSIGN_OP_3=op_dest.front();
   
    op_mul_1=MUL_OP_1;//a
    op_mul_2=MUL_OP_2;//b
    op_add=ADD_OP_2;//c
   
    for(int i=2;i<8;i++)
        cout<<list[i]<<' ';
    cout<<endl;
//    cout<<op_mul_1<<' '<<op_mul_2<<' '<<op_add<<' '<<op_dest.front()<<endl;
}
int main()
{
    PE * pe = new PE;
    int buffer1[15],buffer2[15];
   for(int i=0;i<15;i++)
   {
       buffer1[i]=1;
       buffer2[i]=1;
   }
    for(int i=0;i<15;i++)
    {
/*       
        cout<<pe->op_mul_1<<"a"<<endl;
        cout<<pe->op_mul_2<<"b"<<endl;
        cout<<pe->op_add<<"c"<<endl;
        cout<<pe->op_dest.front()<<"d"<<endl;
*/
        pe->pipeline(buffer1[i],buffer2[i]);
    }
    cout<<pe->op_dest.front()<<endl;
    pe->op_dest.pop();
    cout<<pe->op_dest.front()<<endl;
    pe->op_dest.pop();
    cout<<pe->op_dest.front()<<endl;
}