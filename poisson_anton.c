#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>


// Global flag
// Set to true when operating in debug mode to enable verbose logging
static bool debug = false;

typedef struct
{
    int thread_id;      // Unique id of the worker thread //YOgit
    int start;          // Start index of the worker thread
    int end;            // End index of the worker thread
    int n;
    int n2; 
    float delta;
    int *n_index_lh;
	int *n_index_rh;
    float deltad;
    double *curr;
    double *next;
    double *source;
} WorkerArgs;


double my_clock(void) {
  struct timeval t;
  gettimeofday(&t, NULL);
  return (1.0e-6*t.tv_usec + t.tv_sec);
}





void calc_voxel( double *source, double *curr, double *next, float deltad, int n, int n2, int i, int j, int k, int *n_index_lh, int *n_index_rh )
{
	int xl = n_index_lh[i];
	int xr = n_index_rh[i];
	int yl = n_index_lh[j];
	int yr = n_index_rh[j];
	int zl = n_index_lh[k];
	int zr = n_index_rh[k];
    
	
	int current_index = i + n*j + n2*k;
    //fprintf (stderr, "%i %i %i %i\n", n, i, j, n2);

    //fprintf (stderr, "x1: %i %i %i %i %i %i %i\n", xl, xr, yl,yr,zl,zr,current_index);
	
	
	next[current_index] = ( curr[ xl + ( j * n ) + ( k * n2 ) ] +
							curr[ xr + ( j * n ) + ( k * n2 ) ] +
							curr[ i + ( yl * n ) + ( k * n2 ) ] +
							curr[ i + ( yr * n ) + ( k * n2 ) ] +
							curr[ i + ( j * n ) + ( zl * n2 ) ] +
							curr[ i + ( j * n ) + ( zr * n2 ) ] +
							deltad*source[current_index] ) / 6;

 } // This didnt work due to Aliases as the program must read the array from memory



void* worker (void* pargs)
{
    WorkerArgs* args = (WorkerArgs*)pargs;
    
    int start = args->start;
    int end = args->end;
    int n = args->n;
    int delta = args->delta;
    int n2 = args->n2;
    int *n_index_lh = args->n_index_lh ;
	int *n_index_rh = args->n_index_rh;
    float deltad = args->deltad;
    double *curr = args->curr;
    double *next = args->next;
    double *source = args->source;


    for (int k = 0; k < n; k++)
            {
                for (int j = start; j < end; j++)
                {
                    for (int i = 0; i < end; i++) // Down or up increment doesnt make any changem
                    {
                        //calc_voxel(source, curr, next, deltad, n, n2, i, j, k, n_index_lh, n_index_rh );
                        next[i + n*j + n2*k] = ( curr[ n_index_lh[i] + ( j * n ) + ( k * n2 ) ] +
							curr[ n_index_rh[i] + ( j * n ) + ( k * n2 ) ] +
							curr[ i + ( n_index_lh[j] * n ) + ( k * n2 ) ] +
							curr[ i + ( n_index_rh[j] * n ) + ( k * n2 ) ] +
							curr[ i + ( j * n ) + ( n_index_lh[k] * n2 ) ] +
							curr[ i + ( j * n ) + ( n_index_rh[k] * n2 ) ] +
							deltad*source[i + n*j + n2*k] ) / 6;
                    }
                }
            }
}

double* poisson_neumann (int n, double *source, int iterations, int threads, float delta)
{
    if (debug)
    {
        long number_of_processors = sysconf(_SC_NPROCESSORS_ONLN);

        printf ("Starting solver with:\n"
               "n = %i\n"
               "iterations = %i\n"
               "threads = %i\n"
               "delta = %f\n"
               "CPU Cores Available: %ld\n",
               n, iterations, threads, delta, number_of_processors);
    }

	// Generate indexing arrays n_index_lh, n_index_rh
    int *n_index_lh = (int*)calloc (n, sizeof (int));
    int *n_index_rh = (int*)calloc (n, sizeof (int));
	// int n_index_lh[n];
	// int n_index_rh[n];
	
	for ( int i = 0; i < n; i++ )
	{
		if ( ( i != 0 ) && ( i != ( n - 1 ) ) )
		{
			n_index_lh[i] = i - 1;
			n_index_rh[i] = i + 1;
		}
		else if ( i == 0 )
		{
			n_index_lh[i] = 1;
			n_index_rh[i] = 1;
		}
		else if ( i == ( n - 1 ) )
		{
			n_index_lh[i] = n - 2;
			n_index_rh[i] = n - 2;	
		} 
	}
	
	int xl = 0;
	int xr = 0;
	int yl = 0;
	int yr = 0;
	int zl = 0;
	int zr = 0;
	
	int n2 = n*n;
	float deltad = -delta*delta;
	

    // Allocate some buffers to calculate the solution in
    double *curr = (double*)calloc (n * n * n, sizeof (double));
    double *next = (double*)calloc (n * n * n, sizeof (double));

    // Ensure we haven't run out of memory
    if (curr == NULL || next == NULL)
    {
        fprintf (stderr, "Error: ran out of memory when trying to allocate %i sized cube\n", n);
        exit (EXIT_FAILURE);
    }

    // TODO: solve Poisson's equation for the given inputs
    double* temp;
    // Storage for the thread handles and arguments
    // will exist for the entire lifetime of the program.
    pthread_t threads_arr[threads];
    WorkerArgs args[threads];
    // n is the mesh size; n^3
    // defining x, y, z ordering
    float percentage = 0.0;
    float iterations_f = iterations;
    float iteration_f = 0.0;


    for (int itr = 0; itr < iterations; itr++)
    {
        if ( itr != 0 )
        {
            temp = curr;
            curr = next;
            next = temp;
        }
        for (int i = 0; i < threads; i++)
        {
            // Fill in the arguments to the worker

            args[i].thread_id = i;
            //fprintf (stderr, "Thread: %i\n", args[i].thread_id);
            args[i].start = (n * i) / threads;
            //fprintf (stderr, "Start Index: %i\n", args[i].start);
            args[i].end = ((i+1)*n) / threads;
            //fprintf (stderr, "End Index: %i\n", args[i].end);
            args[i].n = n;
            args[i].n2 = n2;
            args[i].delta = delta;
            args[i].deltad = deltad;
            args[i].curr = curr;
            args[i].source = source;
            args[i].next = next;
            args[i].n_index_lh = n_index_lh;
	        args[i].n_index_rh = n_index_rh;
            
            
            // Create the worker thread
            if (pthread_create (&threads_arr[i], NULL, &worker, &args[i]) != 0)
            {
                fprintf (stderr, "Error creating worker thread!\n");
            }

            //fprintf (stderr, "Creating Thread %i\n", i);
        }  
            // Wait for all the threads to finish using join ()
        for (int i = 0; i < threads; i++)
        {
            //fprintf (stderr, "Waiting for thread %i\n", i);
            pthread_join (threads_arr[i], NULL);
        }    

        if (debug)
        {
            iteration_f = itr;
            if (itr % 10 == 0)
            {
            printf("\rLoading %.0f",((iteration_f/iterations_f)*100.0));
            fflush(stdout);
          
            }
        }

    }
    

    temp = curr;
    curr = next;
    next = temp;

    // Free one of the buffers and return the correct answer in the other.
    // The caller is now responsible for free'ing the returned pointer.
    free (next);

    if (debug)
    {
        printf("\rLoading 100\n");
        printf ("Finished solving.\n");
    }

    return curr;
}


int main (int argc, char **argv)
{
    // Default settings for solver
    int iterations = 300;
    int n = 7;
    int threads = 1;
    float delta = 1;
    double start_time, end_time;
          

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

    source[(n * n * n) / 2] = 1;

    start_time = my_clock();
    double *result = poisson_neumann (n, source, iterations, threads, delta);
    end_time = my_clock();

    if (debug)
    { 
		printf("%lf", end_time-start_time);
	}
	
	printf("%lf", end_time-start_time);

    // Print out the middle slice of the cube for validation
    for (int x = 0; x < n; ++x)
    {
        for (int y = 0; y < n; ++y)
        {
            // printf ("%0.5f ", result[((n / 2) * n + y) * n + x]);
        }
        // printf ("\n");
    }

    free (source);
    free (result);

    return EXIT_SUCCESS;
}
