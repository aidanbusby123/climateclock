#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<curl/curl.h>
#include"main.h"
#include"parse.h"


#define API_URL "https://api.climateclock.world/v1/clock"

struct response *resp;

static size_t write_to_buffer(char *data, size_t size, size_t nmemb, void *stream){
    size_t realsize = size * nmemb;
    resp = (struct response*)stream;
    char *ptr = realloc(resp->buffer, resp->size + realsize + 1);
    resp->buffer = ptr;
    for (int i = 0; i < realsize; i++){
        resp->buffer[resp->size+i] = data[i];
        //putchar(resp->buffer[resp->size+i]);
    }
    resp->size += realsize;
    resp->buffer[resp->size] = 0;
    return realsize;
}

int main(){
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    struct response chunk = {0};
    if (curl){
        curl_easy_setopt(curl, CURLOPT_URL, API_URL);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_buffer);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);
        curl_easy_setopt(curl, CURLOPT_ENCODING, "");
        pid_t pid = fork();
        if (pid > 0){
            while (1){
                res = curl_easy_perform(curl);
                json_p(resp);
            }
        } else if (pid == 0){
            
        }
    } else {
        printf("\nCurl not initialized\n");
    }
}