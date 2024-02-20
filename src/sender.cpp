#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <inc.h>
#include <data.h>
#include <cstdlib>
#include <ctime>
#include <func.h>
#include <coder.h>
#include <queue>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <thread>
#include <mutex>


#define MSG_KEY 1234


#define MAX_MESSAGE_SIZE 1024


struct message_buffer {
    long message_type;
    char message_text[MAX_MESSAGE_SIZE];
};

std::mutex mtx; 

void mainMnu(struct message *msg);
void lanzarMsg(struct message *msg);


int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    struct message msg;
    mainMnu(&msg);

    return 0;
}


void lanzarMsg(struct message *msg) {

    int msgid = msgget(MSG_KEY, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }
    
    struct message_buffer message;
    
    mtx.lock(); // Lock the control
    writeLog("Lanzando 20000 mensaje");
for (int i = 0; i < 100000; i++) {
    
    createMessageStruct(msg);
    std::string message_text = makeMessage(msg);
    message_text = base64_encode(message_text);


    message.message_type = 1;
    strncpy(message.message_text, message_text.c_str(), sizeof(message.message_text)-1);


    if (msgsnd(msgid, &message, sizeof(message), 0) == -1) {
        perror("msgsnd");
        exit(1);
    }


    message_text.clear();
    memset(&message, 0, sizeof(message));
}
    mtx.unlock(); 
}

void mainMnu(struct message *msg) {
    std::string message = makeMessage(msg);
    message = base64_encode(message);

    int expression = 0;
    while (expression != 3) {
        system("clear");
        printf("\033c");
        printf("\n\n\t\tMenu del proceso lanzador\n\n");
        printf("\t1.- Ver contenido ejemplo de mensaje\n");
        printf("\t2.- Lanzar mensajes en paralelo\n");
        printf("\t3.- Salir\n");
        printf("\tOpcion: ");
        scanf("%d", &expression);
        std::thread t;
        switch (expression) {
            case 1:
                createMessageStruct(msg);
                printMessage(msg);
                break;
            case 2:
                // Lanzar contenido
                
                writeLog("Lanzando mensaje");
                t=std::thread(lanzarMsg, msg); 
                t.join(); 
                break;
            case 3:
                break;
            default:
                printf("\n\tOpcion no valida");
                getchar();
                getchar();
                break;
        }
    }
}