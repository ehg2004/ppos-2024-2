// PingPongOS - PingPong Operating System
// Prof. Carlos A. Maziero, DINF UFPR
// Versão 1.1 -- Julho de 2016
//Alunos Enzo Gaio e Thais Say

// Estruturas de dados internas do sistema operacional

#ifndef __PPOS_DATA__
#define __PPOS_DATA__

#include <stdio.h>
#include <ucontext.h>		// biblioteca POSIX de trocas de contexto
#include "queue.h"		// biblioteca de filas genéricas

// Estrutura que define um Task Control Block (TCB)
typedef struct task_t
{
   struct task_t *prev, *next ;		// ponteiros para usar em filas
   int id ;				// identificador da tarefa
   ucontext_t context ;			// contexto armazenado da tarefa
   unsigned char state;  // indica o estado de uma tarefa (ver defines no final do arquivo ppos.h): 
                          // n - nova, r - pronta, x - executando, s - suspensa, e - terminada
   struct task_t* queue;
   struct task_t* joinQueue;
   int exitCode;
   unsigned int awakeTime; // used to store the time when it should be waked up

   void* custom_data; // internal data - do not modify!

   // ... (outros/novos campos deve ser adicionados APOS esse comentario)


} task_t ;

// estrutura que define um mutex
typedef struct {
    int mtx_id;
    int mtx_locked;
    
} mutex_t ;

// estrutura que define um semáforo
typedef struct {
    int sem_state;// counter of available spots
    int sem_id;//semaphore id
    int sem_q_counter;//not used but when commented returns se fault
    queue_t** sem_queue;//not used but when commented returns se fault
    queue_t** sem_queue_ready;//not used but when commented returns se fault
    queue_t* removed_element;//not used but when commented returns se fault
    task_t**   sem_t_queue;//tasks queue
    unsigned char active;// 'n' if the semaphore is already destroyed

} semaphore_t ;

// estrutura que define uma barreira
typedef struct {
    struct task_t *queue;
    int maxTasks;
    int countTasks;
    unsigned char active;
    mutex_t mutex;
} barrier_t ;

// estrutura que define uma fila de mensagens
typedef struct {
    void* content;
    int messageSize;
    int maxMessages;
    int countMessages;
    
    semaphore_t sBuffer;
    semaphore_t sItem;
    semaphore_t sVaga;
    
    unsigned char active;
} mqueue_t ;

#endif

