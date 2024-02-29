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
#include <unistd.h>
#include <pthread.h>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>

#define MSG_KEY 1234

#define MAX_MESSAGE_SIZE 1024

struct message_buffer
{
    long message_type;
    char message_text[MAX_MESSAGE_SIZE];
};

// definición de variables de tiempo
std::chrono::time_point<std::chrono::system_clock> start;
std::chrono::time_point<std::chrono::system_clock> end;
std::chrono::time_point<std::chrono::system_clock> totalTime;

std::atomic<bool> keepProcessing(true);

std::atomic<int> processedMessages(0);

int cambio = 0;
int numThreads = 4;
bool processCompare = false;
std::atomic<bool> cambioTiempo(false);

template <typename T>
class SafeQueue
{
private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;
    int cont = 0;

public:
    SafeQueue() = default;
    SafeQueue(SafeQueue &other) = delete;
    SafeQueue &operator=(SafeQueue &other) = delete;
    void setCont(int c)
    {
        cont = c;
    }
    T pop()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while (queue_.empty())
        {
            if (cont == 0)
            {
                sleep(2);
                cont = 1;
                continue;
            }
            else if (cont == 1)
            {
                return T();
            }
            cond_.wait(lock);
        }
        auto item = queue_.front();
        queue_.pop();
        return item;
    }

    void push(const T &item)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        queue_.push(item);
        lock.unlock();
        cond_.notify_one();
    }

    bool empty()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        return queue_.empty();
    }
};

SafeQueue<struct message_buffer> messageQueue;

void msgProcess(std::string message_text);
void mainMnu();
void rcvMsg();
void processMessage()
{
    while (keepProcessing)
    {
        struct message_buffer message = messageQueue.pop();
        // Verifica si el mensaje recibido indica el fin del procesamiento
        if (message.message_text[0] == '\0')
        {
            keepProcessing = false;
            // printf("Fin de procesamiento\n");
            cambioTiempo = false;
            break;
        }
        cambioTiempo = true;
        msgProcess(message.message_text);
        processedMessages++;
        cambio = 1;
    }
}

void medirTiempo()
{
    int aux = 0;
    long numAuxMsgProcessed = 0;
    while (1)
    {
        if (cambioTiempo && aux == 0)
        {
            start = std::chrono::system_clock::now();
            numAuxMsgProcessed = processedMessages.load();
            aux = 1;
        }
        else if (!cambioTiempo && aux == 1)
        {
            end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end - start;
            numAuxMsgProcessed = processedMessages.load() - numAuxMsgProcessed;
            if (!processCompare)
            {
                printf("Se proceso un total de %ld mensajes en %f segundos usando %d hilos\n", numAuxMsgProcessed, elapsed_seconds.count(), numThreads);
            }
            aux = 0;
            cambioTiempo = false;
        }
    }
}
void processComp(std::atomic<int> &processNum)
{

    while (processNum.load() < 50000)
    {
        struct message_buffer message = messageQueue.pop();
        // Verifica si el mensaje recibido indica el fin del procesamiento
        if (message.message_text[0] == '\0')
        {
            keepProcessing = false;
            cambioTiempo = false;

            break;
        }
        cambioTiempo = true;
        msgProcess(message.message_text);
        processNum++;
        cambio = 1;
    }
}
void comparar()
{
    processCompare = true;
    printf("Para realizar la comparativa se procesaran 100000 mensajes\n");
    std::atomic<int> mensajeInicial(0);

    // Procesamiento secuencial
    auto tstart = std::chrono::high_resolution_clock::now();
    processComp(mensajeInicial);
    auto tend = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = tend - tstart;
    printf("Se procesaron 50000 mensajes de forma secuencial en %f segundos\n", elapsed_seconds.count());

    // Procesamiento paralelo
    mensajeInicial = 0;
    auto tstartPara = std::chrono::high_resolution_clock::now();
    std::thread th2(processComp, std::ref(mensajeInicial));
    std::thread th3(processComp, std::ref(mensajeInicial));
    std::thread th4(processComp, std::ref(mensajeInicial));
    th2.join();
    th3.join();
    th4.join();
    auto tendPara = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_secondsPara = tendPara - tstartPara - std::chrono::seconds(2);

    double speedup = elapsed_seconds.count() / elapsed_secondsPara.count();
    
    double efficiency = speedup / 3;

    // calculo de la eficiencia
    printf("Se procesaron 50000 mensajes de forma paralela con 3 hilos en %f segundos\n", elapsed_secondsPara.count());

    printf("La eficiencia es: %f\n", efficiency);
    // calculo del speedup
    printf("El speedup es: %f\n", speedup);

    printf("Comparación completada.\n");
    getchar();
    getchar();
    processCompare = false;
}

int main()
{

    std::thread rcvThread;
    std::thread tiempoThread;
    tiempoThread = std::thread(medirTiempo);
    rcvThread = std::thread(rcvMsg);
    writeLog("Iniciando el sistema");
    writeLog("Hilo de recepcion de mensajes iniciado, inicia secuencia");
    rcvThread.detach();
    tiempoThread.detach();
    writeLog("Hilo de recepcion de mensajes desvinculado");
    mainMnu();
    return 0;
}

void mainMnu()
{
    int opc = 1;
    while (opc != 0)
    {
        keepProcessing = true;
        messageQueue.setCont(0);
        system("clear");
        printf("\t\tMenu\n");
        printf("1. Entrar a modo escucha\n");
        printf("2. Cambiar parametros de escucha\n");
        printf("3. Comparativa secuencial y concurrente\n");
        printf("0. Salir\n");
        printf("Opcion: ");

        scanf("%d", &opc);
        std::thread Athread[numThreads];

        switch (opc)
        {
        case 1:
            printf("Modo escucha con %d hilos\n", numThreads);
            for (int i = 0; i < numThreads; i++)
            {
                Athread[i] = std::thread(processMessage);
            }

            while (keepProcessing)
            {
                if (cambio == 1)
                {
                    system("clear");
                    printf("Mensajes procesados: %d\n", processedMessages.load());
                    cambio = 0;
                }
            }
            for (int i = 0; i < numThreads; i++)
            {
                Athread[i].join();
            }
            printf("Fin de procesamiento\n");
            getchar();
            getchar();
            break;
        case 2:
            printf("Cambiar parametros de escucha\n");
            printf("Numero de hilos: ");
            scanf("%d", &numThreads);
            // paralelo
            // secuencial
            break;
        case 3:
            comparar();
            break;
        case 0:
            printf("Salir\n");
            break;
        default:
            printf("Opcion no valida\n");
            break;
        }
    }
}

void rcvMsg()
{
    // Attach to the message queue
    int msgid = msgget(MSG_KEY, 0666 | IPC_CREAT);
    if (msgid == -1)
    {
        perror("msgget");
        exit(1);
    }

    struct message_buffer message;

    while (1)
    {
        // Receive a message from the queue
        if (msgrcv(msgid, &message, sizeof(message), 1, 0) == -1)
        {
            perror("msgrcv");
            exit(1);
        }

        messageQueue.push(message);
    }
}

void msgProcess(std::string message_text)
{
    std::string message = base64_decode(message_text);

    decodeMessage(message);
}
