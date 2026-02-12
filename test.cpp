
#include "./includes/tokenizer.hpp"
#include "./includes/parser.hpp"
#include "./includes/config.hpp"
#include <map>
using namespace std;


#include <ctime>
#include <iostream>
class A;

class Bose {
    A *Childs[10];
    public:
    void setChild(A *c) {
        Childs[0] = c;
    }
    void removeChild(){
        delete Childs[0];
    }
};

class A {
    char *buffer;
    Bose *ptr;
public:
    A() {
        buffer = new char[10];
    }
    ~A() {
        delete[] buffer;
    }
    void TryRemove() {
        cout << "hna 1" << endl;
        this->ptr->removeChild();
        cout << "hna 2" << endl;
    }
};

int main()
{
    A *a = new A();
    Bose b;
    b.setChild(a);
    a->TryRemove();
}