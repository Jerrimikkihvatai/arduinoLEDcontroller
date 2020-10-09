#include "FastLED.h"
#include <EEPROM.h>
#include <Bounce2.h>

#define FAN_LED 12 //Кол-во LED в кулере
#define FP_LED  18 //Кол-во LED в ленте на передней панели

#define FAN_PIN 6 //Кулер
#define FP_PIN  5 //Передняя панель

#define BRIGHTNESS         100 //Задаем яркость по умолчанию
#define FRAMES_PER_SECOND  60  //Частота обновления 

#define dly 100; //DELAY

DEFINE_GRADIENT_PALETTE( Mint_Orange ) {
255,   63, 255, 92,   //MINT
255,   255, 79, 3};   //ORANGE
CRGBPalette16 P1 = Mint_Orange;

DEFINE_GRADIENT_PALETTE( Pink_Mint ) {
255,   250, 5, 95,    //PINK
255,   63, 255, 92 }; //MINT
CRGBPalette16 P2 = Pink_Mint;

DEFINE_GRADIENT_PALETTE( Pink_Orange ) {
255,   250, 5, 95,   //PINK
255,   255, 79, 3};  //ORANGE
CRGBPalette16 P3 = Pink_Orange;

int butPin = 2;
Bounce debouncer = Bounce(); // Устранитель дребезга контактов

CRGB fp[FP_LED];             //Переменные для работы со светодиодами
CRGB fan[FAN_LED];

uint8_t gHue = 0;            // Крутилка для радуги

uint8_t currPattern = EEPROM.read(0);    //Переменная в которой лежит текущий режим работы

unsigned long timer = millis();          //Таймер для задержки сохранения режима работы
bool flag = false;

//void fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(250); } }

/*
void Cylon() { 
  static uint8_t hue = 0;
  // First slide the led in one direction
  for(int i = 0; i < NUM_LEDS; i++) {
    // Set the i'th led to red 
    leds[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show(); 
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    fadeall();
    // Wait a little bit before we loop around and do it again
    delay(dly);
  }

  for(int i = (NUM_LEDS)-1; i >= 0; i--) {
    // Set the i'th led to red 
    leds[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    fadeall();
    // Wait a little bit before we loop around and do it again
    delay(dly);
  }
}

*/

//Постоянный розовый цвет______________
void Const_Pink(){ 
  fill_solid(fp, FP_LED,0xfa055f);
  fill_solid(fan, FAN_LED,0xfa055f);
}

//Постоянный оранжевый цвет_____________
void Const_Orange(){ 
  fill_solid(fp, FP_LED,0xff4f03);
  fill_solid(fan, FAN_LED,0xff4f03);
}

//Постоянный оранжевый цвет___________
void Const_Mint(){ 
  fill_solid(fp, FP_LED,0x3fff5c);
  fill_solid(fan, FAN_LED,0x3fff5c);
}

//Постоянный корраловый цвет___________
void Const_Coral(){ 
  fill_solid(fp, FP_LED,0xff3f1c);
  fill_solid(fan, FAN_LED,0xff3f1c);
}

//Постоянный вишневый цвет_____________
void Const_Cherry(){ 
  fill_solid(fp, FP_LED,0x6e0707);
  fill_solid(fan, FAN_LED,0x6e0707);
}

//Градиент_____________________________
void GradientMO(){ 

for (int i = 0; i < FP_LED; i++) {
    fp[i] = ColorFromPalette(P1, (i*255)/FP_LED);
}
for (int i = 0; i < FAN_LED; i++) {
    fan[i] = ColorFromPalette(P1, (i*255)/(FAN_LED));
}
}


void GradientPM(){ 

for (int i = 0; i < FP_LED; i++) {
    fp[i] = ColorFromPalette(P2, (i*255)/FP_LED);
}
for (int i = 0; i < FAN_LED; i++) {
    fan[i] = ColorFromPalette(P2, (i*255)/(FAN_LED));
}
}

void GradientPO(){ 

for (int i = 0; i < FP_LED; i++) {
    fp[i] = ColorFromPalette(P3, (i*255)/FP_LED);
}
for (int i = 0; i < FAN_LED; i++) {
    fan[i] = ColorFromPalette(P3, (i*255)/(FAN_LED));
}
}

//Радуга_______________________________
void Gayshit(){
  fill_rainbow( fp, FP_LED, gHue, 7);
  fill_rainbow( fan, FAN_LED, gHue, 7);
}

//Отключить подсветку__________________
void Dark(){
  fill_solid(fp, FP_LED,0x000000);
  fill_solid(fan, FAN_LED,0x000000);
}

//_________________________________________РЕЖИМЫ СВЕЧЕНИЯ_______________________________________________
typedef void (*SimplePatternList[])();
SimplePatternList Patterns = {Const_Pink, Const_Orange, Const_Mint, Const_Coral, Const_Cherry, Gayshit, GradientMO, GradientPM, GradientPO, Dark};
//_______________________________________________________________________________________________________

//Переключатель режимов
void NextPattern(){
  currPattern += 1;
  Serial.print(currPattern);
  if (currPattern >= (sizeof(Patterns)/sizeof((Patterns)[0]))){ currPattern = 0; } //Зацикливаем переход
  flag = true; // Установка флага на запись. Если через 10 секунд режим не сменится, состояние запишится в память.
  timer = millis();
}

int buttonWasUp = 1;
int buttonIsUp = 1;

void button(){ 
    debouncer.update();
    buttonIsUp = !(debouncer.read()); //1 нажатие - 1 сигнал
    if (buttonIsUp && !buttonWasUp){
      NextPattern();
    }
    buttonWasUp = buttonIsUp;
}

void SaveMode(){
 if ((millis() > timer) && flag && (millis() - timer >= 10000)) { // Задержка по записи на 10 секунд
      flag = false;
      
      Dark();
      FastLED.show();
      delay(300);
      Patterns[currPattern](); 
      FastLED.show(); 
      
      EEPROM.put(0, byte(currPattern));
      }
}
/*_______________________________ИНИЦИАЛИЗАЦИЯ____________________________________*/
void setup() {
  Serial.begin(9600);
  Serial.print("INIZIALIZATION");
  
  FastLED.addLeds<WS2812B, FP_PIN, GRB>(fp, FP_LED);       
  FastLED.addLeds<WS2812B, FAN_PIN, GRB>(fan, FAN_LED);
  FastLED.setBrightness(BRIGHTNESS);

  pinMode(butPin, INPUT); //Подключаем кнопку
  debouncer.attach(butPin); //Устраняем дребезг контакта
  debouncer.interval(5);
  currPattern = EEPROM.read(0); //Считываем сохраненный режим
  if (currPattern >= (sizeof(Patterns)/sizeof((Patterns)[0]))) { currPattern = 0; }
}

/*_______________________________ГЛАВНЫЙ ЦИКЛ_______________________________________*/
void loop() {
    Patterns[currPattern](); //Устанавливаем режим
    button();                //Ждем сигнала с кнопки
    SaveMode();              //Сохраняем режим в случае, если кнопка была нажата и прошло более 10 секунд с этого момента
    
    FastLED.show();  
    FastLED.delay(1000/FRAMES_PER_SECOND);
    gHue++;
}


/*
 * 0xf78d02
  fill_rainbow(fp, FP_LED, gHue, 7);
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 10 ) { gHue++; } // slowly cycle the "base color" through the rainbow
*/
