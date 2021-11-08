#include <stdio.h>
#include <stdlib.h>

typedef struct{
        unsigned int day;
        unsigned int month;
        unsigned int year;
    }date;

    //Doppelt Verkettete Liste
    typedef struct{
        char *surname;
        int martikelnummer;
        date startdate;
        date exitdate;
        date birthdate;
        struct student *next;
        struct student *previous;
    }student;

// Initalisiert start und end mit NULL
void init(student **start, student **end) { // ** sind Pointer auf einen Pointer
   *start = NULL;
   *end = NULL;
}


int main(void){

    

    student *start; //Pointer der auf den ersten Eintrag zeigt
    student *end; //Pointer der auf den letzten Eintrag zeigt
    init(&start, &end);

}