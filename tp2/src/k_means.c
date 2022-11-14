#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <math.h>
#include<omp.h>
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
void inicializa(float *pointX, float *pointY, float *centerX, float *centerY, int *cluster_size) {
    srand(10);
    for(int i = 0; i < N; i++) {        //assigns random values to the coordinates
        pointX[i] = (float)rand()/ RAND_MAX;
        pointY[i] = (float)rand()/ RAND_MAX;
    }
    for(int i = 0; i < K; i++) {    // assigns the first K points as the initial centers
        centerX[i] = pointX[i];
        centerY[i] = pointY[i];
        cluster_size[i] = 0;
    }
}


/**
 * @brief Reevaluate centroides. Calculates the mean of each cluster and assigns that value to the cluster center
 * 
 * @param pointX X coordinate of the point
 * @param pointY Y coordinate of the point
 * @param centerX X coordinate of the center
 * @param centerY Y coordinate of the center
 * @param cluster_size Size of the cluster
 */
//void reevaluate_centers(int *cluster, float *pointX, float *pointY, float *centerX, float *centerY, int *cluster_size) {
//    // initialize variables to 0
//    float sum_x[K], sum_y[K];
//    for(int i = 0; i < K; i++){
//        sum_x[i] = 0;
//        sum_y[i] = 0;
//        cluster_size[i] = 0;
//    }
//    // sum the coordinates of each cluster
//    #pragma omp parallel for reduction(+:sum_x[:K], sum_y[:K], cluster_size[:K])
//    for(int i = 0; i < N; i++){
//        sum_x[cluster[i]] += pointX[i];
//        sum_y[cluster[i]] += pointY[i];
//        cluster_size[cluster[i]]++;
//    }
//
//    // calculate the center of each cluster with the mean of all the points in the cluster
//    for(int i = 0; i < K; i++){
//        centerX[i] = sum_x[i] / cluster_size[i];
//        centerY[i] = sum_y[i] / cluster_size[i];
//    }
//}

void reevaluate_centers(float *centerX, float *centerY, int *cluster_size, float *sumX, float *sumY) {
    #pragma omp simd
    for(int i = 0; i < K; i++){
        centerX[i] = sumX[i] / cluster_size[i];
        centerY[i] = sumY[i] / cluster_size[i];
    }
}

void initialize_centers(float *centerX, float *centerY, int* size){
    for(int i = 0; i < K; i++) {    // assigns the first K points as the initial centers
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
int cluster_points(int* cluster, float *pointX, float *pointY, float *centerX, float *centerY, int *size) {
    int changed = 0;
    float sum_x[K], sum_y[K];
    initialize_centers(sum_x, sum_y, size);
    #pragma omp parallel for reduction(+:sum_x[:K], sum_y[:K], size[:K], changed)
    for(int i = 0; i < N; i++) {
        //calculates the distance to the center of each cluster and saves the cluster with the smallest distance
        float min_dist = distance(pointX[i], pointY[i], centerX[0], centerY[0]);
        int min_cluster = 0;
        for(int j = 1; j < K; j++) {
            float dist = distance(pointX[i], pointY[i], centerX[j], centerY[j]);
            if(dist < min_dist) {
                min_dist = dist;
                min_cluster = j;
            }
        }
        //Verify if the cluster has changed and if so, change the cluster and the changed variable
        if(cluster[i] != min_cluster) {
            changed = 1;
            cluster[i] = min_cluster;
        }
        sum_x[min_cluster] += pointX[i];
        sum_y[min_cluster] += pointY[i];
        size[min_cluster]++;
    }
    //reevaluate the centers
    reevaluate_centers(centerX, centerY, size, sum_x, sum_y);
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
void k_means() {
    //Initialize variables
    int size[K];
    float centerX[K], centerY[K];
    float *pointsX = (float *)malloc(N * sizeof(float));
    float *pointsY = (float *)malloc(N * sizeof(float));
    int *cluster = (int *)malloc(N * sizeof(int));
    inicializa(pointsX, pointsY, centerX, centerY, size);

    // start iteration counter
    int iter = 0;
    
    //while(cluster_points(cluster, pointsX, pointsY, centerX, centerY, size) == 1) {
    while (iter < 20){
        cluster_points(cluster, pointsX, pointsY, centerX, centerY, size);    
        iter++;
    }
    
    print_clusters(iter, centerX, centerY, size);

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
    if (argc == 3 || argc == 4) {
        N = atoi(argv[1]);
        K = atoi(argv[2]);
        if (argc == 4) {
            omp_set_num_threads(atoi(argv[3]));
        }
        else{
            omp_set_num_threads(1);
        }
    }else{
        printf("Usage: %s <Number of points> <Number of clusters> <Number of threads>\n", argv[0]);
        exit(1);
    }
    k_means();
    return 0;
}
