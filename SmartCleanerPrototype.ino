#include <LiquidCrystal.h>

#include <Servo.h>

#define SERVO 13
#define pin_trigger 9
#define pin_echo 8
#define pin_reset 7
#define green A5
#define yellow 6
#define red 10
#define ledRgbR A2
#define ledRgbG A4
#define ledRgbB A3
#define tempSensor A0
// C++ code
//

//TEMPERATURE
int baselineTemp = 0;
int celsius = 0;
int stateMachine = 0;

//LCD
LiquidCrystal lcd(12,11,5,4,3,2);

//SERVO PARAMETERS
Servo s;
int pos;

//CIGANA
long duration;
int distanceCm, distanceInch;

//LEVEL MANAGER
int bottleStartLevel = 200;
int bottleManager = bottleStartLevel;
int redBottleThreshold, yellowBottleThreshold;

//BUTTON MANAGER
int buttonState = 0;
int lastButtonState = 0;


void setup()
{
  redBottleThreshold = int(bottleStartLevel * 0.1);
  yellowBottleThreshold = int(bottleStartLevel * 0.4);
  
  pinMode(LED_BUILTIN, OUTPUT);
  
  //TMPSensor
  pinMode(A0, INPUT);
  
  //LED RGB
  pinMode(ledRgbR, OUTPUT);
  pinMode(ledRgbG, OUTPUT);
  pinMode(ledRgbB, OUTPUT);
  
  //LCD
  lcd.begin(16,2);
  
  //RESET_BUTTON
  pinMode(pin_reset, INPUT);
  
  //LEDS
  pinMode(green, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(red, OUTPUT);
  
  //CIGANA
  pinMode(pin_trigger, OUTPUT);
  pinMode(pin_echo, INPUT);
  
  //SERVO
  s.attach(SERVO);
  Serial.begin(9600);
  s.write(0);
  
  delay(100);
}


void loop(){
  
  checkTemperature();
  resetBottle();
  lightControl();
  dispense();
  
}


void checkTemperature(){

  baselineTemp = 40;
  celsius = map(((analogRead(A0) - 20) * 3.04), 0, 1023, -40, 125);
  Serial.println(String(celsius) +"ºC");
  
  temperatureLed();
  
    
  //Limpa a tela
  lcd.clear();
  //Posiciona o cursor na coluna 3, linha 0;
  lcd.setCursor(3, 0);
  //Envia o texto entre aspas para o LCD
  lcd.print("TEMP: " + String(celsius));
  lcd.setCursor(3, 1);
  
  switch(stateMachine){
    case 2:
		lcd.print("COM FEBRE");
    	break;
    case 1:
    	lcd.print("TEMP. ALTA");
    	break;
    case 0:
		lcd.print("TEMP. OK");
    	break;
  }
  
  delay(5000);
}


void temperatureLed(){
 Serial.println(String(celsius) + " PEGOU O LED");
  if(celsius >= 39){
    digitalWrite(ledRgbR, HIGH);
    digitalWrite(ledRgbG, LOW);
    digitalWrite(ledRgbB, LOW);
    stateMachine = 2;
  }
  else if(celsius >= 37){
    digitalWrite(ledRgbR, HIGH);
    digitalWrite(ledRgbG, HIGH);
    digitalWrite(ledRgbB, LOW);
    stateMachine = 1;
  }
  else{
   	digitalWrite(ledRgbR, LOW);
    digitalWrite(ledRgbG, HIGH);
    digitalWrite(ledRgbB, LOW); 
    stateMachine = 0;
  }
}

//LIGHT BOTTLE INDICATOR MANAGER
void lightControl(){
  if(bottleManager <= redBottleThreshold){
    	digitalWrite(green, LOW);
    	digitalWrite(yellow, LOW);
    	digitalWrite(red, HIGH);
  }
  else if(bottleManager <= yellowBottleThreshold){
    	digitalWrite(green, LOW);
    	digitalWrite(yellow, HIGH);
    	digitalWrite(red, LOW);
  }
  else{
    	Serial.println(String(bottleManager));
    	digitalWrite(green, HIGH);
    	digitalWrite(yellow, LOW);
    	digitalWrite(red, LOW);
  }
}

//LENDO A MAO
long readDistance(){
	digitalWrite(pin_trigger, LOW);
	delayMicroseconds(2);
	digitalWrite(pin_trigger, HIGH);
	delayMicroseconds(10);
	digitalWrite(pin_trigger, LOW);
	duration = pulseIn(pin_echo, HIGH);
	distanceCm= duration*0.034/2;
  	return distanceCm;
}

//SERVO COMMAND FOR DISPENSING
void dispense(){
  
  	if(readDistance() <= 50){
      for(pos = 0; pos < 90; pos++){
    		s.write(pos);
  			delay(5);
  		}
		delay(1000);
  		for(pos = 90; pos >= 0; pos--){
        	s.write(pos);
    		delay(5);
 		}
  
 	 	manageBottle();
      	delay(1000);
  	} 
}


//RESETS COUNTER FOR LEDS
void resetBottle(){
  	
  buttonState = digitalRead(pin_reset);  
  if(buttonState != lastButtonState){
    if(buttonState == HIGH){
     bottleManager = bottleStartLevel;
    }
   	delay(1000);
  }
  	//TO DO: Post para DB com insert de dados modelo "ID, DATETIME"
  	//para registro da troca de bottle
}


//MANAGE COUNTER FOR LEDS
void manageBottle(){
	if(bottleManager > 0){ 
		bottleManager--;
  	}
}
