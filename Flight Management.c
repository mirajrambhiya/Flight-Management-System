#include<stdio.h>
#include<stdlib.h>

static int i = 1;

typedef struct time_tag{
    int hr;
    int min;
}TIME;

typedef struct flight_tag{
    int flight_id;
    TIME* dept_time;
    TIME* arr_time;
    struct flight_tag* next;
}flight_plan;

typedef struct bucket_tag{
    int bucket_id;
    flight_plan* flight_list;
    TIME* start_ETA;
    TIME* end_ETA;
    struct bucket_tag* next;
}bucket;

int maxTime(TIME* a, TIME* b){
    int retVal = 0;
    if(a -> hr > b -> hr){
        retVal = -1;
    }
    else if(b -> hr > a -> hr){
        retVal = 1;
    }
    else if(a -> min > b -> min){
        retVal = -1;
    }
    else if(b -> min > a -> min){
        retVal = 1;
    }

    return retVal;
}

bucket* insert(bucket* main_head, flight_plan* flight){
    if(main_head == NULL || main_head -> start_ETA -> hr > flight -> arr_time -> hr){
        bucket* newBucket = (bucket*)malloc(sizeof(bucket));
        newBucket -> start_ETA = (TIME*)malloc(sizeof(TIME));
        newBucket -> end_ETA = (TIME*)malloc(sizeof(TIME));


        newBucket -> bucket_id = i++;
        newBucket -> flight_list = NULL;
        newBucket -> start_ETA -> hr= flight -> arr_time -> hr;
        newBucket -> start_ETA -> min = 0;
        newBucket -> end_ETA -> hr= ((flight -> arr_time -> hr) + 1) % 24;
        newBucket -> end_ETA -> min = 0;
        newBucket -> next = main_head;

        newBucket -> flight_list = flight;
        main_head = newBucket;
        printf("Created and inserted a new Bucket with ETA %d:%d\n", newBucket -> start_ETA -> hr, newBucket -> end_ETA -> min);
    }
    else{
        bucket* prev = NULL;
        bucket* temp = main_head;

        while(temp != NULL && temp -> start_ETA -> hr < flight -> arr_time -> hr){
            prev = temp;
            temp = temp -> next;
        }

        if(temp == NULL || temp -> start_ETA -> hr > flight -> arr_time -> hr){
            bucket* newBucket = (bucket*)malloc(sizeof(bucket));
            newBucket -> start_ETA = (TIME*)malloc(sizeof(TIME));
            newBucket -> end_ETA = (TIME*)malloc(sizeof(TIME));


            newBucket -> bucket_id = i++;
            newBucket -> flight_list = NULL;
            newBucket -> start_ETA -> hr= flight -> arr_time -> hr;
            newBucket -> start_ETA -> min = 0;
            newBucket -> end_ETA -> hr= ((flight -> arr_time -> hr) + 1) % 24;
            newBucket -> end_ETA -> min = 0;
            newBucket -> next = main_head;

            newBucket -> flight_list = flight;
            prev -> next = newBucket;
            newBucket -> next = temp;
            printf("Created and inserted a new Bucket with ETA %d:%d\n", newBucket -> start_ETA -> hr, newBucket -> end_ETA -> min);
        }
        else if(temp -> start_ETA -> hr == flight -> arr_time -> hr){
            flight_plan* f_temp = temp -> flight_list;
            flight_plan* f_prev = NULL;

            if(maxTime(f_temp -> dept_time, flight -> dept_time) == -1){
                flight -> next = f_temp;
                temp -> flight_list = flight;
            }
            else{
                while(f_temp != NULL && maxTime(f_temp -> dept_time, flight -> dept_time) != -1){
                    f_prev = f_temp;
                    f_temp = f_temp -> next;
                }

                f_prev -> next = flight;
                flight -> next = f_temp;
            }
            printf("Inserted in existing Bucket with ETA %d:%d\n", temp -> start_ETA -> hr, temp -> end_ETA -> min);
        }
    }

    return main_head;
}

void show_status(bucket* main_head, int id){
    int found = 0;
    bucket* temp = main_head;
    flight_plan* found_node;

    while(temp != NULL && found == 0){
        flight_plan* f_temp = temp -> flight_list;

        while(f_temp != NULL && found == 0){
            if(f_temp -> flight_id == id){
                found = 1;
                found_node = f_temp;
            }
            else{
                f_temp = f_temp -> next;
            }
        }

        temp = temp -> next;
    }

    if(found == 0){
        printf("Sorry, the flight with given flight id is not in the data \n");
    }
    else{
        printf("Found the flight, the details are : \n");
        printf("Flight ID : %d     Flight Departure Time : %d:%d    Flight Arrival Time : %d:%d \n", found_node -> flight_id, found_node -> dept_time -> hr, found_node -> dept_time -> min, found_node -> arr_time -> hr, found_node -> arr_time -> min);
    }
}

bucket* delete(bucket* main_head, int id){
    int deleted = 0;
    bucket* temp = main_head;
    bucket* prev = NULL;

    while(temp != NULL && deleted == 0){
        flight_plan* f_temp = temp -> flight_list;
        flight_plan* f_prev = NULL;

        while(f_temp != NULL && deleted == 0){
            if(f_temp -> flight_id == id){
                deleted = 1;
                flight_plan* toBeDeleted = f_temp;
                f_temp = f_temp -> next;
                free(toBeDeleted -> arr_time);
                free(toBeDeleted -> dept_time);
                free(toBeDeleted);
                if(f_prev == NULL){
                    temp -> flight_list = f_temp;
                }
                else{
                    f_prev -> next = f_temp;
                }

                if(f_prev == NULL && f_temp == NULL){
                    //there is no flight list remaining in the bucket -> bucket can be deleted
                    bucket* toBeDeleted = temp;
                    if(prev == NULL){
                        //this is the first bucket of the main head
                        temp = temp -> next;
                        free(toBeDeleted -> start_ETA);
                        free(toBeDeleted -> end_ETA);
                        free(toBeDeleted);
                        main_head = temp;
                    }
                    else{
                        //this is not the first bucket 
                        temp = temp -> next;
                        free(toBeDeleted -> start_ETA);
                        free(toBeDeleted -> end_ETA);
                        free(toBeDeleted);
                        prev -> next = temp;
                    }
                    
                }
            }
            else{
                f_prev = f_temp;
                f_temp = f_temp -> next;
            }
        }

        if(deleted == 0){
            prev = temp;
            temp = temp -> next;
        }
    }

    if(deleted == 0){
        printf("The flight with given id is not present in the data set \n");
    }
    else{
        printf("Deleted the flight with given flight id \n");
    }

    return main_head;
}

bucket* getNewHead(bucket* main_head, TIME* new_time){
    bucket* prev = NULL, *temp = main_head, *newHead = main_head;

    if(temp == NULL || maxTime(temp -> start_ETA, new_time) == -1){
        //when the bucket list is empty or when the bucket head's start ETA is greater than the new input time 
        newHead = (bucket*)malloc(sizeof(bucket));
        newHead -> start_ETA = (TIME*)malloc(sizeof(TIME));
        newHead -> end_ETA = (TIME*)malloc(sizeof(TIME));
        newHead -> bucket_id = i++;
        newHead -> flight_list = NULL;
        newHead -> start_ETA -> hr = new_time -> hr;
        newHead -> start_ETA -> min = new_time -> min;
        newHead -> end_ETA -> hr = (new_time -> hr + 1) % 24;
        newHead -> end_ETA -> min = new_time -> min;
        newHead -> next = temp;

        return newHead;
    }

    while(temp != NULL && maxTime(temp -> start_ETA, new_time) == 1){
        //traversing till we get the appropiate position of the new head
        prev = temp;
        temp = temp -> next;
    }

    if(temp == NULL){
        //end of the list reached
        newHead = (bucket*)malloc(sizeof(bucket));
        newHead -> bucket_id = i++;
        newHead ->flight_list = NULL;
        newHead -> start_ETA = (TIME*)malloc(sizeof(TIME));
        newHead -> end_ETA = (TIME*)malloc(sizeof(TIME));
        newHead -> start_ETA -> hr = new_time -> hr;
        newHead -> start_ETA -> min = new_time -> min;
        newHead -> end_ETA -> hr = (new_time -> hr + 1) % 24;
        newHead -> end_ETA -> min = new_time -> min;

        newHead -> next = main_head;
        main_head = newHead;
    }
    else if(temp != NULL && maxTime(temp -> start_ETA, new_time) == -1){
        //reached in between of the list but the start ETA of temp is greater than the new_time
        newHead = (bucket*)malloc(sizeof(bucket));
        newHead -> start_ETA = (TIME*)malloc(sizeof(TIME));
        newHead -> end_ETA = (TIME*)malloc(sizeof(TIME));
        newHead -> bucket_id = i++;
        newHead -> flight_list = NULL;
        newHead -> start_ETA -> hr = new_time -> hr;
        newHead -> start_ETA -> min = new_time -> min;
        newHead -> end_ETA -> hr = (new_time -> hr + 1) % 24;
        newHead -> end_ETA -> min = new_time -> min;
        newHead -> next = temp;

        prev -> next = newHead;
        temp = newHead;
    }
    //if the value of maxTime(temp -> start_ETA, new_time) is 0, thius means that temp is the required newHead position 

    if(prev != NULL){
        //adjusting the newHead in order to get the appropiate bucket list
        newHead = temp;
        prev -> next = NULL;
        while(temp -> next != NULL){
            temp = temp -> next;
        }

        temp -> next = main_head;
    }

    return newHead;
}

int time_diff(TIME* a, TIME* b){
    //returns a-b
    int diff = 0;
    diff = a -> min - b -> min;
    diff += (a -> hr-b-> hr) * 60;

    return diff;
}

bucket* update(bucket* main_head){
    bucket* temp, *prev;
    while(main_head != NULL && main_head -> flight_list == NULL){
        temp = main_head;
        main_head = main_head -> next;
        printf("%d:%d  ", temp -> start_ETA -> hr, temp -> start_ETA -> min);
        free(temp -> start_ETA);
        free(temp -> end_ETA);
        free(temp);
        printf("Freed \n");
    }

    if(main_head != NULL){
        prev = NULL;
        temp = main_head;

        while(temp){
            if(temp -> flight_list == NULL){
                prev -> next = temp -> next;
                printf("%d:%d  ", temp -> start_ETA -> hr, temp -> start_ETA -> min);
                free(temp -> start_ETA);
                free(temp -> end_ETA);
                free(temp);
                printf("Freed \n");
                temp = prev -> next;
            }
            else{
                prev = temp;
                temp = temp -> next;
            }
        }
    }

    return main_head;
}

void insertSorted(bucket** buc, flight_plan* flight){
    bucket* temp = *buc;
    flight_plan* f_temp, *f_prev;

    if(temp -> flight_list == NULL){
        temp -> flight_list = flight;
    }
    else{
        f_temp = temp -> flight_list;
        f_prev = NULL;

        if(maxTime(f_temp -> dept_time, flight -> dept_time) == -1){
            flight -> next = f_temp;
            temp -> flight_list = flight;
        }
        else{
            while(f_temp && maxTime(f_temp -> dept_time, flight -> dept_time) != -1){
                f_prev = f_temp;
                f_temp = f_temp -> next;
            }

            f_prev -> next = flight;
            flight -> next = f_temp;
        }
    }

    *buc = temp;
}

bucket* reArrange(bucket* main_head, TIME* new_time){
    bucket* temp = main_head, *prev;
    
    while(temp){
        temp -> start_ETA -> min = new_time -> min;
        temp -> end_ETA -> min = new_time -> min;
        temp = temp -> next;
    }

    main_head = getNewHead(main_head, new_time);
    bucket* last = main_head;
    while(last -> next != NULL){
        last = last -> next;
    } 
    prev = last;
    temp = main_head;

    
    flight_plan* tobeInserted;

    while(temp){
        flight_plan* f_temp = temp -> flight_list;
        flight_plan* f_prev = NULL;

        while(f_temp){
            int difference = time_diff(f_temp -> arr_time, temp -> start_ETA);
            if(difference >= 0){
                f_prev = f_temp;
                f_temp = f_temp -> next;
            }
            else{
                //checking if the flight plan is the first node of the bucket or not 
                if(f_prev == NULL){
                    temp -> flight_list = f_temp -> next;
                    tobeInserted = f_temp;
                    f_temp = f_temp -> next;
                }
                else{
                    f_prev -> next = f_temp -> next;
                    tobeInserted = f_temp;
                    f_temp = f_temp -> next;
                }

                //isolating the flight plan
                tobeInserted -> next = NULL;

                //comparing the times of the bucket of prev node
                int compareTime = abs(time_diff(tobeInserted -> arr_time, prev -> start_ETA));
                //if the time is less than one hour, then inserting in the prev in sorted manner
                if(compareTime < 60 || compareTime > 1380){
                    insertSorted(&prev, tobeInserted);
                }
                //making the newNode with appropiate time
                else{
                    //creating the bucket
                    bucket* newNode = (bucket*)malloc(sizeof(bucket));
                    newNode -> bucket_id = i++;
                    newNode -> start_ETA = (TIME*)malloc(sizeof(TIME));
                    newNode -> start_ETA -> min = new_time -> min;
                    newNode -> start_ETA -> hr = temp -> start_ETA -> hr - 1;
                    if(newNode -> start_ETA -> hr < 0){
                        newNode -> start_ETA -> hr += 24;
                    }
                    newNode -> end_ETA = (TIME*)malloc(sizeof(TIME));
                    newNode -> end_ETA -> min = new_time -> min;
                    newNode -> end_ETA -> hr = (temp -> start_ETA -> hr) % 24;
                    newNode -> flight_list = NULL;
                    newNode -> next = NULL;

                    //checking whether the node is last or not 
                    if(prev == last){
                        prev -> next = newNode;
                        last = newNode;
                        prev = newNode;
                    }
                    else{
                        prev -> next = newNode;
                        newNode -> next = temp;
                        prev = newNode;
                    }
                    
                    insertSorted(&prev, tobeInserted);
                }
            }
        }

        prev = temp;
        temp = temp -> next;
    }

    main_head = update(main_head);
    return main_head;
}

void outputData(bucket* main_head){
    FILE* newFile = fopen("output.csv", "w");

    bucket* temp = main_head;
    while(temp){
        
        flight_plan* f_temp = temp -> flight_list;
        if(f_temp != NULL){
            fprintf(newFile, "The start and end ETA are %d:%d  and  %d:%d \n", temp -> start_ETA -> hr, temp -> start_ETA -> min, temp -> end_ETA -> hr, temp -> end_ETA
                -> min);
        }

        while(f_temp){
            fprintf(newFile, "%d,%d,%d,%d,%d \n", f_temp -> flight_id, f_temp -> arr_time -> hr, f_temp -> arr_time -> min, f_temp -> dept_time -> hr, f_temp -> dept_time -> min);

            f_temp = f_temp -> next;
        }

        temp = temp -> next;
        fprintf(newFile, "\n\n\n");
    }
}

int main(){
    bucket* main_head = NULL;
    int data, op, hr, min, id, arr_time_hr, arr_time_min, dep_time_hr, dep_time_min; 

    FILE* ptr = fopen("a.csv", "r");

    while(!feof(ptr)){
        fscanf(ptr, "%d,%d,%d,%d,%d \n", &id, &arr_time_hr, &arr_time_min, &dep_time_hr, &dep_time_min);

        flight_plan* flight = (flight_plan*)malloc(sizeof(flight_plan));
        flight -> arr_time = (TIME*)malloc(sizeof(TIME));
        flight -> dept_time = (TIME*)malloc(sizeof(TIME));
        flight -> flight_id = id;
        flight -> arr_time -> hr = arr_time_hr;
        flight -> arr_time -> min = arr_time_min;
        flight -> dept_time -> hr = dep_time_hr;
        flight -> dept_time -> min = dep_time_min;
        flight -> next = NULL;

        main_head = insert(main_head, flight);
    }


    printf("Enter the operation to be performed \n");
    printf("1. Show Status    2. Insert New Flight Plan    3. Cancel a Flight Plan    4. Rearrange\n");
    scanf("%d", &op);

    if(op>=1 && op <= 4){
        if(op == 1){
            printf("Enter the flight id that you want to search for : ");
            scanf("%d", &id);

            show_status(main_head, id);
        }
        else if(op == 2){
            printf("Enter the flight id, arrival time hr, arrival time min, departure time hr, departure time min in correct order \n");
            scanf("%d %d %d %d %d", &id, &arr_time_hr, &arr_time_min, &dep_time_hr, &dep_time_min);

            flight_plan* flight = (flight_plan*)malloc(sizeof(flight_plan));
            flight -> arr_time = (TIME*)malloc(sizeof(TIME));
            flight -> dept_time = (TIME*)malloc(sizeof(TIME));
            flight -> flight_id = id;
            flight -> arr_time -> hr = arr_time_hr;
            flight -> arr_time -> min = arr_time_min;
            flight -> dept_time -> hr = dep_time_hr;
            flight -> dept_time -> min = dep_time_min;
            flight -> next = NULL;

            main_head = insert(main_head, flight);
        }
        else if(op == 3){
            printf("Enter id of the flight to be cancelled : ");
            scanf("%d", &id);

            main_head = delete(main_head, id);
        }
        else{
            printf("Enter the new time hr and new time min appropiately \n");
            scanf("%d %d", &hr, &min);

            if(hr<0 || hr > 23 || min < 0 || min > 59){
                printf("Enter the correct values of hour and minutes \n");
            }
            else{
                TIME* t = (TIME*)malloc(sizeof(TIME));
                t -> hr = hr;
                t -> min = min;
                main_head = reArrange(main_head, t);

                printf("Reaarange ");

                outputData(main_head);
                printf("Fed the data in output.csv \n");
            }
        }
    }
    else{
        printf("Enter the operation correctly \n");
    }


    return 0;
}