#include "ppos.h"
#include "ppos-core-globals.h"
#include "ppos-disk-manager.h"
#include <stdatomic.h>

// ****************************************************************************
// Coloque as suas modificações aqui, 
// p.ex. includes, defines variáveis, // estruturas e funções
#ifdef DEBUG
#undef DEBUG
#endif

//  #define DEBUG 1 //debugar inicializações



void before_ppos_init () {
    // put your customization here
#ifdef DEBUG
    printf("\ninit - BEFORE");
#endif
}

void after_ppos_init () {
    // put your customization here
#ifdef DEBUG
    printf("\ninit - AFTER");
#endif
    
}

void before_task_create (task_t *task ) {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_create - BEFORE - [%d]", task->id);
#endif
}

void after_task_create (task_t *task ) {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_create - AFTER - [%d]", task->id);
#endif
    
}

void before_task_exit () {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_exit - BEFORE - [%d]", taskExec->id);
#endif
}

void after_task_exit () {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_exit - AFTER- [%d]", taskExec->id);
#endif
    
}

void before_task_switch ( task_t *task ) {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_switch - BEFORE - [%d -> %d]", taskExec->id, task->id);
#endif
    
}

void after_task_switch ( task_t *task ) {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_switch - AFTER - [%d -> %d]", taskExec->id, task->id);
#endif
}

void before_task_yield () {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_yield - BEFORE - [%d]", taskExec->id);
#endif
}
void after_task_yield () {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_yield - AFTER - [%d]", taskExec->id);
#endif
}


void before_task_suspend( task_t *task ) {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_suspend - BEFORE - [%d]", task->id);
#endif
}

void after_task_suspend( task_t *task ) {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_suspend - AFTER - [%d]", task->id);
#endif
}

void before_task_resume(task_t *task) {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_resume - BEFORE - [%d]", task->id);
#endif
}

void after_task_resume(task_t *task) {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_resume - AFTER - [%d]", task->id);
#endif
}

void before_task_sleep () {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_sleep - BEFORE - [%d]", taskExec->id);
#endif
}

void after_task_sleep () {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_sleep - AFTER - [%d]", taskExec->id);
#endif
}

int before_task_join (task_t *task) {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_join - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_task_join (task_t *task) {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_join - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}


int before_sem_create (semaphore_t *s, int value) {
    // put your customization here
#ifdef DEBUG
    printf("\nsem_create - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

//REMEBER:
// semaphore_t* sem = (semaphore_t*) malloc(sizeof(semaphore_t))
// sucesso = sem_create (sem, valor) 
//print (" retorno: %d", &sucesso)

int sem_create (semaphore_t *s, int value) {
    before_sem_create(s,value);

    if(s==NULL){
        return -1;
    }

    s->active=0;

    s->sem_id=1;

    s->sem_state=value;

    // s->removed_element=NULL;

    // s->sem_mutex = (mutex_t*) malloc(sizeof(mutex_t));
    
    // if(s->sem_mutex==NULL)
    //     return -1;

    
    int sucesso = mutex_create(&(s->sem_mutex));
       
    if (sucesso!=0)
        return -1;

    // s->sem_queue= (queue_t**) malloc(sizeof(queue_t*));
    // // if(s->sem_queue==NULL)
    // //     return -1;
    // *(s->sem_queue)=NULL;//empty queue

    // s->sem_queue_ready= (queue_t**) malloc(sizeof(queue_t*));

    // *(s->sem_queue_ready)=NULL;//empty queue

    // s->sem_queue_mutex = (mutex_t*) malloc(sizeof(mutex_t));
    
    // if(s->sem_queue_mutex==NULL)
    //     return -1;

    s->sem_t_queue = (task_t**) malloc(sizeof(task_t*));
    // if(s->sem_queue==NULL)
    //     return -1;
    *(s->sem_t_queue)=NULL;//empty queue
    // sem_t_queue


    // sucesso = mutex_create(&(s->sem_queue_mutex));
       
    if (sucesso!=0)
        return -1;
    after_sem_create(s,value);
    return 0;
}


int after_sem_create (semaphore_t *s, int value) {
    // put your customization here
#ifdef DEBUG
    printf("\nsem_create - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_sem_down (semaphore_t *s) {
    // put your customization here
#ifdef DEBUG
    printf("\nsem_down - BEFORE - [%d]", taskExec->id);
    printf("\n BUGOU" );
#endif
    return 0;
}

int sem_down (semaphore_t *s) {
    if(s==NULL || s->active=='n')
    {
        return -1;
    }
    PPOS_PREEMPT_DISABLE
    // while (s->active!=0){ task_yield();}
    s->active=1;
    // before_sem_down(s);
    // mutex_lock(&(s->sem_mutex));
    s->sem_state=s->sem_state-1;
    int sem_count = s->sem_state;
    // mutex_unlock(&(s->sem_mutex));
    // printf("\nsem_down - BEFORE - [%d] - sem_count - [%d]", taskExec->id , sem_count);
    if(sem_count<0)
    {
        // mutex_lock(&(s->sem_queue_mutex));

        task_t* task_semaphored= taskExec;

        task_suspend(task_semaphored,(s->sem_t_queue));
        // mutex_unlock(&(s->sem_queue_mutex));
            // mutex_unlock(&(s->sem_mutex));
        s->active=0;
        PPOS_PREEMPT_ENABLE
        task_yield();

        // mutex_lock(&(s->sem_queue_mutex));

        // queue_t* queue_e_rem = queue_remove(s->sem_queue_ready,queue_e);

        // while (queue_e_rem==NULL)
        // {
        //     mutex_unlock(&(s->sem_queue_mutex));
        //     mutex_lock(&(s->sem_queue_mutex));
        //     queue_e_rem = queue_remove(s->sem_queue_ready,queue_e);
        // }
        // s->removed_element=NULL;
        // mutex_unlock(&(s->sem_queue_mutex));

        // queue_e->next=NULL;
        // queue_e->prev=NULL;
        // free(queue_e);

        // task_resume(task_sem);
    }
    else{
        s->active=0;
        PPOS_PREEMPT_ENABLE 
        // mutex_unlock(&(s->sem_mutex));

    }

    after_sem_down(s);
    return 0;
}


int after_sem_down (semaphore_t *s) {
    // put your customization here
#ifdef DEBUG
    printf("\nsem_down - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_sem_up (semaphore_t *s) {
    // put your customization here
#ifdef DEBUG
    printf("\nsem_up - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int sem_up (semaphore_t *s){

    if(s==NULL || s->active=='n')
    {
        return -1;
    }
    PPOS_PREEMPT_DISABLE
    before_sem_up(s);
    // while (s->active!=0){ task_yield();}
    s->active=1;
    // mutex_lock(&(s->sem_mutex));
    s->sem_state=s->sem_state+1;
    int sem_count = s->sem_state;
    // mutex_unlock(&(s->sem_mutex));

    if(sem_count<=0){
        // mutex_lock(&(s->sem_queue_mutex));
        task_t* queue_t_e = *(s->sem_t_queue);

        if(queue_t_e==NULL){
            return -1;
        }

        queue_t_e = (task_t*) queue_remove ((queue_t**)(s->sem_t_queue),(queue_t*) queue_t_e) ;
        queue_append((queue_t**)&(readyQueue),(queue_t*) queue_t_e);
        if(queue_t_e==NULL){
            // mutex_unlock(&(s->sem_queue_mutex));
            #ifdef DEBUG
                printf("\nsem_up - FILA VAZIA - [%d]", taskExec->id);
            #endif
            return -1;
        }

        // task_resume(queue_t_e);
        s->active=1;
        PPOS_PREEMPT_ENABLE 
        // mutex_unlock(&(s->sem_mutex));
        // mutex_unlock(&(s->sem_queue_mutex));
        // if(s->sem_queue_ready==NULL){printf("\n NULLL");}

        // if(*(s->sem_queue_ready)==NULL){
        //     queue_e->prev=queue_e;
        //     queue_e->next=queue_e;
        //     *(s->sem_queue_ready)=queue_e; //insere primeiro elemento na fila
        // }
        // else{
        //     queue_append(s->sem_queue_ready,queue_e);
        // }
        // while (s->removed_element!=NULL)
        // {
        //     mutex_unlock(&(s->sem_queue_mutex));
        //     mutex_lock(&(s->sem_queue_mutex));
        // }
        // s->removed_element=queue_e;
        // mutex_unlock(&(s->sem_queue_mutex));
    }
    else{
        PPOS_PREEMPT_ENABLE
        s->active=1;
    }
    // mutex_unlock(&(s->sem_queue_mutex));
    after_sem_up(s);
    return 0;

}

int after_sem_up (semaphore_t *s) {
    // put your customization here
#ifdef DEBUG
    printf("\nsem_up - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_sem_destroy (semaphore_t *s) {
    // put your customization here
#ifdef DEBUG
    printf("\nsem_destroy - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int sem_destroy (semaphore_t *s){

    before_sem_destroy(s);
    mutex_destroy(&(s->sem_mutex));
    // mutex_destroy(&(s->sem_queue_mutex));
    s->sem_state=-__INT_MAX__;
    s->sem_id=0;
    s->active='n';
    // queue_t** sem_queue;
    // if(s->removed_element!=NULL)
    // {
    //     free(s->removed_element);
    //     s->removed_element=NULL;
    // // }
    // queue_t* queue_p = *(s->sem_queue);

    // while (queue_p!=NULL)
    // {
    //     queue_t* queue_a=queue_p->next;
    //     if(queue_p==queue_a){
    //         queue_a=NULL;
    //     }
    //     queue_p=queue_remove(s->sem_queue,queue_p);
    //     free(queue_p);
    //     queue_p=queue_a;
    // }

    // free(s->sem_queue);

    // s->active='n';

    task_t* queue_t_e = *(s->sem_t_queue);

    while(queue_t_e!=NULL)
    {
        queue_t_e = (task_t*) queue_remove ((queue_t**)(s->sem_t_queue),(queue_t*) queue_t_e) ;
        // queue_append((queue_t**)&(sleepQueue),(queue_t*) queue_t_e);
        task_resume(queue_t_e);
        queue_t_e = *(s->sem_t_queue);
    }


    after_sem_destroy(s);
    return 0;
}

int after_sem_destroy (semaphore_t *s) {
    // put your customization here
#ifdef DEBUG
    printf("\nsem_destroy - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}






int before_mutex_create (mutex_t *m) {
    // put your customization here
#ifdef DEBUG
    printf("\nmutex_create - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int mutex_create (mutex_t *m){
    before_mutex_create (m); 
    // m=(mutex_t*) malloc(sizeof m);
    if (m==NULL)
        return -1;

    m->mtx_id=1;
    m->mtx_locked=0;
    after_mutex_create(m);
    return 0;
}


int after_mutex_create (mutex_t *m) {
    // put your customization here
#ifdef DEBUG
    printf("\nmutex_create - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_mutex_lock (mutex_t *m) {
    // put your customization here
#ifdef DEBUG
    printf("\nmutex_lock - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

// Solicita um mutex
int mutex_lock (mutex_t *m) {
    before_mutex_lock(m);
    // mutex_create(m);
    if (m==NULL)
        return -1;
    while (__atomic_test_and_set(&(m->mtx_locked), __ATOMIC_SEQ_CST)) {task_yield();}
    //    { task_sleep(1); task_yield(); }
    // while (m->mtx_locked==0) { }
        // task_yield();    
    // m->mtx_locked=1;
    after_mutex_lock(m);
    return 0;
}


int after_mutex_lock (mutex_t *m) {
    // put your customization here
#ifdef DEBUG
    printf("\nmutex_lock - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_mutex_unlock (mutex_t *m) {
    // put your customization here
#ifdef DEBUG
    printf("\nmutex_unlock - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

// Libera um mutex
int mutex_unlock (mutex_t *m){
    before_mutex_unlock(m);
    if (m==NULL)
        return -1;
    // if (m->mtx_locked==0)
    //     return -1;
    __atomic_clear(&(m->mtx_locked), __ATOMIC_SEQ_CST);
    // task_yield();
    // m->mtx_locked=0;
    after_mutex_unlock(m);
    return 0;
}

int after_mutex_unlock (mutex_t *m) {
    // put your customization here
#ifdef DEBUG
    printf("\nmutex_unlock - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_mutex_destroy (mutex_t *m) {
    // put your customization here
#ifdef DEBUG
    printf("\nmutex_destroy - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

// Destrói um mutex
int mutex_destroy (mutex_t *m) {
    before_mutex_destroy(m);
    if (m==NULL)
        return -1;
    // mutex_lock(m);
    // free(m);
    // m=NULL;
    after_mutex_destroy(m);
    return 0;
}

int after_mutex_destroy (mutex_t *m) {
    // put your customization here
#ifdef DEBUG
    printf("\nmutex_destroy - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_barrier_create (barrier_t *b, int N) {
    // put your customization here
#ifdef DEBUG
    printf("\nbarrier_create - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_barrier_create (barrier_t *b, int N) {
    // put your customization here
#ifdef DEBUG
    printf("\nbarrier_create - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_barrier_join (barrier_t *b) {
    // put your customization here
#ifdef DEBUG
    printf("\nbarrier_join - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_barrier_join (barrier_t *b) {
    // put your customization here
#ifdef DEBUG
    printf("\nbarrier_join - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_barrier_destroy (barrier_t *b) {
    // put your customization here
#ifdef DEBUG
    printf("\nbarrier_destroy - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_barrier_destroy (barrier_t *b) {
    // put your customization here
#ifdef DEBUG
    printf("\nbarrier_destroy - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_mqueue_create (mqueue_t *queue, int max, int size) {
    // put your customization here
#ifdef DEBUG
    printf("\nmqueue_create - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_mqueue_create (mqueue_t *queue, int max, int size) {
    // put your customization here
#ifdef DEBUG
    printf("\nmqueue_create - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_mqueue_send (mqueue_t *queue, void *msg) {
    // put your customization here
#ifdef DEBUG
    printf("\nmqueue_send - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_mqueue_send (mqueue_t *queue, void *msg) {
    // put your customization here
#ifdef DEBUG
    printf("\nmqueue_send - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_mqueue_recv (mqueue_t *queue, void *msg) {
    // put your customization here
#ifdef DEBUG
    printf("\nmqueue_recv - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_mqueue_recv (mqueue_t *queue, void *msg) {
    // put your customization here
#ifdef DEBUG
    printf("\nmqueue_recv - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_mqueue_destroy (mqueue_t *queue) {
    // put your customization here
#ifdef DEBUG
    printf("\nmqueue_destroy - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_mqueue_destroy (mqueue_t *queue) {
    // put your customization here
#ifdef DEBUG
    printf("\nmqueue_destroy - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_mqueue_msgs (mqueue_t *queue) {
    // put your customization here
#ifdef DEBUG
    printf("\nmqueue_msgs - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_mqueue_msgs (mqueue_t *queue) {
    // put your customization here
#ifdef DEBUG
    printf("\nmqueue_msgs - AFTER - [%d]", taskExec->id);
#endif
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