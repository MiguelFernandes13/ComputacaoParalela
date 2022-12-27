#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <math.h>
#include<omp.h>
#include<mpi.h>
#include "../include/utils.h"

int N = 10000000;
int K = 4;

/**
 * @brief Initializes cluster_point struct and initializes with a random value for its center
 * 
 * @param pointX X coordinate of the point
 * @param pointY Y coordinate of the point
 * @param centerX X coordinate of the center
 * @param centerY Y coordinate of the center
 * @param cluster_size Size of the cluster
*/
void inicializa(float *pointX, float *pointY, float *centerX, float *centerY, int *cluster_size, int nprocesses, int rank){
    srand(10);
    int size = N/nprocesses;
    for(int i = 0; i < size; i++) {        //assigns random values to the coordinates
        pointX[i] = (float)rand()/ RAND_MAX;
        pointY[i] = (float)rand()/ RAND_MAX;
    }
    if(rank == 0){
        for(int i = 0; i < K; i++) {    // assigns the first K points as the initial centers
            centerX[i] = pointX[i];
            centerY[i] = pointY[i];
            cluster_size[i] = 0;
        }
        for(int i = 1; i < nprocesses; i++){
            MPI_Send(&centerX, K, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&centerY, K, MPI_FLOAT, i, 1, MPI_COMM_WORLD);
            MPI_Send(&cluster_size, K, MPI_INT, i, 2, MPI_COMM_WORLD);
        }
    } else{
        float auxCenterX[K], auxCenterY[K];
        int auxClusterSize[K];
        MPI_Recv(&auxCenterX, K, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&auxCenterY, K, MPI_FLOAT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&auxClusterSize, K, MPI_INT, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for(int i = 0; i < K; i++){
            centerX[i] = auxCenterX[i];
            centerY[i] = auxCenterY[i];
            cluster_size[i] = auxClusterSize[i];
        }
    }
}


/**
 * @brief Reevaluate centroides. Calculates the mean of each cluster and assigns that value to the cluster center
 * 
 * @param centerX X coordinate of the center
 * @param centerY Y coordinate of the center
 * @param cluster_size Size of the cluster
 * @param sum_x Sum of the X coordinates of the points in the cluster
 * @param sum_y Sum of the Y coordinates of the points in the cluster
 */
void reevaluate_centers(float *centerX, float *centerY, int *cluster_size, float *sumX, float *sumY) {
    for(int i = 0; i < K; i++){
        centerX[i] = sumX[i] / cluster_size[i];
        centerY[i] = sumY[i] / cluster_size[i];
    }
}

/**
 * @brief Initializes the centers and the size of the clusters to 0
 * @param centerX array of the X coordinates of the centers
 * @param centerY array of the Y coordinates of the centers
 * @param cluster_size array of the size of the clusters
*/
void initialize_centers(float *centerX, float *centerY, int* size){
    for(int i = 0; i < K; i++) {  
        centerX[i] = 0;
        centerY[i] = 0;
        size[i] = 0;
    }
}
/**
 * @brief Calculates the distance between two points withouth using sqrt
 * 
 * @param pointX X coordinate of the point
 * @param pointY Y coordinate of the point
 * @param centerX X coordinate of the center
 * @param centerY Y coordinate of the center
 * @return float Distance between two points
 */
float distance(float pointX, float pointY, float centerX, float centerY) {
    return ((pointX - centerX) * (pointX - centerX)) + ((pointY - centerY) * (pointY - centerY));
}

/**
 * @brief Calculates new points and checks if the cluster center has changed
 * 
 * @param cluster Cluster to which the point belongs
 * @param pointX X coordinate of the point
 * @param pointY Y coordinate of the point
 * @param centerX X coordinate of the center
 * @param centerY Y coordinate of the center
 * @param cluster_size Size of the cluster
 * @return int  1 if the cluster center has changed, 0 otherwise
 */
int cluster_points(int* cluster, float *pointX, float *pointY, float *centerX, float *centerY, int *size, int nprocesses, int rank) {
    int changed = 0;
    float sum_x[K], sum_y[K];
    initialize_centers(sum_x, sum_y, size);
    int new_N = N/nprocesses;
    for(int i = 0; i < new_N; i++){
        //calculates the distance to the center of each cluster and saves the cluster with the smallest distance
        int min_cluster = 0;
        float min_dist = distance(pointX[i], pointY[i], centerX[0], centerY[0]);
        for(int j = 1; j < K; j++) {
            if(distance(pointX[i], pointY[i], centerX[j], centerY[j]) < min_dist) {
                min_cluster = j;
                min_dist = distance(pointX[i], pointY[i], centerX[j], centerY[j]);
            }
        }
        //Verify if the cluster has changed and if so, change the cluster and the changed variable
        if(cluster[i] != min_cluster) {
            changed += 1;
            cluster[i] = min_cluster;
        }
        //sum the coordinates of the points in each cluster
        sum_x[min_cluster] += pointX[i];
        sum_y[min_cluster] += pointY[i];
        size[min_cluster]++;
    }
    if (rank != 0){
        for(int i = 0; i < K; i++){
            MPI_Send(&sum_x[i], 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
            MPI_Send(&sum_y[i], 1, MPI_FLOAT, 0, 1, MPI_COMM_WORLD);
            MPI_Send(&size[i], 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
            MPI_Send(&changed, 1, MPI_INT, 0, 3, MPI_COMM_WORLD);
        }
        float aux_centerX[K], aux_centerY[K];
        int aux_changed;
        MPI_Recv(&aux_centerX, K, MPI_FLOAT, 0, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&aux_centerY, K, MPI_FLOAT, 0, 5, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&aux_changed, 1, MPI_INT, 0, 6, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for(int i = 0; i < K; i++){
            centerX[i] = aux_centerX[i];
            centerY[i] = aux_centerY[i];
        }
        changed = aux_changed;
    } else {
        for(int i = 1; i < nprocesses; i++){
            float aux_sum_x[K], aux_sum_y[K];
            int aux_size[K], aux_changed;   
            for(int j = 0; j < K; j++){
                MPI_Recv(&aux_sum_x[j], 1, MPI_FLOAT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&aux_sum_y[j], 1, MPI_FLOAT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&aux_size[j], 1, MPI_INT, i, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&aux_changed, 1, MPI_INT, i, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                sum_x[j] += aux_sum_x[j];
                sum_y[j] += aux_sum_y[j];
                size[j] += aux_size[j];
                changed += aux_changed;

            }
        }
        reevaluate_centers(centerX, centerY, size, sum_x, sum_y);
        for(int i = 1; i < nprocesses; i++){
            MPI_Send(centerX, K, MPI_FLOAT, i, 4, MPI_COMM_WORLD);
            MPI_Send(centerY, K, MPI_FLOAT, i, 5, MPI_COMM_WORLD);
            MPI_Send(&changed, 1, MPI_INT, i, 6, MPI_COMM_WORLD);
        }
    }
    return changed;
}


/**
 * @brief Prints the cluster centers
 * 
 * @param iter The iteration number
 * @param centerX X coordinate of the center
 * @param centerY Y coordinate of the center
 * @param size The size of the cluster
 */
void print_clusters(int iter, float *centerX, float *centerY, int *size) {
    for(int i = 0; i < K; i++) {
        printf("Center: (%.3f, %.3f)", centerX[i], centerY[i]);
        //printf("Center: (%.3f, %.3f) ", center[i].x, center[i].y);
        printf("Size : %d \n", size[i]);
    }
    printf("Iterations: %d \n", iter);
}

/**
 * @brief Function that joins all the steps of the k-means algorithm
 * 
 */
void k_means(int nprocesses, int rank) {
    //Initialize variables
    int size[K];
    float centerX[K], centerY[K];
    float *pointsX = (float *)malloc((N / nprocesses) * sizeof(float));
    float *pointsY = (float *)malloc((N / nprocesses) * sizeof(float));
    int *cluster = (int *)malloc((N / nprocesses) * sizeof(int));
    inicializa(pointsX, pointsY, centerX, centerY, size, nprocesses, rank);

    // start iteration counter
    int iter = 0;
    
    //while(cluster_points(cluster, pointsX, pointsY, centerX, centerY, size) == 1) {
    while (iter < 20 && cluster_points(cluster, pointsX, pointsY, centerX, centerY, size, nprocesses, rank) > 0) {
        iter++;
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0) {
        print_clusters(iter, centerX, centerY, size);
    }

    // free memory
    free(pointsX);
    free(pointsY);
    free(cluster);
}

/**
 * @brief Main function
 * 
 * @return int Returns 0 if the program runs successfully
 */
int main(int argc, char *argv[]) {
    int nprocesses, myrank;
    if (argc >= 2) {
        MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &nprocesses);
        MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

        N = atoi(argv[1]);
        K = atoi(argv[2]);
    }else{
        printf("Usage: %s <Number of points> <Number of clusters> -np <Number of processes>\n", argv[0]);
        exit(1);
    }
    k_means(nprocesses, myrank);
    MPI_Finalize();
    return 0;
}
