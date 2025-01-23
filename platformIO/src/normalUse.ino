#include <SR04.h>
#include <Servo.h>

//define L298n module IO Pin
//le direzioni dei motori sono date guardandolo da davanti
#define ENB 5 //controlla i motori a sinistra
#define ENA 6 //controlla i motori a destra
#define IN1 7 //cavo nero motori a destra 
#define IN2 8 //cavo rosso motori a destra
#define IN3 9 //cavo rosso motori a sinistra 
#define IN4 11 //cavo nero motori a sinistra

#define SERVO_PIN 3

#define TRIG_PIN A5
#define ECHO_PIN A4

Servo motoreSensore;
SR04 ultrasuoni = SR04(ECHO_PIN,TRIG_PIN);

void setup() {
  Serial.begin(9600);

  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  pinMode(ENA,OUTPUT);
  pinMode(ENB,OUTPUT);

  motoreSensore.attach(SERVO_PIN);

  motoreSensore.write(90);
  delay(1000);
}

uint8_t servoGrade=90;
long tempoValoreServo=0;
long tempoUltrasuoni=0;
int movimentoServo=5;
bool somma=true;
String comando="";

void loop() {
  //facciamo girare il sensore ultrasuoni
  if(millis()-tempoValoreServo>25 && movimentoServo!=0){
    if(servoGrade==150){
      somma=false;
    }else if(servoGrade==30){
      somma=true;
    }
    if(somma){
      servoGrade+=movimentoServo;
    }else{
      servoGrade-=movimentoServo;
    }
    motoreSensore.write(servoGrade);
    tempoValoreServo=millis();
  }
  //leggiamo e salviamo la variabile del sensore ultrasuoni
  if(millis()-tempoUltrasuoni>50){
    if(ultrasuoni.Distance()<20){
      Serial.println("pericolo collisione");
      if(servoGrade>135){
        movimento('I',5);
        delay(500);
        movimento('D',5);
        delay(250);
      }else if(servoGrade<45){
        movimento('I',5);
        delay(500);
        movimento('S',5);
        delay(250);
      }else{
        movimento('I',5);
        delay(500);
      }
      movimento('K',0);
      Serial.println("----------risolto");
    }
    tempoUltrasuoni=millis();
  }

  if(Serial.available()){
    delay(10);
    do{
      comando+=char(Serial.read());
    }while(Serial.available());
    int lunghezza=comando.length();
    char caratteri[lunghezza];
    comando.toCharArray(caratteri, lunghezza+1);
    comando="";
    for(int i=2; i<lunghezza; i++){
      comando+=caratteri[i];
    }
    switch(caratteri[0]){
      case 'M':
        //se settiamo un nuovo valore per il moviento
        movimento(caratteri[1], comando.toInt());
        break;
      case 'S':
        //se settiamo un valore che sia per alcuni settaggi
        settaggi(caratteri[1], comando.toInt());
        break;
    }
    comando="";
  }
}

void settaggi(char opzione, int settaggio){
  switch(opzione){
    case 'U': //ultrasuoni
      if(settaggio<=0){
        movimentoServo=0;
        Serial.println("movimento bloccato");
      }else if(settaggio<=15){
        movimentoServo=settaggio;
        Serial.println("valore modificato");
      }else{
        Serial.println("valore troppo alto o basso");
      }
      break;
    default:
      Serial.println("opzione errata");
  }
}

void movimento(char verso, int velocita){
  switch(verso){
    case 'A':
      Serial.println("avanti");
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
      break;
    case 'I':
      Serial.println("indietro");
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
      break;
    case 'D':
      Serial.println("destra");
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
     break;
    case 'S':
      Serial.println("sinistra");
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
      break;
  }
  if(velocita>0){
    velocita=map(velocita, 1, 10, 127, 255);
    analogWrite(ENA, velocita);
    analogWrite(ENB, velocita);
  }else{
    analogWrite(ENA, 0);
    analogWrite(ENB, 0);
  }
}

