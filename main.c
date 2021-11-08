#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define sss 225 // ß
#define oe 148 // ö
#define ae 132 // ä
#define ue 129 // ü

#define RED "\033[0;31m"
#define RESET "\033[0m"

#define ERROR RED //Funktioniert in der .exe nur mit <windows.h> und wenn man system(color...) benutzt hat.


#define TRUE 1
#define FALSE 0


typedef struct{
        unsigned int day;
        unsigned int month;
        unsigned int year;
    }date;

    //char *surname, int matrikelnummer, date startdate, date exitdate, date birthdate, student *previous, student *next
    typedef struct{
        char *surname;
        int matrikelnummer; //Muss 7 Zahlen lang sein
        date startdate;
        date exitdate;
        date birthdate;
        struct student *previous;
        struct student *next;
    }student;

// Initalisiert start und end mit NULL
void init(student **start, student **end) { // ** sind Pointer auf einen Pointer
   *start = NULL;
   *end = NULL;
}
//Überprüft ob ein eingegebenes Datum ein echtes Datum ist.
unsigned char check_date(char *date, unsigned int *day, unsigned int *month, unsigned int *year){
    unsigned char err=TRUE;
    unsigned char leap=FALSE;
    //Erster Check ob das eigegebene Datum nur aus Zahlen besteht
    if(strlen(date)==10){
        if(date[2]=='.' && date[5]=='.'){
            int ic=0;
            for(int i=0;i<10;i++) if(i!=2 && i!=5 && 47<(int)date[i] && (int)date[i]<58) ic++;
            if(ic==8){
                *day=(date[0]-'0')*10;
                *day+=(date[1]-'0');
                *month=(date[3]-'0')*10;
                *month+=(date[4]-'0');
                *year=(date[6]-'0')*1000;
                *year+=(date[7]-'0')*100;
                *year+=(date[8]-'0')*10;
                *year+=(date[9]-'0');
                err=FALSE;
            }
            else printf(ERROR "Min. eins der Eingegebenen Zeichen ist keine Zahl!\n" RESET); 
        }
        else printf(ERROR "Sie haben die Punkte falsch gesetzt!\n" RESET);
    }
    else{
        err=TRUE;
        printf(ERROR "Das Datum ist zu lang oder zu kurz!\n" RESET);
    }
    //Zweiter Check ob das Datum überhaupt Sinn macht
    if(!err){
        //Checken ob das Jahr ein Schaltjahr ist
        if(*year % 4 == 0){
            if(*year % 100 == 0) leap=(*year%400==0)?TRUE:FALSE;
            else leap=TRUE;
        }else leap=FALSE;

        //Die Monate bis Juli, weil ab August die graden Monate die längeren Tage haben
        if(*month<=7){
            if(*month%2==0){
                if(*month!=2 && *day<31){
                    err=TRUE; //Haben max 30 Tage
                    printf(ERROR "Dieser Monat hat nicht so viele Tage!\n" RESET);
                }
                if(*month==2 && leap==TRUE && *day>29){
                    err=TRUE; //Hat max 29 Tage
                    printf(ERROR "Der Monat Februar hat dieses Jahr max. 29 Tage!\n" RESET);
                }
                else if(*month==2 && leap==FALSE && *day>28){
                    err=TRUE; //Hat max 28 Tage
                    printf(ERROR "Der Monat Februar hat dieses Jahr max. 28 Tage!\n" RESET);
                }
            }
        }
        //Die Monate ab August (mit August)
        else if(*month>7 && *month<13){
            if(*month%2==0){ //August, Oktober, Dezember
                if(*day>31){
                    err=TRUE; //Haben max 31 Tage
                    printf(ERROR "Dieser Monat hat nicht so viele Tage!\n" RESET);
                }
            }
            else{ //September, November
                if(*day>30){
                    err=TRUE; //Haben max 30 Tage
                    printf(ERROR "Dieser Monat hat nicht so viele Tage!\n" RESET);
                }
            }
        }
        else{
            err=TRUE;
            printf(ERROR "Ein Jahr kann nicht mehr als 12 Monate haben!\n" RESET);
        }
    }
    if(err){
        printf("Bitte geben Sie das Datum erneut ein (DD.MM.YYYY): ");
    }
    return err;
}

//Fügt den input in die Liste ein
void addstudent(student *start, student *end){

}
//Lässt den User einen Studenten eingeben
void inputStudent(void){
    char in_surname[100];
    int in_matrikelnummer; //Darf nur 6 oder 7 Zeichen lang sein
    char in_startdate[10]; //10 wegen DD.MM.YYYY
    char in_exitdate[10];
    char in_birthdate[10];

    unsigned int    startday, startmonth, startyear,
                    exitday, exitmonth, exityear,
                    birthday, birhtmonth, birhtyear;

    unsigned char err=TRUE;

    printf("Nachname: ");
    scanf ("%[^\n]%*c", in_surname); //Dadruch kann man auch Leerzeichen mit in den String einlesen, es wird im auf \n im buffer gewartet, und mit %*c wird das \n aus dem String und dem buffer geworfen

    printf("Matrikelnummer: ");
    do{
        scanf("%d", &in_matrikelnummer);
        if(!(5<(floor(log10(abs(in_matrikelnummer))) + 1) && (floor(log10(abs(in_matrikelnummer))) + 1)<8)){ //Überprüfung ob 6 oder 7 Zeichen lang
            err=TRUE;
            printf(ERROR "Die Matrikelnummer darf nur zwischen 6 und 7 Stellen lang sein!\n" RESET);
            printf("Bitte geben Sie die Nummer erneut ein: ");
        }
        else{
            err=FALSE;
        }
    }while(err);

    //Geburtdatum<Startdatum<Austrittsdatum
    do{
        err=TRUE;
        printf("Startdatum (DD.MM.YYYY): ");
        do{
            scanf("%s", in_startdate);
        }while(check_date(in_startdate, &startday, &startmonth, &startyear));
    
        printf("Austrittsdatum (DD.MM.YYYY): ");
        do{
            scanf("%s", in_exitdate);
        }while(check_date(in_exitdate, &exitday, &exitmonth, &exityear));

        printf("Geburtstag (DD.MM.YYYY): ");
        do{
            scanf("%s", in_birthdate);
        }while(check_date(in_birthdate, &birthday, &birhtmonth, &birhtyear));
        if(birhtyear<startyear){
            if(startyear<exityear){
                err=FALSE;
            }
            else if(startyear==exityear && startmonth<exitmonth){
                err=FALSE;
            }
            else if(startyear==exityear && startmonth==exitmonth && startday<=exitday){
                err=FALSE;
            }
        }
        if(err){
            printf(ERROR "Die Daten sind widerspr%cchlich, bitte geben Sie die Daten nochmal ein!\n" RESET, ue);
        }
    }while(err);
    

    /*
    ...Übergabe an addStudent();
    */
}


int main(void){
    student *start; //Pointer der auf den ersten Eintrag zeigt
    student *end; //Pointer der auf den letzten Eintrag zeigt
    init(&start, &end);
    inputStudent();
    /*
    ...Code...
    */
}