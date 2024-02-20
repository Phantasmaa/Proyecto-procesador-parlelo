#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<iostream>
#ifndef INC_H
#define INC_H


struct person{
    std::string name;
    std::string age;
    std::string dni;
    std::string adress;
    std::string phone;
    std::string email;
    std::string job;
    std::string company;
};


struct company{
    std::string name;
    std::string adress;
    std::string phone;
    std::string email;
    std::string web;
};

struct message{
    std::string header;
    struct person person;
    struct company company;
};

#endif // INC_H