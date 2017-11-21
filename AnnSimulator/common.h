#include<cstdarg>
#include<cassert>
#include<functional>
#define IN
#define OUT
#define REG int
#define  MAKE_ROOM(a, b) \
    ops = new int[a];\
    results = new int[b];
#define EXECUTE \
    for_each(modules.begin(),modules.end(),[](AtomOperation* e){e->Process();})

enum HW_TYPE {
    UNDEFINE,
    MUL,
    BOBBLE,
    ASSIGN,
    ADD
};

using namespace  PE;
class Mul;
class Add;
class Bobble;
class Assign;

class AtomOperation {
protected:
    //results produced by Process
    int *results = nullptr;
    //operands
    int *ops = nullptr;
    //hardware type
    HW_TYPE type;

public:
    std::function<void(void)> _set_data;
    AtomOperation() {
        type = UNDEFINE;
    }

    HW_TYPE GetType() {
        return type;
    }
    int *Output() {
        return results;
    }

    void Input(int count, ...) {
        assert(ops != nullptr);
        va_list ap;
        va_start(ap, count);
        for (int i = 0; i < count; ++i)
            ops[i] = va_arg(ap, int);
        va_end(ap);
    }

    virtual void Process() = 0;
};



class _Connect {
protected:
    AtomOperation* m1;
    AtomOperation* m2;
    REG* reg;
public:
    virtual void _set() = 0;
    void operator()(AtomOperation* n1, AtomOperation* n2) {
        m1 = n1;
        m2 = n2;
        m1->_set_data = std::bind(&_Connect::_set, this);
    }
};


template <typename T1,typename T2 >
class Connect
{

};

template<>
class Connect<PE::Mul,PE::Bobble>:public _Connect{
public:
    virtual  void _set() override
    {
        m2->Input(1,m1->Output()[0]);
    }
};

template<>
class Connect<PE::Bobble,PE::Add>:public _Connect{
public:
    virtual  void _set() override
    {
        m2->Input(1,m1->Output()[0]);
    }
};

