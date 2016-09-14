#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library
#include <Encoder.h>

#define STROBE 12 // strobe pins on digital 4
#define RESET 13 // reset pins on digital 5

#define CLK 8  // MUST be on PORTB! (Use pin 11 on Mega)
#define LAT A5
#define OE  9
#define A   A2
#define B   A3
#define C   A4
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);

#define ENCA 11
#define ENCB 10
Encoder myEnc(ENCA, ENCB);
int valLeft2[13] , valRight2[13];
int encVal;
int stp = 0;
int i,j,z;
long oldPosition  = -999;
uint16_t colors[14], tablica_tmp[14];
uint16_t colors2[14];
uint16_t GREEN,BLACK, color;

int left[7]; // store band values in these arrays
int right[7];
int band;

void setup() {
  Serial.begin(19200);
  matrix.begin();

  pinMode(RESET, OUTPUT); // reset
  pinMode(STROBE, OUTPUT); // strobe
  digitalWrite(RESET,LOW); // reset low
  digitalWrite(STROBE,HIGH); //pin 5 is RESET on the shield
  
  color = matrix.ColorHSV(500,255, 16, false);
  GREEN = matrix.ColorHSV(500, 255, 2, false);
  BLACK = matrix.Color333(0, 0, 0);
  
  // fill the screen with 'black'
  matrix.fillScreen(BLACK);

  // fill array colors with colors
  for (int color = 0; color < 14; color++) {
    colors[color] = matrix.ColorHSV((110 * color), 255, 255, false);
    colors2[color] = matrix.ColorHSV((110 * color), 255, 50, false);
    Serial.println(colors[color]);
  }
}

void readMSGEQ7() { // Function to read 7 band equalizers
  digitalWrite(RESET, HIGH);
  digitalWrite(RESET, LOW);
  for (int band = 0; band < 7; band++ ) {
    digitalWrite(STROBE, LOW); // strobe pin on the shield - kicks the IC up to the next band 
    delayMicroseconds(30);
    left[band] = analogRead(0); // store left band reading
    right[band] = analogRead(1); // ... and the right
    digitalWrite(STROBE, HIGH); 
  }
}

unsigned long T, lastTime, time;
void loop() {
  countLoopTime();
  readMSGEQ7();
  movement(20); 
  encVal = readEnc();
}

void movement(uint8_t wait) {
  uint16_t i, j, val[7], vap[7], valLeft[13], valRight[13];
  
  for(band = 0; band < 7; band++) {
    val[band] = map(left[band], 0, 1023, 0, 17);
    vap[band] = map(right[band], 0, 1023, 0, 17);
  }
  

  //PRZEPISANIE DO DRUGIEJ TABLICY 16  
  for(band = 0; band < 8; band++){ 
    valLeft[band*2] = val[band];
    valRight[band*2] = vap[band];
  }
  
  for(band=0; band < 14; band++) { 
    if (band % 2 != 0) {
      valLeft[band] = (valLeft[band-1] + valLeft[band+1]) / 2;
      valRight[band] = (valRight[band-1] + valRight[band+1]) / 2;
    }
  }
  
  
  
  delay(5);

  matrix.fillScreen(BLACK);
  if (encVal > 14  || encVal < 0) {
    encVal = 0;
  }
  uint16_t COL = matrix.ColorHSV(encVal * 25, 255, 255, false);
  //
  stp++;
  
  
  if (stp == 2) {
    stp = 0;
    
    for (band = 0; band < 14; band++) {
      if (valRight[band] < valRight2[band]) {
        valRight2[band] -= 0.5;
      } else {
        valRight2[band] = valRight[band];
      }
      if (valLeft[band] < valLeft2[band]) {
        valLeft2[band] -= 0.5;
      } else {
        valLeft2[band] = valLeft[band];
      }

       
      matrix.drawPixel(band + 18, 16-valRight2[band], colors[band] );
      matrix.drawPixel(band + 0, 16-valLeft2[band] , colors[band] ); 
      
    }
    
  }

  //

  for (band = 0; band < 14; band++ ){

   //matrix.drawLine(band, 16, band, 16-valLeft[band], colors[band]);
   //matrix.fillCircle(8, 8, (valLeft[band]/3), colors[10] );
    //matrix.drawPixel(band, 17-valLeft[band], colors2[band] ); 
    //matrix.drawPixel(band, 16-valLeft[band], colors[band] ); 
   // matrix.drawPixel(band, 15-valLeft[band], colors2[band] ); 
     
        /*  DRUGI KANAL */
    // matrix.drawLine(band + 18, 16, band + 18, 16-valRight[band], colors[band]);
   // matrix.fillCircle(24, 8, (valRight[band]/2), matrix.ColorHSV(00,255,255,false));
    //matrix.drawPixel(band + 18, 17-valRight[band], colors2[band] );
    //matrix.drawPixel(band + 18, 16-valRight[band], colors[band] );
    //matrix.drawPixel(band + 18, 15-valRight[band], colors2[band] );  
  }
  
  //matrix.fillCircle(8, 8, (valLeft[3]/2), colors[4] );
  //matrix.fillCircle(24, 8, (valRight[3]/2), colors[4]);
}

int readEnc(){
  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    Serial.println(newPosition);
    return newPosition;
  }
}

int countLoopTime(){
  time = millis();
  T = time - lastTime ;
  lastTime = time;
  //Serial.println(T);
  return T;
}
