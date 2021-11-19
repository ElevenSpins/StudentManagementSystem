#include <stdio.h>
#include <stdlib.h>
#include <string.h> //Damit strcpy() finktioniert
#include <math.h> //Damit log() funktioniert
#include <conio.h> //Damit getch() funktioniert
#include <windows.h>

//Umlaute, um diese in der Konsole ausgeben zu können
#define sss 0xe1 // ß
#define oe 0x94 // ö
#define ae 0x84 // ä
#define ue 0x81 // ü

#define UE 0x9a // Ü

//Farben
#define BLACK "\x1b[30m"
#define RED "\033[0;31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define WHITE "\x1b[37m"
#define RESET "\033[0m"

//Farbzuweisung
#define ERR RED //Funktioniert in der .exe nur mit <windows.h> und wenn man system(color...) benutzt hat.
#define SUCCESS GREEN
#define IMPORTANTTEXT CYAN

#define TRUE 1
#define FALSE 0

#define MENU_ARROW 0x1a //Pfeil der nach rechts zeigt
#define SPACE 0x20 //Leerzeichen

#define HORIZONLINE 0xcd
#define VERTICALLINE 0xba
#define CROSS 0xce
#define CORNERDOWNLEFT 0xc8
#define CORNERDOWNRIGHT 0xbc
#define CORNERUPLEFT 0xc9
#define CORNERUPRIGHT 0xbb
#define TCROSSUP 0xca
#define TCROSSDOWN 0xcb
#define TCROSSRIGHT 0xcc
#define TCROSSLEFT 0xb9

#define MENUMAX 54 //Wie größ das Menü sein soll (in der Breite)

//unsigned in day, unsigned in month, unsigned in year
struct date{
        unsigned int day;
        unsigned int month;
        unsigned int year;
};

//char *surname, int matrikelnummer, date startdate, date exitdate, date birthdate, student *previous, student *next
struct student{
    char *surname;
    int matrikelnummer; //Muss 7 Zahlen lang sein
    struct date startdate;
    struct date exitdate;
    struct date birthdate;
    struct student *previous;
    struct student *next;
}*start=NULL, *end=NULL;

//Warten auf eine Nutzereingabe
void wait(void){
    printf("\n\t\t  Dr%ccken Sie eine Taste um fortzufahren...", ue);
    getch();
    printf("\n");
}

//Gibt den die Länge eines int wieder (z.B. 10 = 2, 100=3) Überprüft auch ob nummer=0 ist, da sonst Fehler entstehen
int getLength(int nummer){
    if(nummer==0) return 1;
    return (floor(log10(abs(nummer))) + 1);
}

//Überprüft ob ein eingegebenes Datum ein echtes Datum ist. Returned FALSE wenn das Datum richtig ist, TRUE wenn es falsch ist!
unsigned char checkDate(char *date, unsigned int *day, unsigned int *month, unsigned int *year){
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
            else printf("\t\t%c " ERR "Min. eins der Eingegebenen Zeichen ist keine Zahl!\n" RESET, VERTICALLINE); 
        }
        else printf("\t\t%c " ERR "Sie haben die Punkte falsch gesetzt!\n" RESET, VERTICALLINE);
    }
    else{
        err=TRUE;
        printf("\t\t%c " ERR "Das Datum ist zu lang oder zu kurz!\n" RESET, VERTICALLINE);
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
                if(*month!=2 && *day>30){
                    err=TRUE; //Haben max 30 Tage
                    printf("\t\t%c " ERR "Dieser Monat hat nicht so viele Tage!\n" RESET, VERTICALLINE);
                }
                if(*month==2 && leap==TRUE && *day>29){
                    err=TRUE; //Hat max 29 Tage
                    printf("\t\t%c " ERR "Der Monat Februar hat dieses Jahr max. 29 Tage!\n" RESET, VERTICALLINE);
                }
                else if(*month==2 && leap==FALSE && *day>28){
                    err=TRUE; //Hat max 28 Tage
                    printf("\t\t%c " ERR "Der Monat Februar hat dieses Jahr max. 28 Tage!\n" RESET, VERTICALLINE);
                }
            }
        }
        //Die Monate ab August (mit August)
        else if(*month>7 && *month<13){
            if(*month%2==0){ //August, Oktober, Dezember
                if(*day>31){
                    err=TRUE; //Haben max 31 Tage
                    printf("\t\t%c " ERR "Dieser Monat hat nicht so viele Tage!\n" RESET, VERTICALLINE);
                }
            }
            else{ //September, November
                if(*day>30){
                    err=TRUE; //Haben max 30 Tage
                    printf("\t\t%c " ERR "Dieser Monat hat nicht so viele Tage!\n" RESET, VERTICALLINE);
                }
            }
        }
        else{
            err=TRUE;
            printf("\t\t%c " ERR "Ein Jahr kann nicht mehr als 12 Monate haben!\n" RESET, VERTICALLINE);
        }
    }
    if(err){
        printf("\t\t%c Bitte geben Sie das Datum erneut ein (DD.MM.YYYY): ", VERTICALLINE);
    }
    return err;
}

//Diese Funktion gibt den pointer auf den Studenten mit der Matrikelnummer zurück, wenn keiner gefunden wurde wird NULL returned! Diese Funktion testet nicht ob es bereits Einträge gibt 
struct student *search(int sMatrikelnummer){
    struct student *now;
    now=start;
    while(now!=NULL){ //Solange now auf ein Element zeigt wird überprüft ob die Matrikelnummer übereinstimmt
        if(now->matrikelnummer==sMatrikelnummer){
            return now;
        }
        now=now->next;
    }
    return NULL;
}

//Checkt ob die Matrikelnummer zwischen 6 und 7 Ziffern lang ist und speichert die Nummer dann in der Variable auf den der Pointer zeigt
void setMatrikel(int *pMatrikelnummer){
    unsigned char err=TRUE;
    do{
        scanf("%d", pMatrikelnummer);
        fflush(stdin);
        int matrikelLength=getLength(*pMatrikelnummer);
        if(!(5<matrikelLength && matrikelLength<8)){ //Überprüfung ob 6 oder 7 Zeichen lang
            err=TRUE;
            printf("\t\t%c ",VERTICALLINE);
            printf(ERR "Die Matrikelnummer darf nur zwischen 6 und 7 Stellen lang sein!\n" RESET);
            printf("\t\t%c ",VERTICALLINE);
            printf("Bitte geben Sie die Nummer erneut ein: ");
        }
        else{
            err=FALSE;
        }
    }while(err);
}

//Fügt den input in die Liste ein (Bekommt nur pointer übergeben)
void addStudent(char *in_surname, int *in_matrikelnummer,
                unsigned int *pstartday, unsigned int *pstartmonth, unsigned int *pstartyear,
                unsigned int *pexitday, unsigned int *pexitmonth, unsigned int *pexityear,
                unsigned int *pbirthday, unsigned int *pbirthmonth, unsigned int *pbirthyear){
    int c_counter=0;
    struct student *now, *before; //now ist ein pointer auf den jetzigen Listeneintrag, before ist der Eintrag vor dem now Eintrag
    if(end==NULL){
        end=malloc(sizeof(struct student));
        if(!end){
            printf(ERR "F%cr den end pointer konnte kein Speicher reserviert werden!" RESET, ue);
            return;
        }
    }
    if(start==NULL){
        start=malloc(sizeof(struct student));
        if(!start){
            printf(ERR "F%cr den ersten student konnte kein Speicher reserviert werden!" RESET, ue);
            return;
        }
        while(in_surname[c_counter]!='\0'){
            c_counter++;
        }
        //noch einmal +1, da das \0 auch übergeben werden muss, sonst weiß strcpy() nicht wann der String endet
        c_counter++;
        //----
        start->surname=malloc(sizeof(char)*c_counter);
        if(!start->surname){
            printf(ERR "F%cr den Nachnamen konnte kein SPeicher reserviert werden!" RESET, ue);
            return;
        }
        c_counter=0;
        strcpy(start->surname,in_surname);
        start->matrikelnummer=*in_matrikelnummer;
        start->startdate.day=*pstartday;
        start->startdate.month=*pstartmonth;
        start->startdate.year=*pstartyear;
        start->exitdate.day=*pexitday;
        start->exitdate.month=*pexitmonth;
        start->exitdate.year=*pexityear;
        start->birthdate.day=*pbirthday;
        start->birthdate.month=*pbirthmonth;
        start->birthdate.year=*pbirthyear;

        start->next=NULL;
        end=start;
        end->previous=NULL;
    }
    else{
        now=start; //Wir zeigen auf das aller erste Element
        while(now->next!=NULL){
            now=now->next;    //Wir gehen über den next pointer über alle Elemente, bis das next Element NULL ist, also bis wir beim letzten Element ankommen
        }
        now->next=malloc(sizeof(struct student));
        if(!now->next){
            printf(ERR "F%cr student konnte kein Speicher reserviert werden!" RESET, ue);
            return;
        }
        before=now;
        now=now->next;
        while(in_surname[c_counter]!='\0'){
            c_counter++;
        }
        //noch einmal +1, da das \0 auch übergeben werden muss, sonst weiß strcpy() nicht wann der String endet
        c_counter++; 
        //----
        now->surname=malloc(sizeof(char)*c_counter);
        if(!now->surname){
            printf(ERR "F%cr den Nachnamen konnte kein SPeicher reserviert werden!" RESET, ue);
            return;
        }
        c_counter=0;
        strcpy(now->surname,in_surname);
        now->matrikelnummer=*in_matrikelnummer;
        now->startdate.day=*pstartday;
        now->startdate.month=*pstartmonth;
        now->startdate.year=*pstartyear;
        now->exitdate.day=*pexitday;
        now->exitdate.month=*pexitmonth;
        now->exitdate.year=*pexityear;
        now->birthdate.day=*pbirthday;
        now->birthdate.month=*pbirthmonth;
        now->birthdate.year=*pbirthyear;

        now->next=NULL;
        end=now;
        now->previous=before;
        before->next=now;
    }
}

//Lässt den User einen Studenten eingeben
void inputStudent(void){
    char in_surname[25]; //max 24 Zeichen
    int in_matrikelnummer; //Darf nur 6 oder 7 Zeichen lang sein
    char in_startdate[11]; //11 wegen DD.MM.YYYY + \0
    char in_exitdate[11];
    char in_birthdate[11];

    unsigned int    startday, startmonth, startyear,
                    exitday, exitmonth, exityear,
                    birthday, birthmonth, birthyear;

    unsigned char err=TRUE;
    printf("\t\t%c Nachname: ", VERTICALLINE);
    fgets(in_surname,25,stdin); //Setzt immer ein \0 ans Ende + man kann auch Leerzeichen einlesen, man muss auf überschuss Aufpassen bei zulangen eingaben, darum fflush() 
    fflush(stdin);
    for(int i=0;i<25;i++){  //Da fgets auch das Enter in das Array packt und das später bei der Ausgabe Probleme macht entfernen wir das hier 
        if(in_surname[i]=='\n'){
            in_surname[i]='\0';
            break;
        }
    }
    printf("\t\t%c Matrikelnummer: ", VERTICALLINE);
    do{
        err=TRUE;
        setMatrikel(&in_matrikelnummer);
        if(!start) err=FALSE; //Muss getestet werden, da search das nicht macht, ohne diesen Test würde search abstürzen (Absturz da end=NULL führt zu NULL->matrikelnummer)
        else (!search(in_matrikelnummer))?err=FALSE:printf("\t\t%c " ERR "Es gibt bereits einen Studenten mit dieser Matrikelnummer!" RESET "\n\t\t%c Bitte geben Sie eine neue Nummer ein: ", VERTICALLINE, VERTICALLINE);
    }while(err);
    
    
    //Geburtdatum<Eintrittsdatum<Austrittsdatum
    do{
        err=TRUE;
        printf("\t\t%c Eintrittsdatum (DD.MM.YYYY): ", VERTICALLINE);
        do{
            scanf("%10s", in_startdate);
            fflush(stdin);
        }while(checkDate(in_startdate, &startday, &startmonth, &startyear));
    
        printf("\t\t%c Austrittsdatum (DD.MM.YYYY): ", VERTICALLINE);
        do{
            scanf("%10s", in_exitdate);
            fflush(stdin);
        }while(checkDate(in_exitdate, &exitday, &exitmonth, &exityear));

        printf("\t\t%c Geburtstag (DD.MM.YYYY): ", VERTICALLINE);
        do{
            scanf("%10s", in_birthdate);
            fflush(stdin);
        }while(checkDate(in_birthdate, &birthday, &birthmonth, &birthyear));
        if(birthyear<startyear){
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
            printf("\t\t%c " ERR "Die Daten sind widerspr%cchlich, bitte geben Sie die Daten nochmal ein!\n" RESET, VERTICALLINE, ue);
        }
    }while(err);
    addStudent(in_surname, &in_matrikelnummer, &startday, &startmonth, &startyear, &exitday, &exitmonth, &exityear, &birthday, &birthmonth, &birthyear);
}

//Gibt die Anzahl der gespeicherten Studenten zurück
int countStudent(void){
    int countStudent=0;
    if(!start){
        return countStudent;
    }
    struct student *now;
    now=start; //Wir zeigen auf das aller erste Element
    while(now!=NULL){ //Solange now auf ein Element zeigt
        now=now->next;
        countStudent++;
    }
    return countStudent;
}

//Gibt den Studenten mit der gesuchten Matrikelnummer aus
void printStudent(void){
    int sMatrikelnummer;
    printf("\t\t%c", CORNERUPLEFT); for(int i=1;i<=91;i++) printf("%c", HORIZONLINE); printf("%c\n", CORNERUPRIGHT);
    if(!start){
        printf("\t\t%c ",VERTICALLINE);
        printf(ERR "Es gibt noch keine Eintr%cge in der Datenbank!\n" RESET, ae);
        printf("\t\t%c", CORNERDOWNLEFT); for(int i=1;i<=91;i++) printf("%c", HORIZONLINE); printf("%c\n", CORNERDOWNRIGHT);
        return;
    }
    printf("\t\t%c Zu suchende Matrikelnummer: ", VERTICALLINE);
    setMatrikel(&sMatrikelnummer);
    struct student *now=search(sMatrikelnummer);
    if(!now){
        printf("\t\t%c ",VERTICALLINE);
        printf(ERR "Es konnte kein Student mit der Nummer %d gefunden werden!\n" RESET, sMatrikelnummer);
        printf("\t\t%c", CORNERDOWNLEFT); for(int i=1;i<=91;i++) printf("%c", HORIZONLINE); printf("%c\n", CORNERDOWNRIGHT);
        return;
    }
    printf("\t\t%c",TCROSSRIGHT); for(int i=1;i<=27;i++) printf("%c", HORIZONLINE); printf("%c", TCROSSDOWN); for(int i=1;i<=16;i++) printf("%c", HORIZONLINE); printf("%c", TCROSSDOWN); for(int i=1;i<=12;i++) printf("%c", HORIZONLINE); printf("%c", TCROSSDOWN); for(int i=1;i<=16;i++) printf("%c", HORIZONLINE); printf("%c", TCROSSDOWN); for(int i=1;i<=16;i++) printf("%c", HORIZONLINE); printf("%c\n", TCROSSLEFT); 
    printf("\t\t%c Name                      %c Matrikelnummer %c Geburtstag %c Eintrittsdatum %c Austrittsdatum %c\n", VERTICALLINE, VERTICALLINE, VERTICALLINE, VERTICALLINE, VERTICALLINE, VERTICALLINE);
    printf("\t\t%c %s", VERTICALLINE, now->surname); for(int i=0;i<(25-strlen(now->surname));i++) printf(" "); printf(" %c %d", VERTICALLINE, now->matrikelnummer); if(getLength(now->matrikelnummer)==6) printf(" "); for(int i=0;i<7;i++) printf(" "); printf(" %c %02d.%02d.%04d", VERTICALLINE, now->birthdate.day, now->birthdate.month, now->birthdate.year); printf(" %c %02d.%02d.%04d    ", VERTICALLINE, now->startdate.day, now->startdate.month, now->startdate.year); printf(" %c %02d.%02d.%04d     %c\n", VERTICALLINE, now->exitdate.day, now->exitdate.month, now->exitdate.year, VERTICALLINE); 
    printf("\t\t%c",CORNERDOWNLEFT); for(int i=1;i<=27;i++) printf("%c", HORIZONLINE); printf("%c", TCROSSUP); for(int i=1;i<=16;i++) printf("%c", HORIZONLINE); printf("%c", TCROSSUP); for(int i=1;i<=12;i++) printf("%c", HORIZONLINE); printf("%c", TCROSSUP); for(int i=1;i<=16;i++) printf("%c", HORIZONLINE); printf("%c", TCROSSUP); for(int i=1;i<=16;i++) printf("%c", HORIZONLINE); printf("%c\n", CORNERDOWNRIGHT); 
}

//Löscht den Studenten mit der gesuchten Matrikelnummer
void deleteStudent(void){
    int sMatrikelnummer;
    printf("\t\t%c", CORNERUPLEFT); for(int i=1;i<=91;i++) printf("%c", HORIZONLINE); printf("%c\n", CORNERUPRIGHT);
    if(!start){
        printf("\t\t%c ",VERTICALLINE);
        printf(ERR "Es gibt noch keine Eintr%cge in der Datenbank!\n" RESET, ae);
        printf("\t\t%c", CORNERDOWNLEFT); for(int i=1;i<=91;i++) printf("%c", HORIZONLINE); printf("%c\n", CORNERDOWNRIGHT);
        return;
    }
    printf("\t\t%c Zu l%cschende Matrikelnummer: ", VERTICALLINE, oe);
    setMatrikel(&sMatrikelnummer);
    struct student *del=search(sMatrikelnummer);
    struct student *delNext=NULL; //delNext wird als pointer auf das nächste Element nach del genutzt 
    struct student *delPrev=NULL; //delPrev wird als pointer auf das Element vor dem zu löschenden genutzt
    if(!del){
        printf("\t\t%c ",VERTICALLINE);
        printf(ERR "Es konnte kein Student mit der Nummer %d gefunden werden!\n" RESET, sMatrikelnummer);
        printf("\t\t%c", CORNERDOWNLEFT); for(int i=1;i<=91;i++) printf("%c", HORIZONLINE); printf("%c\n", CORNERDOWNRIGHT);
        return;
    }
    /*LÖSCHEN*/
    if(del==start){ //Soll das erste Element gelöscht werden 
        delNext=start->next;
        if(!delNext){ //Falls es nach dem ersten Element keine weiteren 
            free(start);
            start=NULL;
            end=NULL;
            printf("\t\t%c Der Student wurde gel%cscht!\n", VERTICALLINE, oe);
            printf("\t\t%c", CORNERDOWNLEFT); for(int i=1;i<=91;i++) printf("%c", HORIZONLINE); printf("%c\n", CORNERDOWNRIGHT);
            return;
        }
        delNext->previous=NULL;
        free(start);
        start=delNext;
    }
    else if(del==end){ //Wenn das letze Element gelöscht werden soll
        delPrev=end->previous;
        delPrev->next=NULL;
        end=delPrev;
        free(del);
    }
    else{ //Ein anderes Element in der Liste soll gelöscht werden
        delPrev=del->previous;
        delNext=del->next;
        delPrev->next=delNext;
        delNext->previous=delPrev;
        free(del);
    }
    printf("\t\t%c Der Student wurde gel%cscht!\n", VERTICALLINE, oe);
    printf("\t\t%c", CORNERDOWNLEFT); for(int i=1;i<=91;i++) printf("%c", HORIZONLINE); printf("%c\n", CORNERDOWNRIGHT);
}

//Wir benutzen merge sort in der top down variante
//Bekommt zwei pointer auf den start zweier Listen, diese beiden Listen werden sortiert und zu einer Liste zusammen gesetzt, es wird ein pointer auf den start der zusammengesetzten Liste zurück gegeben
struct student *merge(struct student *list1, struct student *list2) {
    struct student *head=NULL, **pp;
    pp = &head;
    while(TRUE){
        if (strcmp(list1->surname, list2->surname) <= 0){ //Wenn list1 mit a anfängt und list2 mit a oder b (nur ein Beispiel)
            *pp = list1; //*pp=liste1 == head=liste1 (aber nur im aller ersten durchlauf der while-Schleife!!!!) 
            pp = &list1->next; //pp wird zum pointer auf die Adresse vom nächsten Element 
            list1 = list1->next; //liste1 wird zum pointer auf das nächste Element
            if (list1 == NULL){ //Wenn liste1 nur einen Eintrag hat
                *pp = list2; //liste1->next=list2
                break;      //Wenn es also nur einen Eintrag in list1 gibt und der surname z.B. mit a anfängt und der surname in list2 mit a oder allem anderen beginnt, wird hier der list1 Eintrag vor den list2 Eintrag gesezt
            } //Da head jetzt auf das am anfang übergebene list1 Element zeigt wird head am Ende zurück gegeben, da man so über head->next auf das list2 Element kommt
        }
        else{
            *pp = list2;    //Das selbe nur wenn list2 mit a anfängt und list1 mit b (nur ein Beispiel)
            pp = &list2->next;
            list2 = list2->next;
            if (list2 == NULL){
                *pp = list1;
                break;
            }
        }
    }
    return head;
}

//Bekommt einen Pointer, ab diesem Pointer wird die Liste sortiert 
struct student *msort(struct student *sortStart){
    struct student *now, *after;
    //Wenn der pointer=Null ist oder die Liste auf die der Pointer verweist nur einen Eintrag ist ist die Liste bereits sortiert
    if ((sortStart==NULL) || (sortStart->next==NULL)) return sortStart;
    //Die Liste wird geteilt, um diese sortieren zu können, darum suchen wir mit dieser for-Schleife die Mitte der Liste ab den Eintrag auf den sortStart zeigt
    //Dabei geht now immer einen Eintrag weiter, währen after immer zwei Einträge weiter geht, wenn z.B. after->next==NULL ist wird die Schleife abgebrochen, weil after dann am letzen Eintrag angekommen ist.
    //(Das passiert wenn die Anzahl der Elemente ab sortStart grade ist.)
    //Wenn die Liste ab sortStart ungrade ist, währe after->next->next=NULL, wenn man am vorletzen Element währe, dadurch würde die SChleife auch abgebrochen werden, da jetzt after==NULL ist 
    for (now=sortStart, after=sortStart->next; after && after->next; after=after->next->next) now=now->next;
    /* split the list at mid-point */
    after=now->next;
    now->next=NULL;//Die Liste wird zwischen now und after getrennt, man kann also nur noch von after->previous zurück kommen
    now=sortStart; //now wird jetzt zum sortStart, aber die Liste die man jetzt ab sortStart abrufen kann ist nur noch halb so lang
    /* sort the sublists recursively */
    now=msort(now);
    after=msort(after);
    return merge(now, after);
}

//Sortiert die Liste
void sort(void){
    struct student *now, *before; //now ist ein pointer auf den jetzigen Listeneintrag, before ist ein pointer auf den Eintrag vor dem now Eintrag
    //Die Liste wird so sortiert, als ob es nur eine einfach verkettete Liste wäre
    start=msort(start);
    // previous Pointer sind jetzt komplett durcheinander, darum muss man diese wieder reparieren
    // Der Pointer before ist immer vor now
    before=NULL;
    for (now=start; now; now=now->next) {   //Da now durch now=now->next am Ende der Liste zu NULL wird
        now->previous=before;
        before=now;
    }
    end=before;
}

//Gibt alle Stundenten in der Liste aus
void printAllStudents(void){
    if(!start){
        printf("\t\t%c", CORNERUPLEFT); for(int i=1;i<=91;i++) printf("%c", HORIZONLINE); printf("%c\n", CORNERUPRIGHT);
        printf("\t\t%c ",VERTICALLINE);
        printf(ERR "Es gibt noch keine Eintr%cge in der Datenbank!\n" RESET, ae);
        printf("\t\t%c", CORNERDOWNLEFT); for(int i=1;i<=91;i++) printf("%c", HORIZONLINE); printf("%c\n", CORNERDOWNRIGHT);
        return;
    }
    printf("\t\t%c",CORNERUPLEFT); for(int i=1;i<=27;i++) printf("%c", HORIZONLINE); printf("%c", TCROSSDOWN); for(int i=1;i<=16;i++) printf("%c", HORIZONLINE); printf("%c", TCROSSDOWN); for(int i=1;i<=12;i++) printf("%c", HORIZONLINE); printf("%c", TCROSSDOWN); for(int i=1;i<=16;i++) printf("%c", HORIZONLINE); printf("%c", TCROSSDOWN); for(int i=1;i<=16;i++) printf("%c", HORIZONLINE); printf("%c\n", CORNERUPRIGHT); 
    printf("\t\t%c Name                      %c Matrikelnummer %c Geburtstag %c Eintrittsdatum %c Austrittsdatum %c\n", VERTICALLINE, VERTICALLINE, VERTICALLINE, VERTICALLINE, VERTICALLINE, VERTICALLINE);
    if(start==end){
        printf("\t\t%c %s", VERTICALLINE, start->surname); for(int i=0;i<(25-strlen(start->surname));i++) printf(" "); printf(" %c %d", VERTICALLINE, start->matrikelnummer); if(getLength(start->matrikelnummer)==6) printf(" "); for(int i=0;i<7;i++) printf(" "); printf(" %c %02d.%02d.%04d", VERTICALLINE, start->birthdate.day, start->birthdate.month, start->birthdate.year); printf(" %c %02d.%02d.%04d    ", VERTICALLINE, start->startdate.day, start->startdate.month, start->startdate.year); printf(" %c %02d.%02d.%04d     %c\n", VERTICALLINE, start->exitdate.day, start->exitdate.month, start->exitdate.year, VERTICALLINE);  
    }
    else{
        sort();
        struct student *now;
        now=start; //Wir zeigen auf das erste Element
        while(now!=NULL){ //Solange now noch auf ein Element zeigt
            printf("\t\t%c %s", VERTICALLINE, now->surname); for(int i=0;i<(25-strlen(now->surname));i++) printf(" "); printf(" %c %d", VERTICALLINE, now->matrikelnummer); if(getLength(now->matrikelnummer)==6) printf(" "); for(int i=0;i<7;i++) printf(" "); printf(" %c %02d.%02d.%04d", VERTICALLINE, now->birthdate.day, now->birthdate.month, now->birthdate.year); printf(" %c %02d.%02d.%04d    ", VERTICALLINE, now->startdate.day, now->startdate.month, now->startdate.year); printf(" %c %02d.%02d.%04d     %c\n", VERTICALLINE, now->exitdate.day, now->exitdate.month, now->exitdate.year, VERTICALLINE); 
            now=now->next;
        }
    }
    printf("\t\t%c",CORNERDOWNLEFT); for(int i=1;i<=27;i++) printf("%c", HORIZONLINE); printf("%c", TCROSSUP); for(int i=1;i<=16;i++) printf("%c", HORIZONLINE); printf("%c", TCROSSUP); for(int i=1;i<=12;i++) printf("%c", HORIZONLINE); printf("%c", TCROSSUP); for(int i=1;i<=16;i++) printf("%c", HORIZONLINE); printf("%c", TCROSSUP); for(int i=1;i<=16;i++) printf("%c", HORIZONLINE); printf("%c\n", CORNERDOWNRIGHT);
}

void read(void){
    //Variablen zum Einlesen der Datei
    FILE *db;
    char row[256];
    char *split; //Welches Zeichen die csv Datei einteilt

    //Variablen zum richtigen Zuordnen der eingelesenen Werte
    int order[5]={0,1,2,3,4}; //0=surname, 1=matrikelnummer, 2=startdate, 3=exitdate, 4=birthdate
    int run=0;
    int err_run=0; //Sagt wie oft in der header Line abfrage das else{} durchlaufen ist
    int line=0;
    int c_null=0;
    //Variablen zur übergabe an addStudent()
    char in_surname[25]; //max 24 Zeichen
    int in_matrikelnummer; //Darf nur 6 oder 7 Zeichen lang sein
    char in_startdate[11]; //11 wegen DD.MM.YYYY + \0
    char in_exitdate[11];
    char in_birthdate[11];
    unsigned int    startday, startmonth, startyear,
                    exitday, exitmonth, exityear,
                    birthday, birthmonth, birthyear;
    unsigned char err=TRUE; //Wird genutzt um do-while Schleifen zu loopen, wenn der Nutzer fehlerhafte Eingaben tätigt
    unsigned char did_err=FALSE; //Sagt ob es in einem switch case einen Fehler gab (Für die Ausgabe der Fehler Nachricht)
    //------------------------------------------------------
    db=fopen("studentDB.csv","r");
    if(!db) return; //Wenn die Datei nicht existiert return
    printf("\t\t%c", CORNERUPLEFT); for(int i=1;i<=MENUMAX+20;i++) printf("%c", HORIZONLINE); printf("%c\n", CORNERUPRIGHT);
    //Test ob die erste Zeile der csv ein header ist, wenn nicht werden alle Einträge wie folgt eingelesen: surname, matrikelnummer, startdate, exitdate, birthdate 
    if(!feof(db)){
        fgets(row,100,db);
        split=strtok(row, ",");
        run=0;
        while(split){
            if(row[0]=='\n') break;
            if(run==4){
                for(int i=0;i<25;i++){
                    if(split[i]=='\n'){
                        split[i]='\0';
                    break;
                    }
                }
            }
            if(!strcmp(split,"surname")){
                order[run]=0;
            }
            else if(!strcmp(split,"matrikelnummer")){
                order[run]=1;
            }
            else if(!strcmp(split,"startdate")){
                order[run]=2;
            }
            else if(!strcmp(split,"exitdate")){
                order[run]=3;
            }
            else if(!strcmp(split,"birthdate")){
                order[run]=4;
            }
            else{
                for(int i=0;i<5;i++) order[i]=i;
                if(run==err_run){
                    switch(order[run]){
                        case 0:
                            c_null=0;                          
                            while(split[c_null]!='\0') c_null++;
                            if(c_null>24) split[24]='\0';
                            strcpy(in_surname,split);
                        break;
                        case 1:
                            err=FALSE;
                            in_matrikelnummer=atoi(split);
                            do{
                                do{
                                    if(err){
                                        setMatrikel(&in_matrikelnummer);
                                        err=FALSE;
                                    }
                                    else{
                                        int matrikelLength=getLength(in_matrikelnummer);
                                        if(!(5<matrikelLength && matrikelLength<8)){ //Überprüfung ob 6 oder 7 Zeichen lang
                                            err=TRUE;
                                            printf("\t\t%c ",VERTICALLINE);
                                            printf(ERR "Die Matrikelnummer darf nur zwischen 6 und 7 Stellen lang sein!\n" RESET);
                                            printf("\t\t%c ",VERTICALLINE);
                                            printf("Bitte geben Sie die Nummer erneut ein: ");
                                            did_err=TRUE;
                                        }
                                    }
                                }while(err);
                                err=TRUE;
                                if(!start) err=FALSE; //Muss getestet werden, da search das nicht macht, ohne diesen Test würde search abstürzen (Absturz da end=NULL führt zu NULL->matrikelnummer)
                                else{
                                    if(!search(in_matrikelnummer)) err=FALSE;
                                    else{
                                        printf("\t\t%c " ERR "Es gibt bereits einen Studenten mit dieser Matrikelnummer!" RESET "\n\t\t%c Bitte geben Sie eine neue Nummer ein: ", VERTICALLINE, VERTICALLINE);
                                        did_err=TRUE;
                                    }
                                }
                            }while(err);
                            if(did_err){
                                printf("\t\t%c " ERR "Dieser Fehler wurde durch die .csv Datei erzeugt!\n" RESET, VERTICALLINE);
                                printf("\t\t%c " ERR "Position des Fehlers: (Zeile: %d / Spalte: %d)\n" RESET, VERTICALLINE, line+1, run+1);
                                did_err=FALSE;
                            }
                        break;
                        case 2:
                            if(split[10]!='\0') split[10]='\0';
                            strcpy(in_startdate,split);
                            while(checkDate(in_startdate, &startday, &startmonth, &startyear)){
                                printf("(Eintrittsdatum) ");
                                scanf("%10s", in_startdate);
                                fflush(stdin);
                                did_err=TRUE;
                            }
                            if(did_err){
                                printf("\t\t%c " ERR "Dieser Fehler wurde durch die .csv Datei erzeugt!\n" RESET, VERTICALLINE);
                                printf("\t\t%c " ERR "Position des Fehlers: (Zeile: %d / Spalte: %d)\n" RESET, VERTICALLINE, line+1, run+1);
                                did_err=FALSE;
                            }
                        break;
                        case 3:
                            if(split[10]!='\0') split[10]='\0';
                            strcpy(in_exitdate,split);
                            while(checkDate(in_exitdate, &exitday, &exitmonth, &exityear)){
                                printf("(Austrittsdatum) ");
                                scanf("%10s", in_exitdate);
                                fflush(stdin);
                                did_err=TRUE;
                            }
                            if(did_err){
                                printf("\t\t%c " ERR "Dieser Fehler wurde durch die .csv Datei erzeugt!\n" RESET, VERTICALLINE);
                                printf("\t\t%c " ERR "Position des Fehlers: (Zeile: %d / Spalte: %d)\n" RESET, VERTICALLINE, line+1, run+1);
                                did_err=FALSE;
                            }
                        break;
                        case 4:
                            if(split[10]!='\0') split[10]='\0';
                            strcpy(in_birthdate,split);
                            while(checkDate(in_birthdate, &birthday, &birthmonth, &birthyear)){
                                printf("(Geburtsdatum) ");
                                scanf("%10s", in_birthdate);
                                fflush(stdin);
                                did_err=TRUE;
                            }
                            if(did_err){
                                printf("\t\t%c " ERR "Dieser Fehler wurde durch die .csv Datei erzeugt!\n" RESET, VERTICALLINE);
                                printf("\t\t%c " ERR "Position des Fehlers: (Zeile: %d / Spalte: %d)\n" RESET, VERTICALLINE, line+1, run+1);
                                did_err=FALSE;
                            }
                        break;
                    }
                }
                err_run++;
            }
            run++;
            split=strtok(NULL, ",");
        }
    }
    else{
        fclose(db);
        return;
    }
    if(row[0]!='\n'){
        if(err_run==run){
            err=TRUE;
            if(birthyear<startyear){
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
                printf("\t\t%c " ERR "Die Daten in Zeile %d sind widerspr%cchlich, bitte geben Sie die Daten nochmal ein!\n" RESET, VERTICALLINE, line+1, ue);
            }
            while(err){
                err=TRUE;
                printf("\t\t%c Eintrittsdatum (DD.MM.YYYY): ", VERTICALLINE);
                do{
                    scanf("%10s", in_startdate);
                    fflush(stdin);
                }while(checkDate(in_startdate, &startday, &startmonth, &startyear));
    
                printf("\t\t%c Austrittsdatum (DD.MM.YYYY): ", VERTICALLINE);
                do{
                    scanf("%10s", in_exitdate);
                    fflush(stdin);
                }while(checkDate(in_exitdate, &exitday, &exitmonth, &exityear));

                printf("\t\t%c Geburtstag (DD.MM.YYYY): ", VERTICALLINE);
                do{
                    scanf("%10s", in_birthdate);
                    fflush(stdin);
                }while(checkDate(in_birthdate, &birthday, &birthmonth, &birthyear));
                if(birthyear<startyear){
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
                    printf("\t\t%c " ERR "Die Daten in Zeile %d sind widerspr%cchlich, bitte geben Sie die Daten nochmal ein!\n" RESET, VERTICALLINE, line+1, ue);
                }
            }
            addStudent(in_surname, &in_matrikelnummer, &startday, &startmonth, &startyear, &exitday, &exitmonth, &exityear, &birthday, &birthmonth, &birthyear);
            printf("\t\t%c " SUCCESS "Zeile %d konnte eingelesen werden!\n" RESET, VERTICALLINE, line+1);
        }
        else if(err_run==0 && (order[0]+order[1]+order[2]+order[3]+order[4])==10){
            printf("\t\t%c " SUCCESS "Die header Zeile konnte erfolgreich gelesen werden!\n" RESET, VERTICALLINE);
            printf("\t\t%c " SUCCESS "Einlese Array: %d %d %d %d %d\n" RESET, VERTICALLINE, order[0], order[1], order[2], order[3], order[4]);
        }
    }
    else{
        printf("\t\t%c " ERR "Die .csv Datei hat eine fehlerhafte header Zeile!\n" RESET, VERTICALLINE);
        for(int i=0;i<5;i++) order[i]=i;
        printf("\t\t%c " ERR "Einlese Array ge%cndert zu: %d %d %d %d %d\n" RESET, VERTICALLINE, ae, order[0], order[1], order[2], order[3], order[4]);
    }
    line++;

    //Einlesen der restlichen Zeilen in der .csv Datei -----------------------------------
    while(!feof(db)){
        fgets(row,60,db);
        split=strtok(row, ",");
        run=0;
        while(split){
            if(row[0]=='\n') break;
            if(run==4){
                for(int i=0;i<25;i++){
                    if(split[i]=='\n'){
                        split[i]='\0';
                    break;
                    }
                }
            }
            switch(order[run]){
                case 0:
                    c_null=0;
                    while(split[c_null]!='\0') c_null++;
                    if(c_null>24) split[24]='\0';
                    strcpy(in_surname,split);
                break;
                case 1:
                    err=FALSE;
                    in_matrikelnummer=atoi(split);
                    do{
                        do{
                            if(err){
                                setMatrikel(&in_matrikelnummer);
                                err=FALSE;
                            }
                            else{
                                int matrikelLength=getLength(in_matrikelnummer);
                                if(!(5<matrikelLength && matrikelLength<8)){ //Überprüfung ob 6 oder 7 Zeichen lang
                                    err=TRUE;
                                    printf("\t\t%c ",VERTICALLINE);
                                    printf(ERR "Die Matrikelnummer darf nur zwischen 6 und 7 Stellen lang sein!\n" RESET);
                                    printf("\t\t%c ",VERTICALLINE);
                                    printf("Bitte geben Sie die Nummer erneut ein: ");
                                    did_err=TRUE;
                                }
                            }
                        }while(err);
                        err=TRUE;
                        if(!start) err=FALSE; //Muss getestet werden, da search das nicht macht, ohne diesen Test würde search abstürzen (Absturz da end=NULL führt zu NULL->matrikelnummer)
                        else{
                            if(!search(in_matrikelnummer)) err=FALSE;
                            else{
                                printf("\t\t%c " ERR "Es gibt bereits einen Studenten mit dieser Matrikelnummer!" RESET "\n\t\t%c Bitte geben Sie eine neue Nummer ein: ", VERTICALLINE, VERTICALLINE);
                                did_err=TRUE;
                            }
                        }
                    }while(err);
                    if(did_err){
                        printf("\t\t%c " ERR "Dieser Fehler wurde durch die .csv Datei erzeugt!\n" RESET, VERTICALLINE);
                        printf("\t\t%c " ERR "Position des Fehlers: (Zeile: %d / Spalte: %d)\n" RESET, VERTICALLINE, line+1, run+1);
                        did_err=FALSE;
                    }
                break;
                case 2:
                    if(split[10]!='\0') split[10]='\0';
                    strcpy(in_startdate,split);
                    while(checkDate(in_startdate, &startday, &startmonth, &startyear)){
                        printf("(Eintrittsdatum) ");
                        scanf("%10s", in_startdate);
                        fflush(stdin);
                        did_err=TRUE;
                    }
                    if(did_err){
                        printf("\t\t%c " ERR "Dieser Fehler wurde durch die .csv Datei erzeugt!\n" RESET, VERTICALLINE);
                        printf("\t\t%c " ERR "Position des Fehlers: (Zeile: %d / Spalte: %d)\n" RESET, VERTICALLINE, line+1, run+1);
                        did_err=FALSE;
                    }
                break;
                case 3:
                    if(split[10]!='\0') split[10]='\0';
                    strcpy(in_exitdate,split);
                    while(checkDate(in_exitdate, &exitday, &exitmonth, &exityear)){
                        printf("(Austrittsdatum) ");
                        scanf("%10s", in_exitdate);
                        fflush(stdin);
                        did_err=TRUE;
                    }
                    if(did_err){
                        printf("\t\t%c " ERR "Dieser Fehler wurde durch die .csv Datei erzeugt!\n" RESET, VERTICALLINE);
                        printf("\t\t%c " ERR "Position des Fehlers: (Zeile: %d / Spalte: %d)\n" RESET, VERTICALLINE, line+1, run+1);
                        did_err=FALSE;
                    }
                break;
                case 4:
                    if(split[10]!='\0') split[10]='\0';
                    strcpy(in_birthdate,split);
                    while(checkDate(in_birthdate, &birthday, &birthmonth, &birthyear)){
                        printf("Geburtsdatum) ");
                        scanf("%10s", in_birthdate);
                        fflush(stdin);
                        did_err=TRUE;
                    }
                    if(did_err){
                        printf("\t\t%c " ERR "Dieser Fehler wurde durch die .csv Datei erzeugt!\n" RESET, VERTICALLINE);
                        printf("\t\t%c " ERR "Position des Fehlers: (Zeile: %d / Spalte: %d)\n" RESET, VERTICALLINE, line+1, run+1);
                        did_err=FALSE;
                    }
                break;
            }
            run++;
            split=strtok(NULL, ",");
        }
        err=TRUE;
        if(row[0]!='\n'){
            if(birthyear<startyear){
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
                printf("\t\t%c " ERR "Die Daten in Zeile %d sind widerspr%cchlich, bitte geben Sie die Daten nochmal ein!\n" RESET, VERTICALLINE, line+1, ue);
            }
            while(err){
                err=TRUE;
                printf("\t\t%c Eintrittsdatum (DD.MM.YYYY): ", VERTICALLINE);
                do{
                    scanf("%10s", in_startdate);
                    fflush(stdin);
                }while(checkDate(in_startdate, &startday, &startmonth, &startyear));
    
                printf("\t\t%c Austrittsdatum (DD.MM.YYYY): ", VERTICALLINE);
                do{
                    scanf("%10s", in_exitdate);
                    fflush(stdin);
                }while(checkDate(in_exitdate, &exitday, &exitmonth, &exityear));

                printf("\t\t%c Geburtstag (DD.MM.YYYY): ", VERTICALLINE);
                do{
                    scanf("%10s", in_birthdate);
                    fflush(stdin);
                }while(checkDate(in_birthdate, &birthday, &birthmonth, &birthyear));
                if(birthyear<startyear){
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
                    printf("\t\t%c " ERR "Die Daten in Zeile %d sind widerspr%cchlich, bitte geben Sie die Daten nochmal ein!\n" RESET, VERTICALLINE, line+1, ue);
                }
            }
            addStudent(in_surname, &in_matrikelnummer, &startday, &startmonth, &startyear, &exitday, &exitmonth, &exityear, &birthday, &birthmonth, &birthyear);
            printf("\t\t%c " SUCCESS "Zeile %d konnte eingelesen werden!\n" RESET, VERTICALLINE, line+1);
            line++;
        }
        else{
            printf("\t\t%c " ERR "Zeile %d ist leer!\n" RESET, VERTICALLINE, line+1);
        }
    }
    
    fclose(db);
    printf("\t\t%c", CORNERDOWNLEFT); for(int i=1;i<=MENUMAX+20;i++) printf("%c", HORIZONLINE); printf("%c\n", CORNERDOWNRIGHT);
}

void save(void){
    FILE *db;
    db=fopen("studentDB.csv","w");
    if(!db) return;
    struct student *now=start;
    fprintf(db, "surname,matrikelnummer,startdate,exitdate,birthdate\n");
    while(now){
        fprintf(db, "%s,%d,%02d.%02d.%04d,%02d.%02d.%04d,%02d.%02d.%04d", now->surname, now->matrikelnummer, now->startdate.day, now->startdate.month, now->startdate.year, now->exitdate.day, now->exitdate.month, now->exitdate.year, now->birthdate.day, now->birthdate.month, now->birthdate.year);
        now=now->next;
        if(now){
            fprintf(db, "\n");
        }
    }
    fclose(db);
}

//Menü
int menu(void){
    int pos=0;
    char input='o';
    do{
        system("cls");
        switch(input){
            case 'w':
                pos==0?pos=5:pos--;
            break;
            case 's':
                pos==5?pos=0:pos++;
            break;
        }
        printf("\t\t%c", CORNERUPLEFT); for(int i=1;i<=MENUMAX;i++) printf("%c", HORIZONLINE); printf("%c\n", CORNERUPRIGHT);
        printf("\t\t%c", VERTICALLINE); for(int i=1;i<=(MENUMAX/2)-2;i++) printf("%c", SPACE); printf("MEN%c", UE); for(int i=1;i<=(MENUMAX/2)-2;i++) printf("%c", SPACE); printf("%c\n", VERTICALLINE);
        printf("\t\t%c Dr%ccken Sie x, wenn Sie eine Auswahl getroffen haben %c\n", VERTICALLINE, ue, VERTICALLINE);
        printf("\t\t%c         Dr%ccken Sie w oder s um zu navigieren        %c\n", VERTICALLINE, ue, VERTICALLINE);
        printf("\t\t%c", TCROSSRIGHT); for(int i=1;i<=MENUMAX;i++) printf("%c", HORIZONLINE); printf("%c\n", TCROSSLEFT);

        printf("\t\t%c", VERTICALLINE); for(int i=1;i<=5;i++) printf("%c", SPACE); pos==0?printf("%c", MENU_ARROW):printf("%c", SPACE); for(int i=1;i<=5;i++) printf("%c", SPACE); printf("1. Eingabe eines neuen Studenten"); for(int i=1;i<=11;i++) printf("%c", SPACE); printf("%c\n", VERTICALLINE);
        printf("\t\t%c", VERTICALLINE); for(int i=1;i<=5;i++) printf("%c", SPACE); pos==1?printf("%c", MENU_ARROW):printf("%c", SPACE); for(int i=1;i<=5;i++) printf("%c", SPACE); printf("2. Gesamte Anzahl aller Studenten"); for(int i=1;i<=10;i++) printf("%c", SPACE); printf("%c\n", VERTICALLINE);
        printf("\t\t%c", VERTICALLINE); for(int i=1;i<=5;i++) printf("%c", SPACE); pos==2?printf("%c", MENU_ARROW):printf("%c", SPACE); for(int i=1;i<=5;i++) printf("%c", SPACE); printf("3. Studenten suchen"); for(int i=1;i<=24;i++) printf("%c", SPACE); printf("%c\n", VERTICALLINE);
        printf("\t\t%c", VERTICALLINE); for(int i=1;i<=5;i++) printf("%c", SPACE); pos==3?printf("%c", MENU_ARROW):printf("%c", SPACE); for(int i=1;i<=5;i++) printf("%c", SPACE); printf("4. Ausgabe aller Studenten"); for(int i=1;i<=17;i++) printf("%c", SPACE); printf("%c\n", VERTICALLINE);
        printf("\t\t%c", VERTICALLINE); for(int i=1;i<=5;i++) printf("%c", SPACE); pos==4?printf("%c", MENU_ARROW):printf("%c", SPACE); for(int i=1;i<=5;i++) printf("%c", SPACE); printf("5. Studenten l%cschen", oe); for(int i=1;i<=23;i++) printf("%c", SPACE); printf("%c\n", VERTICALLINE);
        printf("\t\t%c", VERTICALLINE); for(int i=1;i<=5;i++) printf("%c", SPACE); pos==5?printf("%c", MENU_ARROW):printf("%c", SPACE); for(int i=1;i<=5;i++) printf("%c", SPACE); printf("6. Beenden"); for(int i=1;i<=33;i++) printf("%c", SPACE); printf("%c\n", VERTICALLINE);

        printf("\t\t%c", CORNERDOWNLEFT); for(int i=1;i<=MENUMAX;i++) printf("%c", HORIZONLINE); printf("%c\n", CORNERDOWNRIGHT);
        printf("\t\t");
        fflush(stdin);
        input=getch();
        printf("\n");

    }while(input!='x');
    return pos;
}

//Main
int main(void){
    system("color");
    read();
    //wait();
    system("cls");
    int select;
    do{
        select=menu();
        switch (select)
        {
        case 0:
        system("cls");
            printf("\t\t%c", CORNERUPLEFT); for(int i=1;i<=MENUMAX;i++) printf("%c", HORIZONLINE); printf("%c\n", CORNERUPRIGHT);
            inputStudent();
        break;
        case 1:
            //Schöne Ausgabe
            system("cls");
            printf("\t\t%c", CORNERUPLEFT); for(int i=1;i<=MENUMAX;i++) printf("%c", HORIZONLINE); printf("%c\n", CORNERUPRIGHT);
            int len=3;
            int count=countStudent();
            len=4-getLength(count);
            printf("\t\t%c Es befinden sich " IMPORTANTTEXT "%d" RESET, VERTICALLINE, count);  printf(" Eintr%cge in der Datenbank!     ", ae); for(int i=0;i<len;i++) printf(" "); printf("%c\n", VERTICALLINE);
            printf("\t\t%c", CORNERDOWNLEFT); for(int i=1;i<=MENUMAX;i++) printf("%c", HORIZONLINE); printf("%c\n", CORNERDOWNRIGHT);
            wait();
        break;
        case 2:
            system("cls");
            printStudent();
            wait();
        break;
        case 3:
            system("cls");
            printAllStudents();
            wait();
        break;
        case 4:
            system("cls");
            deleteStudent();
            wait();
        break;
        
        case 5:
            //Ende des Programms (Hier muss nichts hinzugefügt werden!)
        break;
        
        default:
            printf("\t\t%c " ERR "!Fehler select hat den ung%cltigen Wert '%d'!\n" RESET, VERTICALLINE, ue, select);
        break;
        }
    }while(select!=5);
    save();
    system("cls");
    return 0;
}
