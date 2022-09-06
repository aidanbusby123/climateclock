#include<stdio.h>
#include<string.h>
#include<json-c/json.h>
#include"parse.h"
#include"main.h"
int json_p(struct response *resp, struct clock_data *c_data){
    char *carbon_deadline_1_timestamp_s;
    int renewables_1_rate_i;
    char* renewables_1_timestamp_s;
    json_object *obj;
    obj = json_tokener_parse(resp->buffer);
    json_object *data = json_object_object_get(obj, "data");
    json_object *modules = json_object_object_get(data, "modules");
    json_object *carbon_deadline_1 = json_object_object_get(modules, "carbon_deadline_1");
    json_object *carbon_deadline_1_timestamp = json_object_object_get(carbon_deadline_1, "timestamp");
    json_object *renewables_1 = json_object_object_get(modules, "renewables_1");
    json_object *renewables_1_rate = json_object_object_get(renewables_1, "rate");
    json_object *renewables_1_timestamp = json_object_object_get(renewables_1, "timestamp");

    carbon_deadline_1_timestamp_s = (char*)json_object_get_string(carbon_deadline_1_timestamp);
    renewables_1_rate_i = json_object_get_int(renewables_1_rate);
    renewables_1_timestamp_s = (char*)json_object_get_string(renewables_1_timestamp);

    c_data->carbon_deadline_1 = (char*)malloc(*carbon_deadline_1_timestamp_s);
    c_data->renewables_1_timestamp = (char*)malloc(*renewables_1_timestamp_s);
    strcpy(c_data->carbon_deadline_1, carbon_deadline_1_timestamp_s);
    c_data->renewables_1_rate = renewables_1_rate_i;
    strcpy(c_data->renewables_1_timestamp, renewables_1_timestamp_s);
}