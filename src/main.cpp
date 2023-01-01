/*****************************************************************************************
 *** sterownik etykieciarki by matrix0606 dla Greg ***************************************
 ***             v1.0 @ 12.10.2020                 ***************************************
 ****************************************************************************************/
#include <Arduino.h>

int wejsciePrzycisku = 2; // D2 jako wejsćie padału rozpoczynania programu
int wejscieCzujki = 10;   // D10 jako wejście dla czujnika IR
int pk1 = 3;              // wyjście na przekaźnik 1 (pk1, active = LOW)
int pk2 = 11;             // wyjście na przekaźnik nr 2 (pk2, active = LOW)
int debug = 13;
int cosik = 1; // test

unsigned long staryCzas; // zmienna do porównania z aktualnym czasem
unsigned long nowyCzas;  // zmienna do wpisywania aktualnego czasu
byte polSekundy = 0;
bool przycisk;
bool czujka;
bool startFunkcji = 0;

void setup()
{
  pinMode(wejsciePrzycisku, INPUT_PULLUP); // wejście przycisku ustawiona i podciągnięte do +5V
  pinMode(wejscieCzujki, INPUT_PULLUP);    // wejście czujki ustawione i podciągnięte do +5V
  pinMode(pk1, OUTPUT);                    // ustawienie pinu jako wyjście pk1
  pinMode(pk2, OUTPUT);                    // ustawienie pinu jako wyjście pk2
  digitalWrite(pk1, 1);                    // wyłączenie przekaźnika 1
  digitalWrite(pk2, 1);                    // wyłączenie przekaźnika 2
  Serial.begin(9600);                      // start seriala
  pinMode(debug, OUTPUT);
}

void odczytWejsc();
void odliczanieCzasu();
void funkcjaGlowna();

void loop()
{ // początek pętli głównej
  odczytWejsc();
  { // skok do funkcji odczytu stanu wejść
    if (przycisk == 0)
    {                // jeśli odczytano wciśnięty przycisk (pedał = 0) to
      delay(50);     // czekamy 50ms
      odczytWejsc(); // ponownie odczytuję czy wciąż jest wciśnięty przycisk
      if (przycisk == 0)
      {                  // i jeśli jest wciśnięty to
        funkcjaGlowna(); // skaczę do głównej funkcji programu
      }                  // i to wszystko co robi pętla główna
    }
  }
}

void funkcjaGlowna()
{                   // funkcja główna programu
  startFunkcji = 1; // wpisuję wartość 1 do zmiennej żeby pozwolić na zliczanie 0,5s odcinków czasu
  do
  { // wchodzę do pętli z której wyjdę tylko pod pewnym warunkiem
    odliczanieCzasu();
    // Serial.println(polSekundy);
    if (polSekundy == 2)
    {
      digitalWrite(pk1, 0);
    } // jeżeli czas 0,5s ma wartość 2 czyli zliczono 1s to włączam pk1
    if (polSekundy == 3)
    {
      digitalWrite(pk1, 1);
    }                        // jeżeli czas 0,5s ma wartość 3 czyli minęło 0,5s to wyłączam pk1
  } while (polSekundy <= 3); // tu wspomniana pętla z której wyjdę dopiero jak miną 4 x 0,5s czyli 2s
  startFunkcji = 0;          // do zmiennej startFunkcji wpisuję wartość 0 żeby czas nie był zliczany
  polSekundy = 0;            // na wszelki wypadek zeruję zmienną czasu, zmienna ta i tak jest wyzerowana w warunku niżej

  bool czekanieNaIR = 1; // tworzę zmienną lokalną aby móc czekać na stan niski czujki
  do
  { // co z samą czujką nie ma nic wspólnego
    odliczanieCzasu();
    odczytWejsc(); // odczytuję stan wejść
    if (czujka == 0)
    {                   // jeśli czujka jest aktywna to
      startFunkcji = 1; // ustawiam zmienną startFunkcji by czas znowu mógł być zliczany
    }
    // Serial.println(polSekundy);
    if (startFunkcji == 1)
    { // i jeśli startFunkcji = 1 co zostało przed chwilą wpisane to
      if (polSekundy == 2)
      {
        digitalWrite(pk2, 0);
      } // po upływie 1s włączam przekaźnik 2
      if (polSekundy == 8)
      {                       // jeśli czas polSekundy zliczono do 8 -> (8-2)/2 = 3s
        digitalWrite(pk2, 1); // to wyłączam pk2
        startFunkcji = 0;     // zeruję startFunkcji by zakończyć zliczanie
        polSekundy = 0;       // zeruję czas polSekundy
        czekanieNaIR = 0;     // zeruję zmienną czekanieNaIR by móc opuścić aktualną pętlę do...while
      }
    }
  } while (czekanieNaIR == 1); // jeśli czekanieNaIR = 1 co zostało wpisane przed wejściem do tej pętli
                               // to pętlo trwaj, a jeżeli czekanieNaIR = 0 to z pętli wychodzę
  startFunkcji = 1;            // ponownie ustawiam zmienną startFunkcji na 1 żeby móc włączyć znowu pk1
  do
  {
    odliczanieCzasu();
    if (polSekundy == 2)
    {
      digitalWrite(pk1, 0);
    } // po 1s właczam przekaźnik pk1
    if (polSekundy == 3)
    {
      digitalWrite(pk1, 1);
    }                        // jeśli czas zliczy 0,5s to wyłączam pk1
  } while (polSekundy <= 3); // i czekam aż czas zliczy jeszcze 0,5s po wyłączeniu pk1 i wychodzę z pętli do...while
  startFunkcji = 0;          // zeruję zmienną startFunkcji żeby wyłączyć zliczanie czasu
  polSekundy = 0;            // zeruję czas i kończe działanie całego programu wracając do pętli głównej
                             // by czekać na wciśnięcie przycisku
}

void odliczanieCzasu()
{
  nowyCzas = millis(); // wpisuję wartość aktualnego czasu systemu do zmiennej newTime
  if (nowyCzas - staryCzas >= 500)
  {                       // jeżeli różnica między aktualnym czasem a poprzednim jest większa lub równa 0,5sek to
    staryCzas = nowyCzas; // wpisuję nową wartość czasu do starego czasu
    if (startFunkcji == 1)
    {
      polSekundy++;
    }
    else
    {
      polSekundy = 0;
    }
  }
}

void odczytWejsc()
{
  przycisk = digitalRead(wejsciePrzycisku);
  czujka = digitalRead(wejscieCzujki);
}

void debuging()
{
  digitalWrite(debug, 1);
  delay(1000);
  digitalWrite(debug, 0);
  delay(1000);
}