#include <signal.h>
#include <errno.h>
#include <string.h>
#include "ppos.h"
#include "ppos-core-globals.h"
#include "disk-driver.h"
#include "ppos-disk-manager.h"

// adicione todas as variaveis globais necessarias para implementar o gerenciado do disco
#define FCFS 0
#define CSCAN 1
#define SSTF 2
int scheduler_algorithm = FCFS;
disk_t disk;
int position;
int total_distance;
task_t* disk_mgr_task;


diskrequest_t* disk_schedulerSSTF(diskrequest_t* queue);
diskrequest_t* disk_schedulerCSCAN(diskrequest_t* queue);
diskrequest_t* disk_schedulerFCFS(diskrequest_t* queue);

void bodyDiskManager(void* arg)  {

}
void diskSignalHandler() {
    sem_down(&(disk.disk_semaphore));
    task_t* ready_task = (task_t*)queue_remove((queue_t**)&(disk.disk_task_queue), (queue_t*)disk.disk_task);
    if(ready_task != NULL) task_resume(ready_task);
    diskrequest_t* next_request;
    if(scheduler_algorithm == FCFS)
        next_request = disk_schedulerFCFS(disk.disk_queue);
    else if(scheduler_algorithm == CSCAN)
        next_request = disk_schedulerCSCAN(disk.disk_queue);
    else if(scheduler_algorithm == SSTF)
        next_request = disk_schedulerSSTF(disk.disk_queue);
    if(next_request != NULL) {
        if(next_request->request_type == 1) {
            disk_cmd(DISK_CMD_READ, next_request->request_block, next_request->request_buffer);
        }
        else  disk_cmd(DISK_CMD_WRITE, next_request->request_block, next_request->request_buffer);
        total_distance += (next_request->request_block >= position)?(next_request->request_block - position):(position - next_request->request_block );
        position = next_request->request_block;
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
    
    disk.disk_queue = NULL;//filas vazias
    disk.disk_task = NULL;
    disk.disk_task_queue = NULL;

    disk_mgr_task = (task_t*) malloc(sizeof(task_t));
    task_create(disk_mgr_task, bodyDiskManager, NULL);	
    
    position = 0;
    total_distance = 0;
    signal(SIGUSR1, diskSignalHandler);
    // o seu codigo deve terminar ate aqui. 
    // As proximas linhas dessa função não devem ser modificadas
    signal(SIGSEGV, clean_exit_on_sig);

    return 0;
}

int disk_block_read(int block, void* buffer) {
    diskrequest_t* request = (diskrequest_t*) malloc(sizeof(diskrequest_t));
    request->request_type = 1;
    request->request_block = block;
    request->request_buffer = buffer;
    request->request_task = taskExec;
    request->prev = request;
    request->next = request;
    sem_down(&(disk.disk_semaphore));
    queue_append((queue_t**)&(disk.disk_queue), (queue_t*)request);
    sem_up(&(disk.disk_semaphore));

    task_suspend(taskExec, &(disk.disk_task_queue));
    task_resume(disk_mgr_task);

    task_yield();
    return 0;
}

int disk_block_write(int block, void* buffer) {
    diskrequest_t* request = (diskrequest_t*) malloc(sizeof(diskrequest_t));
    request->request_type = 0;
    request->request_block = block;
    request->request_buffer = buffer;
    request->request_task = taskExec;
    request->prev = request;
    request->next = request;
    sem_down(&(disk.disk_semaphore));
    queue_append((queue_t**)&(disk.disk_queue), (queue_t*)request);
    sem_up(&(disk.disk_semaphore));

    task_suspend(taskExec, &(disk.disk_task_queue));
    task_resume(disk_mgr_task);

    task_yield();
    return 0;
}


// Essa função implemeneta o escalonador de requisicoes de 
// leitura/scrita do disco usado pelo gerenciador do disco
// A função implementa a política FCFS.
diskrequest_t* disk_schedulerFCFS(diskrequest_t* queue) {
     // FCFS scheduler
    if ( queue != NULL ) {
        PPOS_PREEMPT_DISABLE
        diskrequest_t* request = queue;
        PPOS_PREEMPT_ENABLE
        return request;
    }
    return NULL;
}
diskrequest_t* disk_schedulerCSCAN(diskrequest_t* queue) {
    // CSCAN scheduler
   if ( queue != NULL ) {
       PPOS_PREEMPT_DISABLE
       diskrequest_t* request = queue;
       int best_block = request->request_block;
       diskrequest_t* r = queue->next;
       while(queue != r) {
            if(r->request_block < best_block) {
                best_block = r->request_block;
                request = queue;
            }
            r = r->next;
       }
       PPOS_PREEMPT_ENABLE
       return request;
   }
   return NULL;
}
diskrequest_t* disk_schedulerSSTF(diskrequest_t* queue) {
    //SSTF scheduler
   if ( queue != NULL ) {
       PPOS_PREEMPT_DISABLE
       diskrequest_t* request = queue;
       int best_dist = (position >= request->request_block) ? (position - request->request_block):(request->request_block - position);
       diskrequest_t* r = queue->next;
       while(r != queue) {
            int dist = (position >= r->request_block) ? (position - r->request_block):(r->request_block - position);
            if(dist < best_dist) {
                best_dist = dist;
                request = queue;
            }
            r = r->next;
       }
       PPOS_PREEMPT_ENABLE
       return request;
   }
   return NULL;
}