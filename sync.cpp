#include "semaphore_class.h" 

// ------------------------------------------------------------
// Lightswitch 
// ------------------------------------------------------------
class Lightswitch {
public:
    Lightswitch() : counter(0), mutex(1) {}

    void lock(Semaphore &sem) {
        mutex.wait();
        counter++;
        if (counter == 1) {
            sem.wait();
        }
        mutex.signal();
    }

    void unlock(Semaphore &sem) {
        mutex.wait();
        counter--;
        if (counter == 0) {
            sem.signal();
        }
        mutex.signal();
    }

private:
    int counter;
    Semaphore mutex;
};


// ------------------------------------------------------------
// Global Variables
// ------------------------------------------------------------
const int NUM_READERS = 5;
const int NUM_WRITERS = 5;
const int NUM_PHILOSOPHERS = 5;


// ------------------------------------------------------------
// Problem 1: No-starve Readers-Writers
// ------------------------------------------------------------
int sharedData1 = 0;
Lightswitch readSwitch1;
Semaphore roomEmpty1(1);
Semaphore turnstile1(1);

void *Reader1(void *threadID)
{
    long id = (long)threadID;  

    while (1) {
        sleep(1);
        turnstile1.wait();
        turnstile1.signal();
        readSwitch1.lock(roomEmpty1);
        printf("Reader %ld is reading...\n", id);
        fflush(stdout);
        sleep(1);
        readSwitch1.unlock(roomEmpty1);
    }

    return NULL;
}

void *Writer1(void *threadID)
{
    long id = (long)threadID; 

    while (1) {
        sleep(2);
        turnstile1.wait();
        roomEmpty1.wait();
        sharedData1++;
        printf("Writer %ld is writing...\n", id);
        fflush(stdout);
        sleep(1);
        roomEmpty1.signal();
        turnstile1.signal();
    }

    return NULL;
}

void run_problem1()
{
    pthread_t readerThread[NUM_READERS];
    pthread_t writerThread[NUM_WRITERS];
    printf("Running Problem 1: No-starve readers-writers\n");

    for (long r = 0; r < NUM_READERS; r++)
    {
        int rc = pthread_create(&readerThread[r], NULL, Reader1, (void *)(r + 1));
        if (rc) {
            printf("ERROR creating reader thread # %ld; "
                   "return code from pthread_create() is %d\n", r, rc);
            exit(-1);
        }
    }

    for (long w = 0; w < NUM_WRITERS; w++)
    {
        int rc = pthread_create(&writerThread[w], NULL, Writer1, (void *)(w + 1));
        if (rc) {
            printf("ERROR creating writer thread # %ld; "
                   "return code from pthread_create() is %d\n", w, rc);
            exit(-1);
        }
    }

    printf("Main: program completed. Exiting.\n");
    pthread_exit(NULL);
}


// ------------------------------------------------------------
// Problem 2: Writer-priority Readers-Writers
// ------------------------------------------------------------
int sharedData2 = 0;
Lightswitch readSwitch2;
Lightswitch writeSwitch2;
Semaphore noReaders2(1);
Semaphore noWriters2(1);

void *Reader2(void *threadID)
{
    long id = (long)threadID;
    
    while (1) {
        sleep(1);
        noReaders2.wait();
        readSwitch2.lock(noWriters2);
        noReaders2.signal();
        printf("Reader %ld is reading...\n", id);
        fflush(stdout);
        sleep(1);
        readSwitch2.unlock(noWriters2);
    }

    return NULL;
}

void *Writer2(void *threadID)
{
    long id = (long)threadID;
    
    while (1) {
        sleep(2);
        writeSwitch2.lock(noReaders2);
        noWriters2.wait();
        sharedData2++;
        printf("Writer %ld is writing...\n", id);
        fflush(stdout);
        sleep(1);
        noWriters2.signal();
        writeSwitch2.unlock(noReaders2);
    }

    return NULL;
}

void run_problem2()
{
    pthread_t readerThread[NUM_READERS];
    pthread_t writerThread[NUM_WRITERS];
    printf("Running Problem 2: Writer-priority readers-writers\n");

    for (long r = 0; r < NUM_READERS; r++)
    {
        int rc = pthread_create(&readerThread[r], NULL, Reader2, (void *)(r + 1));
        if (rc) {
            printf("ERROR creating reader thread # %ld; "
                   "return code from pthread_create() is %d\n", r, rc);
            exit(-1);
        }
    }

    for (long w = 0; w < NUM_WRITERS; w++)
    {
        int rc = pthread_create(&writerThread[w], NULL, Writer2, (void *)(w + 1));
        if (rc) {
            printf("ERROR creating writer thread # %ld; "
                   "return code from pthread_create() is %d\n", w, rc);
            exit(-1);
        }
    }

    printf("Main: program completed. Exiting.\n");
    pthread_exit(NULL);
}


// ------------------------------------------------------------
// Problem 3: Dining Philosopher - Footman
// ------------------------------------------------------------
Semaphore fork3_0(1), fork3_1(1), fork3_2(1), fork3_3(1), fork3_4(1);
Semaphore* forks3[NUM_PHILOSOPHERS] = { &fork3_0, &fork3_1, &fork3_2, &fork3_3, &fork3_4 };
Semaphore footman3(4);   

void *Philosopher3(void *threadID)
{
    long id = (long)threadID;
    int i = id - 1;
    int left = i;
    int right = (i + 1) % NUM_PHILOSOPHERS;

    while (1) {
        printf("Philosopher %ld is thinking...\n", id);
        fflush(stdout);
        sleep(1);
        footman3.wait();
        forks3[right]->wait();
        forks3[left]->wait();
        printf("Philosopher %ld is eating...\n", id);
        fflush(stdout);
        sleep(2);
        forks3[left]->signal();
        forks3[right]->signal();
        footman3.signal();
    }

    return NULL;
}

void run_problem3()
{
    pthread_t philos[NUM_PHILOSOPHERS];
    printf("Running Problem 3: Dining philosophers - Footman\n");

    for (long p = 0; p < NUM_PHILOSOPHERS; ++p)
    {
        int rc = pthread_create(&philos[p], NULL, Philosopher3, (void *)(p + 1));
        if (rc) {
            printf("ERROR creating philosopher thread # %ld; "
                   "return code from pthread_create() is %d\n", p, rc);
            exit(-1);
        }
    }

    printf("Main: program completed. Exiting.\n");
    pthread_exit(NULL);
}


// ------------------------------------------------------------
// Problem 4: Dining Philosophers - Asymmetric
// ------------------------------------------------------------
Semaphore fork4_0(1), fork4_1(1), fork4_2(1), fork4_3(1), fork4_4(1);
Semaphore* forks4[NUM_PHILOSOPHERS] = { &fork4_0, &fork4_1, &fork4_2, &fork4_3, &fork4_4 };

void *Philosopher4(void *threadID)
{
    long id = (long)threadID;
    int i = id - 1;
    int left  = i;
    int right = (i + 1) % NUM_PHILOSOPHERS;

    while (1) {
        printf("Philosopher %ld is thinking...\n", id);
        fflush(stdout);
        sleep(1);

        if (id == 1) {
            forks4[left]->wait();
            forks4[right]->wait();
        } else {
            forks4[right]->wait();
            forks4[left]->wait();
        }

        printf("Philosopher %ld is eating...\n", id);
        fflush(stdout);
        sleep(2);

        forks4[left]->signal();
        forks4[right]->signal();
    }

    return NULL;
}

void run_problem4()
{
    pthread_t philos[NUM_PHILOSOPHERS];
    printf("Running Problem 4: Dining philosophers - Asymmetric\n");

    for (long p = 0; p < NUM_PHILOSOPHERS; ++p)
    {
        int rc = pthread_create(&philos[p], NULL, Philosopher4, (void *)(p + 1));
        if (rc) {
            printf("ERROR creating philosopher thread # %ld; "
                   "return code from pthread_create() is %d\n", p, rc);
            exit(-1);
        }
    }

    printf("Main: program completed. Exiting.\n");
    pthread_exit(NULL);
}


// ------------------------------------------------------------
// Main
// ------------------------------------------------------------
int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("Usage: %s <problem#>\n", argv[0]);
        printf("1 = No-starve readers-writers\n");
        printf("2 = Writer-priority readers-writers\n");
        printf("3 = Dining philosophers - Footman\n");
        printf("4 = Dining philosophers - Asymmetric\n");
        exit(1);
    }

    int problem = atoi(argv[1]);
    switch (problem) {
        case 1:
            run_problem1();
            break;
        case 2:
            run_problem2();
            break;
        case 3:
            run_problem3();
            break;
        case 4:
            run_problem4();
            break;
        default:
            printf("Invalid problem number: %d\n", problem);
            exit(1);
    }

    return 0;
}