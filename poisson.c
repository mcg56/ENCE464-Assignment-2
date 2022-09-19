#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


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


// Global flag
// Set to true when operating in debug mode to enable verbose logging
static bool debug = true;


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
        for (int k = 1; k <= n; k++)
        {
            for (int j = 1; j <= n; j++)
            {
                for (int i = 1; i <= n; i++)
                {
                    *(next + ((k * n_bloat * n_bloat) + j * n_bloat + i)) = 1.0/6.0 * 
                    (
                        *(curr + (i + 1) + (j * n_bloat) + (k * n_bloat * n_bloat)) + *(curr + (i - 1) + (j * n_bloat) + (k * n_bloat * n_bloat))
                        + *(curr + i + ((j + 1) * n_bloat) + (k * n_bloat * n_bloat)) + *(curr + i + ((j - 1) * n_bloat) + (k * n_bloat * n_bloat))
                        + *(curr + i + (j * n_bloat) + ((k + 1) * n_bloat * n_bloat)) + *(curr + i + (j * n_bloat) + ((k - 1) * n_bloat * n_bloat))
                        - delta * delta * *(source + (i-1) + ((j-1) * n) + ((k-1) * n * n))
                    );
                } 


            } 
        } 

        //Updating ghost points
        for (int k = 1; k <= n; k++)
        {
            for (int j = 1; j <= n; j++)
            {
                *(next + ((k * n_bloat * n_bloat) + j * n_bloat)) = *(next + ((k * n_bloat * n_bloat) + j * n_bloat + 2));
                *(next + ((k * n_bloat * n_bloat) + j * n_bloat + n + 1)) = *(next + ((k * n_bloat * n_bloat) + j * n_bloat + n - 1));
            }
        }

        for (int k = 1; k <= n; k++)
        {
            for (int i = 1; i <= n; i++)
            {
                *(next + ((k * n_bloat * n_bloat) + i)) = *(next + ((k * n_bloat * n_bloat) + 2 * n_bloat + i));
                *(next + ((k * n_bloat * n_bloat) + (n + 1) * n_bloat + i)) = *(next + ((k * n_bloat * n_bloat) + (n - 1) * n_bloat + i));
            }
        }        

        for (int j = 1; j <= n; j++)
        {
            for (int i = 1; i <= n; i++)
            {
                *(next + (j * n_bloat + i)) = *(next + ((2 * n_bloat * n_bloat) + j * n_bloat + i));
                *(next + (((n+1) * n_bloat * n_bloat) + j * n_bloat + i)) = *(next + (((n-1) * n_bloat * n_bloat) + j * n_bloat + i));
            }
        } 


        curr = next;
    }

    //TODO: Extract cube from bloated cube

    // Free one of the buffers and return the correct answer in the other.
    // The caller is now responsible for free'ing the returned pointer.
    free (next);

    if (debug)
    {
        printf ("Finished solving.\n");
    }

    return curr;
}



int main (int argc, char **argv)
{
    // Default settings for solver
    int iterations = 100;
    int n = 7;
    int threads = 1;
    float delta = 1;

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
    source[(n * n * n) / 2] = 1;

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
