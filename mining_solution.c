#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>

// ANSI color codes for light blue, yellow, and pink
#define CYAN	   "\x1B[96m"
#define YELLOW     "\x1B[93m"
#define PINK       "\x1B[95m"
#define RESET      "\x1B[0m"

#define NUM_EXPLORERS 2
#define NUM_MINERS 3
#define NUM_TRANSPORTERS 3

// Semaphore declarations
sem_t found_material;
sem_t miner_ready;
sem_t material_extracted;

// Thread arrays
pthread_t explorers[NUM_EXPLORERS];
pthread_t miners[NUM_MINERS];
pthread_t transporters[NUM_TRANSPORTERS];

// Idle counts and mined quantities
int idle_miners = NUM_MINERS;
int idle_transporters = NUM_TRANSPORTERS;
int mined_quantities[NUM_MINERS] = {0};

// Mutex for printing and updating idle counters
pthread_mutex_t print_mutex;

// Function prototypes
void* explorer_activity(void* arg);
void* miner_activity(void* arg);
void* transporter_activity(void* arg);
void handle_interrupt(int sig);
void print_status(const char* color, const char* message, int id);
void init_waiting_status();

int main() {
    // Initialize semaphores and mutex
    sem_init(&found_material, 0, 0);
    sem_init(&miner_ready, 0, NUM_MINERS);
    sem_init(&material_extracted, 0, 0);
    pthread_mutex_init(&print_mutex, NULL);

    // Register interrupt handler for CTRL+C
    signal(SIGINT, handle_interrupt);
    
    // Initialize waiting status for miners and transporters
    init_waiting_status();

    // Create threads
    for (int i = 0; i < NUM_EXPLORERS; i++) {
        pthread_create(&explorers[i], NULL, explorer_activity, (void*)(long)i+1);
    }
    for (int i = 0; i < NUM_MINERS; i++) {
        pthread_create(&miners[i], NULL, miner_activity, (void*)(long)i+1);
    }
    for (int i = 0; i < NUM_TRANSPORTERS; i++) {
        pthread_create(&transporters[i], NULL, transporter_activity, (void*)(long)i+1);
    }

    // Wait for threads to finish (though they are infinite loops)
    for (int i = 0; i < NUM_EXPLORERS; i++) {
        pthread_join(explorers[i], NULL);
    }
    for (int i = 0; i < NUM_MINERS; i++) {
        pthread_join(miners[i], NULL);
    }
    for (int i = 0; i < NUM_TRANSPORTERS; i++) {
        pthread_join(transporters[i], NULL);
    }

    // Clean up
    sem_destroy(&found_material);
    sem_destroy(&miner_ready);
    sem_destroy(&material_extracted);
    pthread_mutex_destroy(&print_mutex);

    return 0;
}

void init_waiting_status() {
    for (int i = 1; i <= NUM_MINERS; i++) {
        printf(YELLOW "Miner-%d: waiting...\n" RESET, i);
    }
    for (int i = 1; i <= NUM_TRANSPORTERS; i++) {
        printf(PINK "Transporter-%d: waiting...\n" RESET, i);
    }
}

void* explorer_activity(void* arg) {
    int id = (int)(long)arg;
    char buffer[100];
    while (1) {
        sprintf(buffer, "Explorer-%d: exploring...", id);
        print_status(CYAN, buffer, id);
        sleep(rand() % 3 + 2); // Exploring takes 2-4 seconds
        sprintf(buffer, "Explorer-%d: target found", id);
        print_status(CYAN, buffer, id);
        sem_post(&found_material);
    }
    return NULL;
}

void* miner_activity(void* arg) {
    int id = (int)(long)arg;
    char buffer[100];
    while (1) {
        sem_wait(&found_material);
        pthread_mutex_lock(&print_mutex);
        idle_miners--;
        mined_quantities[id-1] = 0; // Reset the mined quantity
        pthread_mutex_unlock(&print_mutex);
        sprintf(buffer, "Miner-%d: mining... \t\t\tIdle miners: %d", id, idle_miners);
        print_status(YELLOW, buffer, id);
        sleep(rand() % 4 + 2); // Extraction takes 2-5 seconds
        mined_quantities[id-1] = rand() % 6 + 1; // Quantity between 1 and 6
        pthread_mutex_lock(&print_mutex);
        idle_miners++;
        pthread_mutex_unlock(&print_mutex);
        sprintf(buffer, "Miner-%d: mined quantity = %d \t\tIdle miners: %d", id, 		 mined_quantities[id-1],idle_miners);
        print_status(YELLOW, buffer, id);
        sem_post(&miner_ready);
        sem_post(&material_extracted);
    }
    return NULL;
}

void* transporter_activity(void* arg) {
    int id = (int)(long)arg;
    char buffer[100];
    while (1) {
        sem_wait(&material_extracted);
        pthread_mutex_lock(&print_mutex);
        idle_transporters--;
        pthread_mutex_unlock(&print_mutex);
        sprintf(buffer, "Transporter-%d: transporting... \t\tIdle transporters: %d", id, idle_transporters);
        print_status(PINK, buffer, id);
        sleep(4); // Transportation takes 4 seconds
        pthread_mutex_lock(&print_mutex);
        idle_transporters++;
        pthread_mutex_unlock(&print_mutex);
        sprintf(buffer, "Transporter-%d: waiting... \t\tIdle transporters: %d", id, idle_transporters);
        print_status(PINK, buffer, id);
        
    }
    return NULL;
}

void handle_interrupt(int sig) {
    printf("\n=========================\n");
    printf("Cancel all threads\n");

    // Cancel all threads
    for (int i = 0; i < NUM_EXPLORERS; i++) {
        pthread_cancel(explorers[i]);
        printf("Cancelling Explorer-%d\n", i+1);
    }
    for (int i = 0; i < NUM_MINERS; i++) {
        pthread_cancel(miners[i]);
        printf("Cancelling Miner-%d\n", i+1);
    }
    for (int i = 0; i < NUM_TRANSPORTERS; i++) {
        pthread_cancel(transporters[i]);
        printf("Cancelling Transporter-%d\n", i+1);
    }
    printf("Terminated.\n");
    exit(0);
}

void print_status(const char* color, const char* message, int id) {
    pthread_mutex_lock(&print_mutex);
    printf("%s%s\n%s", color, message, RESET);
    pthread_mutex_unlock(&print_mutex);
}
