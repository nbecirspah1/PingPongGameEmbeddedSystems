
#include "mbed.h"
#include "Arial12x12.h"
#include "Arial24x23.h"
#include "Arial28x28.h"
#include "SPI_TFT_ILI9341.h"
#include "ctime"
#include "font_big.h"
#include "stdio.h"
#include "string"
#include "cmath"


#define PI 3.14159265358979323846

SPI_TFT_ILI9341 uLCD(PTD2, PTD3, PTD1, PTD0, PTD5, PTA13, "uLCD");
AnalogIn a1(PTB0);
AnalogIn a2(PTB1);

int loptaX = 160, loptaY = 135;
int reket1x1 = 1, reket1x2 = 5, reket1y1, reket1y2;
int reket2x1 = 315, reket2x2 = 319, reket2y1, reket2y2;
int brzinaLopte,duzinaReketa1,duzinaReketa2,koeficijent1,koeficijent2;
int poeniIgrac1 = 0, poeniIgrac2 = 0, ukupniPlasman1=0, ukupniPlasman2=0;
float ugao;
int introx1=20,introy1=0,introx2=300,introy2=240;
int starix=310;
time_t t;

void winner(){
    uLCD.background(Black);
    uLCD.cls();    
    uLCD.set_font((unsigned char *)Arial28x28);
    uLCD.foreground(White);
    uLCD.set_orientation(1);
    uLCD.cls();   
    uLCD.locate(45,80);
    uLCD._printf("THE WINNER IS", 14);
    uLCD.locate(45,110);
    if(ukupniPlasman1>=2) uLCD._printf("Player 1",9);
    else uLCD._printf("Player 2",9);
}

void gameover(){
    uLCD.background(Black);
    uLCD.cls();    
    uLCD.set_font((unsigned char *)Arial28x28);
    uLCD.foreground(White);
    uLCD.set_orientation(1);
    uLCD.cls();   
    uLCD.locate(45,90);
    uLCD._printf("Game over!", 10);
}

int dajUgao(int donjaGranica, int gornjaGranica) {
  int ugao;
  return donjaGranica + (rand() % (gornjaGranica - donjaGranica + 1));
}

void pozicionirajReket(){
    //postavljanje koordinata reketa u odnosu na vrijednosti potenciometara
    double a1Suprotno = 1 - a1;
    reket1y1 = (a1Suprotno * koeficijent1) + 30;
    reket1y2 = reket1y1 + duzinaReketa1;
    reket2y1 = (a2 * koeficijent2 + 30);
    reket2y2 = reket2y1 + duzinaReketa2;
}

void postaviVrijednostiIgre(float brzina, int duzina1, int duzina2, int koef1, int koef2){
    brzinaLopte = brzina;
    duzinaReketa1 = duzina1;
    duzinaReketa2 = duzina2;
    koeficijent1 = koef1;
    koeficijent2 = koef2;
}

void vratiNaSredinu(){
    loptaX = 160;
    loptaY = 135;
    ugao = rand() % 360;
}

void igra(int mod){
    poeniIgrac1 = 0;
    poeniIgrac2 = 0;   
    
    //mod 0 je solo igranje, pa se ovdje simulira zid umjesto drugog reketa
    if(mod==0) postaviVrijednostiIgre(27, 35, 210, 175, 0);

    //u zavisnosti od moda, mijenja se brzina loptice i duzina reketa
    else if(mod==1) postaviVrijednostiIgre(19, 40, 40, 170, 170);
    else if(mod==2) postaviVrijednostiIgre(23, 35, 35, 175, 175);
    else if(mod==3) postaviVrijednostiIgre(29, 30, 30, 170, 170);

    do{
    pozicionirajReket();

    //brisanje prethodne pozicije lopte, odnosno bojenje u zeleno
    uLCD.fillcircle(loptaX, loptaY, 5, Green);

    if (loptaX == 310 && loptaY >= reket2y1 && loptaY <= reket2y2) ugao = dajUgao(91, 269);
    else if (loptaX == 10 && loptaY >= reket1y1 && loptaY <= reket1y2) ugao = dajUgao(271, 359);
    else if (loptaX >= 310 && (loptaY < reket2y1 || loptaY > reket2y2)) {
        poeniIgrac1++;
        vratiNaSredinu();
    } 
    else if (loptaX <= 10 && (loptaY < reket1y1 || loptaY > reket1y2)) {
        poeniIgrac2++;
        vratiNaSredinu();
    }
    else if (loptaY >= 235) {
        if (ugao >= 180 && ugao <= 270) ugao = dajUgao(90, 180);
        if (ugao > 270 && ugao <= 360) ugao = dajUgao(0, 90);
    } 
    else if (loptaY <= 35) {
        if (ugao >= 0 && ugao <= 90) ugao = dajUgao(270, 360);
        if (ugao > 90 && ugao <= 180) ugao = dajUgao(180, 270);
    }

    //nova pozicija lopte koju racunamo u odnosu na trenutni ugao i brzinu koju se krece
    loptaX = loptaX + brzinaLopte * cos(ugao * 2 * PI / 360);
    loptaY = loptaY - brzinaLopte * sin(ugao * 2 * PI / 360);

    //ogranicavanje nove pozicije loptice da bude unutar terena
    if (loptaX >= 310 && (loptaY >= reket2y1 && loptaY <= reket2y2)) loptaX = 310;
    if (loptaX <= 10 && (loptaY > reket1y1 && loptaY < reket1y2)) loptaX = 10;
    if (loptaY <= 35) loptaY = 35;
    if (loptaY >= 235) loptaY = 235;

    //prikaz reketa i loptice sa novim pozicijama
    uLCD.fillcircle(loptaX, loptaY, 5, Black);
    uLCD.fillrect(reket1x1, reket1y1, reket1x2, reket1y2, Red);
    uLCD.fillrect(reket2x1, reket2y1, reket2x2, reket2y2, Red);

    //brisanje prethodne pozicije oba reketa, odnosno vracanje zelene boje na cijeloj traci kretanja reketa, osim na mjestu gdje je trenutno reket
    uLCD.fillrect(0, 30, 5, reket1y1 - 1, Green);
    uLCD.fillrect(0, reket1y2 + 1, 5, 240, Green);
    uLCD.fillrect(315, 30, 320, reket2y1 - 1, Green);
    uLCD.fillrect(315, reket2y2 + 1, 320, 240, Green);

    uLCD.line(0, 30, 320, 30, White);

    //pretvaranje poena u cstringove kako bi se mogli ispisati na displej
    uLCD.locate(5, 5);
    int i;
    char p1[3], p2[3];

    if (poeniIgrac1 <= 9) {
      p1[0] = 0;
      p1[1] = poeniIgrac1 + '0';
      p1[2] = '\0';
    }
    else {
      p1[0] = poeniIgrac1 / 10 + '0';
      p1[1] = poeniIgrac1 % 10 + '0';
      p1[2] = '\0';
    }
    if (poeniIgrac2 <= 9) {
      p2[0] = 0;
      p2[1] = poeniIgrac2 + '0';
      p2[2] = '\0';
    }
    else {
      p2[0] = poeniIgrac2 / 10 + '0';
      p2[1] = poeniIgrac2 % 10 + '0';
      p2[2] = '\0';
    }

    if(mod!=0) {
        uLCD._printf("P1:", 4);
        uLCD._printf(p1, 3);
        uLCD.locate(210, 5);
        uLCD._printf("P2:", 4);
        uLCD._printf(p2, 3);
    }
    else{
        //preostali zivoti u obliku loptica u soloplayer modu
        int bodovi = 11 - poeniIgrac2;
        int poluprecnik = 0;
        uLCD.fillcircle(starix, 15, 10, Green);
        while(bodovi!=0) {
            uLCD.fillcircle(15+poluprecnik,15,10, Magenta);
            poluprecnik = poluprecnik + 25;
            bodovi--;
        }
        starix = poluprecnik - 10;
    }
    wait_us(2000);
  } while(poeniIgrac1<=10 && poeniIgrac2<=10);
}

void letiLoptica(){
    //pocetni izgled displeja sa dvije loptice koje se vertikalno pomjeraju u suprotnim smjerovima
    uLCD.fillcircle(introx1, introy1, 5, Black);
    introy1 = introy1 + 5;
    if(introy1>=240) introy1 = 0;
    uLCD.fillcircle(introx1, introy1, 5, Blue);
    uLCD.fillcircle(introx2, introy2, 5, Black);
    introy2 = introy2 - 5;
    if(introy2<=0) introy2 = 240;
    uLCD.fillcircle(introx2, introy2, 5, Blue);
}

void welcome(){
    uLCD.cls();    
    uLCD.set_font((unsigned char *)Arial24x23);
    uLCD.foreground(White);
    uLCD.background(Blue);
    uLCD.set_orientation(1);
    Ticker startLoptica;
    startLoptica.attach(&letiLoptica,30ms);
    uLCD.locate(45,50);
    uLCD._printf("WELCOME!", 9);
    uLCD.set_font((unsigned char *)Arial12x12);
    uLCD.locate(45,100);
    uLCD._printf("One player",11);
    uLCD.locate(45,140);
    uLCD._printf("Two players",12);
    wait_us(3000000);
    startLoptica.detach();
}

void azurirajPlasman(){
    if(poeniIgrac1==11) ukupniPlasman1++;
    if(poeniIgrac2==11) ukupniPlasman2++;
}

void multiplayerIgra(){
    igra(1);
    azurirajPlasman();
    igra(2);
    azurirajPlasman();
    igra(3);
    azurirajPlasman();
    winner();
}

void soloplayerIgra(){
    igra(0);
    gameover();
}

int main() {    
  srand(static_cast<unsigned int>(time(0)));
  welcome();
  uLCD.set_font((unsigned char *)Arial28x28);
  uLCD.foreground(Magenta);
  uLCD.background(Green);
  uLCD.set_orientation(1);
  uLCD.cls();
  ugao = rand() % 360;
  
  //kad se drugi potenc zavrne do kraja u nekom smjeru, krece multiplayer
  if(a2<0.9  && a2>0.1) multiplayerIgra(); 
  else soloplayerIgra();

}






