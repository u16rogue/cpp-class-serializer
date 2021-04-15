
/*
Output:
 
Age: 21
Alive: 1

Age: 30
Alive: 0

Age: 21
Alive: 1
*
*/

#include "../include/cppcsr.hpp"
#include <iostream>

class Person
{
    cppcsr_member(int, age);
    cppcsr_member(bool, alive);
};

void print_person(Person &p)
{
    std::cout << "\n\nAge: " << p.age
              << "\nAlive: " << p.alive;
}

int main()
{
    Person p;
    p.age = 21;
    p.alive = true;
    print_person(p);

    if (!cppcsr::serialize<Person>(p, L"person"))
        return 1;

    p.age = 30;
    p.alive = false;
    print_person(p);

    if (!cppcsr::deserialize<Person>(L"person", p))
        return 1;

    print_person(p);

    return 0;
}