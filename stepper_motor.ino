int bttnCW = 1;
int bttnCCW = 2;
int pin1 = 3;
int pin2 = 4;
int pin3 = 5;
int pin4 = 6;

int pole1[] = {0,0,0,0, 0,1,1,1, 0};
int pole2[] = {0,0,0,1, 1,1,0,0, 0};
int pole3[] = {0,1,1,1, 0,0,0,0, 0};
int pole4[] = {1,1,0,0, 0,0,0,1, 0};

int stepMotor = 0;
int dirStatus = 3;

void setup() {
pinMode(pin1, OUTPUT);
pinMode(pin2, OUTPUT);
pinMode(pin3, OUTPUT);
pinMode(pin4, OUTPUT);

pinMode(bttnCW, INPUT_PULLUP);
pinMode(bttnCCW, INPUT_PULLUP);
}

void loop() {
if(digitalRead(bttnCCW) == LOW){
  dirStatus = 1;
}
else if (digitalRead(bttnCW) == LOW){
  dirStatus = 2;
}
else{
  dirStatus = 3;
}
if(dirStatus == 1){
  stepMotor++;
    driveStepper(stepMotor);
}
else if(dirStatus == 2){
  stepMotor--;
    driveStepper(stepMotor);
}
else{
  driveStepper(8);
}
if(stepMotor>7){
  stepMotor = 0;
}
if(stepMotor < 0){
  stepMotor = 7;
}
delay(1);
}


void driveStepper(int c){
  digitalWrite(pin1, pole1[c]);
  digitalWrite(pin2, pole2[c]);
  digitalWrite(pin3, pole3[c]);
  digitalWrite(pin4, pole4[c]);
}