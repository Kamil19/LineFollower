#define lewyP 12
#define lewyT 11
#define prawyP 4
#define prawyT 5
#define sensor1 A0
#define sensor2 A1
#define sensor3 A2
#define sensor4 A3
#define sensor5 A6
#define sensor6 A5
#define sensor7 A4

//ODCZYT CZUJNIKI

    int odczyt;
    int odczyt2;
    int odczyt3;
    int odczyt4;
    int odczyt5;
    int odczyt6;
    int odczyt7;
    float kat, odleglosc;
    int start = 1, start2 = 1;
    int jedz = 1;

//

float f = 100;
long CurrentTime;
float odlegloscL = 0, odlegloscR = 0, enk_totalS = 0, enk_totalT = 0;
 

// ZMIENNE PROFILERA TRANSLACJI

char stan = 'J';
float Vakt = 0;    //cm s
float Vdoc = 80;  //cm s
float przysp = 400;   //cm s^2
float Zad_S = 0;  // cm
float t = 1/f;         // s
float Vnext = 25;      //cm s

float target_S = 20;  // cm

// ZMIENNE PROFILERA ROTACJI

char  stanT = 'J';
float Takt = 0;       //stopnie s
float przyspT = 50;   //stopnie s^2
float target_T = 0;  //stopnie
float zad_T = 0;      //stopnie
float Tdoc = 50;      //stopnie s
float Tnext = 0;      //stopnie 3


// ZMIENNE REGULATORA

float reg_newPWML = 0, reg_newPWMP = 0;
float regV_aktE = 0, regV_prevE = 0;
float regT_aktE =0, regT_prevE = 0;
float regV_P = 0.5, regV_D = 0.1, regT_P = 2, regT_D = 0.4 ;         

float a = 0,b = 0, trans=0;
float x = 0, y = 0;

//FUNKCJE

void sprawdzSensory();
void prawyEnkoder();
void lewyEnkoder();
void profiler_trans();
void profiler_rot();
int trzeba_hamowac();
int dojechal_do_celu();
void ustawPWM(int,int);
void Reg();

void setup() {
  pinMode(prawyT, OUTPUT);
  pinMode(prawyP, OUTPUT);
  pinMode(lewyT, OUTPUT);
  pinMode(lewyP, OUTPUT);
  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);
  pinMode(sensor3, INPUT);
  pinMode(sensor4, INPUT);
  pinMode(sensor5, INPUT);
  pinMode(sensor6, INPUT);
  pinMode(sensor7, INPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  attachInterrupt(7, lewyEnkoder, RISING);
  attachInterrupt(9, prawyEnkoder, RISING);
  Serial.begin(9600);
}

void loop() {
    if(jedz == 1)
    {
      delay(10000);
      jedz++;
    }
     ZmianaPredkosci();
     odczyt  = analogRead(sensor1);
     odczyt2 = analogRead(sensor2);
     odczyt3 = analogRead(sensor3);
     odczyt4 = analogRead(sensor4);
     odczyt5 = analogRead(sensor5);
     odczyt6 = analogRead(sensor6);
     odczyt7 = analogRead(sensor7);
  CurrentTime = millis(); // AKTUALNY CZAS
  if(CurrentTime % 10 == 0.0) // 
    {
    sensorCheck();
    enk_totalS = (odlegloscL + odlegloscR)/2;
    enk_totalT = (odlegloscL - odlegloscR)/11;
    profiler_trans();
    profiler_rot();
    Reg();
    Serial.println(enk_totalT);
    }
  }


void sprawdzSensry()
{
    if(odczyt5 > 900 )
    {
      target_T =  12;
    }
    if(odczyt5 > 900 && odczyt4 > 900 )
    {
      target_T =  10;
    }
    if(odczyt4 > 900  )
    {
      target_T =  8;
    }
    if(odczyt3 > 900 && odczyt4 > 500 )
    {
      target_T =  4;
    }
    if(odczyt3 > 900)
    {
      target_T = 0 ;
    }
    if(odczyt3 > 900 && odczyt2 > 900)
    {
      target_T = -4;
    }
    if(odczyt2 > 900)
    {
      target_T =  -8;
    }
    if(odczyt2 > 900 && odczyt > 900)
    {
      target_T = -10;
    }
    if(odczyt > 900)
    {
      target_T =  -12;
    }

}

//=============================================================
// ODLEGLOSC Z EKODEROW

// LICZENIE ODLEGLOSCI LEWEGO SILNIKA
void lewyEnkoder() 
  {
    if (digitalRead(6) == HIGH) 
      {
      odlegloscL += 0.33; 
      } 
  }

// LICZENIE ODLEGLOSCI PRAWEGO SILNIKA
void prawyEnkoder() 
  {
    if (digitalRead(8) == HIGH) 
      { 
      odlegloscR += 0.33;  
      } 
  }

//=============================================================
//PROFILER TRANSLACJI
void profiler_trans()
  {  

      if(trzeba_hamowac())
        {
         Vdoc = Vnext;   
        }
    
    
    //ZMIANA PREDKOSCI
    if(Vakt < Vdoc)
    {
    Vakt = Vakt + przysp*t;
      if(Vakt > Vdoc)
      Vakt = Vdoc;  
    }
     if(Vakt > Vdoc)
    {
    Vakt = Vakt - przysp*t;
      if(Vakt < Vdoc)
      Vakt = Vdoc;  
    }
    //Zmiana drogi
  Zad_S = Zad_S + Vakt*t;  
  } 

//=============================================================
//PROFILER ROTACJI
void profiler_rot() 
{  
     if(Takt < Tdoc)
    {
    Takt = Takt + przyspT*t;
      if(Takt > Tdoc)
      Takt = Tdoc;  
    }
     if(Takt > Tdoc)
    {
    Takt = Takt - przyspT*t;
      if(Takt < Tdoc)
      Takt = Tdoc;  
    }

    if(target_T > 0) 
    {
        zad_T += Takt*t;
        if(zad_T > target_T)
        zad_T = target_T; 
    }
    else 
    {
        zad_T -= Takt*t;
        if(zad_T < target_T)
        zad_T = target_T;
    }     
}

int trzeba_hamowac()
  {
    if(((Vakt*Vakt) - (Vnext*Vnext))/(2*przysp) >= (target_S - enk_totalS))
    return 1;
    else 
    return 0;
  }
int dojechal_do_celu()
  {
    if(Zad_S >= target_S)
    return 1;
    else
    return 0;
  }
void ustawPWM(int reg_newPWML, int reg_newPWMP) 
  { 
  analogWrite(leftF,0);
  analogWrite(leftB,(int)reg_newPWML);
  analogWrite(rightF, 0);
  analogWrite(rightB,(int)reg_newPWMP);
  }
void Reg()
{  
    regV_aktE = Zad_S - enk_totalS;                // obliczenie aktualnego błędu translacji 
    regT_aktE = zad_T - enk_totalT;
   
    reg_newPWML = (regV_P * regV_aktE + regV_D * (regV_aktE - regV_prevE)/t
                + regT_P * regT_aktE + regT_D * (regT_aktE - regT_prevE)/t);                
  
    reg_newPWMP = (regV_P * regV_aktE + regV_D * (regV_aktE - regV_prevE)/t
                - (regT_P * regT_aktE + regT_D * (regT_aktE - regT_prevE)/t));
    
    if(reg_newPWML < 0)
    reg_newPWML =0;
    else if(reg_newPWML > 100)
    reg_newPWML =100;
    if(reg_newPWMP < 0)
    reg_newPWMP =0;
    else if(reg_newPWMP > 100)
    reg_newPWMP =100;
    regV_prevE = regV_aktE; 
    regT_prevE = regT_aktE; 

    setPWM((reg_newPWML*2.55), (reg_newPWMP*2.55));
}
void Startuj()
{
  start2 = start;
if(odczyt7 < 100)
  start = 1;
  else 
  start = 0;
  

  if(start == 1 && start2 == 0)
  {
  jedz = 0;
  }
}
void ZmianaPredkosci()
{
 
  if(enk_totalS > 110 && enk_totalS < 140)
  {
  Vdoc = 100;
  Vnext = 25;
  target_S = 140;  
  }
  if(enk_totalS > 220 && enk_totalS < 250)
  {
  Vdoc = 90;
  Vnext = 30;
  target_S = 260;  
  } 
  
  if(enk_totalS > 20 && enk_totalS < 110)
  target_S = 110;
  if(enk_totalS > 140 && enk_totalS < 220)
  target_S = 220;
  if(enk_totalS > 250 && enk_totalS < 260 )
  target_S = 260;
 if(enk_totalS > 260 )
  {
  target_S = 550;
  Vdoc = 30;
  Vnext =0;
  przysp = 5000;
  }

}






   
