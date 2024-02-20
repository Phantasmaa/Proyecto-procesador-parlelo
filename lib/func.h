#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<iostream>
#include<data.h>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <ctime>

void dataHeadComplete(struct message *msg);

std:: string completeField(int pos);

void createMessageStruct(struct message *message);

struct company completeCompany(std::string companyname);

int getRandomNumber(int min, int max);

std::string saveData(int xlvar, int value);

void printMessage(struct message *msg);

std::string makeMessage(struct message *msg);

void createMessageStruct(struct message *message){
    message->person.name = completeField(1);
    message->person.age = completeField(2);
    message->person.dni = completeField(3);
    message->person.adress = completeField(4);
    message->person.phone = completeField(5);
    message->person.email = completeField(6);
    message->person.job = completeField(7);
    message->person.company = completeField(8);
    message->company = completeCompany(message->person.company);
    dataHeadComplete(message);
}

int getRandomNumber(int min, int max) {
    static const double fraction = 1.0 / (RAND_MAX + 1.0);
    return min + static_cast<int>((max - min + 1) * (std::rand() * fraction));
}

std:: string completeField(int pos){
    
    int randomNum;
    int arraySize;
    std::string aux;
    switch (pos)
    {
    case 1://name
        arraySize = sizeof(names) / sizeof(names[0]);
        randomNum = getRandomNumber(0, arraySize - 1);
        aux = names[randomNum];
        arraySize = sizeof(surnames) / sizeof(surnames[0]);
        randomNum = getRandomNumber(0, arraySize - 1);
        aux += "|"+surnames[randomNum];
        break;
    case 2://age
        arraySize = sizeof(ages) / sizeof(ages[0]);
        randomNum = getRandomNumber(0, arraySize - 1);
        aux = ages[randomNum];
        break;
    case 3://dni
        arraySize = sizeof(dnis) / sizeof(dnis[0]);
        randomNum = getRandomNumber(0, arraySize - 1);
        aux = dnis[randomNum];
        break;
    case 4://adress
        arraySize = sizeof(adresses) / sizeof(adresses[0]);
        randomNum = getRandomNumber(0, arraySize - 1);
        aux = adresses[randomNum];        
        break;
    case 5://phone
        arraySize = sizeof(phones) / sizeof(phones[0]);
        randomNum = getRandomNumber(0, arraySize - 1);
        aux = phones[randomNum];
        break;
    case 6://email
        arraySize = sizeof(emails) / sizeof(emails[0]);
        randomNum = getRandomNumber(0, arraySize - 1);
        aux = emails[randomNum];
        break;
    case 7://job
        arraySize = sizeof(jobs) / sizeof(jobs[0]);
        randomNum = getRandomNumber(0, arraySize - 1);
        aux = jobs[randomNum];
        break;
    case 8://company
        arraySize = sizeof(companys) / sizeof(companys[0]);
        randomNum = getRandomNumber(0, arraySize - 1);
        aux = companys[randomNum];
        break;
    default://error
        exit(1);
        break;
    }
    return aux;
}
struct company completeCompany(std::string companyname){
    if (companyname == "google") {
        return com[0];
    } else if (companyname == "facebook") {
        return com[1];
    } else if (companyname == "microsoft") {
        return com[2];
    } else if (companyname == "apple") {
        return com[3];
    } else {
        exit(1);
    }
}
void dataHeadComplete(struct message *msg){
    int totalSize = 0;
    int personSize = 0;
    int companySize = 0;
    int nameSize = msg->person.name.size();
    int ageSize = msg->person.age.size();
    int dniSize = msg->person.dni.size();
    int adressSize = msg->person.adress.size();
    int phoneSize = msg->person.phone.size();
    int emailSize = msg->person.email.size();
    int jobSize = msg->person.job.size();
    int companynameSize = msg->person.company.size();
    int companyAdressSize = msg->company.adress.size();
    int companyPhoneSize = msg->company.phone.size();
    int companyEmailSize = msg->company.email.size();
    int companyWebSize = msg->company.web.size();
    personSize = nameSize + ageSize + dniSize + adressSize + phoneSize + emailSize + jobSize + companynameSize;
    companySize = companynameSize + companyAdressSize + companyPhoneSize + companyEmailSize + companyWebSize;
    totalSize = personSize + companySize;
    msg->header =saveData(3, totalSize)+saveData(3, personSize)+saveData(3, companySize)+saveData(2, nameSize)+saveData(2, ageSize)+saveData(2, dniSize)+saveData(2, adressSize)+saveData(2, phoneSize)+saveData(2, emailSize)+saveData(2, jobSize)+saveData(2, companynameSize)+saveData(2, companyAdressSize)+saveData(2, companyPhoneSize)+saveData(2, companyEmailSize)+saveData(2, companyWebSize);
}
std::string saveData(int xlvar, int value){
    std::string aux;
    if(xlvar == 2 ){
        if(value < 10){
            aux = "0"+std::to_string(value);
        }else{
            aux = std::to_string(value);
        }
    }else{
        if(value < 10){
            aux = "00"+std::to_string(value);
        }else if(value < 100){
            aux = "0"+std::to_string(value);
        }else{
            aux = std::to_string(value);
        }
    }
    return aux;
}
void printMessage(struct message *msg){
    system("clear");
    std::cout <<"\n\n\t"<< "Message: " << std::endl;
    std::cout <<"\t"<< "Header: " << msg->header << std::endl;
    std::cout <<"\t"<< "Name: " << msg->person.name << std::endl;
    std::cout <<"\t"<< "Age: " << msg->person.age << std::endl;
    std::cout <<"\t"<< "Dni: " << msg->person.dni << std::endl;
    std::cout <<"\t"<< "Adress: " << msg->person.adress << std::endl;
    std::cout <<"\t"<< "Phone: " << msg->person.phone << std::endl;
    std::cout <<"\t"<< "Email: " << msg->person.email << std::endl;
    std::cout <<"\t"<< "Job: " << msg->person.job << std::endl;
    std::cout <<"\t"<< "Company: " << msg->person.company << std::endl;
    std::cout <<"\t"<< "Company name: " << msg->company.name << std::endl;
    std::cout <<"\t"<< "Company adress: " << msg->company.adress << std::endl;
    std::cout <<"\t"<< "Company phone: " << msg->company.phone << std::endl;
    std::cout <<"\t"<< "Company email: " << msg->company.email << std::endl;
    std::cout <<"\t"<< "Company web: " << msg->company.web << std::endl;
    std::cout <<"\n\n\t"<< "Press enter to continue" << std::endl;

    }
std::string makeMessage(struct message *msg){
    std::string aux;
    aux = msg->header + msg->person.name + msg->person.age + msg->person.dni + msg->person.adress + msg->person.phone + msg->person.email + msg->person.job + msg->person.company + msg->company.adress + msg->company.phone + msg->company.email + msg->company.web;
    return aux;
}

void writeLog(const std::string& message) {
    std::time_t now = std::time(nullptr);
    std::string timestamp = std::asctime(std::localtime(&now));
    timestamp.pop_back(); // Remove newline character

    std::ofstream file("/home/phantasma/ParallelProject/log/info.log", std::ios::app);
    if (file.is_open()) {
        file << timestamp << " - " << message << std::endl;
        file.close();
    } else {
        std::cerr << "Failed to open file for writing." << std::endl;
    }
}
