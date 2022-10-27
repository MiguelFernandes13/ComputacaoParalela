#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <math.h>
#include "../include/utils.h"

#define N 10000000
#define K 4

/**
 * @brief Struct to save a point that is part of the cluster
 * 
 */
struct cluster_point{   
    float x;
    float y;
    int cluster;
};

/**
 * @brief Struct that holds the coordinates of a 2D point
 * 
 */
struct point{   
    float x;
    float y;
};



/**
 * @brief Initializes cluster_point struct and initializes with a random value for its center
 * 
 * @param center The center of the cluster
 * @param cluster_size The size of the cluster
 * @return struct cluster_point* Returns a pointer to the new cluster_point struct
 */
struct cluster_point* inicializa(struct point *center, int *cluster_size) {
    struct cluster_point *A = (struct cluster_point *)malloc(N*sizeof(struct cluster_point));
    srand(10);
    for(int i = 0; i < N; i++) {        //assigns random values to the coordinates
        A[i].x = (float) rand() / RAND_MAX;
        A[i].y = (float) rand() / RAND_MAX;
    }
    for(int i = 0; i < K; i++) {    // assign cluster number and its size
        A[i].cluster = i;
        center[i].x = A[i].x;
        center[i].y = A[i].y;
        cluster_size[i] = 0;
    }
    return A;
}


/**
 * @brief Reevaluate centroides. Calculates the mean of each cluster and assigns that value to the cluster center
 * 
 * @param A The cluster_point struct
 * @param center The center of the cluster
 * @param size The size of the cluster
 */
void reevaluate_centers(struct cluster_point *A, struct point *center, int *size) {
    // initialize variables
    float sum_x[K], sum_y[K];
    for(int i = 0; i < K; i++){
        sum_x[i] = 0;
        sum_y[i] = 0;
        size[i] = 0;
    }
    // sum of coordinates in each cluster
    for(int i = 0; i < N; i++){
        sum_x[A[i].cluster] += A[i].x;
        sum_y[A[i].cluster] += A[i].y;
        size[A[i].cluster]++;
    }

    //calculates the result of the mean of the points of each cluster and assigns that value to the cluster center
    for(int i = 0; i < K; i++){
        center[i].x = sum_x[i] / size[i];
        center[i].y = sum_y[i] / size[i];
    }
}

/**
 * @brief Calculates the distance between two points withouth using sqrt
 * 
 * @param a All points
 * @param b Cluster centers
 * @return float Distance between two points
 */
float distance(struct cluster_point A, struct point center) {
    return ((A.x - center.x) * (A.x - center.x)) + ((A.y - center.y) * (A.y - center.y));
}

/**
 * @brief Calculates new points and checks if the cluster center has changed
 * 
 * @param A The cluster_point struct
 * @param center The center of the cluster
 * @param size The size of the cluster
 * @return int  1 if the cluster center has changed, 0 otherwise
 */
int cluster_points(struct cluster_point *A, struct point *center, int *size) {
    int changed = 0;
    for(int i = 0; i < N; i++) {
        float min_dist = distance(A[i], center[0]);
        int min_cluster = 0;
        //calculates the distance to the center of each cluster and saves the cluster with the smallest distance
        for(int j = 1; j < K; j++) {
            float dist = distance(A[i], center[j]);
            if(dist < min_dist) {
                min_dist = dist;
                min_cluster = j;
            }
        }
        //Verify if the cluster has changed
        if(A[i].cluster != min_cluster) {
            A[i].cluster = min_cluster;
            changed = 1;
        }
    }
    //reevaluate the centers
    reevaluate_centers(A, center, size);
    return changed;
}


/**
 * @brief Prints the cluster centers
 * 
 * @param iter The iteration number
 * @param center The center of the cluster
 * @param size The size of the cluster
 */
void print_clusters(int iter, struct point *center, int *size) {
    for(int i = 0; i < K; i++) {
        printf("Center: (%.3f, %.3f) ", center[i].x, center[i].y);
        printf("Size : %d \n", size[i]);
    }
    printf("Iterations: %d \n", iter);
}


/**
 * @brief Function that joins all the steps of the k-means algorithm
 * 
 */
void k_means() {
    //Initialize clusters
    int size[K];
    struct point center[K];
    struct cluster_point* A = inicializa(center, size);

    // start iteration counter
    int iter = 0;
    
    // while the cluster center has changed
    while(cluster_points(A, center, size) == 1) {
        iter++;
    }
    
    // print results whenever the cluster has not changed
    print_clusters(iter, center, size);
    
    // release memory
    free(A);
}

/**
 * @brief Main function
 * 
 * @return int Returns 0 if the program runs successfully
 */
int main(){
    k_means();
    return 0;
}
