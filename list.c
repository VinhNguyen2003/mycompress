#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

void list_init(list_t *list) {
    list->head = NULL;
    list->size = 0;
}

void list_add(list_t *list, const char* data) {
    node_t* newnode = (node_t*)malloc(sizeof(node_t));
    strcpy(newnode->data, data);
    newnode->next = NULL;
    if (list->head == NULL) {
        list->head = newnode;
    } else {
        node_t* ptr = list->head;
        while (ptr->next != NULL) {
            ptr = ptr->next;
        }
        ptr->next = newnode;
    }
    list->size += 1;
}

int list_size(const list_t *list) {
    return list->size;
}

char *list_get(const list_t *list, int index) {
    int i = 0;
    node_t *ptr = list->head;
    if(index>list->size || index < 0){
        return NULL;
    }
    while(i!=index){
        i++;
        ptr = ptr->next;
    }
    return ptr->data;
}

void list_clear(list_t* list) {
    node_t *ptr = list->head;
    while(ptr != NULL){
        node_t *tmp = ptr;
        ptr = ptr->next;
        free(tmp);
    }
    list->head = NULL;
    list->size = 0;
    return;
}

int list_contains(const list_t *list, const char *query) {
    node_t* ptr = list->head;
    if (ptr == NULL) {
        return 0;
    }
    for (int i = 0; i < list->size; i++) {
        if (strcmp(ptr->data, query) == 0) {
            return 1;
        }
        ptr = ptr->next;
    }
    return 0; 
}

void list_print(const list_t *list) {
    int i = 0;
    node_t *current = list->head;
    while (current != NULL) {
        printf("%d: %s\n", i, current->data);
        current = current->next;
        i++;
    }
}
