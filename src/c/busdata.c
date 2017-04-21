#include "busdata.h"

Bus* get_bus_list_instance(const RawBusData* rawBus, Bus* bus){
	if(NULL == bus){
		bus = malloc(sizeof(Bus) * rawBus->numberOfBus);
	}
	return bus;
}

uint16_t bus_occurences(char* data,  char delim, char subdelim) {
  if (NULL == data) {
    return 0;
  }
  if (strlen(data) == 0) {
    return 0;
  }
  char* pos = data;
  uint16_t count = 0;
  uint16_t countField = 0;
  while (*pos != '\0') {
    if (*pos == delim) {
      count += 1;
    }else if(*pos == subdelim){
    	countField += 1;
    }
    pos++;
  }

  if(count*2+2 == countField){
	  return ++count;
  }
  return 0;
}

Bus* bus_list_create(const RawBusData* rawBus){
    return malloc(sizeof(Bus) * rawBus->numberOfBus);
}
RawBusData* bus_raw_data_create(char* data, char delim, char subdelim) {
  RawBusData* state = malloc(sizeof(RawBusData));
  state->busString = data;
  state->delim = delim;
  state->subdelim = subdelim;
  state->numberOfBus = bus_occurences(data, delim, subdelim);
  return state;
}
void bus_destroy(RawBusData* rawBus, Bus* bus) {
   if (NULL == rawBus && NULL == bus) {
     return;
   }
   if(NULL == rawBus){
     free(bus);
     bus = NULL;
     return;
   }
    free(rawBus);
    rawBus = NULL;
    free(bus);
    bus = NULL;

   return;
 }

Bus* get_bus_list(const RawBusData* rawBus, Bus* bus){
  if(0 == rawBus->numberOfBus){
    return bus;
  }
  
  uint16_t i = 0;
  char delim[2], subdelim[2];
  delim[0] = rawBus->delim;
  delim[1] = '\0';
  subdelim[0] = rawBus->subdelim;
  subdelim[1] = '\0';

   char *p = strtok (rawBus->busString, delim);
   char *array[rawBus->numberOfBus];

   while (p != NULL)
   {
       array[i++] = p;
       p = strtok (NULL, delim);
   }

   for (i = 0; i < rawBus->numberOfBus ; ++i){//TODO : fix null cases :D
	   strcpy(bus[i].id, strtok (array[i], subdelim));
	   strcpy(bus[i].name, strtok (NULL, subdelim));
	   strcpy(bus[i].time, strtok (NULL, subdelim));

   }
  return bus;
}

void print_bus_list(const RawBusData* rawBus, const Bus* bus){
  if(NULL == bus) return;
    for (int i = 0; i < rawBus->numberOfBus ; ++i){
    	printf("bus[%d] = %s\n", i, bus[i].id);
    	printf("bus[%d] = %s\n", i, bus[i].name);
    	printf("bus[%d] = %s\n", i, bus[i].time);
    	printf("\n");
    }
}

char * strtok(s, delim)	register char *s;	register const char *delim;{
	register char *spanp;
	register int c, sc;
	char *tok;
	static char *last;
	
	if (s == NULL && (s = last) == NULL)
		return (NULL);
	
cont:
	c = *s++;
	for (spanp = (char *)delim; (sc = *spanp++) != 0;) {
		if (c == sc)
			goto cont;
	}
	
	if (c == 0) {		/* no non-delimiter characters */
		last = NULL;
		return (NULL);
	}
	tok = s - 1;

	for (;;) {
		c = *s++;
		spanp = (char *)delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					s = NULL;
				else
					s[-1] = 0;
				last = s;
				return (tok);
			}
		} while (sc != 0);
	}
}

