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
#define COLORARROW GREEN
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
}*start, *end;

//Initalisiert start und end mit NULL
void init(void){ // ** sind Pointer auf einen Pointer
   start = NULL;
   end = NULL;
}

//Warten auf eine Nutzereingabe
void wait(void){
    printf("\t\t  Dr%ccken Sie eine Taste um fortzufahren...", ue);
    getch();
    printf("\n");
}

//Gibt den die Länge eines int wieder (z.B. 10 = 2, 100=3) Überprüft auch ob nummer=0 ist, da sonst Fehler entstehen
int getLength(int nummer){
    if(nummer==0) return 1;
    return (floor(log10(abs(nummer))) + 1);
}

//Überprüft ob ein eingegebenes Datum ein echtes Datum ist.
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
            else printf(ERR "Min. eins der Eingegebenen Zeichen ist keine Zahl!\n" RESET); 
        }
        else printf(ERR "Sie haben die Punkte falsch gesetzt!\n" RESET);
    }
    else{
        err=TRUE;
        printf(ERR "Das Datum ist zu lang oder zu kurz!\n" RESET);
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
                    printf(ERR "Dieser Monat hat nicht so viele Tage!\n" RESET);
                }
                if(*month==2 && leap==TRUE && *day>29){
                    err=TRUE; //Hat max 29 Tage
                    printf(ERR "Der Monat Februar hat dieses Jahr max. 29 Tage!\n" RESET);
                }
                else if(*month==2 && leap==FALSE && *day>28){
                    err=TRUE; //Hat max 28 Tage
                    printf(ERR "Der Monat Februar hat dieses Jahr max. 28 Tage!\n" RESET);
                }
            }
        }
        //Die Monate ab August (mit August)
        else if(*month>7 && *month<13){
            if(*month%2==0){ //August, Oktober, Dezember
                if(*day>31){
                    err=TRUE; //Haben max 31 Tage
                    printf(ERR "Dieser Monat hat nicht so viele Tage!\n" RESET);
                }
            }
            else{ //September, November
                if(*day>30){
                    err=TRUE; //Haben max 30 Tage
                    printf(ERR "Dieser Monat hat nicht so viele Tage!\n" RESET);
                }
            }
        }
        else{
            err=TRUE;
            printf(ERR "Ein Jahr kann nicht mehr als 12 Monate haben!\n" RESET);
        }
    }
    if(err){
        printf("Bitte geben Sie das Datum erneut ein (DD.MM.YYYY): ");
    }
    return err;
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
                    birthday, birhtmonth, birhtyear;

    unsigned char err=TRUE;
    printf("Nachname: ");
    fgets(in_surname,25,stdin); //Setzt immer ein \0 ans Ende + man kann auch Leerzeichen einlesen, man muss auf überschuss Aufpassen bei zulangen eingaben, darum fflush() 
    fflush(stdin);

    printf("Matrikelnummer: ");
    setMatrikel(&in_matrikelnummer);
    
    //Geburtdatum<Eintrittsdatum<Austrittsdatum
    do{
        err=TRUE;
        printf("Eintrittsdatum (DD.MM.YYYY): ");
        do{
            scanf("%10s", in_startdate);
            fflush(stdin);
        }while(checkDate(in_startdate, &startday, &startmonth, &startyear));
    
        printf("Austrittsdatum (DD.MM.YYYY): ");
        do{
            scanf("%10s", in_exitdate);
            fflush(stdin);
        }while(checkDate(in_exitdate, &exitday, &exitmonth, &exityear));

        printf("Geburtstag (DD.MM.YYYY): ");
        do{
            scanf("%10s", in_birthdate);
            fflush(stdin);
        }while(checkDate(in_birthdate, &birthday, &birhtmonth, &birhtyear));
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
            printf(ERR "Die Daten sind widerspr%cchlich, bitte geben Sie die Daten nochmal ein!\n" RESET, ue);
        }
    }while(err);
    
    addStudent(in_surname, &in_matrikelnummer, &startday, &startmonth, &startyear, &exitday, &exitmonth, &exityear, &birthday, &birhtmonth, &birhtyear);
}

//Gibt die Anzahl der gespeicherten Studenten zurück
int countStudent(void){
    int countStudent=0;
    if(!start){
        return countStudent;
    }
    struct student *now;
    countStudent++; //Da es einen Eintrag gibt (sonst wäre if(!start) ausgeführt worden)
    now=start; //Wir zeigen auf das aller erste Element
    while(now->next!=NULL){ //Wir gucken ob das erste Element auf ein weiteres Zeigt (Dafür muss es bereit 1. Element geben)
        now=now->next;
        countStudent++;
    }
    return countStudent;
}

//Geht die Liste durch bis die matrikelnummer gefunden wird, oder das Ende der Liste erreich ist
void printStudent(int sMatrikelnummer){
    if(!start){
        printf("\t\t%c ",VERTICALLINE);
        printf(ERR "Es gibt noch keine Eintr%cge in der Datenbank!\n" RESET, ae);
        printf("\t\t%c", CORNERDOWNLEFT); for(int i=1;i<=91;i++) printf("%c", HORIZONLINE); printf("%c\n", CORNERDOWNRIGHT);
        return;
    }
    //Es wird der letzte Eintrag überprüft, da die while Schleife alle Einträge, außer den letzten Überprüft (Der letzte Eintrag hat nähmlich now->next==NULL darum wird die while schleife abgebrochen)
    if(end->matrikelnummer==sMatrikelnummer){
            printf("\t\t%c",TCROSSRIGHT); for(int i=1;i<=27;i++) printf("%c", HORIZONLINE); printf("%c", TCROSSDOWN); for(int i=1;i<=16;i++) printf("%c", HORIZONLINE); printf("%c", TCROSSDOWN); for(int i=1;i<=12;i++) printf("%c", HORIZONLINE); printf("%c", TCROSSDOWN); for(int i=1;i<=16;i++) printf("%c", HORIZONLINE); printf("%c", TCROSSDOWN); for(int i=1;i<=16;i++) printf("%c", HORIZONLINE); printf("%c\n", TCROSSLEFT); 
            printf("\t\t%c Name                      %c Matrikelnummer %c Geburtstag %c Eintrittsdatum %c Austrittsdatum %c\n", VERTICALLINE, VERTICALLINE, VERTICALLINE, VERTICALLINE, VERTICALLINE, VERTICALLINE);
            printf("\t\t%c",CORNERDOWNLEFT); for(int i=1;i<=27;i++) printf("%c", HORIZONLINE); printf("%c", TCROSSUP); for(int i=1;i<=16;i++) printf("%c", HORIZONLINE); printf("%c", TCROSSUP); for(int i=1;i<=12;i++) printf("%c", HORIZONLINE); printf("%c", TCROSSUP); for(int i=1;i<=16;i++) printf("%c", HORIZONLINE); printf("%c", TCROSSUP); for(int i=1;i<=16;i++) printf("%c", HORIZONLINE); printf("%c\n", CORNERDOWNRIGHT); 
            return;
    }

    struct student *now;
    now=start;
    while(now->next!=NULL){ //Der letzte Eintrag wird nicht getestet, da dort der Zeiger auf NULL zeigt!
        if(now->matrikelnummer==sMatrikelnummer){
            printf("\t\t%c",TCROSSRIGHT); for(int i=1;i<=27;i++) printf("%c", HORIZONLINE); printf("%c", TCROSSDOWN); for(int i=1;i<=16;i++) printf("%c", HORIZONLINE); printf("%c", TCROSSDOWN); for(int i=1;i<=12;i++) printf("%c", HORIZONLINE); printf("%c", TCROSSDOWN); for(int i=1;i<=16;i++) printf("%c", HORIZONLINE); printf("%c", TCROSSDOWN); for(int i=1;i<=16;i++) printf("%c", HORIZONLINE); printf("%c\n", TCROSSLEFT); 
            printf("\t\t%c Name                      %c Matrikelnummer %c Geburtstag %c Eintrittsdatum %c Austrittsdatum %c\n", VERTICALLINE, VERTICALLINE, VERTICALLINE, VERTICALLINE, VERTICALLINE, VERTICALLINE);
            printf("\t\t%c",CORNERDOWNLEFT); for(int i=1;i<=27;i++) printf("%c", HORIZONLINE); printf("%c", TCROSSUP); for(int i=1;i<=16;i++) printf("%c", HORIZONLINE); printf("%c", TCROSSUP); for(int i=1;i<=12;i++) printf("%c", HORIZONLINE); printf("%c", TCROSSUP); for(int i=1;i<=16;i++) printf("%c", HORIZONLINE); printf("%c", TCROSSUP); for(int i=1;i<=16;i++) printf("%c", HORIZONLINE); printf("%c\n", CORNERDOWNRIGHT); 
            return;
        }
        now=now->next;
    }
    printf("\t\t%c ",VERTICALLINE);
    printf(ERR "Es konnte kein Student mit der Nummer %d gefunden werden!\n" RESET, sMatrikelnummer);
    printf("\t\t%c", CORNERDOWNLEFT); for(int i=1;i<=91;i++) printf("%c", HORIZONLINE); printf("%c\n", CORNERDOWNRIGHT);
}
//Menü
int menu(void){
    system("color");
    int pos=0;
    char input='o';
    do{
        //system("cls");
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
    init();
    //read();
    int select;
    do{
        select=menu();
        switch (select)
        {
        case 0:
            inputStudent();
        break;
        case 1:
            //Schöne Ausgabe
            {
                system("cls");
                printf("\t\t%c", CORNERUPLEFT); for(int i=1;i<=MENUMAX;i++) printf("%c", HORIZONLINE); printf("%c\n", CORNERUPRIGHT);
                int len=3;
                int count=countStudent();
                len=4-getLength(count);
                printf("\t\t%c Es befinden sich " IMPORTANTTEXT "%d" RESET, VERTICALLINE, count);  printf(" Eintr%cge in der Datenbank!     ", ae); for(int i=0;i<len;i++) printf(" "); printf("%c\n", VERTICALLINE);
                printf("\t\t%c", CORNERDOWNLEFT); for(int i=1;i<=MENUMAX;i++) printf("%c", HORIZONLINE); printf("%c\n", CORNERDOWNRIGHT);
            }
            //countStudent(); Ohne pointer, da in der Aufgabe steht, dass diese Funktion einen Wert zurück gibt
            wait();
        break;
        case 2:
            {
                int search;
                printf("\t\t%c", CORNERUPLEFT); for(int i=1;i<=91;i++) printf("%c", HORIZONLINE); printf("%c\n", CORNERUPRIGHT);
                printf("\t\t%c Zu suchende Matrikelnummer: ", VERTICALLINE);
                setMatrikel(&search);
                printStudent(search);
            }
            wait();
            //printStudent(Matrikelnummer)
        break;
        case 3:
            //printAllStudents()
        break;
        case 4:
            //deleteStudent(matrikelnummer)
        break;
        
        case 5:
            //Ende des Programms (Hier muss nichts hinzugefügt werden!)
        break;
        
        default:
            printf(ERR "!Fehler select hat den ung%cltigen Wert '%d'!\n" RESET, ue, select);
        break;
        }
    }while(select!=5);
    //save();
    return 0;
}

//der start und end pointer sind jetzt global