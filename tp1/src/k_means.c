#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <math.h>
#include "../include/utils.h"

#define N 10000000
#define K 4


struct point{
    float x;
    float y;
};

struct cluster{
    int size;
    float center[2];
    struct point *elements;
};


struct point *A;
struct cluster *clusters, *old_clusters;

void allocate()
{
    A = (struct point *)malloc(N*sizeof(struct point));
    clusters = (struct cluster *)malloc(K*sizeof(struct cluster));
    old_clusters = (struct cluster *)malloc(K*sizeof(struct cluster));
    for(int i=0;i<K;i++)
    {
        clusters[i].size = 0;
    }
}

void inicializa() {
    srand(10);
    for(int i = 0; i < N; i++) {
        A[i].x = (float) rand() / RAND_MAX;
        A[i].y = (float) rand() / RAND_MAX;
    }
    for(int i = 0; i < K; i++) {
        clusters[i].center[0] = A[i].x;
        clusters[i].center[1] = A[i].y;
    }
}

void cluster_points() {
    for(int i = 0; i < N; i++) {
        float min_dist = 1000000000.0;
        int index = 0;
        for(int j = 0; j < K; j++) {
            float dist = sqrt(pow(A[i].x - clusters[j].center[0], 2) + pow(A[i].y - clusters[j].center[1], 2));
            if(dist < min_dist) {
                min_dist = dist;
                index = j;
            }
        }
        clusters[index].elements = realloc(clusters[index].elements, (clusters[index].size + 1) * sizeof(struct point));
        clusters[index].elements[clusters[index].size].x = A[i].x;
        clusters[index].elements[clusters[index].size].y = A[i].y;
        clusters[index].size++;
    }
}

void clear_clusters(){
    for(int i = 0; i < K; i++){
        if(clusters[i].size > 0){
            clusters[i].size = 0;
        }
    }
}

void reevaluate_centers() {
    for(int i = 0; i < K; i++) {
        float sum_x = 0.0;
        float sum_y = 0.0;
        for(int j = 0; j < clusters[i].size; j++) {
            sum_x += clusters[i].elements[j].x;
            sum_y += clusters[i].elements[j].y;
        }
        clusters[i].center[0] = sum_x / clusters[i].size;
        clusters[i].center[1] = sum_y / clusters[i].size;
    }
}

int has_converged() {
    for(int i = 0; i < K; i++) {
        for(int j = 0; j < clusters[i].size; j++) {
            if(clusters[i].elements[j].x != old_clusters[i].elements[j].x || 
               clusters[i].elements[j].y != old_clusters[i].elements[j].y) {
                return 0;
            }
        }
    }
    return 1;
}

void clone_cluster(){
    for(int i = 0; i < K; i++) {
        old_clusters[i].elements = realloc(old_clusters[i].elements, clusters[i].size * sizeof(struct point));
        for(int j = 0; j < clusters[i].size; j++) {
            old_clusters[i].elements[j].x = clusters[i].elements[j].x;
            old_clusters[i].elements[j].y = clusters[i].elements[j].y;
        }
    }
}


void print_clusters(int iter) {
    for(int i = 0; i < K; i++) {
        printf("Center: (%.3f, %.3f) ", clusters[i].center[0], clusters[i].center[1]);
        printf("Size : %d \n", clusters[i].size);
    }
    printf("Iterations: %d \n", iter);
}

void free_clusters(){
    free(A);
    for(int i = 0; i < K; i++) {
        free(clusters[i].elements);
    }
    free(clusters);
    //for (int i = 0; i < K; i++) {
    //    free(old_clusters[i].elements);
    //} 
    //free(old_clusters);
}


int main()
{
    int iter = 0;
    //Iniciar clusters
    allocate();
    inicializa();
    cluster_points();
    reevaluate_centers();
    clone_cluster();
    clear_clusters();
    //Primeira iteração
    cluster_points();
    reevaluate_centers();
    iter++;
    //Iterar até convergir
    while (has_converged() == 0) {
        clone_cluster();
        clear_clusters();
        cluster_points();
        reevaluate_centers();
        iter++;
    }
    print_clusters(iter);
    free_clusters();
    
    return 0;
}