#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"

struct Performance *newPerformance(){
    /* allocating memory for a new performance */
    struct Performance *newPerf = malloc(sizeof(struct Performance));
    if (newPerf == NULL){
        printf("Error: Memory could not be allocated to the new performance\n");
        exit(0);
    } /* error check */
    newPerf->reads = 0;
    newPerf->writes = 0;
    newPerf->mallocs = 0;
    newPerf->frees = 0;
    /* setting performance values to 0 */
    return(newPerf);
}

void attachNode(struct Performance *performance, struct Node **node_ptr, void *src, unsigned int width){
    /* allocating memory for a new node */
    struct Node *newNode = malloc(sizeof(struct Node));
    if (newNode == NULL){
        printf("Error: Memory for the new node could not be allocated\n");
        exit(1);
    } /* error check */

    /* allocating memory for data in node */
    newNode->data = malloc(width);
    if (newNode->data == NULL){
        printf("Error: Memory for the new node->data could not be allocated\n");
        exit(2);
    } /* error check */
    /* memcpy(newNode->data, src, width); memcpy doesnt work :( */
    for(int i = 0; i<width; i++) {
        ((unsigned char *)(newNode->data))[i] = ((unsigned char *)src)[i];
    } /* manually copying width bytes of memory */

    newNode->lt = NULL;
    newNode->gte = NULL;
    *node_ptr = newNode;
    
    performance->mallocs++;
    performance->writes++;
}

int comparNode(struct Performance *performance, struct Node **node_ptr, int (*compar)(const void *, const void *), void *target){
    performance->reads++;
    return(compar(target, (*node_ptr)->data)); /* returning compared value */
}

struct Node **next(struct Performance *performance, struct Node **node_ptr, int direction){
    if (*node_ptr == NULL){
        printf("Error: The tree is empty\n");
        exit(3);
    } /* NULL check */

    /* returning gte or lt based on direction */
    if (direction < 0) {
        performance->reads++;
        return(&((*node_ptr)->lt));
    } else {
        performance->reads++;
        return(&((*node_ptr)->gte));
    }
}

void readNode(struct Performance *performance, struct Node **node_ptr, void *dest, unsigned int width){
    if (*node_ptr == NULL){
        printf("Error: The tree is empty\n");
        exit(4);
    } /* NULL check */
    /* memcpy(&(*node_ptr)->data, dest, width); memcpy doesnt work :( */
    for(int i = 0; i<width; i++) {
        ((unsigned char *)dest)[i] = ((unsigned char *)((*node_ptr)->data))[i];
    } /* manually copying width bytes of data */
    performance->reads++;
}

void detachNode(struct Performance *performance, struct Node **node_ptr){
    if (*node_ptr == NULL){
        printf("Error: The tree is empty\n");
        exit(5);
    } /* NULL check */

    /* freeing all pointers in node_ptr */
    free((*node_ptr)->gte);
    free((*node_ptr)->lt);
    free((*node_ptr)->data);
    free(*node_ptr);
    *node_ptr = NULL;

    performance->frees++;
}

int isEmpty(struct Performance *performance, struct Node **node_ptr){
    if (*node_ptr == NULL){
        return(1);
    } else {
        return(0);
    }
}

void addItem(struct Performance *performance, struct Node **node_ptr, int (*compar)(const void *, const void *), void *src, unsigned int width){
    int direction;
    struct Node **bufferNode = node_ptr;
    while (isEmpty(performance, bufferNode) != 1){
        /* finding direction with comparNode */
        direction = comparNode(performance, bufferNode, compar, src);
        /* going next in the appropriate direction */
        bufferNode = next(performance, bufferNode, direction);
    }
    /* attatching node when appropriate destination reached */
    attachNode(performance, bufferNode, src, width);
}

void freeTree(struct Performance *performance, struct Node **node_ptr){
    /* checking if immediate lt or gte nodes are null */
    if ((*node_ptr)->gte == NULL && (*node_ptr)->lt == NULL){
        detachNode(performance, node_ptr);
    } else {
        /* freeing nodes from the gte side of the root */
        if ((*node_ptr)->gte != NULL){
            freeTree(performance, next(performance, node_ptr, 1));
        }
        /* freeing nodes from the lt side of the root */
        if ((*node_ptr)->lt != NULL){
            freeTree(performance, next(performance, node_ptr, -1));
        }
        /* detatching the last node that is left after if statements*/
        detachNode(performance, node_ptr);
    }
}

int searchItem(struct Performance *performance, struct Node **node_ptr, int (*compar)(const void *, const void *), void *target, unsigned int width){ 
    int direction;
    /* checking if node_ptr is already empty */
    if (isEmpty(performance, node_ptr) == 1){  
        return(0);
    }
    /* finding direction */
    direction = comparNode(performance, node_ptr, compar, target);
    if (direction == 0){
        /* reading node in the direction */
        readNode(performance, node_ptr, target, width);
        return(1);              
    }
    else{
        /* use recursion at the next node in the direction if not found */
        return searchItem(performance, next(performance, node_ptr, direction), compar, target, width);
    } 
}
