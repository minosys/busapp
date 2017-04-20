#include <stdio.h>
#include "busdata.h"


static RawBusData* global = NULL;
static Bus* buses = NULL;

uint16_t bus_occurences(char* data,  char delim) {
  if (NULL == data) {
    return 0;
  }
  if (strlen(data) == 0) {
    return 0;
  }
  char* pos = data;
  uint16_t count = 0;
  while (*pos != '\0') {
    if (*pos == delim) {
      count += 1;
    }
    pos++;
  }
  return ++count;
}

RawBusData* bus_data_create(char* data, char delim) {
  RawBusData* state = malloc(sizeof(RawBusData));
  state->busString = data;
  state->delim = delim;
  state->numberOfBus = bus_occurences(data, delim);
  return state;
}

void bus_data_destroy(RawBusData* state) {
  if (NULL == state && NULL == buses) {
    return;
  }
  if(NULL == state){ 
    free(state);
    return;
  }
    free(state);
    free(buses);
  
  return;
}

void bus_data_deinit() {
  bus_data_destroy(global);
  global = NULL;
}

Bus* get_bus_data(RawBusData* state){// , Bus* bus){

   uint16_t i = 0;
  char delim[2];
  delim[0] = state->delim;
  delim[1] = '\0';
  
   char *p = strtok (state->busString, delim);
   char *array[state->numberOfBus];

   while (p != NULL)
   {
       array[i++] = p;
       p = strtok (NULL, "|");
   }
  
  buses = malloc(sizeof(Bus) * state->numberOfBus );

   for (i = 0; i < state->numberOfBus ; ++i){
   	buses[i].id = strtok (array[i], ",");
   	buses[i].name= strtok (NULL, ",");
   	buses[i].time= strtok (NULL, ",");
   }
  
  return buses;
}

void bus_data_init(char* data, char delim) {
  bus_data_deinit();
  global = bus_data_create(data, delim);
  buses = get_bus_data(bus_data_get_global());
}

void print_bus_list(Bus* buses){
  if(NULL == buses) return;
    for (int i = 0; i < bus_data_get_global()->numberOfBus ; ++i){
    	printf("bus[%d] = %s\n", i, buses[i].id);
    	printf("bus[%d] = %s\n", i, buses[i].name);
    	printf("bus[%d] = %s\n", i, buses[i].time);
    }
  
}

uint16_t bus_data_count(RawBusData* state) {
  return state->numberOfBus;
}

Bus* bus_data_get_buses(void) {
  return buses;
}

RawBusData* bus_data_get_global(void) {
  return global;
}
