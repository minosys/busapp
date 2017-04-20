#pragma once
#include <pebble.h>

typedef struct {
  char* busString;
  uint16_t numberOfBus;
  char delim;
}RawBusData;

typedef struct {
  char* id; // a modifier
  char* name;  // Name of bus
  char* time;       // description of bus
}Bus;

void bus_data_init(char* data, char delim);
void bus_data_destroy(RawBusData* state);
void bus_data_deinit();
RawBusData* bus_data_get_global(void);
uint16_t bus_data_count(RawBusData* state);
Bus* get_bus_data(RawBusData* state);
void print_bus_list(Bus* buses);
Bus* bus_data_get_buses(void);