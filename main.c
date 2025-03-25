/*
    Parallel Implementation of Lab 4
    Author: Dang Nguyen, Shammir Jose
*/

#define LAB4_EXTEND

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <mpi.h>
#include "Lab4_IO.h"
#include "timer.h"

#define EPSILON 0.00001
#define D 0.85

#define DEBUG 0 // Set to 1 to enable debug mode. Turns this on if all you want to test is time to avoid taking time printing to data_output

typedef struct node Node;

MPI_Comm comm;
int npros, myRank; // Number of process, this process's rank

/* Intialize all required data arraays 
 * 
 * Para: node -> The node map to be returned to
 *       r    -> The to be returned probability of each node at time t
 *       rPre -> The to be returned probability of each node at time t-1
 * 
 * Return: The number of nodes if sucess, a negative number otherwise
 */
int __Init__(Node **node, double **r, double **rPre) {
    const char *meta_file = "data_input_meta";
    int numNodes;
    FILE *input;

    if ((input = fopen(meta_file, "r")) == NULL) {
        fprintf(stderr, "ERROR pros %d: Cannot open meta file \'%s\'\n", myRank, meta_file);
        MPI_Abort(comm, -1);
    }
    fscanf(input, "%d\n", &numNodes);
    if (fclose(input) < 0) {
        fprintf(stderr, "ERROR pros %d: Cannot close meta file \'%s\'\n", myRank, meta_file);
        MPI_Abort(comm, -1);
    }

    if (node_init(node, 0, numNodes)) {
        fprintf(stderr, "ERROR pros %d: Cannot initialized nodes\n", myRank);
        MPI_Abort(comm, -1);
    }

    // Initial probability of each node
    double init_prob = 1.0f / (double) numNodes;

    if (myRank == 0) {// Only the master process needs a full array of r
        if ((*r = malloc(numNodes * sizeof(double))) == NULL) {
            fprintf(stderr, "ERROR pros %d: Could not allocate memory for r\n", myRank);
            MPI_Abort(comm, -1);
        }

        for (int i = 0; i < numNodes; i++)
            (*r)[i] = init_prob;
    }

    if ((*rPre = malloc(numNodes * sizeof(double))) == NULL) {
        fprintf(stderr, "ERROR pros %d: Could not allocate memory for rPre\n", myRank);
        MPI_Abort(comm, -1);
    }

    return numNodes;
}

/* Clean up all dynamic memories */
void __Clean__(Node *node, double *r, double *rPre, int numNodes) {
    node_destroy(node, numNodes);
    free(rPre);
    if (myRank == 0)
        free(r);
}

/* Append the run time to an output file, used for time debug mode */
void Append_Time(double time) {
    char outf[50];
    sprintf(outf, "p_out%d.log", npros);
    
    FILE *t = fopen(outf, "a");
    if (t == NULL) {
        fprintf(stderr, "Could not open file \'%s\'\n", outf);
        return;
    }

    fprintf(t, "%.10f\n", time);

    fclose(t);
}

int main (int argc, char* argv[]){
    Node *nodes, *node_start, *node_end, *node_i;
    int numNodes;
    
    double *r, *rPre, *r_local;
    double start, end; // Measure time
    double len_rPre_sq, len_diff_local_sq;
    double diff_sq;
    
    int partition, start_index; // Work load each process will handle = numNodes / npros, start index of each process in all shared arrays
    int par, mod; // numNodes / npros and numNodes % npros
    int num_in_link; // Number of outgoing edges that lead to a node
    int l_j;

    /* Probabnility that the random clicker stop traversing
     * Equal: (1-D) * 1/N
     */
    double STOP_P;

    /* Extra variables */
    int i, j;

    MPI_Init(NULL, NULL);
    comm = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &npros);
    MPI_Comm_rank(comm, &myRank);

    if ((numNodes = __Init__(&nodes, &r, &rPre)) <= 0) {
        fprintf(stderr, "ERROR pros %d: Initialized Failure! <numNodes = %d>\n", myRank, numNodes);
        MPI_Abort(comm, -1);
    }

    // Sanity check
    if (nodes[numNodes - 1].num_out_links <= 0) {
        int n_outs = nodes[numNodes - 1].num_out_links;
        fprintf(stderr, "ERROR pros %d: All node must have > 0 num_out_link! <node[%d] has %d\n", myRank, numNodes - 1, n_outs);
        MPI_Abort(comm, -1);
    }

    // Sanity check
    if (myRank == 0)
        for (int i = 0; i < numNodes; i++) {
            double init_prob = 1.0f / (double) numNodes;
            if (r[i] != init_prob) {
                fprintf(stderr, "ERROR pros %d: Invalid initial r[%d]! <expected %.15f, got %.15f>\n", myRank, i, init_prob, r[i]);
                MPI_Abort(comm, -1);
            }
        }

    if (myRank == 0)
        printf("\'%s\': numNodes = %d, init_prob = %f, npros = %d\n", argv[0], numNodes, *r, npros);
    
    if (myRank == 0) // Only the master process should measure time
        GET_TIME(start);
    
    STOP_P = (1 - D) / numNodes;
    
    if (numNodes <= npros) {
        partition = 1;
        start_index = myRank;
    }
    else {
        /* If numNodes is not divisible by npros, then up to numNodes % npros needs to take an extra partiton for a balance workloads */

        par = numNodes / npros;
        mod = numNodes % npros;
        partition = (myRank < mod) ? par + 1 : par;
        start_index = (myRank < mod) ? myRank * partition : mod * (partition + 1) + (myRank - mod) * partition;

        // Debug
        printf("Process %d: partition = %d, start_index = %d\n", myRank, partition, start_index);
    }

    node_start = nodes + start_index;
    node_end = node_start + partition - 1; // -1 to avoid seg fault
    
    if ((r_local = malloc(partition * sizeof(double))) == NULL) {
        fprintf(stderr, "ERROR pros %d: Cannot allocate r_local\n", myRank);
        MPI_Abort(comm, -1);
    }

    do {
        if (myRank == 0) 
            memcpy(rPre, r, numNodes * sizeof(double));
        else
            diff_sq = 100; // Assigned some abitrary number for the non-master process
        MPI_Bcast((void*) rPre, numNodes, MPI_DOUBLE, 0, comm); // Broadcasr r(t-1) to every process

        /* Calculate ||rPre||^2 */
        len_rPre_sq = 0;
        for (i = 0; i < numNodes; i++)
            len_rPre_sq += rPre[i] * rPre[i];

        len_diff_local_sq = 0;
        for (node_i = node_start, i = 0; node_i <= node_end; node_i++, i++) {
            r_local[i] = 0.0f;
            num_in_link = node_i->num_in_links;
            
            for (j = 0; j < num_in_link; j++) {
                l_j = nodes[node_i->inlinks[j]].num_out_links;
                r_local[i] += rPre[node_i->inlinks[j]] / l_j;
            }

            r_local[i] *= D;
            r_local[i] += STOP_P;

            len_diff_local_sq += ((r_local[i] - rPre[start_index + i]) * (r_local[i] - rPre[start_index + i])) / len_rPre_sq;
        }

        // Need to update to using MPI_Gatherv()
        MPI_Gather((void*) r_local, partition, MPI_DOUBLE, (void*) r, partition, MPI_DOUBLE, 0, comm);
        MPI_Reduce((void*) &len_diff_local_sq, (void*) &diff_sq, 1, MPI_DOUBLE, MPI_SUM, 0, comm);
    }
    while (diff_sq >= EPSILON * EPSILON);

    if (myRank == 0) { // Only the master process should measure time and save the outputs
        GET_TIME(end);
        if (!DEBUG)
            Lab4_saveoutput(r, numNodes, end - start);
        if (DEBUG)
            Append_Time(end - start);
        MPI_Abort(comm, 0); // Kill all other process
    }

    MPI_Finalize();

    free(r_local);
    __Clean__(nodes, r, rPre, numNodes);

    return 0;
}
