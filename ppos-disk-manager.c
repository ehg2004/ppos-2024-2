#include <signal.h>
#include <errno.h>
#include <string.h>
#include "ppos.h"
#include "ppos-core-globals.h"
#include "disk-driver.h"
#include "ppos-disk-manager.h"

// adicione todas as variaveis globais necessarias para implementar o gerenciado do disco
disk_t disk;


void bodyDiskManager(void* arg);
void diskSignalHandler() {
    sem_down(&(disk.disk_semaphore));
    task_t* ready_task = (task_t*)queue_remove((queue_t**)&(disk.disk_task_queue), (queue_t*)disk.disk_task);
    if(ready_task != NULL) task_resume(ready_task);
    diskrequest_t* next_request = disk_scheduler(disk.disk_queue);
    if(next_request != NULL) {
        if(next_request->request_type == 1) {
            disk_cmd(DISK_CMD_READ, next_request->request_block, next_request->request_buffer);
        }
        else  disk_cmd(DISK_CMD_WRITE, next_request->request_block, next_request->request_buffer);
        disk.disk_task = next_request->request_task;
        task_suspend(taskExec, &(disk.disk_task_queue));
    }
    sem_up(&(disk.disk_semaphore));
    if(next_request != NULL) task_yield();
}

// função para o tratamento de erros dos sinais - usada em disk_mgr_init()
void clean_exit_on_sig(int sig_num) {
    printf ("\n ERROR[Signal = %d]: %d \"%s\"", sig_num, errno, strerror(errno));
    exit(errno);
}

int disk_mgr_init (int *numBlocks, int *blockSize) {
    // coloque o codigo para inicializar o disco aqui
    if(numBlocks == NULL || blockSize == NULL) return -1;
    if(disk_cmd (DISK_CMD_INIT, 0, 0) < 0) return -1;
    if(*(numBlocks) = disk_cmd (DISK_CMD_DISKSIZE, 0, 0), *(numBlocks) < 0) return -1;
    if(*(blockSize) = disk_cmd (DISK_CMD_BLOCKSIZE, 0, 0), *(blockSize) < 0) return -1;
    disk.n_blocks = (*numBlocks);
    disk.blocks_sz = (*blockSize);
    if(sem_create(&(disk.disk_semaphore), 1) < 0) return -1;
    if(sem_create(&(disk.disk_queue_semaphore), 1) < 0) return -1;
    disk.disk_queue = NULL;
    disk.disk_task = NULL;
    disk.disk_task_queue = NULL;

    signal(SIGUSR1, diskSignalHandler);
    // o seu codigo deve terminar ate aqui. 
    // As proximas linhas dessa função não devem ser modificadas
    signal(SIGSEGV, clean_exit_on_sig);

    return 0;
}

int disk_block_read(int block, void* buffer) {
    sem_down(&(disk.disk_semaphore));
    if(disk_cmd (DISK_CMD_STATUS, 0, 0) == 1) {
        disk_cmd (DISK_CMD_READ, block, buffer);
        disk.disk_task = taskExec;
    }
    else {
        diskrequest_t* request = (diskrequest_t*) malloc(sizeof(diskrequest_t));
        request->request_type = 1;
        request->request_block = block;
        request->request_buffer = buffer;
        request->request_task = taskExec;
        request->prev = NULL;
        request->next = NULL;
        sem_down(&(disk.disk_queue_semaphore));
        queue_append((queue_t**)&(disk.disk_queue), (queue_t*)request);
        sem_up(&(disk.disk_queue_semaphore));
    }
    task_suspend(taskExec, &(disk.disk_task_queue));
    sem_up(&(disk.disk_semaphore));
    task_yield();
    return 0;
}

int disk_block_write(int block, void* buffer) {
    sem_down(&(disk.disk_semaphore));
    if(disk_cmd (DISK_CMD_STATUS, 0, 0) == 1) {
        disk_cmd (DISK_CMD_WRITE, block, buffer);
        disk.disk_task = taskExec;
    }
    else {
        diskrequest_t* request = (diskrequest_t*) malloc(sizeof(diskrequest_t));
        request->request_type = 0;
        request->request_block = block;
        request->request_buffer = buffer;
        request->request_task = taskExec;
        request->prev = NULL;
        request->next = NULL;
        sem_down(&(disk.disk_queue_semaphore));
        queue_append((queue_t**)&(disk.disk_queue), (queue_t*)request);
        sem_up(&(disk.disk_queue_semaphore));
    }
    task_suspend(taskExec, &(disk.disk_task_queue));
    sem_up(&(disk.disk_semaphore));
    task_yield();
    return 0;
}


// Essa função implemeneta o escalonador de requisicoes de 
// leitura/scrita do disco usado pelo gerenciador do disco
// A função implementa a política FCFS.
diskrequest_t* disk_scheduler(diskrequest_t* queue) {
     // FCFS scheduler
    if ( queue != NULL ) {
        PPOS_PREEMPT_DISABLE
        diskrequest_t* request = queue;
        PPOS_PREEMPT_ENABLE
        return request;
    }
    return NULL;
}