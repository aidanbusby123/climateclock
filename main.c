#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<curl/curl.h>
#include<pthread.h>
#include<rpi-rgb-led-matrix/led-matrix-c.h>
#include"main.h"
#include"parse.h"

// todo make json_time_stamp_to_seconds return leap years
#define API_URL "https://api.climateclock.world/v1/clock"

struct RGBLedMatrixOptions options = {0};
struct RGBLedMatrix *ctx;
struct LedCanvas *vbuf;
struct LedFont *font;

struct response *resp;
struct clock_data c_data = {0};
struct timestamp_struct{
    int years;
    int days;
    int hours;
    int minutes;
    int seconds;
}json_timestamp_struct, currenttime_struct, deadline_struct;
int lock = 0;

int json_time_stamp_to_seconds(char *json_timestamp){

    int month_days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    int i = 0;
    
    int year = 0, month = 0, day = 0, hours = 0;
    int seconds = 0;

    int nleapyears = 0;

    while (json_timestamp[i] != '-'){
        year = (10 * year) + (json_timestamp[i] - '0');
        i++;
    }
    i++;
    while (json_timestamp[i] != '-'){
        month = (10 * month) + (json_timestamp[i] - '0');
        i++;
    }
    i++;
    while (json_timestamp[i] != 'T'){
        day = 10 * day + (json_timestamp[i] - '0');
        i++;
    }
    i++;
    while (json_timestamp[i] != ':'){
        hours = 10 * hours + (json_timestamp[i] - '0');
        i++;
    }
    for (int n = 0; n < month-1; n++){
        seconds += month_days[n] * 24 * 60 * 60;
    }
    for (int n = 1970; n < year; n++){
        if ((n % 4 == 0 && n % 100 != 0) || (n % 400 == 0)) nleapyears++;
    }
    int deadline_is_leap_year = 0;
    if (((year % 4 == 0 || year % 400 == 0) && year % 100 == 0) && ((year*365)-day) > 59){ nleapyears++; deadline_is_leap_year = 1;}
    
    seconds += ((year-1970) * 365 * 24 * 60 * 60) + ((day-1) * 24 * 60 * 60) + (hours * 60 * 60) + (nleapyears * 24 * 60 * 60);
    //printf("%d\n", seconds);
    deadline_struct.years = year-1970;
    deadline_struct.days = day + nleapyears;
    deadline_struct.hours = hours;
    deadline_struct.minutes = 0;
    deadline_struct.seconds = 0;

    if (deadline_struct.days >= 365 && !deadline_is_leap_year == 1 || deadline_struct.days >= 366 && deadline_is_leap_year == 1){
        deadline_struct.years++;
        deadline_struct.days -= (deadline_is_leap_year == 1) ? 366 : 365;
    }

    return seconds;


}

void *render(void* arg){
    font = load_font("~/rpi-rgb-led-matrix/fonts/10x20.bdf");
    char clock_text[50];
    int temp_time = 0;
    while (1){
    temp_time= time(NULL);
    if(time(NULL) - temp_time == 0);
        if (lock == 0){
            sprintf(clock_text, "%d years %d days %d:%d:%d", json_timestamp_struct.years, json_timestamp_struct.days, json_timestamp_struct.hours, json_timestamp_struct.minutes, json_timestamp_struct.seconds);
            printf("%d years %d days %d:%d:%d\n", json_timestamp_struct.years, json_timestamp_struct.days, json_timestamp_struct.hours, json_timestamp_struct.minutes, json_timestamp_struct.seconds);

            
            draw_text(vbuf, font, 0, 0, 64, 64, 64, clock_text, 0);
            vbuf = led_matrix_swap_on_vsync(ctx, vbuf);
            lock = 1;
        }
    }
}

static size_t write_to_buffer(char *data, size_t size, size_t nmemb, void *stream){
    size_t realsize = size * nmemb;
    resp = (struct response*)stream;
    char *ptr = (char*)realloc(resp->buffer, resp->size + realsize + 1);
    resp->buffer = ptr;
    for (int i = 0; i < realsize; i++){
        resp->buffer[resp->size+i] = data[i];
    }
    resp->size += realsize;
    resp->buffer[resp->size] = 0;
    return realsize;
}

int main(int argc, char **argv){
    int width, height;
    int x, y, i;

    options.rows = 32;
    options.chain_length = 1;
//putchar(resp->buffer[resp->size+i]);
    ctx = led_matrix_create_from_options(&options, &argc, &argv);
    if (ctx == NULL)
        return 1;
    vbuf = led_matrix_create_offscreen_canvas(ctx);
    led_canvas_get_size(vbuf, &width, &height);

    time_t secs = time(NULL);
    time_t clock_time;

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
        pthread_t tid;
        
        pthread_create(&tid, NULL, render, NULL);

        
            res = curl_easy_perform(curl);
            json_p(resp, &c_data);
            printf("%s\n", c_data.carbon_deadline_1);
            int seconds = 0;
            int currenttime = time(NULL);
            int nleapyears = 0;

            seconds = json_time_stamp_to_seconds(c_data.carbon_deadline_1);
            currenttime_struct.seconds = currenttime;
            currenttime_struct.minutes = currenttime_struct.seconds / 60;
            currenttime_struct.seconds %= 60;
            currenttime_struct.hours = currenttime_struct.minutes / 60;
            currenttime_struct.minutes %= 60;
            currenttime_struct.days = currenttime_struct.hours / 24;
            currenttime_struct.hours %= 24;
            currenttime_struct.years = currenttime_struct.days / 365;
            currenttime_struct.days %= 365;
            for (int i = currenttime_struct.years; i < deadline_struct.years; i++){
                if ((i % 4 == 0 && i % 100 != 0) || (i % 400 == 0)) nleapyears++;
            }
            if (((currenttime_struct.years % 4 == 0 || currenttime_struct.years % 400 == 0) && currenttime_struct.years % 100 == 0) && ((currenttime_struct.years*365)-currenttime_struct.days) > 59) nleapyears++;        
                    json_timestamp_struct.seconds = seconds-currenttime-(nleapyears*24*60*60);
                    json_timestamp_struct.minutes = json_timestamp_struct.seconds / 60;
                    json_timestamp_struct.seconds %= 60;
                    json_timestamp_struct.hours = json_timestamp_struct.minutes / 60;
                    json_timestamp_struct.minutes %= 60;
                    json_timestamp_struct.days = json_timestamp_struct.hours / 24;
                    json_timestamp_struct.hours %= 24;
                    json_timestamp_struct.years = json_timestamp_struct.days / 365;
                    json_timestamp_struct.days %= 365;

            nleapyears = 0;
            while (1){
                
                if (currenttime - secs >= 86400){
                    res = curl_easy_perform(curl);
                    json_p(resp, &c_data);
                    secs = time(NULL);
                    seconds = json_time_stamp_to_seconds(c_data.carbon_deadline_1);
                    currenttime_struct.seconds = currenttime;
                    currenttime_struct.minutes = currenttime_struct.seconds / 60;
                    currenttime_struct.seconds %= 60;
                    currenttime_struct.hours = currenttime_struct.minutes / 60;
                    currenttime_struct.minutes %= 60;
                    currenttime_struct.days = currenttime_struct.hours / 24;
                    currenttime_struct.hours %= 24;
                    currenttime_struct.years = currenttime_struct.days / 365;
                    currenttime_struct.days %= 365;
                    for (int i = currenttime_struct.years; i < deadline_struct.years; i++){
                        if ((i % 4 == 0 && i % 100 != 0) || (i % 400 == 0)) nleapyears++;
                    }
                    if (((currenttime_struct.years % 4 == 0 || currenttime_struct.years % 400 == 0) && currenttime_struct.years % 100 == 0) && ((currenttime_struct.years*365)-currenttime_struct.days) > 59) nleapyears++;
                    
                    

                    
                    json_timestamp_struct.seconds = seconds-currenttime-(nleapyears*24*60*60)+1;
                    json_timestamp_struct.minutes = json_timestamp_struct.seconds / 60;
                    json_timestamp_struct.seconds %= 60;
                    json_timestamp_struct.hours = json_timestamp_struct.minutes / 60;
                    json_timestamp_struct.minutes %= 60;
                    json_timestamp_struct.days = json_timestamp_struct.hours / 24;
                    json_timestamp_struct.hours %= 24;
                    json_timestamp_struct.years = json_timestamp_struct.days / 365;
                    json_timestamp_struct.days %= 365;

                    nleapyears = 0;
                }
                while (currenttime-secs < 86400){
                    while (lock == 0);
                    lock = 1;
                    currenttime = time(NULL);
                    while (currenttime == time(NULL));
                    json_timestamp_struct.seconds -= 1;

                    if (json_timestamp_struct.seconds < 0){
                        json_timestamp_struct.minutes -= 1;
                        json_timestamp_struct.seconds += 60;
                    }
                    if (json_timestamp_struct.minutes < 0){
                        json_timestamp_struct.hours -= 1;
                        json_timestamp_struct.minutes += 60;
                    }
                    if (json_timestamp_struct.hours < 0){
                        json_timestamp_struct.days -= 1;
                        json_timestamp_struct.hours += 24;
                    }
                    if (json_timestamp_struct.days < 0){
                        json_timestamp_struct.years -= 1;
                        json_timestamp_struct.days += 365;
                    }

                    
                    lock = 0;
                    
                    
                }
            }
    } else {
        printf("\nCurl not initialized\n");
    }
}