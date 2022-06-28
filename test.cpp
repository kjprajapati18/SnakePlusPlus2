#include <iostream>
#include <thread>

class Object{
    public:
        Object(int a) : num(a) {
            std::cout<< "Object created\n";
        }

        Object(const Object& other) : num(other.num) {
            std::cout << "Copy constructor called\n";
        }
        Object(Object&& obj) 
            : num{ obj.num }
        {
            // Nulling out the pointer to the temporary data
            obj.num = 0;
            std::cout << "Move constructor called\n";
        }

        Object& operator=(Object&& other){
            this->num = other.num;
            other.num = 0;
            std::cout << "Move Assignment operator called\n";
            return *this;
        }

        Object& operator=(const Object& other){
            this->num = other.num;
            std::cout<< "Copy Assignment operator called\n";
            return *this;
        }

        ~Object(){
            std::cout<< "Destroyed\n";
        }

        int getNum(){
            return num;
        }
    private:
        int num;
};

void fun(Object b, Object& a){
    // delete b;
    std::cout<<"Begin fun\n";
    int by = 23;
    // std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    std::cout<< "Value = " << b.getNum() << "\n";
    std::cout<< "Value = " << a.getNum() << "\n";
    std::cout<<"End fun\n";
}

int main() {

    // std::thread(fun, Object(7)).detach();
    // std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    Object a(2);
    fun(std::move(a), a);

    std::cout<< "end main\n";
    int x;
    std::cin >> x;
    return 0;
}