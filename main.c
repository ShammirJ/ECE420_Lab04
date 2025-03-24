/*
    Parallel Implementation of Lab 4
    Author: Dang Nguyen, Shammir Jose
*/

#define LAB4_EXTEND

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include "Lab4_IO.h"
#include "timer.h"

#define EPSILON 0.00001
#define D 0.85

typedef struct node Node;

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
        fprintf(stderr, "ERROR: Cannot open meta file \'%s\'\n", meta_file);
        return -1;
    }
    fscanf(input, "%d\n", &numNodes);
    if (fclose(input) < 0) {
        fprintf(stderr, "ERROR: Cannot close meta file \'%s\'\n", meta_file);
        return -1;
    }

    if (node_init(node, 0, numNodes)) {
        fprintf(stderr, "ERROR: Cannot initialized nodes\n");
        return -1;
    }

    // Initial probability of each node
    double init_prob = 1.0f / (double) numNodes;

    if ((*r = malloc(numNodes * sizeof(double))) == NULL) {
        fprintf(stderr, "ERROR: Could not allocate memory for r\n");
        return -1;
    }
    if ((*rPre = malloc(numNodes * sizeof(double))) == NULL) {
        fprintf(stderr, "ERROR: Could not allocate memory for rPre\n");
        return -1;
    }

    for (int i = 0; i < numNodes; i++)
        (*r)[i] = init_prob;

    return numNodes;
}

/* Clean up all dynamic memories */
void __Clean__(Node *node, double *r, double *rPre, int numNodes) {
    node_destroy(node, numNodes);
    free(r);
    free(rPre);
}

int main (int argc, char* argv[]){
    Node *nodes, *node_start, *node_end;
    MPI_Comm comm;
    int numNodes;
    double *r, *rPre;
    double start, end; // Measure time
    int partition; // Work load each process will handle = numNodes / npros
    int npros, myRank; // Number of process, this process's rank
    
    /* Probabnility that the random clicker stop traversing
     * Equal: (1-D) * 1/N
     */
    double STOP_P;

    MPI_Init(NULL, NULL);
    comm = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &npros);
    MPI_Comm_rank(comm, &myRank);

    if ((numNodes = __Init__(&nodes, &r, &rPre)) <= 0) {
        if (myRank == 0)
            fprintf(stderr, "ERROR: Initialized Failure! <numNodes = %d>\n", numNodes);
        return -1;
    }

    // Sanity check
    if (nodes[numNodes - 1].num_out_links <= 0) {
        int n_outs = nodes[numNodes - 1].num_out_links;
        if (myRank == 0)
            fprintf(stderr, "ERROR: All node must have > 0 num_out_link! <node[%d] has %d\n", numNodes - 1, n_outs);
        return -1;
    }

    // Sanity check
    for (int i = 0; i < numNodes; i++) {
        double init_prob = 1.0f / (double) numNodes;
        if (r[i] != init_prob) {
            if (myRank == 0)
                fprintf(stderr, "ERROR: Invalid initial r[%d]! <expected %f, got %f>\n", i, init_prob, r[i]);
            return -1;
        }
    }

    if (myRank == 0)
        printf("\'%s\': numNodes = %d, init_prob = %f, npros = %d\n", argv[0], numNodes, *r, npros);
    
    if (myRank == 0) // Only the master process should measure time
        GET_TIME(start);
    
    const STOP_P = (1 - D) / (double) numNodes;
    
    if (numNodes <= npros) {
        partition = 1;
        node_start = nodes + myRank;
    }
    else {
        /* If numNodes is not divisible by npros, then up to numNodes % npros needs to take an extra partiton for a balance workloads */

        int mod, par;
        par = numNodes / npros;
        mod = numNodes % npros;
        partition = (myRank < mod) ? par + 1 : par;
        node_start = (myRank < mod) ? nodes + myRank * partition : nodes + mod * (partition + 1) + (myRank - mod - 1) * partition;
    }

    node_end = node_start + partition;
    int r_local[partition], rPre_local[partition];

    while (node_start < node_end) {


        node_start++;
    }

    do {
        
    }
    while (rel_error(r, rPre, numNodes) >= EPSILON);

    if (myRank == 0) { // Only the master thread should measure time and save the outputs
        GET_TIME(end);
        Lab4_saveoutput(r, numNodes, end - start);
    }
    MPI_Finalize();

    __Clean__(nodes, r, rPre, numNodes);

    return 0;
}
