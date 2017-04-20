#pragma once
#include <stdint.h>
#include <stdio.h>
#include <pebble.h>

typedef struct {
  char* busString;
  uint16_t numberOfBus;
  char delim;
}RawBusData;

typedef struct {
  char id[5]; // a modifier
  char name[20];  // Name of bus
  char time[20];       // description of bus
}Bus;

Bus* get_bus_list_instance(const RawBusData* rawBus, Bus* bus);

uint16_t bus_occurences(char* data,  char delim);

RawBusData* bus_raw_data_create(char* data, char delim);

Bus* bus_list_create(const RawBusData* rawBus);

void bus_destroy(RawBusData* rawBus, Bus* bus);

Bus* get_bus_list(const RawBusData* rawBus, Bus* bus);

void print_bus_list(const RawBusData* rawBus, const Bus* bus);

