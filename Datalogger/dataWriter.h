
#ifndef DATAWRITER_H
#define DATAWRITER_H
#include "Datalogger.h"
//it writes the data. dont close the file after each write sequence. 
//Thats how you get this thing to slow down to a crawl
void writeData(DataSet * d, int power);

#endif
