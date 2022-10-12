// Poisson MK3 improved how current array is updated

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>

/**
 * poisson.c
 * Implementation of a Poisson solver with Neumann boundary conditions.
 *
 * This template handles the basic program launch, argument parsing, and memory
 * allocation required to implement the solver *at its most basic level*. You
 * will likely need to allocate more memory, add threading support, account for
 * cache locality, etc...
 *
 * BUILDING:
 * gcc -o poisson poisson.c -lpthread
 *
 * [note: linking pthread isn't strictly needed until you add your
 *        multithreading code]
 *
 * TODO:
 * 1 - Read through this example, understand what it does and what it gives you
 *     to work with.
 * 2 - Implement the basic algorithm and get a correct output.
 * 3 - Add a timer to track how long your execution takes.
 * 4 - Profile your solution and identify weaknesses.
 * 5 - Improve it!
 * 6 - Remember that this is now *your* code and *you* should modify it however
 *     needed to solve the assignment.
 *
 * See the lab notes for a guide on profiling and an introduction to
 * multithreading (see also threads.c which is reference by the lab notes).
 */

#define NUM_THREADS     4

typedef struct
{
    double *curr;          // Start index of the worker thread
    double *next;          // End index of the worker thread
    int n;
    int n_bloat;
    double *source;
    double delta;
    int k_start;
    int k_end;

} WorkerArgs;

double my_clock(void) {
  struct timeval t;
  gettimeofday(&t, NULL);
  return (1.0e-6*t.tv_usec + t.tv_sec);
}


// Global flag
// Set to true when operating in debug mode to enable verbose logging
static bool debug = false;


void update_cell(int n, int n_bloat, double *curr, double *next, double *source, double delta, int cell_index, int inner_index)
{
    int n_2 = n_bloat*n_bloat;
    *(next + cell_index) = (1.0/6.0) * 
    (
        *(curr + cell_index + 1) + *(curr + cell_index - 1)
        + *(curr + cell_index + n_bloat) + *(curr + cell_index - n_bloat)
        + *(curr + cell_index + n_2) + *(curr + cell_index - n_2)
        - delta * delta * *(source + inner_index)
    );
}

void* worker (void* pargs)
{
    WorkerArgs* args = (WorkerArgs*)pargs;

    for (int k = args->k_start; k < args->k_end; k++) {
        for (int j = 1; j <= args->n; j++)
        {
            for (int i = 1; i <= args->n; i++)
            {
                int cell_index = i + (j * args->n_bloat) + (k * args->n_bloat * args->n_bloat);
                int inner_index = (i-1) + ((j-1) * args->n) + ((k-1) * args->n * args->n);
                update_cell(args->n, args->n_bloat, args->curr, args->next, args->source, args->delta, cell_index, inner_index);
            } 
        } 
    }
    return NULL;
}


void update_inner(int n, int n_bloat, double *curr, double *next, double*source, double delta)
{
    pthread_t threads[NUM_THREADS];
    WorkerArgs args[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++)
    {   
        // Fill in the arguments to the worker
        args[i].curr = curr;
        args[i].next = next;
        args[i].n = n;
        args[i].n_bloat = n_bloat;
        args[i].k_start = (n * i) / NUM_THREADS + 1;
        args[i].k_end = (n * (i + 1)) / NUM_THREADS + 1;
        args[i].source = source;
        args[i].delta = delta;

        // Create the worker thread
        if (pthread_create (&threads[i], NULL, &worker, &args[i]) != 0)
        {
            fprintf (stderr, "Error creating worker thread!\n");
        }
    } 

    // Wait for all the threads to finish using join ()
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join (threads[i], NULL);
    }
}

void update_boundary(int n, int n_bloat, double *next)
{
    //Updating ghost points
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
    if (debug)
    {
        printf ("Starting solver with:\n"
               "n = %i\n"
               "iterations = %i\n"
               "threads = %i\n"
               "delta = %f\n",
               n, iterations, threads, delta);
    }

    int n_bloat = n+2;

    // Allocate some buffers to calculate the solution in
    double *curr = (double*)calloc (n_bloat * n_bloat * n_bloat, sizeof (double));
    double *next = (double*)calloc (n_bloat * n_bloat * n_bloat, sizeof (double));

    // Ensure we haven't run out of memory
    if (curr == NULL || next == NULL)
    {
        fprintf (stderr, "Error: ran out of memory when trying to allocate %i sized cube\n", n);
        exit (EXIT_FAILURE);
    }

    // TODO: solve Poisson's equation for the given inputs
    for (int t_step = 0; t_step < iterations; t_step++)
    {
        update_inner(n, n_bloat, curr, next, source, delta);

        update_boundary(n, n_bloat, next);

        // update_curr(n_bloat, curr, next);

        double* temp;
        temp = curr;
        curr = next;
        next = temp;
        // printf ("%p \n", temp);

    }

    // Free one of the buffers and return the correct answer in the other.
    // The caller is now responsible for free'ing the returned pointer.
    // free (next);

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

    if (debug)
    {
        printf ("Finished solving.\n");
    }

    return inner;
}



int main (int argc, char **argv)
{
    // Default settings for solver
    float delta = 1;
    int iterations = 100;
    int n = 101;
    int threads = 1;


    // parse the command line arguments
    for (int i = 1; i < argc; ++i)
    {
        if (strcmp (argv[i], "-h") == 0 || strcmp (argv[i], "--help") == 0)
        {
            printf ("Usage: poisson [-n size] [-i iterations] [-t threads] [--debug]\n");
            return EXIT_SUCCESS;
        }

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

        if (strcmp (argv[i], "-t") == 0)
        {
            if (i == argc - 1)
            {
                fprintf (stderr, "Error: expected threads after -t!\n");
                return EXIT_FAILURE;
            }

            threads = atoi (argv[++i]);
        }

        if (strcmp (argv[i], "--debug") == 0)
        {
            debug = true;
        }
    }

    // Ensure we have an odd sized cube
    if (n % 2 == 0)
    {
        fprintf (stderr, "Error: n should be an odd number!\n");
        return EXIT_FAILURE;
    }

    // Create a source term with a single point in the centre
    double *source = (double*)calloc (n * n * n, sizeof (double));
    if (source == NULL)
    {
        fprintf (stderr, "Error: failed to allocated source term (n=%i)\n", n);
        return EXIT_FAILURE;
    }

    //Set source to single point charge in centre of volume
    source[(n * n * n) / 2] = 1/delta;

    //Start timer
    double begin = my_clock();

    // Calculate the resulting field with Neumann conditions
    double *result = poisson_neumann (n, source, iterations, threads, delta);

    double end = my_clock();
    double time_spent = (double)(end - begin);
    // printf ("Time taken: %0.5f \n", time_spent);

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
