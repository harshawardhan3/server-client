#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <syscall.h>
#include <sys/fcntl.h>
#include <dirent.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>

//facilitation of data structure store commands
#define CHECK_ALLOC(p) if(p == NULL) { perror(__func__); exit(EXIT_FAILURE); }

//strsplit() function declared globally
extern char **strsplit(const char *str, int *nwords);
extern void free_words(char **words);

//simple data structure to store data after parsing rakefile
typedef struct _data{
    char *type;
    char *value;
    int busyflag;                             // if busyflag =0 free else busy
    struct _data *next;
} DATA;

//simple data structure to store actionsets and their supporting sub-commands
typedef struct _act{
    int act_num;                              //actionset number
    int action_number;                        //action number irrespective of actionset number
    bool place;                               //1 for local actions and 0 for remote actions
    char *value;                              // command to be executed
    struct _act *next;
} ACT;

//simple data structure to store required files
typedef struct _files{
    int action_number;                        //action number irrespective of actionset number
    char *files_required;                     //files required by the action having action_number
    struct _files *next;
} FILES;

//declaring functions concerning DATA structure
extern DATA *create_data(void);
extern DATA *new_item(char *type, char *value, int busyflag);
extern DATA *add_item(DATA *data, char *type, char *value, int busyflag);
extern bool data_find(DATA *data, char *wanted_type, char *wanted_value, int wanted_busyflag);

//declaring functions concerning FILES structure
extern FILES *create_files(void);
extern FILES *new_files_itemm(int action_number, char *files_required);
extern FILES *add_files_item(FILES *files, int action_number, char *files_required);
extern bool files_find(FILES *files, int action_number, char *wanted_files_required);

//declaring functions concerning ACT structure
extern ACT *create_act(void);
extern ACT *new_act_item(int act_num, int action_number, bool place, char *value);
extern ACT *add_act_item(ACT *act, int act_num, int action_number, bool place, char *value);
extern bool act_find(ACT *act, int wanted_act_num,int wanted_action_number, bool wanted_place, char *wanted_value);

