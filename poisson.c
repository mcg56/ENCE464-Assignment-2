#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>
#include <semaphore.h>

/**
 * poisson.c
 * Implementation of a Poisson solver with Neumann boundary conditions.
 *
 * BUILDING:
 * gcc -O3 -pg -g -o poisson poisson.c -lpthread
 *
 * RUNNING:
 * ./poisson -n [dimension of cube] -i [number of iterations]
 **/


pthread_barrier_t barrier;
pthread_mutex_t lock;
int threads_completed = 0;


typedef struct
{
    int iters;
    double *curr;         
    double *next;          
    int n;
    int n_bloat;
    double *source;
    double delta2;
    int k_start;
    int k_end;
    int num_threads;
    bool last_thread;

} WorkerArgs;

/**
 * @brief Updates the value of a single cell using Poissons equation
 *
 * @param n_bloat       The edge length of the bloated cube.
 * @param curr          Pointer to the start of the cube in the current time step.
 * @param next          Pointer to the start of the cube in the next time step.
 * @param source        Pointer to the source term cube, a.k.a. forcing function.
 * @param delta2        Grid spacing squared
 * @param cell_index    Index of the current cell within the bloated cubes memory allocation
 * @param inner_index   Index of the current cell within the inner cubes memory allocation
 */
void update_cell(int n_bloat, double *curr, double *next, double *source, double delta2, int cell_index, int inner_index)
{
    int n_2 = n_bloat*n_bloat;
    *(next + cell_index) = (1.0/6.0) * (  *(curr + cell_index + 1) 
                                        + *(curr + cell_index - 1)
                                        + *(curr + cell_index + n_bloat) 
                                        + *(curr + cell_index - n_bloat)
                                        + *(curr + cell_index + n_2) 
                                        + *(curr + cell_index - n_2)
                                        - delta2 * *(source + inner_index));
}

/**
 * @brief Updating ghost points
 *
 * @param n             The edge length of the inner cube.
 * @param n_bloat       The edge length of the bloated cube.
 * @param next          Pointer to the start of the cube in the next time step.
 */
void update_boundary(int n, int n_bloat, double *next)
{
    int n_2 = n_bloat*n_bloat;
        for (int k = 1; k <= n; k++)
        {
            for (int j = 1; j <= n; j++)
            {
                *(next + ((k * n_2) + j * n_bloat)) = *(next + ((k * n_2) + j * n_bloat + 2));
                *(next + ((k * n_2) + j * n_bloat + n + 1)) = *(next + ((k * n_2) + j * n_bloat + n - 1));
            }
        }

        for (int k = 1; k <= n; k++)
        {
            for (int i = 1; i <= n; i++)
            {
                *(next + ((k * n_2) + i)) = *(next + ((k * n_2) + 2 * n_bloat + i));
                *(next + ((k * n_2) + (n + 1) * n_bloat + i)) = *(next + ((k * n_2) + (n - 1) * n_bloat + i));
            }
        }        

        for (int j = 1; j <= n; j++)
        {
            for (int i = 1; i <= n; i++)
            {
                *(next + (j * n_bloat + i)) = *(next + ((2 * n_2) + j * n_bloat + i));
                *(next + (((n+1) * n_2) + j * n_bloat + i)) = *(next + (((n-1) * n_2) + j * n_bloat + i));
            }
        } 
}

/**
 * @brief Function that contains all of the operations for a single thread. Includes cycling through 
 *        each time step, and cycling through each cell within that. Also waits for all other threads
 *        to finish each timestep before proceeding.
 *
 * @param pargs     Arguments passed between threads.
 */
void* worker (void* pargs)
{
    WorkerArgs* args = (WorkerArgs*)pargs;

    for (int t_step = 0; t_step < args->iters; t_step++)
    {
        for (int k = 1; k <= args->n; k++) {
            for (int j = args->k_start; j < args->k_end; j++) {
                for (int i = 1; i <= args->n; i++)
                {
                    int cell_index = i + (j * args->n_bloat) + (k * args->n_bloat * args->n_bloat);
                    int inner_index = (i-1) + ((j-1) * args->n) + ((k-1) * args->n * args->n);
                    update_cell(args->n_bloat, args->curr, args->next, args->source, args->delta2, cell_index, inner_index);
                } 
            } 
        }
    
        //Mutex is used to protect variable accessed by multiple threads
        pthread_mutex_lock(&lock);
        threads_completed++;
        pthread_mutex_unlock(&lock);

        //Only update boundary once all threads have finished
        if (threads_completed >= args->num_threads) {
            update_boundary(args->n, args->n_bloat, args->next);
            threads_completed = 0;
        }
        
        //Rendezvous point for threads to sychronise
        pthread_barrier_wait (&barrier);

        //Switch pointers between current and next
        double* temp;
        temp = args->curr;
        args->curr = args->next;
        args->next = temp;
    }

    return NULL;
}


/**
 * @brief Solve Poissons equation for a given cube with Neumann boundary
 * conditions on all sides.
 *
 * @param n             The edge length of the cube. n^3 number of elements.
 * @param source        Pointer to the source term cube, a.k.a. forcing function.
 * @param iterations    Number of iterations to perform.
 * @param threads       Number of threads to use for solving.
 * @param delta         Grid spacing.
 * @return double*      Solution to Poissons equation.  Caller must free.
 */
double* poisson_neumann (int n, double *source, int iterations, int threads, float delta)
{
    int n_bloat = n+2;
    int delta2 = delta * delta;

    // Allocate some buffers to calculate the solution in
    double *curr = (double*)calloc (n_bloat * n_bloat * n_bloat, sizeof (double));
    double *next = (double*)calloc (n_bloat * n_bloat * n_bloat, sizeof (double));

    pthread_barrier_init (&barrier, NULL, threads);
    pthread_mutex_init(&lock, NULL);

    pthread_t worker_threads[threads];
    WorkerArgs args[threads];

    for (int i = 0; i < threads; i++)
    {   

        // Fill in the arguments to the worker
        args[i].iters = iterations;
        args[i].curr = curr;
        args[i].next = next;
        args[i].n = n;
        args[i].n_bloat = n_bloat;
        args[i].k_start = (n * i) / threads + 1;
        args[i].k_end = (n * (i + 1)) / threads + 1;
        args[i].source = source;
        args[i].delta2 = delta2;
        args[i].num_threads = threads;
        args[i].last_thread = false;

        // Create the worker thread
        pthread_create (&worker_threads[i], NULL, &worker, &args[i]);
    } 

    // Wait for all the threads to finish using join ()
    for (int i = 0; i < threads; i++)
    {
        pthread_join (worker_threads[i], NULL);
    }

    //Extract cube from bloated cube
    double *inner = (double*)calloc (n * n * n, sizeof (double));

    for (int k = 1; k <= n; k++)
    {
        for (int j = 1; j <= n; j++)
        {
            for (int i = 1; i <= n; i++)
            {
                *(inner + ((k-1) * n * n) + ((j-1) * n) + (i-1)) =  *(curr + (k * n_bloat * n_bloat) + (j * n_bloat) + i);
            }
        }
    }

    return inner;
}



int main (int argc, char **argv)
{
    // Default settings for solver
    float delta = 1;
    int iterations = 900;
    int n = 301;
    int threads = 3;


    // parse the command line arguments
    for (int i = 1; i < argc; ++i)
    {

        if (strcmp (argv[i], "-n") == 0)
        {
            if (i == argc - 1)
            {
                fprintf (stderr, "Error: expected size after -n!\n");
                return EXIT_FAILURE;
            }

            n = atoi (argv[++i]);
        }

        if (strcmp (argv[i], "-i") == 0)
        {
            if (i == argc - 1)
            {
                fprintf (stderr, "Error: expected iterations after -i!\n");
                return EXIT_FAILURE;
            }

            iterations = atoi (argv[++i]);
        }
    }

    // Create a source term with a single point in the centre
    double *source = (double*)calloc (n * n * n, sizeof (double));

    //Set source to single point charge in centre of volume
    source[(n * n * n) / 2] = 1/delta;

    // Calculate the resulting field with Neumann conditions
    double *result = poisson_neumann (n, source, iterations, threads, delta);

    // Print out the middle slice of the cube for validation
    for (int x = 0; x < n; ++x)
    {
        for (int y = 0; y < n; ++y)
        {
            printf ("%0.5f ", result[((n / 2) * n + y) * n + x]);
        }
        printf ("\n");
    }

    free (source);
    free (result);

    return EXIT_SUCCESS;
}
