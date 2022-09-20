#ifndef MAIN_H
#define MAIN_H
struct response{
    char *buffer;
    size_t size;
};
struct clock_data{
    char *carbon_deadline_1;
    char *renewables_1_timestamp;
    int renewables_1_seconds;
    double renewables_1_rate; 
    double renewables_1_initial;
};
#endif