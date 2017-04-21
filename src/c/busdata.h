#pragma once
#include <stdint.h>
#include <stdio.h>
#include <pebble.h>

typedef struct {
  char* busString;
  uint16_t numberOfBus;
  char delim;
  char subdelim;
}RawBusData;


typedef struct {
  char id[5]; // a modifier
  char name[20];  // Name of bus
  char time[20];       // description of bus
}Bus;


enum BusKey {
  BUS_ICON_KEY = 0x0,         // TUPLE_INT
  BUS_NAME_KEY = 0x1,  // TUPLE_CSTRING
  ARRIVE_TIME_KEY = 0x2, 
  BUS_STRING_KEY = 0x3,
  BUS_STOP_KEY = 0x4,
  ERROR_MESSAGE_KEY = 0x5,
};

typedef struct {
  int id;
  char name[16];  // Name of bus
  char desc[16];       // description of bus
  bool sens;
} Station;

Bus* get_bus_list_instance(const RawBusData* rawBus, Bus* bus);

uint16_t bus_occurences(char* data,  char delim, char subdelim);

RawBusData* bus_raw_data_create(char* data, char delim, char subdelim);

Bus* bus_list_create(const RawBusData* rawBus);

void bus_destroy(RawBusData* rawBus, Bus* bus);

Bus* get_bus_list(const RawBusData* rawBus, Bus* bus);

void print_bus_list(const RawBusData* rawBus, const Bus* bus);

