/*
    Serial Implementation of Lab 4
*/

#define LAB4_EXTEND

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "Lab4_IO.h"
#include "timer.h"

#define EPSILON 0.00001
#define DAMPING_FACTOR 0.85

#define DEBUG 1 // Set to 1 to enable debug mode. Turns this on if all you want to test is time to avoid taking time printing to data_output

/* Append the run time to an output file, used for time debug mode */
void Append_Time(double time) {
    char outf[50];
    sprintf(outf, "s_out.log");
    
    FILE *t = fopen(outf, "a");
    if (t == NULL) {
        fprintf(stderr, "Could not open file \'%s\'\n", outf);
        return;
    }

    fprintf(t, "%.10f\n", time);

    fclose(t);
}


int main (int argc, char* argv[]){
    // instantiate variables
    struct node *nodehead;
    int nodecount;
    double *r, *r_pre;
    int i, j, k, m;
    double start, end;
    FILE *ip;
    /* INSTANTIATE MORE VARIABLES IF NECESSARY */

    
    // load data 
    if ((ip = fopen("data_input_meta","r")) == NULL) {
        printf("Error opening the data_input_meta file.\n");
        return 253;
    }
    fscanf(ip, "%d\n", &nodecount);
    fclose(ip);
    if (node_init(&nodehead, 0, nodecount)) return 254;
    
    // initialize variables
    r = malloc(nodecount * sizeof(double));
    r_pre = malloc(nodecount * sizeof(double));
    
    for ( i = 0; i < nodecount; ++i)
        r[i] = 1.0 / nodecount;
    /* INITIALIZE MORE VARIABLES IF NECESSARY */

    GET_TIME(start);

    // core calculation
    do{
        /* IMPLEMENT ITERATIVE UPDATE */

        memcpy(r_pre, r, nodecount * sizeof(double)); // r[t-1] = r[t]
        
        for (i = 0; i < nodecount; i++) {
            r[i] = 0.0f;
            k = nodehead[i].num_in_links;
            for (j = 0; j < k; j++) {
                m = nodehead[nodehead[i].inlinks[j]].num_out_links;
                r[i] += (r_pre[nodehead[i].inlinks[j]] / m);
            }
            r[i] *= DAMPING_FACTOR;
            r[i] += ((1 - DAMPING_FACTOR) / nodecount);
        }

    }while(rel_error(r, r_pre, nodecount) >= EPSILON);

    GET_TIME(end);

    if (!DEBUG)
        Lab4_saveoutput(r, nodecount, end - start);

    if (DEBUG)
        Append_Time(end - start);

    // post processing
    node_destroy(nodehead, nodecount);
    free(r); free(r_pre);
    return 0;
}
