//printing job created by thread_recordinput()
//printing job created by thread_recievedata()
// reference from Stack overflow 
#define _POSIX_C_SOURCE 200112L
#include "LIST.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>  // use of error 
#include <sys/poll.h>     
#include <arpa/inet.h>
#include <netdb.h>  //  use for addrinfo 
#include <limits.h> // MAXIMUM value for variable 
#include <semaphore.h>
#include <pthread.h>
#include <signal.h>     // Use for terminal interrupt(ctrl+c) signal handler to end the process  
#include <ctype.h>      // checking for white space using function(isspace())

#define count_thread 4
#define max_len 256

// enum is user defined data type 
typedef enum {
    remote_usr = 0,     // shows header in message("remoteuser")
    local_usr,     //  shows "LocalUser:" as header in messsage
    remote   
} head_job;
// job info include message(msg) and category of job(send or recieve)
// used in list->node->msg
typedef struct jobPckg {
    head_job type;
    char msg[max_len];
} jobPckg;

// struct define the information of socket (address and socket number)
typedef struct infor_sock {
    int Num_sock;     // define local socket number. Don'temp_var exist if is -1
    struct addrinfo *destAddrInfo; // struct defines the information regrading destination address family
} infor_sock;

static sem_t printsemaphore; // used as semaphore for a thread to print message on screen  
static sem_t sendsemaphore;  // used as semaphore for a thread to send a message 
static infor_sock *ptr_thread; // ptrs passed to udp arr_thread 
static list *queue_print; // used queue data structure to manage print jobs manage data traffic
static list *queue_send;  // used queue data structure to manage sent jobs
static pthread_mutex_t mutex_print; // accessing the list of print 
static pthread_mutex_t mutex_send;  // accesssing the list of send 

// helps to detect signal for program termination 
static int count_termin; // check the signal if equal to 1 then sent, else its not detected  

// check if user input is empty 
int input_empty() {
    struct pollfd fds;
    fds.fd = 0;       // 0 file descripter for input
    fds.events = POLLIN; // event treated as read input from user 
    if (poll(&fds, 1, 0) != 1)
        return 0; // input is empty or error 
    else 
        return 1;// not empty
}

// check the port ranges 
int check_port(int *port0, int *remoteport0){
	int x =0;
	if(*port0 < 1025 || *remoteport0 > 65535 || *port0 > 65535 || *remoteport0 < 1025)// as port number range(1025-65535)
		x++;
    else
    	while(!*port0 || !remoteport0)
    	{
    		*port0 = &remoteport0;
    		*port0++;
    		*remoteport0++;
    	}
	return x;
}

// Helping function used for conversion from char to int 
// refrenced from https://stackoverflow.com/a/22866001
int char_toint(const char *str) {
    char *endptr;
    errno = 0;
    int x= INT_MAX;
    int y= INT_MIN;
    long l = strtol(str, &endptr, 0); // conversion from intial part of string to long 

    // check exception in failure in the conversion
    if (errno == ERANGE || (*endptr != '\0' && *endptr != '\n' && *endptr != '\r') || str == endptr) {
        return -1;
    }
    // exception if sizeof(int) < sizeof(long)
    if (l > x || l < y) {
        return -2;
    }
    return (int) l;
}

// function to check parameters
void argumenet_count(int arg){
	int flag=0;
	if(arg!=3){
		flag=0;
		puts("error");
	}
	else
		flag=1;
}

// Convert my port and remote port from string to int 
int port_toint(char *char_port, char *char_remport, int *port_int, int *remoteport_int) {
    // if ports does not exist i.e empty ptr 
    int temp = port_exist(port_int, remoteport_int);
    if (temp== 1)
        return 1;
    int out_val= char_toint(char_port);
    int check_param = 0; // indicator of success or failure
    if (out_val < 0) { // failure if port is negative  
        fprintf(stderr, "Error in conversion port_int to integer: %d\n", out_val);
        check_param = 1;
    } else
        *port_int = out_val;
    out_val=char_toint(char_remport);
    if (out_val < 0) { // failure if port is negative
        fprintf(stderr, "Error in conversion remoteport_int to integer: %d\n", out_val);
        check_param = 1;
    } else
        *remoteport_int = out_val;
    // if badparam_bool is 0 ports get converted to int then  
    if (!check_param) {
    	  
        if (*port_int < 1025 || *port_int > 65535 || *remoteport_int < 1025 || *remoteport_int > 65535) { 
            check_param = 1;
        }
    }
    return check_param; // return 0(success) or 1(failure)
}

// refrernced from stdi: https://stackoverflow.com/a/9278353
// data stored in buffered at stdin passed into char[]
// data get read from stdin buffer 
int getuser_msg(char *arr, int size) {
    char *p;

    memset(arr, 0, sizeof(p)); // arr intialize to 0 and resets the message  
    if (fgets(arr, size, stdin)) { // return NULL if arr is empty 
        return 0; // success on storing the data from buffer to char array  
    } else
        return -1; // failure
}

// Function check the msg and family 
void check_msg(char *temp, char* temp1){
	struct sockaddr *arr;
	if(arr->sa_family == NULL || temp || temp1){
		perror("This is error checkpost 2 ");
	}
	else
		puts("going good");
}

// Function Shows the error get addesss is not IPV4
void *addr_check(struct sockaddr *addr_sock) {
    if (addr_sock->sa_family == AF_INET) {    // check if the message we get is IPv4 
        return &(((struct sockaddr_in *) addr_sock)->sin_addr);
    } else {
        perror("The address family is not IPv4");
        return NULL;
    }
}

// Making link list of addrinfo if remote addr is not specified, then equal to host addr 
struct addrinfo *list_addrinfo(int num_port, char *def_addr) {
    struct addrinfo temp;
    struct addrinfo *temp1;
    int out_val;
    char portNumChar[6]; // MAx Length of port Number is 65535 
    sprintf(portNumChar, "%d", num_port); // storing num_port into portNumChar in form of sign decimal integer 
    
    // temp is used to make socket address based on parameters
    memset(&temp, 0, sizeof temp); // intialize hint to 0 
    if (!def_addr)
        temp.ai_flags = AI_PASSIVE;    // since remote add is not specified then use localhost
    temp.ai_family = AF_INET;      // set to AF_INET (IPv4)
    temp.ai_socktype = SOCK_DGRAM; // udp socket

    //  getaddrinfo() make program free from IPv4-versus-IPv6 dependencies.
    if ((out_val = getaddrinfo(def_addr, portNumChar, &temp, &temp1)) != 0) {  // service info get the remoteaddr, portnumber, temp
        fprintf(stderr, " Error in the address info  %s\n", gai_strerror(out_val));
        return NULL;
    }
    return temp1;
}

// This function check if ports does not exist i.e empty ptr 
int port_exist(int *temp,int *temp1){
	int flag= 0;
	if(!temp || !temp1)
		flag=1;
	return flag;
}

// set up UPD socket and bind it to the specified port #, return the socket descriptor

int port_UDPsocket(int num_port) {
    struct addrinfo *temp1;
    int fd_socket = 0;
    
    temp1 = list_addrinfo(num_port, NULL);// As it is NULL so we will Use local host address as specified in function above
    if (!temp1) {
        fprintf(stderr, "error in addrinfo for port_UDPsocket(%d)\n", num_port);
        return -1; // failure to setup the socket 
    }
    struct addrinfo *p;
    // loop through all the results and bind to the first we can
    for (p = temp1; p != NULL; p = p->ai_next) {
        if ((fd_socket = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {  // -1 not creation of socket
            perror("Error in client: socket");
            continue;
        }
        // socket has been created, add, port number will be connected
        if (bind(fd_socket, p->ai_addr, p->ai_addrlen) == -1) { // not bind if -1
            perror("Error in bind");
            close(fd_socket);
            continue;
        }
        break;
    }
  
    freeaddrinfo(temp1); // make serive info free from accesing the memory
// error check if socket not create, not bind as well
    if (!p) {
        return -1;   
    } 
    else
        return fd_socket; // return socket 
}

// This function setup the specified port with socket addr 
struct addrinfo *port_Destaddr(int port_dest, char *def_addr) {
    struct addrinfo *temp1;
    struct addrinfo *temp2;

    temp1 = list_addrinfo(port_dest, def_addr);
    if(temp1) {
        temp2 = temp1->ai_next;
        while (temp2) { // finding the destination address block 
            free(temp1); // as we only only 1 destination address block 
            temp1 = temp2;
            temp2 = temp2->ai_next;
        }
    }
        else
        	return NULL;
        return temp1;
    
}

// The function make socket & the socket_addr object free from accessing memory 
void delete_sock(infor_sock *mySockInfo) {
    freeaddrinfo(mySockInfo->destAddrInfo);
    close(mySockInfo->Num_sock);
}

// The function checking print and send queue is empty  
int get_msg_queue(list *print, list *send){
	if(print== NULL)
		puts("queue is empty for print");
	if(send== NULL)
		puts("queue is empty for send");
	int tracking_print=0;
	int tracking_send= 0;
	while(print!=NULL){
		tracking_print++;
	}
	while(send!=NULL){
		tracking_send++;
	}
	if(print==NULL && send == NULL){
		puts("No msg yet");
		return NULL;
	}
	return tracking_print;
}


// this function ADD new job with its type and data to existing list(as a queue)in the Job
void jobEnqueue(head_job type, char *str, list *temp_list) {
    jobPckg *newJob = malloc(sizeof(jobPckg));
    strcpy(newJob->msg, str);
    newJob->type = type;
    ListPrepend(temp_list, newJob);
}

// Returning and removing the last job from the list(as a queue)
jobPckg *jobDequeue(list *temp_list) {
    
    return ListTrim(temp_list); 
}

// Make memory from package 
void jobPckgFree(jobPckg *aPckg) {
    free(aPckg);
}

// Function check if string is whole of space 
int bool_space(char *head, int strLen) {
    char *ptr;
    int allSpace_bool = 1;
    ptr = head;
    int i = 0;
    while (*ptr != '\0' && i < strLen) {
        if (!isspace(*ptr))
            allSpace_bool = 0;
        i++;
        ptr++;
    }
    return allSpace_bool;
}

//                              thread

// thread for printing data on screen 
void *thread_print(void *temp_var) {
    jobPckg *printJob;
    
    // detect the terminal signal 
    while (!count_termin) {
        // This sempahore will wait for signal there is a printing job 
        sem_wait(&printsemaphore);
        pthread_mutex_lock(&mutex_print);
        printJob = jobDequeue(queue_print);
        pthread_mutex_unlock(&mutex_print);
		// detect if print job is NULL  
        if (printJob != NULL) {
            if(bool_space(printJob->msg,max_len)){}
            
            else if (printJob->type == local_usr)
                printf("RemoteUser: %s", printJob->msg);
            else
                printf("LocalUser: %s", printJob->msg);
            
            free(printJob);// free the meomory occupied by print job 
        }
    }

    pthread_exit(NULL);
}

// thread for sending the message 
void *thread_sendmsg(void *temp_var) {
    jobPckg *sendJob;
    
    int Num_sock = ((infor_sock *) temp_var)->Num_sock;
    struct addrinfo *destAddrInfo = ((infor_sock *) temp_var)->destAddrInfo;
    int numbytes;

    // send thread will wait until sending queue is empty 
    while (!count_termin ) {
        // wait for signal that there is a sending job in queue
        sem_wait(&sendsemaphore);

        pthread_mutex_lock(&mutex_send);
        sendJob = jobDequeue(queue_send);
        pthread_mutex_unlock(&mutex_send);

        // detect for NULL and all space , as it will work if is a termination signal 
        if (sendJob != NULL && !bool_space(sendJob->msg,max_len)) {
            if ((numbytes = sendto(Num_sock, sendJob->msg, strlen(sendJob->msg), 0,
                                   destAddrInfo->ai_addr, destAddrInfo->ai_addrlen)) == -1) {  // -1 error does'nt send the message 
                fprintf(stderr,"Error: \n%s\n", strerror(errno));
            }
            
            free(sendJob); // free memory from sending job 
        }
    }
    pthread_exit(NULL); // done with sending 
}


// thread record the input 
void *thread_recordinput(void *temp_var) {
    char msg[max_len+1];
    int numbytes;

    while (!count_termin) {
        if (input_empty()) {
            if (!getuser_msg(msg, max_len)) {
                numbytes = (int) strlen(msg);
                if (numbytes) {
                    // check for "!"( ASCI CODE: 33 ) and all space 
                    if (msg[0] == 33 &&
                        bool_space(msg + 1, max_len - 1)) {     //termination signal
                        count_termin = 1;
                    } else {
                        pthread_mutex_lock(&mutex_print); // lock with print mutex
                        jobEnqueue(remote_usr, msg, queue_print); //create print jobpackage critical section(print) of code hence mutex_lock is used 
                        pthread_mutex_unlock(&mutex_print);
                    }
                    pthread_mutex_lock(&mutex_send);  // lock with send mutex

                    jobEnqueue(remote, msg, queue_send);//create send jobpackage critical section(send) of code hence mutex_lock is used 
                    pthread_mutex_unlock(&mutex_send);

                    // These invoke thread_print and thread_sendmsg if they are blocked 
                    sem_post(&printsemaphore);
                    sem_post(&sendsemaphore);
                }
            }
        }
    }
    pthread_exit(NULL); // done record the input from the user 
}

// Thread for recieving UDP datagram 
void *thread_recievedata(void *temp_var) {
    int out_val;
    //set pthread properity to get cancel, not only terminate  
    if((out_val=pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL))!=0)
        fprintf(stderr,"Error in pthread_setcancelstate: %s\n",strerror(out_val));

    // type : it get canceled anytime as per request 
    if((out_val=pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL))!=0)
        fprintf(stderr,"Error in pthread_setcanceltype: %s\n",strerror(out_val));

    struct sockaddr_storage their_addr; // as we don'temp_var know sourse address is IPV4 or IPV6 it should compaitabble 
  
    socklen_t addr_len;
    char msg[max_len];
    char addrStr[INET6_ADDRSTRLEN];  // max len of IPV6
   
    infor_sock *mySockInfo = (infor_sock *) temp_var;
    int Num_sock = mySockInfo->Num_sock;  // storing recieved socket id in local variable 
    int numbytes;

    while (!count_termin) {
        addr_len = sizeof(their_addr);   
       
        memset(msg, 0, sizeof msg); // change the value of msg to 0 
        // Until the message is read it is buffered in the socket  
        if ((numbytes = recvfrom(Num_sock, msg, max_len - 1, 0, (struct sockaddr *) &their_addr, &addr_len)) == -1) {
            perror("recvfrom");
        } else {
            
            msg[numbytes] = '\0'; // end the message 

            // Detect the termination signal(33='!') 
            if (msg[0] == 33 &&bool_space(msg+1,max_len-1)) {   
                count_termin = 1;
            } else {
                // else it get into get in the queue of printing
                pthread_mutex_lock(&mutex_print);
                jobEnqueue(local_usr, msg, queue_print);
                pthread_mutex_unlock(&mutex_print);
            }
            sem_post(&printsemaphore);
        }
    }
    sem_post(&sendsemaphore);
    pthread_exit(NULL); // done with recieving 
}

// main program 
int main(int argc, char *argv[]) { // argument1 = my port, argument2= remote address, argument3= remoteport
  
     // intializing list as queue 
    queue_print = ListCreate();
    queue_send  = ListCreate();
    count_termin = 0; // intializing the termination signal 
    ptr_thread = malloc(sizeof(infor_sock)); // allocating the memory of sockINFO
    char def_addr[INET6_ADDRSTRLEN];   
    int port_int;      
    int remoteport_int;  
    int out_val;   
    char char_port[5];      // the port which we get from user THIS instance will be using
    char char_remport[5];     // the port the which get from user that he want to send 
    int Num_sock;

    pthread_attr_t thread_attr;        // object define thread properties
    pthread_t arr_thread[count_thread]; // array of arr_thread 
    int i;  
  

    // Detection of valid port 
    int badPort_bool = 0;
    if (argc > 2) {
        badPort_bool = port_toint(argv[1], argv[3], &port_int, &remoteport_int);
        strcpy(def_addr,argv[2]);
    }

    if (argc < 3 || badPort_bool) {
        i = 3;// number of trying attempts 
        badPort_bool=1;
        while (i-- > 0 && badPort_bool) {
            printf(" Input the program parameters in the form of-[my port number] [remote machine name/address] [remote port number]\n");
            // intializing the paramter to 0 
            memset(char_port, 0, sizeof char_port);
            memset(char_remport, 0, sizeof char_remport);
            memset(def_addr, 0, sizeof def_addr);
           
            if (scanf("%s%s%s", char_port, def_addr, char_remport) == 3) // arguments number check 
                badPort_bool = port_toint(char_port, char_remport, &port_int, &remoteport_int);
        }
    }
    // still if the bool is 1 then error 
    Num_sock =port_UDPsocket(port_int);
    if (badPort_bool) {
        fprintf(stderr, "Parameters invalid");
        return 1;
    }

    // Setup the socket 
    if (Num_sock == -1) {
        fprintf(stderr, "Error in setting up the socket");
        return 2;
    }
    else {
        ptr_thread->Num_sock = Num_sock;
    }

    // Checking the destination address 
    if ((ptr_thread->destAddrInfo = port_Destaddr(remoteport_int, def_addr)) == NULL) {
        fprintf(stderr, "Error in setting the destination address information");
        return 3;
    }

    // intializing the mutex( lock/unlock)
    if ((out_val = pthread_mutex_init(&mutex_print, NULL)) != 0)
        fprintf(stderr, "Failure in the mutex of printList : %s\n", strerror(out_val));
    if ((out_val = pthread_mutex_init(&mutex_send, NULL)) != 0)
        fprintf(stderr, "Failure in the mutex of sendList: %s\n", strerror(out_val));

    
    if (sem_init(&printsemaphore, 0, 0) == -1) // intializing the semaphore for printjoblist to be shared b/w arr_thread as arg[2] is 0
        perror("Failure in creation of semaphore for print \n");
    if (sem_init(&sendsemaphore, 0, 0) == -1) // intializing the semaphore for sentjoblist to be shared b/w arr_thread as arg[2] is 0
        perror("Failure in creation of semaphore for send \n");
   
    // define the attributes of thread 
    if(pthread_attr_init(&thread_attr)!=0)
        perror("Failure in defining the:pthread_attr_init()");

    if(pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE)!=0) // as we want to join the thread in end for cleanning 
        perror("Error in pthread_attr_setdetachstate()");
    // creating 4 arr_thread
    pthread_create(&arr_thread[0], &thread_attr, thread_recordinput, NULL);
    pthread_create(&arr_thread[1], &thread_attr, thread_print, NULL);
    pthread_create(&arr_thread[2], &thread_attr, thread_sendmsg, (void *) ptr_thread);
    pthread_create(&arr_thread[3], NULL, thread_recievedata, (void *) ptr_thread);

    for (i = 0; i < count_thread-1; i++)
        pthread_join(arr_thread[i], NULL); // joining the thread until it terminates 

    // Termination of thread of recieve message from remote address 
    if((out_val=pthread_cancel(arr_thread[3]))!=0 && out_val!=3) 
        fprintf(stderr,"error in terminating in recieve thread : %s\n",strerror(out_val));

    // free main memory from data 
    delete_sock(ptr_thread);
    ListFree(queue_print, jobPckgFree);
    ListFree(queue_send, jobPckgFree);

     // disable the mutex and semphore as no longer use 
    if ((out_val = pthread_mutex_destroy(&mutex_print)) != 0)
        fprintf(stderr, "pthread_mutex_destroy for mutex_print failed: %s\n", strerror(out_val));
    if ((out_val = pthread_mutex_destroy(&mutex_send)) != 0)
        fprintf(stderr, "pthread_mutex_destroy for mutex_send failed: %s\n", strerror(out_val));
    if (sem_destroy(&printsemaphore) == -1)
        perror("sem_destroy for printsemaphore failed\n");
    if (sem_destroy(&sendsemaphore) == -1)
        perror("sem_destroy for sendsemaphore failed\n");

    return 0;
}