#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>

#define MAX_PROCESSES 10

int processes[MAX_PROCESSES];
int n;
int initiator;
int dead[MAX_PROCESSES];
int dead_count = 0;
int coordinator = -1;

pthread_mutex_t lock;

// Function prototypes
void* start_election(void* arg);
void* heartbeat(void* arg);

// Check if a process is dead
bool is_dead(int pid) {
    for (int i = 0; i < dead_count; i++) {
        if (dead[i] == pid) return true;
    }
    return false;
}

// Log messages with timestamps
void timestamp_log(const char* msg, int pid) {
    time_t now;
    struct tm* timeinfo;
    char buffer[80];
    time(&now);
    timeinfo = localtime(&now);
    strftime(buffer, 80, "[%H:%M:%S]", timeinfo);
    printf("%s Process %d: %s\n", buffer, pid, msg);
}

// Election process
void* start_election(void* arg) {
    int pid = *(int*)arg;
    usleep((rand() % 400 + 100) * 1000);  // Simulate delay

    char msg[100];
    bool higher_responded = false;

    for (int i = 0; i < n; i++) {
        if (processes[i] > pid && !is_dead(processes[i])) {
            snprintf(msg, sizeof(msg), "sends ELECTION to Process %d", processes[i]);
            timestamp_log(msg, pid);
            usleep((rand() % 300 + 100) * 1000);  // Simulate lag

            snprintf(msg, sizeof(msg), "responds OK to Process %d", pid);
            timestamp_log(msg, processes[i]);
            higher_responded = true;

            // Trigger election from that higher process
            pthread_t new_thread;
            int* new_pid = malloc(sizeof(int));
            *new_pid = processes[i];
            pthread_create(&new_thread, NULL, start_election, new_pid);
            pthread_detach(new_thread);
        }
    }

    if (!higher_responded) {
        pthread_mutex_lock(&lock);
        if (pid > coordinator) {
            coordinator = pid;
        }
        pthread_mutex_unlock(&lock);
    }

    return NULL;
}

// Heartbeat process to check coordinator failure
void* heartbeat(void* arg) {
    int pid = *(int*)arg;

    while (1) {
        usleep((rand() % 500 + 100) * 1000);  // Simulate heartbeat delay

        char msg[100];
        snprintf(msg, sizeof(msg), "Heartbeat check from Process %d", pid);
        timestamp_log(msg, pid);

        // If the coordinator is dead, start a new election
        if (is_dead(coordinator)) {
            printf("[Heartbeat] Coordinator Process %d is dead. Starting election...\n", coordinator);
            pthread_t new_thread;
            int* new_pid = malloc(sizeof(int));
            *new_pid = initiator;  // Initiator starts the election
            pthread_create(&new_thread, NULL, start_election, new_pid);
            pthread_detach(new_thread);
            break;  // Exit heartbeat once election is started
        }

        usleep(1000000);  // Simulate periodic check every second
    }

    return NULL;
}

int main() {
    srand(time(NULL));
    pthread_mutex_init(&lock, NULL);

    printf("Enter number of processes (max %d): ", MAX_PROCESSES);
    scanf("%d", &n);

    printf("Enter process IDs in ascending order:\n");
    for (int i = 0; i < n; i++) {
        printf("Process %d ID: ", i + 1);
        scanf("%d", &processes[i]);
    }

    printf("Enter initiator process ID: ");
    scanf("%d", &initiator);

    printf("Enter number of dead processes: ");
    scanf("%d", &dead_count);

    for (int i = 0; i < dead_count; i++) {
        printf("Dead Process %d ID: ", i + 1);
        scanf("%d", &dead[i]);
    }

    printf("\nElection initiated by Process %d...\n", initiator);
    pthread_t thread;
    int* pid_ptr = malloc(sizeof(int));
    *pid_ptr = initiator;
    pthread_create(&thread, NULL, start_election, pid_ptr);
    pthread_join(thread, NULL);

    // Start heartbeat check in a separate thread
    pthread_t heartbeat_thread;
    pthread_create(&heartbeat_thread, NULL, heartbeat, pid_ptr);
    pthread_detach(heartbeat_thread);

    // Small wait for any detached threads to complete
    sleep(2);

    printf("\nElection completed. 🏆 Process %d is elected as the new coordinator.\n", coordinator);

    printf("\nCoordinator announcement:\n");
    for (int i = 0; i < n; i++) {
        if (!is_dead(processes[i]) && processes[i] != coordinator) {
            printf("Process %d acknowledges Process %d as the coordinator.\n", processes[i], coordinator);
        }
    }

    pthread_mutex_destroy(&lock);
    return 0;
}
