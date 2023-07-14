#include "client.h"


//flag supporting tab characters
int tab = 0;

//action set counter
int act_counter = 0;

//action number
int act_number=0;

// loacl or remote indicator, TRUE is local , FALSE is remote
bool placeflag;

// port number
int portNum;

//declare data structure to store host values
DATA *host_data; 

//declare action data structure to store actions
ACT *action_data;

//declare action data structure to store  require files
FILES *files_data;

// create another pointer which points to the beginning of the data structure
ACT *x_command;  

// create another pointer which points to the beginning of the data structure
DATA *x_host;



// function to reverse the structure since we add new items at the head
void reverse_action(ACT** head)
{
    ACT* previous = NULL;   // the previous pointer
    ACT* current = *head;   // the main pointer
 
    // traverse the list
    while (current != NULL)
    {
        // tricky: note the next node
         ACT* next = current->next;
 
        current->next = previous;    // fix the current node
 
        // advance the two pointers
        previous = current;
        current = next;
    }
 
    // fix the head pointer to point to the new front
    *head = previous;
}

// function to reverse the structure since we add new items at the head
void reverse_files(FILES** head)
{
    FILES* previous = NULL;   // the previous pointer
    FILES* current = *head;   // the main pointer

    // traverse the list
    while (current != NULL)
    {
        // tricky: note the next node
         FILES* next = current->next;

        current->next = previous;    // fix the current node

        // advance the two pointers
        previous = current;
        current = next;
    }

    // fix the head pointer to point to the new front
    *head = previous;
}

// classify things we read from rakefile and store it in different data struture
void classifier(char **words, int nwords){
    
    
    if(tab==0 && strcmp(words[0], "\n") !=0 && words[0][0]!= '#'){     //Ignoring comments (i.e lines starting with #) and no tabs
        if(strcmp(words[0], "PORT") == 0){                             //For storing port number
            for(int i = 1; i<nwords; ++i){
                if(strcmp(words[i], "=")!=0){
                   portNum = atoi(words[i]);
                }
            }
        }
        else if(strcmp(words[0], "HOSTS") == 0){                        //For storing hostnames in the DATA structure
            for(int i = 1; i<nwords; ++i){
                if(strcmp(words[i], "=")!=0){
                    if(strcmp(words[i], "localhost")!=0)
                    host_data = add_item(host_data, words[0], words[i], 0);     // need to support hostname:0000 not done!
                }
            }
        }
        
        else {
            ++act_counter;                                             //else increment the actionset counter
        }

    }
    //If there is only one tab
    else if(tab==1&& words[0][0]!= '#'){
        char remote_str[8];
        strncpy(remote_str,&words[0][0],7);
        remote_str[7] = '\0';
       
        // If remote action
        if(strcmp(remote_str, "remote-") == 0){
            placeflag=false;
        }
        //Else local action
        else{
            placeflag=true;
        }
        int MAX_NCHARS=1000;
        char *temp=calloc(MAX_NCHARS, sizeof(char*));
        
        //storing the actions command in temp
        for(int i=0; i<nwords; ++i){
           
            if(i==0 &&  placeflag==false){
		    int num = strlen(words[i])-7;
		    char command[num+1];
		    strncpy(command,&words[i][7],num);
		    command[num] = '\0';
		    strcat(temp,command);
	            if(i != 1){
		         strcat(temp," "); }}
	    else{
		    strcat(temp, words[i]);
		    if(i != nwords-1){
                strcat(temp, " ");
	        }
        } 
	} 
        
        act_number++; 
        action_data = add_act_item(action_data, act_counter,act_number, placeflag, temp);
        
    }
    // If 2 tabs, then store required files in FILES structure
    else if(tab==2){
        char support[1000]="";
        for(int i=1; i<nwords; ++i){
            strcat(support, words[i]);
            if (i != nwords-1){
            strcat(support, " ");}
        }
        files_data = add_files_item(files_data, act_number, support);
        



    }
   
}


// requests for a server to provide a quotation on a command
char *cost(DATA *hosts, int port){                                                                                
    int minrnum=10000;
    char *minhostname;
    minhostname = calloc(40, sizeof(char));
    while(hosts!=NULL){
        if(hosts->busyflag==0){                         // see if the server is free
            char hostname[40];
            strcpy(hostname, hosts->value);             //copy the host name into the char hostname
            struct hostent *he;                          //host structure     
            he = gethostbyname(hostname);
            if(he==NULL)
            exit(1);
            char destIP[128];                           // store the address of  the remote server
            char **phe = NULL;
            for( phe=he->h_addr_list ; NULL != *phe ; ++phe)
            {
                inet_ntop(he->h_addrtype,*phe,destIP,sizeof(destIP));
                
            
            }

            int sock = 0;
            struct sockaddr_in serv_addr;
            char buffer[1024] = { 0 };
            if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                printf("\n Socket creation error \n");
            }
            else
            printf("Socket successsfully Created ..\n");
        
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(port);
        
            // Convert IPv4 and IPv6 addresses from text to binary
            // form
            if (inet_pton(AF_INET, destIP, &serv_addr.sin_addr)
                <= 0) {
                printf(
                    "\nInvalid address/ Address not supported \n");
            }
        
            if (connect(sock, (struct sockaddr*)&serv_addr,
                        sizeof(serv_addr))
                < 0) {
                printf("\nConnection Failed \n");
            }
            else{
                printf("Connected to server ..\n");
                send(sock, "rnum", 1024, 0);
                recv(sock, buffer, 1024, 0);
                printf("%s\n", buffer);
                if(atoi(buffer)<minrnum){
                    minrnum=atoi(buffer);
                    strcpy(minhostname, hostname);
                }
                close(sock);
            }
        }

        hosts = hosts->next;

    }



    return minhostname;

}



int main(int argc, char *argv[]){
    if (argc < 1){
        return EXIT_FAILURE;
    }
    //create head pointers for each data structure
    host_data = create_data();
    action_data = create_act();

    //If file named Rakefile is present in current directory, parse in Rakefile. Otherwise the name of the file to be parsed should be provided as an argument.
    char *address = (argc > 1) ? argv[1] : "Rakefile";
    int MAX_NCHARS=1000;
    char *temp=calloc(MAX_NCHARS, sizeof(char*));
    FILE *fp = fopen(address, "r");
    if (!fp)
    {
        perror(address);
        return EXIT_FAILURE;
    }
    while(fgets(temp, MAX_NCHARS, fp)!=NULL){
        int len = strlen(temp);
        if(!(temp[0]=='\n')){
            if(temp[len-1]=='\n'){temp[len-1]=0;}

            if(temp[0] == '\t'){
                if(temp[1] == '\t'){
                tab=2;
                }
                else{
                tab = 1;
                }
            }
        int nwords;
        char **words = strsplit(temp, &nwords);
        classifier(words, nwords);
        free_words(words);
        tab = 0;    //reset tab counter
        }
    }
     
    // reverse the order, so actions and files can be executed in the right order 
    reverse_action(&action_data);
    reverse_files(&files_data);         

     if (fclose(fp))
    {
        return EXIT_FAILURE;
        perror(address);
    }
    free(temp);


    
    //network part

    fd_set readfds, testfds;                            // select() variables
    FD_ZERO(&readfds);
    FD_ZERO(&testfds);
    int fd;
    int setnum;                                        // the current actionset
    bool setsuccess = true;                            //see if the current actionset succeed

    x_command = create_act();
    x_command = action_data;             // create another pointer which points to the beginning of the data structure

    
    for(setnum=1;setnum<=act_counter;setnum++){
        while(x_command!=NULL){

            if(x_command->act_num==setnum){
                if(x_command->place==1){                              //check if it is local action
                    char buff[1024];
                    char localcom[1024];                              // set the buffer which store the command line
                    int sock = 0;
                    struct sockaddr_in serv_addr;
                    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                        printf("\n Socket creation error \n");
                        return -1;
                    }
                    else
                    printf("Socket successsfully Created ..\n");
                
                    serv_addr.sin_family = AF_INET;
                    serv_addr.sin_port = htons(portNum);
                
                    // Convert IPv4 and IPv6 addresses from text to binary
                    // form
                    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)
                        <= 0) {
                        printf(
                            "\nInvalid address/ Address not supported \n");
                        return -1;
                    }
                
                    if (connect(sock, (struct sockaddr*)&serv_addr,
                                sizeof(serv_addr))
                        < 0) {
                        printf("\nConnection Failed \n");
                        return -1;
                    }
                    else{
                        printf("Connected to server ..\n");
                        strcpy(localcom, x_command->value);                 //copy the command line into the buffer
                        send(sock, localcom, 1024, 0);                      //send the command to localhost
                        while(recv(sock, buff, 1024, 0)!=0){
                            if(strcmp(buff,"end")==0){
                                break;
                            }
                            printf("%s",buff);
                            memset(buff, 0, 1024);
                            
                        }
                        close(sock);
                        memset(localcom,0,1024);
                    }
                }

                //if it is remote action
                else{                                                                           
                    x_host = create_data();
                    x_host = host_data;
                    char localcom[1024];

                    char *execution_hostname;
                    execution_hostname = calloc(40, sizeof(char));
                    strcpy(execution_hostname, cost(x_host, portNum));

                    //--------
                    struct hostent *he;                          //host structure     
                    he = gethostbyname(execution_hostname);
                    char destIP[128];                           // store the address of  the remote server
                    char **phe = NULL;
                    for( phe=he->h_addr_list ; NULL != *phe ; ++phe)
                    {
                        inet_ntop(he->h_addrtype,*phe,destIP,sizeof(destIP));
                        
                    
                    }
                    //--------

                    x_host = host_data;
                    while(x_host!=NULL){
                        if(strcmp(x_host->value, execution_hostname)==0){
                            x_host->busyflag = 1;           // set this server to busy
                            break;
                        }
                        x_host = x_host->next;
                    }

                    int sock = 0;                                   //connect to the server which has the lowest cost
                    struct sockaddr_in serv_addr;

                    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                        printf("\n Socket creation error \n");
                        return -1;
                    }
                    else
                    printf("Socket successsfully Created ..\n");

                    serv_addr.sin_family = AF_INET;
                    serv_addr.sin_port = htons(portNum);

                    // Convert IPv4 and IPv6 addresses from text to binary
                    // form
                    if (inet_pton(AF_INET, destIP, &serv_addr.sin_addr)
                        <= 0) {
                        printf(
                            "\nInvalid address/ Address not supported \n");
                        return -1;
                    }

                    if (connect(sock, (struct sockaddr*)&serv_addr,
                                sizeof(serv_addr))
                        < 0) {
                        printf("\nConnection Failed \n");
                        return -1;
                    }
                    else{
                        printf("Connected to server ..\n");
                        strcpy(localcom, x_command->value);                 //copy the command line into the buffer
                        send(sock, localcom, 1024, 0);                      //send the command to localhost
                        FD_SET(sock, &readfds);                             // add this socket to the readset
                        memset(localcom,0,1024);
                    }

                }

                int allbusyflag = 1;                // by defalut all server is busy
                x_host = host_data;
                    while(x_host!=NULL){
                        if(x_host->busyflag==0){
                            allbusyflag = 0;          // if there is a free server, change all busy flag to 0;
                        }
                        x_host = x_host->next;
                    }
		
                if(allbusyflag==1){
                    int servernum = 0;
                    struct timeval timeout;
                    timeout.tv_sec=5;
                    timeout.tv_usec=0;
                    while(servernum<100){
                    // use select to see if any "execution is completed"
                        testfds = readfds;
                        if(select(FD_SETSIZE, &testfds, (fd_set *)0,(fd_set *)0, &timeout)>0){
                            for(fd = 0; fd < FD_SETSIZE; fd++) {
                                char buff[1024] = { 0 };
                                if(FD_ISSET(fd,&testfds)){
                                    while(recv(fd, buff, 1024, 0)!=0){
                                        if(strcmp(buff,"end")==0){
                                            break;
                                        }       
                                        printf("%s",buff);
                                        if(strcmp(buff, "failed!")==0)
                                        setsuccess = false;
                                        memset(buff, 0, 1024);
                                    }
                                    printf("%s\n",buff);      // print the output
                                    close(fd);
                                    FD_CLR(fd, &readfds);
                                    printf("removing server on fd %d\n", fd); 
                                }
                            }
                        }
                        servernum++;
                    }    


                    x_host = host_data;
                    while(x_host!=NULL){
                            x_host->busyflag = 0;           // set all servers to free
                            x_host = x_host->next;
                    }     
                                    

                }     

            }

            else
            break;

            x_command = x_command->next;


        }

        if(setsuccess==false)
        break;

    }    
    
    
    //Execution has been finished/terminated, terminate local server
    char buff[1024];
    int sock = 0;
    struct sockaddr_in serv_addr;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    	printf("\n Socket creation error \n");
    	return -1;
    }
    else
    printf("Socket successsfully Created ..\n");
                
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portNum);
                
   // Convert IPv4 and IPv6 addresses from text to binary
   // form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)
    <= 0) {
    printf(
	"\nInvalid address/ Address not supported \n");
                        return -1;
                    }
                
                    if (connect(sock, (struct sockaddr*)&serv_addr,
                                sizeof(serv_addr))
                        < 0) {
                        printf("\nConnection Failed \n");
                        return -1;
                    }
                    else{
                        printf("Connected to server ..\n");
                        send(sock, "terminate", 1024, 0);                      //send the command to localhost
                        while(recv(sock, buff, 1024, 0)!=0){
                            if(strcmp(buff,"end")==0){
                                break;
                            }
                            printf("%s",buff);
                            memset(buff, 0, 1024);
                            
                        }
                        close(sock);
                    }
    
    
    
    x_host = create_data();
    x_host = host_data;
    //Execution has been finished/terminated, terminate remote servers
    while(x_host!=NULL){
            char hostname[40];
            strcpy(hostname, x_host->value);             //copy the host name into the char hostname
            struct hostent *he;                          //host structure     
            he = gethostbyname(hostname);
            if(he==NULL)
            exit(1);
            char destIP[128];                           // store the address of  the remote server
            char **phe = NULL;
            for( phe=he->h_addr_list ; NULL != *phe ; ++phe)
            {
                inet_ntop(he->h_addrtype,*phe,destIP,sizeof(destIP));
                //printf("addr:%s\n",destIP);
            
            }

            int sock = 0;
            struct sockaddr_in serv_addr;
            char buffer[1024] = { 0 };
            if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                printf("\n Socket creation error \n");
            }
            else
            printf("Socket successsfully Created ..\n");
        
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(portNum);
        
            // Convert IPv4 and IPv6 addresses from text to binary
            // form
            if (inet_pton(AF_INET, destIP, &serv_addr.sin_addr)
                <= 0) {
                printf(
                    "\nInvalid address/ Address not supported \n");
            }
        
            if (connect(sock, (struct sockaddr*)&serv_addr,
                        sizeof(serv_addr))
                < 0) {
                printf("\nConnection Failed \n");
            }
            else{
                printf("Connected to server ..\n");
                send(sock, "terminate", 1024, 0);
                recv(sock, buffer, 1024, 0);
                printf("%s\n", buffer);
                close(sock);
            }

        x_host = x_host->next;

    }
    

    return 0;  

}
