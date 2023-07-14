 #include "client.h"

#define CHECK_ALLOC(p) if(p == NULL) { perror(__func__); exit(EXIT_FAILURE); }
#if	defined(__linux__)
extern	char	*strdup(char *string);
#endif

//create a new data structure to facilitate primitive data (hosts, ports etc.), return a null pointer
DATA *create_data(void){
    return NULL;
}

//allocate space for a new data item in our DATA structure
DATA *new_item(char *type, char *value, int busyflag){
    DATA *new = calloc(1, sizeof(DATA));
    CHECK_ALLOC(new);
    new->type = strdup(type);
    CHECK_ALLOC(new->type);
    new->value = strdup(value);
    CHECK_ALLOC(new->value);
    new->busyflag = busyflag;
    new->next = NULL;
    return new;
}

//add a new item to DATA at the head
DATA *add_item(DATA *data, char *type, char *value, int busyflag){
    if(data_find(data, type, value, busyflag)) {        
        return data;
    }
    else {                            
        DATA *new   = new_item(type, value, busyflag);
        new->next   = data;
        return new;
    }
}

//chech if the data item already exists in our DATA structure
bool data_find(DATA *data, char *wanted_type, char *wanted_value, int wanted_busyflag){
    while(data != NULL) {
	if((strcmp(data->type, wanted_type) == 0)&&(strcmp(data->value, wanted_value) == 0)&&(wanted_busyflag==data->busyflag)) {
	    return true;
	}
	data = data->next;
    }
    return false;
}

//create a new data structure (ACT) to facilitate the storage of actionsets and their supporting material
ACT *create_act(void){
    return NULL;
}

//allocate space for new ACT data item in our ACT structure
ACT *new_act_item(int act_num, int action_number, bool place, char *value){
    ACT *new = calloc(1, sizeof(ACT));
    CHECK_ALLOC(new);
    new->act_num = act_num;
    new->action_number = action_number;
    new->place = place;
    new->value = strdup(value);
    CHECK_ALLOC(new->value);
    return new;
}

//add a new ACT item to structure at the end
ACT *add_act_item(ACT *act, int act_num, int action_number, bool place, char *value){
    if(act_find(act, act_num, action_number, place, value)){
        return act;
    }
    else {
        ACT *new = new_act_item(act_num, action_number, place, value);
        new->next = act;
        return new;
    }
}

//find ACT items in our ACT structure
bool act_find(ACT *act, int wanted_act_num, int wanted_action_number, bool wanted_place, char *wanted_value){
    while(act!=NULL){
        if((act->act_num == wanted_act_num) && (act->action_number == wanted_action_number) && (act->place == wanted_place) && (act->value == wanted_value) ){
            return true;
        }
        act = act->next;
    }
    return false;
}

//create a new data structure to facilitate (FILES) to facilitate the storage of required files as specfied by action_number
FILES *create_files(void){
    return NULL;
}

//allocate space for a new FILES item in our DATA structure
FILES *new_files_item(int action_number, char *files_required){
    FILES *new = calloc(1, sizeof(FILES));
    CHECK_ALLOC(new);
    new->action_number = action_number;
    new->files_required = strdup(files_required);
    CHECK_ALLOC(new->files_required);
    new->next = NULL;
    return new;
}

//add a new item to FILES at the head
FILES *add_files_item(FILES *files, int action_number, char *files_required){
    if(files_find(files, action_number, files_required)) {        
        return files;
    }
    else {                            
        FILES *new   = new_files_item(action_number, files_required);
        new->next   = files;
        return new;
    }
}

//check if the item already exists in our FILES structure
bool files_find(FILES *files, int wanted_action_number, char *wanted_files_required){
    while(files != NULL) {
	if((files->action_number == wanted_action_number) &&(files->files_required== wanted_files_required)) {
	    return true;
	}
	files = files->next;
    }
    return false;
}
