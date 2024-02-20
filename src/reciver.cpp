#include<stdlib.h>
#include<string.h>
#include<iostream>
#include<inc.h>
#include<data.h>
#include<cstdlib>
#include<ctime>
#include<func.h>
#include<coder.h>
#include <queue>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h> 
#include <pthread.h>
#include <mutex> 
#include <thread>
#include <condition_variable>

#define MSG_KEY 1234


#define MAX_MESSAGE_SIZE 1024


struct message_buffer {
    long message_type;
    char message_text[MAX_MESSAGE_SIZE];
};

bool keepProcessing = true;




int processedMessages = 0;
int cambio=0;


template <typename T>
class SafeQueue {
private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;

public:
    SafeQueue() = default;
    SafeQueue(SafeQueue& other) = delete;
    SafeQueue& operator=(SafeQueue& other) = delete;

    T pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        while(queue_.empty()) {
            cond_.wait(lock);
        }
        auto item = queue_.front();
        queue_.pop();
        return item;
    }

    void push(const T& item) {
        std::unique_lock<std::mutex> lock(mutex_);
        queue_.push(item);
        lock.unlock();
        cond_.notify_one();
    }

    bool empty() {
        std::unique_lock<std::mutex> lock(mutex_);
        return queue_.empty();
    }
};

SafeQueue<struct message_buffer> messageQueue;

void msgProcess(std::string message_text);
void mainMnu();
void rcvMsg();
void decodeMessage(std::string message);

void writeInfo(struct message *msg);

void processMessage() {
    while (true) {
        struct message_buffer message = messageQueue.pop(); 

        msgProcess(message.message_text);
        processedMessages++;
        cambio=1; 
    }
}

int main(){
    mainMnu();
    return 0;
}

void mainMnu(){
    int opc=0;   
    while (opc != 3)
    {
        system("clear");
        printf("\t\tMenu\n");
        printf("1. Entrar a modo escucha\n");
        printf("2. Cambiar parametros de escucha\n");
        printf("3. Salir\n");
        printf("Opcion: ");
        
        scanf("%d",&opc);
        std::thread rcvThread;
        std::thread thread1;
        std::thread thread2;
        std::thread thread3;

        switch(opc){
            case 1:
                printf("Modo escucha\n");
                rcvThread = std::thread(rcvMsg);
                thread1 = std::thread(processMessage);
                thread2 = std::thread(processMessage);
                thread3 = std::thread(processMessage);

                while(1){
                    if(cambio == 1 ){
                        printf("Mensajes procesados: %d\n", processedMessages);
                        cambio = 0;
                        sleep(3);
                    }
                    
                }

                rcvThread.join();
                thread1.join();
                thread2.join();
                thread3.join();
                break;
            case 2:
                printf("Cambiar parametros de escucha\n");
                //paralelo
                //secuencial
                break;
            case 3:
                printf("Salir\n");
                break;
            default:
                printf("Opcion no valida\n");
                break;
        }
    }
}

void rcvMsg(){
    // Attach to the message queue
    int msgid = msgget(MSG_KEY, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    struct message_buffer message;

    while(1){
        // Receive a message from the queue
        if (msgrcv(msgid, &message, sizeof(message), 1, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }

        messageQueue.push(message); 
    }
}

void msgProcess(std::string message_text){
    std::string message = base64_decode(message_text);

    decodeMessage(message);
}

void decodeMessage(std::string message){


   int totalSize = stoi(message.substr(0,3));
    int personSize = stoi(message.substr(3,3));
    int companySize = stoi(message.substr(6,3));
    int nameSize = stoi(message.substr(9,2));
    int ageSize = stoi(message.substr(11,2));
    int dniSize = stoi(message.substr(13,2));
    int adressSize = stoi(message.substr(15,2));
    int phoneSize = stoi(message.substr(17,2));
    int emailSize = stoi(message.substr(19,2));
    int jobSize = stoi(message.substr(21,2));
    int companynameSize = stoi(message.substr(23,2));
    int companyAdressSize = stoi(message.substr(25,2));
    int companyPhoneSize = stoi(message.substr(27,2));
    int companyEmailSize = stoi(message.substr(29,2));
    int companyWebSize = stoi(message.substr(31,2));
    std::string name = message.substr(33,nameSize);
    std::string age = message.substr(33+nameSize,ageSize);
    std::string dni = message.substr(33+nameSize+ageSize,dniSize);
    std::string adress = message.substr(33+nameSize+ageSize+dniSize,adressSize);
    std::string phone = message.substr(33+nameSize+ageSize+dniSize+adressSize,phoneSize);
    std::string email = message.substr(33+nameSize+ageSize+dniSize+adressSize+phoneSize,emailSize);
    std::string job = message.substr(33+nameSize+ageSize+dniSize+adressSize+phoneSize+emailSize,jobSize);
    std::string companyname = message.substr(33+nameSize+ageSize+dniSize+adressSize+phoneSize+emailSize+jobSize,companynameSize);
    std::string companyAdress = message.substr(33+nameSize+ageSize+dniSize+adressSize+phoneSize+emailSize+jobSize+companynameSize,companyAdressSize);
    std::string companyPhone = message.substr(33+nameSize+ageSize+dniSize+adressSize+phoneSize+emailSize+jobSize+companynameSize+companyAdressSize,companyPhoneSize);
    std::string companyEmail = message.substr(33+nameSize+ageSize+dniSize+adressSize+phoneSize+emailSize+jobSize+companynameSize+companyAdressSize+companyPhoneSize,companyEmailSize);
    std::string companyWeb = message.substr(33+nameSize+ageSize+dniSize+adressSize+phoneSize+emailSize+jobSize+companynameSize+companyAdressSize+companyPhoneSize+companyEmailSize,companyWebSize);
    struct message msg;
    msg.person.name = name;
    msg.person.age = age;
    msg.person.dni = dni;
    msg.person.adress = adress;
    msg.person.phone = phone;
    msg.person.email = email;
    msg.person.job = job;
    msg.company.name = companyname;
    msg.person.company = companyname;
    msg.company.adress = companyAdress;
    msg.company.phone = companyPhone;
    msg.company.email = companyEmail;
    msg.company.web = companyWeb;
    writeInfo(&msg);
    //printMessage(&msg);
}
void writeInfo(struct message *msg){

    std::string ruta="/home/phantasma/ParallelProject/log/dataLog/";
    if(msg->person.company == "google"){
        ruta+="google.data";
    }else if(msg->person.company == "facebook"){
        ruta+="facebook.data";
    }else if(msg->person.company == "microsoft"){
        ruta+="microsoft.data";
    }else if(msg->person.company == "apple"){
        ruta+="apple.data";
    }

    std::ofstream file(ruta, std::ios::app);
    if (file.is_open()) {
        file<< std::endl;
        file << "Nombre: " << msg->person.name << std::endl;
        file << "Edad: " << msg->person.age << std::endl;
        file << "DNI: " << msg->person.dni << std::endl;
        file << "Direccion: " << msg->person.adress << std::endl;
        file << "Telefono: " << msg->person.phone << std::endl;
        file << "Email: " << msg->person.email << std::endl;
        file << "Trabajo: " << msg->person.job << std::endl;
        file << "Compañia: " << msg->person.company << std::endl;
        file << "Direccion de la compañia: " << msg->company.adress << std::endl;
        file << "Telefono de la compañia: " << msg->company.phone << std::endl;
        file << "Email de la compañia: " << msg->company.email << std::endl;
        file << "Pagina web de la compañia: " << msg->company.web << std::endl;
        file << "-----------------------------------" << std::endl;
        file.close();
    } else {
        std::cerr << "Failed to open file for writing." << std::endl;
    }

}