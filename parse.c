#include<stdio.h>
#include<json-c/json.h>
#include"parse.h"
int json_p(struct response *resp){
    for(int i = 0; i < resp->size; i++){
        //putchar(resp->buffer[i]);
    }
    json_object *obj;
    obj = json_tokener_parse(resp->buffer);
    json_object *data = json_object_object_get(obj, "data");
    json_object *modules = json_object_object_get(data, "modules");
    json_object *carbon_deadline_1 = json_object_object_get(modules, "carbon_deadline_1");
    json_object *carbon_deadline_1_timestamp = json_object_object_get(carbon_deadline_1, "timestamp");
    printf("%s\n", json_object_get_string(carbon_deadline_1_timestamp));
}