/*
Output:
	

Health: 100
Pets:   3
Alive:  1
Age:    30

Health: 100
Pets:   3
Alive:  1
Age:    30

Health:   100
Pets:     3
Alive:    1
Age:      30
Families: 8

Health:   100
Pets:     3
Alive:    0
Age:      30
Families: 10

Health:   100
Pets:     3
Alive:    0
Age:      30
Families: 10

*/

#include <iostream>
#include "../include/cppcsr.hpp"

#pragma pack(push, 1)
class Person
{
    cppcsr_member(int, health);
    cppcsr_member(int, pets);
    cppcsr_member(bool, alive);
    cppcsr_member(int, age);
};
#pragma pack(pop)

#pragma pack(push, 1)
class Person2
{
    cppcsr_member(bool, alive);
    cppcsr_member(int, health);
    cppcsr_member(int, families);
    cppcsr_member(int, pets);
    cppcsr_member(int, age);
};
#pragma pack(pop)

int main()
{
    Person person_a;

    person_a.health = 100;
    person_a.pets = 3;
    person_a.alive = true;
    person_a.age = 30;

    std::cout << "\nHealth: " << person_a.health
              << "\nPets:   " << person_a.pets
              << "\nAlive:  " << person_a.alive
              << "\nAge:    " << person_a.age
              << "\n";

    cppcsr_declare_std_array(Person) buffer = {};
    cppcsr::serialize<Person>(person_a, buffer);

    Person person_b;
    cppcsr::deserialize<Person>(buffer.data(), person_b);
    std::cout << "\nHealth: " << person_a.health
              << "\nPets:   " << person_a.pets
              << "\nAlive:  " << person_a.alive
              << "\nAge:    " << person_a.age
              << "\n";


    Person2 person2_a;
    person2_a.families = 8;
    cppcsr::deserialize<Person2>(buffer.data(), person2_a);
    std::cout << "\nHealth:   " << person2_a.health
              << "\nPets:     " << person2_a.pets
              << "\nAlive:    " << person2_a.alive
              << "\nAge:      " << person2_a.age
              << "\nFamilies: " << person2_a.families
              << "\n";

    cppcsr_declare_std_array(Person2) buffer2 = {};
    cppcsr::serialize<Person2>(person2_a, buffer2);

    person2_a.families = 10;
    person2_a.alive = false;

    std::cout << "\nHealth:   " << person2_a.health
              << "\nPets:     " << person2_a.pets
              << "\nAlive:    " << person2_a.alive
              << "\nAge:      " << person2_a.age
              << "\nFamilies: " << person2_a.families
              << "\n";

    Person2 person2_b;
    cppcsr::deserialize<Person2>(buffer2.data(), person2_b);
    std::cout << "\nHealth:   " << person2_a.health
              << "\nPets:     " << person2_a.pets
              << "\nAlive:    " << person2_a.alive
              << "\nAge:      " << person2_a.age
              << "\nFamilies: " << person2_a.families
              << "\n";

    return 0;
}
