#include<stdio.h>
#include<stdlib.h>
#include<string.h>


#define N 1000
#define K 4

struct cluster{
    int size;
    float center[2];
    float *elements;
};



float *A;
struct cluster *clusters, *old_clusters;

void allocate()
{
    A = (float *)malloc(2*N*sizeof(float));
    clusters = (struct cluster *)malloc(K*sizeof(struct cluster));
    for(int i=0;i<K;i++)
    {
        clusters[i].size = 0;
        clusters[i].elements = (float *)malloc(N*sizeof(float));
    }
}

void inicializa() {
    srand(10);
    for(int i = 0; i < N; i+=2) {
        A[i] = (float) rand() / RAND_MAX;
        A[i+1] = (float) rand() / RAND_MAX;
    }
    int j = 0;
    for(int i = 0; i < K; i++) {
        clusters[i].center[0] = A[j];
        clusters[i].center[1] = A[j+1];
        j += 2;
    }
}

void cluster_points() {
    for(int i = 0; i < N; i+=2) {
        float min_dist = 1000000000.0;
        int index = 0;
        for(int j = 0; j < K; j++) {
            float dist = (A[i] - clusters[j].center[0]) * (A[i] - clusters[j].center[0]) + 
                         (A[i+1] - clusters[j].center[1]) * (A[i+1] - clusters[j].center[1]);
            if(dist < min_dist) {
                min_dist = dist;
                index = j;
            }
        }
        clusters[index].elements[clusters[index].size++] = A[i];
        clusters[index].elements[clusters[index].size++] = A[i+1];
    }
}

void clear_clusters(){
    for(int i = 0; i < K; i++){
        if(clusters[i].size > 0){
            clusters[i].size = 0;
            memset(clusters[i].elements, '\0', N*sizeof(float));
        }
    }
}

void reevaluate_centers() {
    for(int i = 0; i < K; i++) {
        float sum_x = 0.0;
        float sum_y = 0.0;
        for(int j = 0; j < clusters[i].size; j+=2) {
            sum_x += clusters[i].elements[j];
            sum_y += clusters[i].elements[j+1];
        }
        clusters[i].center[0] = sum_x / clusters[i].size;
        clusters[i].center[1] = sum_y / clusters[i].size;
    }
}

int has_converged() {
    for(int i = 0; i < K; i++) {
        for(int j = 0; j < clusters[i].size; j+=2) {
            if(clusters[i].elements[j] != old_clusters[i].elements[j] || 
               clusters[i].elements[j+1] != old_clusters[i].elements[j+1]) {
                return 0;
            }
        }
    }
    return 1;
}

void print_clusters() {
    for(int i = 0; i < K; i++) {
        printf("Cluster %d \n: ", i);
        printf("Cluster center: (%f, %f) \n", clusters[i].center[0], clusters[i].center[1]);
        printf("Cluster size %d \n", clusters[i].size);
    }
}


int main()
{
    allocate();
    inicializa();
    cluster_points();
    reevaluate_centers();
    old_clusters = (struct cluster *)malloc(K*sizeof(struct cluster));
    memcpy(old_clusters, clusters, K*sizeof(struct cluster));
    for(int i=0;i<K;i++)
    {
        old_clusters[i].elements = (float *)malloc(clusters[i].size*sizeof(float));
        memcpy(old_clusters[i].elements, clusters[i].elements, sizeof(float) * clusters[i].size);
    }
    clear_clusters();
    cluster_points();
    reevaluate_centers();
    while (has_converged(old_clusters) == 0) {
        clear_clusters();
        cluster_points();
        reevaluate_centers();
        memcpy(old_clusters, clusters, K*sizeof(struct cluster));
        for(int i=0;i<K;i++)
        {
            old_clusters[i].elements = (float *)malloc(clusters[i].size*sizeof(float));
            memcpy(old_clusters[i].elements, clusters[i].elements, sizeof(float) * clusters[i].size);
        }
    }
    print_clusters();
    //free(clusters);
    //free(A);
    return 0;
}