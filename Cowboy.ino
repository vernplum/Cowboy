/*****************************************************************
Cowboy.ino

Based on 'Gunfight' (Midway - 1975)

Music by Ennio Morricone

Everything else: edquek@hotmail.com 2015
*****************************************************************/

#define DEBUG

#include <SPI.h>
#include <Wire.h>
#include <Flash.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <avr/pgmspace.h>
#include <EEPROM.h>

#define OLED_CLK   5
#define OLED_MOSI  6  // data
#define OLED_RESET 7
#define OLED_DC    8
#define OLED_CS    9
#define P1POTPINX 2    // x axis - must go to analog pin such as A0
#define P1POTPINY 3   // y
#define P2POTPINX 1    // x axis - must go to analog pin such as A0
#define P2POTPINY 0   // y
#define MAX_BULLETS 3
#define CENTERED 100
#define PLAYER1 0
#define PLAYER2 1
#define NOTE_C5  524
#define NOTE_D5  623
#define NOTE_E5  660
#define NOTE_F5  740
#define NOTE_G5  784
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4698
#define NOTE_A8  7040
#define NOTE_REST 0
#define NOTE_END 9998


Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
    
    
int GBUpitches[] PROGMEM = 
{ 
    NOTE_A7, NOTE_D8, NOTE_A7, NOTE_D8, NOTE_A7,  
    NOTE_F6, NOTE_G6, NOTE_D6, NOTE_REST, NOTE_END
};

int GBUlengths[] PROGMEM = 
{ 
    2, 2, 2, 2, 16, 
    8, 8, 16, 8
};

int Watchpitches[] PROGMEM = 
{ 
    NOTE_E7, NOTE_A6, NOTE_F7, NOTE_D7, NOTE_A6, NOTE_F7, NOTE_E7, NOTE_A6, NOTE_F7, NOTE_D7, NOTE_A6,   // music by Ennio Morricone
    NOTE_E7, NOTE_A6, NOTE_F7, NOTE_D7, NOTE_A6, NOTE_E7, NOTE_F7, NOTE_E7, NOTE_A6, NOTE_F7, NOTE_D7, NOTE_A6,
    NOTE_F7, NOTE_AS6, NOTE_G7, NOTE_E7, NOTE_AS6, NOTE_G7, NOTE_F7, NOTE_AS6, NOTE_G7, NOTE_E7, NOTE_AS6,        
    NOTE_E7, NOTE_A6, NOTE_F7, NOTE_D7, NOTE_A6, NOTE_F7, NOTE_E7, NOTE_A6, NOTE_F7, NOTE_D7, NOTE_A6,
    NOTE_A7, NOTE_C7, NOTE_AS7, NOTE_G7, NOTE_C7, NOTE_AS7, NOTE_A7, NOTE_C7, NOTE_AS7, NOTE_G7, NOTE_C7, 
    NOTE_A7, NOTE_CS7, NOTE_AS7, NOTE_A7, NOTE_G7, NOTE_CS7, NOTE_A7, NOTE_AS7, NOTE_A7, NOTE_CS7, NOTE_AS7, NOTE_G7, NOTE_CS7, 
    NOTE_END
};
  
static unsigned char Watchlengths[] PROGMEM = 
{ 
    8, 4, 4, 8, 4, 4, 8, 4, 4, 8, 8,
    8, 4, 4, 8, 4, 2, 2, 8, 4, 4, 8, 8,
    8, 4, 4, 8, 4, 4, 8, 4, 4, 8, 8,
    8, 4, 4, 8, 4, 4, 8, 4, 4, 8, 8,                 
    8, 4, 4, 8, 4, 4, 8, 4, 4, 8, 8,
    8, 4, 2, 2, 8, 4, 2, 2, 8, 4, 4, 8, 8   
};
          
static unsigned char cowboy[] PROGMEM = 
{
    0x0, 0x0, 0x0, 0x4, 0x2, 0xE8, 0x1F, 0x0, 0xA0, 0xF, 0x0, 0xE0,    //head
    0x4, 0x3, 0xE7, 0x4F, 0xC4, 0xE4, 0xF, 0x1, 0x10, 0x22, 0x3, 0x30,  // straight walk 1
    0x4, 0x1, 0xE7, 0x1F, 0xC2, 0xE4, 0x2E, 0x0, 0xE0, 0x4, 0x0, 0x60,   // straight walk 2 
    0x4, 0x21, 0xE4, 0x1F, 0xC2, 0xE2, 0x2E, 0x0, 0xE0, 0x4, 0x0, 0x60,  // gun up
    0x4, 0x1, 0xE0, 0x1F, 0x82, 0xEC, 0x2E, 0x20, 0xE0, 0x4, 0x0, 0x60,  // gun down

    0x0, 0x0, 0x0, 0x2, 0x1, 0x74, 0xF, 0x80, 0x50, 0xF, 0x0, 0x70,    // mirrored for P2
    0x2, 0xE, 0x78, 0x3F, 0x82, 0x74, 0x7, 0x40, 0x70, 0x2, 0x0, 0x60,
    0x2, 0xE, 0x7C, 0x3F, 0x22, 0x72, 0xF, 0x0, 0x88, 0x4, 0x40, 0xCC,
    0x42, 0x2, 0x78, 0x3F, 0x84, 0x74, 0x7, 0x40, 0x70, 0x2, 0x0, 0x60,
    0x2, 0x0, 0x78, 0x1F, 0x83, 0x74, 0x47, 0x40, 0x70, 0x2, 0x0, 0x60    
};

static unsigned char cactus[] PROGMEM = 
{ 
    0x18, 0x18, 0xD8, 0xD8, 0xD8, 0xF8, 0x7B, 0x1B, 
    0x1F, 0x1E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18 
};

static unsigned char coach[] PROGMEM = 
{
    0x0F, 0xF0, 0x3F, 0xFC, 0x7F, 0xFE, 0xFC, 0x3F, 0xFC, 0x3F, 0xF8, 0x1F, 0x70, 0x0E, 0x18, 0x18,
    0x3F, 0xFC, 0x0F, 0xF0, 0x1F, 0xF8, 0x48, 0x12, 0x68, 0x16, 0xFF, 0xFF, 0x68, 0x16, 0x40, 0x02
};

const unsigned char cowboyShot[12][8] PROGMEM = 
{
    { 0x00, 0x00, 0x08, 0x08, 0x78, 0x7C, 0x36, 0xDC },
    { 0x00, 0x00, 0x00, 0x20, 0x20, 0x30, 0x00, 0x10 },
    { 0x0E, 0x07, 0x07, 0x09, 0x08, 0x04, 0x00, 0x00 },
    { 0x60, 0x80, 0xC0, 0xE0, 0xD4, 0x4C, 0x40, 0x60 },
    { 0x00, 0x00, 0x00, 0x04, 0x04, 0x0C, 0x00, 0x08 },
    { 0x00, 0x00, 0x10, 0x10, 0x0E, 0x3E, 0x6C, 0x3B },
    { 0x06, 0x01, 0x03, 0x07, 0x2B, 0x32, 0x02, 0x06 },
    { 0x70, 0xE0, 0xE0, 0x90, 0x10, 0x20, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x40, 0x58, 0x6D, 0xFF },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF9, 0xFF },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x9F, 0xFF },
    { 0x00, 0x00, 0x00, 0x00, 0x02, 0x1A, 0xB6, 0xFF }
};


enum GameStage
{
    Attract, 
    NewGame,
    NewLevel,
    GetReady, 
    InGame,    
    Hit,
    GameOver,
};


enum gunPoint 
{
  PointingUp,
  PointingStraight,
  PointingDown
};


byte x[2];
byte y[2];
byte ox[2];
byte oy[2];
int analogValX[2];
int analogValY[2];
bool animStep[2];
byte counterSinceFired[2];
bool fireWasPressed[2];
byte bulletsFired[2];
bool hasBeenShot[2];
gunPoint gunDirection[2];
byte bulletsRemaining[2];
byte score[2];
int time;
byte j = 0;        // note counter for music
byte m;             // general purpose loop counter
unsigned char counter;
int flashCounter;
byte level;
GameStage stage; 
bool hasCoach = false;
int coachY;
int numCactus;
byte cactusX[2];
byte cactusY[2];


class bullet
{
    public:
         int x, y, dx, dy;
         bool alive;
    
    public:  
        void fire(byte x1, byte y1)
        {                      
            x = x1;
            y = y1;
            alive = true;
        }
     
        void move()
        {
            x += dx;
            y += dy;
                      
            if (dx > 0 && x >= 127)
            {
                alive = false;
                bulletsFired[PLAYER1]--;
            }
            else if (dx < 0 && x <=0)
            {
                alive = false;
                bulletsFired[PLAYER2]--;
            }
                             
            if (
                (numCactus == 1 && x >= 60 && x <= 64 && y >= cactusY[0] && y <= (cactusY[0] + 16))
                ||
                (numCactus == 2 && x >= cactusX[0] && x <= cactusX[0] + 8 && y >= cactusY[0] && y <= (cactusY[0] + 16))
                ||
                (numCactus == 2 && x >= cactusX[1] && x <= cactusX[1] + 8 && y >= cactusY[1] && y <= (cactusY[1] + 16))   )
            {
                alive = false;
                playHitCactusSound();
                bulletsFired[(dx > 0) ? PLAYER1 : PLAYER2]--;
            }
            
            if (hasCoach && x >= 56 && x <= 72 && y >= coachY && y <= coachY + 16)
            {
                alive = false;
                playHitCactusSound();
                bulletsFired[(dx > 0) ? PLAYER1 : PLAYER2]--;
            }
                      
            if (y <= 0)
            {
                y = 0;
                dy = 3;
            }
            
            if (y >= 63)
            {
                y = 63;
                dy = -3;
            }
            
            display.drawPixel(x, y, WHITE);
        }
};
 
bullet bullets[2][MAX_BULLETS];
long timerCounter;
int fpsCounter, fpsCounterOld;
    
#ifdef DEBUG    
int freeRam () 
{
    extern int __heap_start, *__brkval; 
    int v; 
    return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
#endif


void setup()
{
    display.begin(SSD1306_SWITCHCAPVCC);        

    fpsCounter = 0;
    timerCounter = millis();
    fpsCounterOld = 0;

    pinMode(10, INPUT);  // fire 
    pinMode(11, INPUT);  // fire
    pinMode(12, OUTPUT); //buzzer
    
    stage = Attract;
    counter = 0;
    flashCounter = 0;
}


static unsigned char gameString[] PROGMEM = { 16, 10, 22, 14, 99 };
static unsigned char upString[] PROGMEM = { 30, 25, 99 };
static unsigned char getString[] PROGMEM = { 16, 14, 29, 99 };
static unsigned char ReadyString[] PROGMEM = { 27, 14, 10, 13, 34, 36, 99 };
static unsigned char WildWest[] PROGMEM = { 32, 18, 21, 13, 0, 32, 14, 28, 29, 99 };
static unsigned char drawGunString[] PROGMEM = { 13, 27, 10, 32, 36, 99 };
static unsigned char drawScoreString[] PROGMEM = { 13, 27, 10, 32, 99 };
static unsigned char TimeString[] PROGMEM = { 29, 18, 22, 14, 37, 99 };
static unsigned char TimesString[] PROGMEM = { 29, 18, 22, 14, 99 };
static unsigned char WinnerString[] PROGMEM = { 32, 18, 23, 23, 14, 27, 99 };
static unsigned char scoreString[] PROGMEM = { 28, 12, 24, 27, 14, 37, 99 };



void loop()
{
    display.clearDisplay();

    display.drawLine(0, 0, 127, 0, 1);      // draw border
    display.drawLine(0, 63, 127, 63, 1);
    display.drawLine(0, 0, 0, 63, 1);
    display.drawLine(127, 0, 127, 63, 1);
    
    
    for (m = 0; m < 2; m++)
    {
        drawNumber(30 + (64 * m), 2, score[m], false);
    }
    
    for (m = 0; m < bulletsRemaining[PLAYER1]; m++)
    {
        drawChar(1 + (m << 2), 56, 41);
    }
    
    for (m = bulletsRemaining[1]; m > 0; m--)
    {
        drawChar(127 - (m << 2), 56, 41);
    }
    
    drawString(50, 2, TimeString, false);
    drawNumber(70, 2, time, false);
        
    for (m = 0; m < numCactus; m++)
    {
        display.drawBitmap(cactusX[m], cactusY[m], cactus, 8, 16, 1);  
    }
    
    if (hasCoach)
    {
        display.drawBitmap(56, coachY, coach, 16, 16, 1);
    }
            
    if (stage == Hit)
    {
        for (m = 0; m < 2; m++)
        {
            if (hasBeenShot[m])
            {
                int ghostX = x[m] +  ((m == 0) ? -10 : 10) - 6;
                int ghostY = y[m] + 10;
                
                if (counter < 10)
                {
                    display.drawBitmap(x[m] + ((m == 0) ? -4 : 4) - 6, y[m] - 8, cowboyShot[0 + (m << 2)], 8, 8, WHITE);
                    display.drawBitmap(x[m] + ((m == 0) ? -4 : 4) - 6 + 8, y[m] - 8, cowboyShot[1 + (m << 2)], 8, 8, WHITE);
                    display.drawBitmap(x[m] + ((m == 0) ? -4 : 4) - 6, y[m], cowboyShot[2 + (m << 2)], 8, 8, WHITE);
                    display.drawBitmap(x[m] + ((m == 0) ? -4 : 4) - 6 + 8, y[m], cowboyShot[3 + (m << 2)], 8, 8, WHITE);
                }
                           
                if (counter >= 10)
                {     
                    display.drawBitmap(ghostX, y[m], cowboyShot[8 + (m << 1)], 8, 8, WHITE);
                    display.drawBitmap(ghostX + 8, y[m], cowboyShot[9 + (m << 1)], 8, 8, WHITE);
                    
                    if (counter % 2 == 0)  // draw the ghost
                    {
                        display.drawBitmap(ghostX, ghostY - counter, cowboyShot[8 + (m << 1)], 8, 8, WHITE);
                        display.drawBitmap(ghostX + 8, ghostY - counter, cowboyShot[9 + (m << 1)], 8, 8, WHITE);
                        drawChar(ghostX + (m * 12), ghostY - counter - 1, 42);               
                    }            
                           
                    if (counter == ghostY)
                    {
                        counter = 0; 
                        stage = InGame; 
                        hasBeenShot[m] = false;
                    }
                }
            }
        }
    }
  
#ifdef DEBUG                    
    drawNumber(2, 8, fpsCounterOld, false);  
#endif          
    if (millis() - timerCounter >= 1000)
    { 
        fpsCounterOld = fpsCounter;
        fpsCounter = 0;
        timerCounter = millis();
        
        if (stage == InGame)
        {
            time--;
        
            if (time % 10 == 0 && time != -1)
            {
                level++;
                stage = NewGame;
            }
        
            if (time == -1)
            {
               time = 0;
               stage = GameOver; 
               counter = 0;
            }
        }
    }
#ifdef DEBUG       
    fpsCounter++; 
    drawNumber(2, 2, freeRam(), false);
#endif

    counter++;

    if (stage == Attract)
    {
        j = 0;
        showSplash();
               
        int notePitch;
        uint8_t noteLength;
        
        for (;;)
        {         
            notePitch = pgm_read_word(&(Watchpitches[j]));
            noteLength = pgm_read_byte(&(Watchlengths[j]));
            
            if (notePitch == NOTE_END) 
            {
                j = 0;
                continue;
            }       
            
            tone(12, notePitch, noteLength * 80);
            delay(noteLength * 80);
            noTone(12);
            
            if (digitalRead(11) == HIGH || digitalRead(10) == HIGH) 
            {
                playFireSound();
                break;  
            }
         
            j++;
        };     

        randomSeed(j);
        level = 0;
        stage = NewGame;
    }
         
      
    if (stage == GetReady)
    {     
      
      
      
        if (counter < 5)
        {
            drawString(CENTERED, 29, getString, false);
            drawString(CENTERED, 39, ReadyString, false);         
        }
        else if (counter >= 5 && counter < 25)
        {
            drawString(47, 28, drawGunString, true);
        }
        
        if (counter == 25)
        {
            counter = 0;
            stage = InGame;  
        }
    }
    
    
    if (stage == NewGame)
    {
        bulletsRemaining[PLAYER1] = 6;   
        bulletsRemaining[PLAYER2] = 6;   
        resetBullets();
        stage = InGame;       

        if (level == 0)
        {
            hasCoach = false;
            x[PLAYER1] = 20;
            y[PLAYER1] = 32;
            score[PLAYER1] = 0;    
            x[PLAYER2] = 108;
            y[PLAYER2] = 32;
            score[PLAYER2] = 0;    
            time = 60;
             
            resetBullets();

            numCactus = 0;
            stage = GetReady;
            counter = 0;  
        }
               
        if (level == 1)
        { 
            numCactus = 1;  
            cactusX[0] = 60;
            cactusY[0] = random(56);  
        }
               
        if (level == 2)
        { 
            numCactus = 2;
            
            cactusX[0] = 52;
            cactusY[0] = random(56);   
   
            cactusX[1] = 68;
            cactusY[1] = random(56);         
        }
              
        if (level == 3)
        { 
            hasCoach = true;
            numCactus = 0;
            coachY = 64;       
        }
          
        if (level == 4)
        {
            numCactus = 2;  
            cactusX[0] = 48;
            cactusY[0] = random(56);    
            cactusX[1] = 72;
            cactusY[1] = random(56);   
        }
        
        if (level == 5)
        {
            numCactus = 0;
            hasCoach = false;
        }
    }
            
            
    if (stage == GameOver)
    {        
        if (counter <= 30)
        {
            drawString(CENTERED, 19, TimesString, true);
            drawString(CENTERED, 36, upString, true);           
        }
        
        if (counter > 30)
        {
            if (score[PLAYER1] < 10)
            {
                drawNumber(48, 36, score[PLAYER1], true);
            }
            else
            {
                drawNumber(40, 36, score[PLAYER1], true);
            }
            
            drawInitialsChar(60, 34, 37);
            drawNumber(72, 36, score[PLAYER2], true);
            
            if (score[PLAYER1] > score[PLAYER2])
            {
                drawString(8, 20, WinnerString, true);     
            }
            
            if (score[PLAYER1] == score[PLAYER2])
            {
                drawString(CENTERED, 20, drawScoreString, true);
            }
            
            if (score[PLAYER1] < score[PLAYER2])
            {
                drawString(72, 20, WinnerString, true);
            }
        }
        
        if (counter == 90)
        {
            stage = Attract;
        }
    }
    
    
    if (stage == GetReady || stage == InGame || stage == Hit)
    {   
        for (m = 0; m < 2; m++)
        {
            if (!hasBeenShot[m])
            {
                draw12x8Bitmap(x[m] - 6, y[m] - 8, 0 + (m * 5));  
                
                if (!fireWasPressed[m])
                {  
                    draw12x8Bitmap(x[m] - 6, y[m], 1 + (5 * m) + animStep[m]);
                }
                else
                {
                    switch (gunDirection[m])
                    {
                        case PointingUp: draw12x8Bitmap(x[m] - 6, y[m], 3 + (5 * m)); break;
                        case PointingStraight: draw12x8Bitmap(x[m] - 6, y[m], 1 + animStep[m] + (5 * m)); break;
                        case PointingDown: draw12x8Bitmap(x[m] - 6, y[m], 4 + (5 * m)); break;  
                    }
                }
            }
        }
    }
    

    if (stage == InGame)
    {
        analogValX[PLAYER1] = analogRead(P1POTPINX);
        analogValY[PLAYER1] = analogRead(P1POTPINY);        
        analogValX[PLAYER2] = analogRead(P2POTPINX);
        analogValY[PLAYER2] = analogRead(P2POTPINY);
 
        for (m = 0; m < 2; m++)
        {
            counterSinceFired[m]++;
            ox[m] = x[m];
            oy[m] = y[m];       
        }
             
        if (!fireWasPressed[PLAYER1])
        {
            if (analogValX[PLAYER1] > 700 && x[PLAYER1] < 40) x[PLAYER1]++;    // too much shit here to optimize
            if (analogValX[PLAYER1] < 400 && x[PLAYER1] > 6) x[PLAYER1]--;
            
            if (analogValY[PLAYER1] < 400 && y[PLAYER1] > 8) y[PLAYER1]--;
            if (analogValY[PLAYER1] > 700 && y[PLAYER1] < 55) y[PLAYER1]++;
        }
        else
        {
            if (analogValY[PLAYER1] < 400) gunDirection[PLAYER1] = PointingUp;
            else if (analogValY[PLAYER1] > 700) gunDirection[PLAYER1] = PointingDown;
            else gunDirection[PLAYER1] = PointingStraight;
        }
                  
        if (!fireWasPressed[PLAYER2])
        {
            if (analogValX[PLAYER2] < 400 && x[PLAYER2] < 122) x[PLAYER2]++;
            if (analogValX[PLAYER2] > 700 && x[PLAYER2] > 88) x[PLAYER2]--;
            
            if (analogValY[PLAYER2] > 700 && y[PLAYER2] > 8) y[PLAYER2]--;
            if (analogValY[PLAYER2] < 400 && y[PLAYER2] < 55) y[PLAYER2]++;
        }
        else
        {
            if (analogValY[PLAYER2] > 700) gunDirection[PLAYER2] = PointingUp;
            else if (analogValY[PLAYER2] < 400) gunDirection[PLAYER2] = PointingDown;
            else gunDirection[PLAYER2] = PointingStraight;
        }
      
      
      
        for (m = 0; m < 2; m++)
        {        
            if (digitalRead(10 + m) == HIGH) fireWasPressed[m] = true;    
                        
            if (digitalRead(10 + m) == LOW && fireWasPressed[m] == true)
            {
                fireWasPressed[m] = false; 
               
                if (bulletsFired[m] < MAX_BULLETS && counterSinceFired[m] > 8 && bulletsRemaining[m] > 0)
                {
                    playFireSound();
                    bulletsRemaining[m]--;       
                          
                    for (byte i = 0; i < MAX_BULLETS; i++)
                    {
                        if (bullets[m][i].alive == false)
                        {
                             bullets[m][i].fire(x[m] + ((m == 0) ? 6 : -6), y[m]); 
                             bullets[m][i].alive = true;
                             counterSinceFired[m] = 0;
                             bulletsFired[m]++;
                             bullets[m][i].dx = (m == 0) ? 3 : -3;
                            
                             switch (gunDirection[m])
                             {
                                 case PointingUp: bullets[m][i].dy = -3; break;                            
                                 case PointingStraight: bullets[m][i].dy = 0; break;
                                 case PointingDown: bullets[m][i].dy = 3; break;  
                             }
                                              
                             break;
                        }
                    }     
                }
            }    
        }                 
            
    
        for (m = 0 ; m < 2; m++)
        {
            for (byte i = 0; i < MAX_BULLETS; i++)
            {
                if (bullets[m][i].alive)
                {
                    if (bullets[m][i].x >= x[!m] - 6 
                    && bullets[m][i].x <= x[!m] + 6 
                    && bullets[m][i].y >= y[!m] - 8 
                    && bullets[m][i].y <= y[!m] + 8)
                    {
                        playCowboyHitSound();
                        score[(m == 0) ? PLAYER1 : PLAYER2]++;
                        bullets[m][i].alive = false;
                        bulletsFired[m]--;
                        stage = Hit;      
                        hasBeenShot[!m] = true;
                        counter = 0;
                        resetBullets();
                    }
                  
                    bullets[m][i].move();
                }
            }    
        }
        
        
        for (m = 0; m < 2; m++)
        {      
            if (ox[m] != x[m] || oy[m] != y[m]) animStep[m] = !animStep[m];
        }
          
        if (--coachY < -16) coachY = 128;
    }
       
    display.display();  
    
    if (stage == GetReady && counter == 1)
    {     
        int notePitch;
        uint8_t noteLength;
        j = 0;
        
        for (;;)
        {         
            notePitch = pgm_read_word(&(GBUpitches[j]));
            noteLength = pgm_read_byte(&(GBUlengths[j]));
            
            if (notePitch == NOTE_END) 
            {
                break;
            }       
            
            tone(12, notePitch, noteLength * 80);
            delay(noteLength * 80);
            noTone(12);
         
            j++;
        }     
    }
}


void resetBullets()
{
    for (m = 0; m < MAX_BULLETS; m++)
    {
        for (int i = 0; i < 6; i++)
        {
            bullets[m][i].alive = false;
        }
    }  
    
    bulletsFired[PLAYER1] = 0;
    bulletsFired[PLAYER2] = 0;
}


void playHitCactusSound()
{
    tone(12, 200, 10);
    delay(10);
}


void playFireSound()
{
    tone(12, 350, 5);
    delay(2);
    noTone(12);
    tone(12, 330, 5);
    delay(5);
    noTone(12);
    tone(12, 330, 5);
    delay(5);
    noTone(12);
    tone(12, 330, 5);
    delay(5);
    noTone(12);
    tone(12, 350, 5);
    delay(5);
    noTone(12);
    tone(12, 330, 5);
    delay(5);
    noTone(12);
    tone(12, 330, 5);
    delay(5);
    noTone(12);
    tone(12, 330, 5);
    delay(5);
    noTone(12);
}  
  
  
void playCowboyHitSound()
{
    unsigned char bleep3[]  = { 140, 125, 100, 75, 50 };
    byte lengths3[]  = { 1, 1, 1, 1, 1 } ; 
       
    for (j = 0; j < 5; j++)
    {
        tone(12, bleep3[j], lengths3[j] * 75);
        delay (75);
    }   
}


static unsigned char font[] PROGMEM = 
{
	0xEA, 0xAA, 0xE0,
	0x4C, 0x44, 0xE0,
	0xE2, 0xE8, 0xE0,
	0xE2, 0x62, 0xE0,
	0xAA, 0xE2, 0x20,
	0xE8, 0xE2, 0xE0,
	0xE8, 0xEA, 0xE0,
	0xE2, 0x22, 0x20,
	0xEA, 0xEA, 0xE0,
	0xEA, 0xE2, 0xE0,
	0x4A, 0xEA, 0xA0,
	0xCA, 0xCA, 0xC0,
	0x68, 0x88, 0x60,
	0xCA, 0xAA, 0xC0,
	0xE8, 0xC8, 0xE0,
	0xE8, 0xC8, 0x80,
	0x68, 0xAA, 0xE0,
	0xAA, 0xEA, 0xA0,
	0xE4, 0x44, 0xE0,
	0x62, 0x22, 0xC0,
	0xAA, 0xCA, 0xA0,
	0x88, 0x88, 0xE0,
	0xAE, 0xEA, 0xA0,
	0xCA, 0xAA, 0xA0,
	0x4A, 0xAA, 0x40,
	0xCA, 0xE8, 0x80,
	0x6A, 0xE2, 0x20,
	0xCA, 0xCA, 0xA0,
	0x68, 0xE2, 0xC0,
	0xE4, 0x44, 0x40,
	0xAA, 0xAA, 0xE0,
	0xAA, 0xAA, 0x40,
	0xAA, 0xEE, 0xA0,
	0xAA, 0x4A, 0xA0,
	0xAA, 0x62, 0x60,
	0xE2, 0x48, 0xE0,
        0x44, 0x40, 0x40,
        0x0, 0x40, 0x40,
        0x6F, 0xF7, 0x31, 
        0xCE, 0xEC, 0x80,
        0x0, 0xE0, 0x0, 
        0x66, 0x66, 0x6,
        0x69, 0x60, 0x0,  // halo 42
};



void draw12x8Bitmap(char x, char y, char which)
{
    for (register byte i = 0; i < 4; i++)
    {
        // draw first byte easy      
        for (register byte j = 0 ; j < 8; j++)
        {
            uint8_t nextByte = pgm_read_byte((which << 3) + (which << 2) + cowboy + (i << 1) + i);
            
            if (nextByte & (128 >> j))
            {
                display.drawPixel(x + j, y + (i << 1), WHITE);
            }
        }    
        
        // draw the next upper nybble    
        for (register byte j = 0 ; j < 4; j++)
        {
            uint8_t nextByte = pgm_read_byte((which << 3) + (which << 2) + cowboy + (i << 1) + i + 1);
            
            if (nextByte & (128 >> j))
            {
                display.drawPixel(x + j + 8, y + (i << 1), WHITE);
            }
         }
        
        // draw the next lower nybble       
        for (register byte j = 0 ; j < 4; j++)
        {
            uint8_t nextByte = pgm_read_byte((which << 3) + (which << 2) + cowboy + (i << 1) + i + 1);
            
            if (nextByte & (8 >> j))
            {
                display.drawPixel(x + j, y + (i << 1) + 1, WHITE);
            }
         }
        
        
        // draw the trailing byte
        for (register byte j = 0 ; j < 8; j++)
        {
            uint8_t nextByte = pgm_read_byte((which << 3) + (which << 2) + cowboy + (i << 1) + i + 2);
            
            if (nextByte & (128 >> j))
            {
                display.drawPixel(x + j + 4, y + (i << 1) + 1, WHITE);
            }
        }
    }
}

void drawChar(char x, char y, char which)
{
    for (register byte i = 0; i < 3; i++)
    {
        for (register byte j = 0 ; j < 4; j++)
        {
            uint8_t nextByte = pgm_read_byte(font + (which * 3) + i);
            
            if (nextByte & (128 >> j))
            {
                display.drawPixel(x + j, y + (i << 1), WHITE);
            }
            
            if (nextByte & (8 >> j))
            {
                display.drawPixel(x + j, y  + (i << 1) + 1, WHITE);
            }
        }
    }
}


void drawString(byte x, byte y, unsigned char* string, bool big)
{
    if (y < 0 || y > 63) return;
    
    unsigned char i = 0;
    unsigned char nextByte = 0;
    
    if (x == CENTERED)
    {
        while (1)
        {
            nextByte = pgm_read_byte(string + i++);
            if (nextByte == 99) break;  
        }
    
        if (!big)
        {
            i <<= 2;
        }
        else
        {
            i <<= 3;
        }
        
        x = 68 - (i >> 1);    
        
        i = 0;
    }
    
    while (1)
    {
        nextByte = pgm_read_byte(string + i++);
        
        if (nextByte == 0) continue; // space
    
        if (nextByte == 99) break;
        
        if (!big)
        {
            drawChar(x + ((i - 1) << 2), y, nextByte);
        }
        else
        {
            drawInitialsChar(x + ((i - 1) << 3), y, nextByte);  
        }
    }  
}


void drawNumber(byte x, byte y, unsigned int a, bool big)  // prints out a 5 digit number max 65535 with leading zeroes
{
    unsigned int b;
    byte offset = 0;
    unsigned int startnum = a;
    
    if (a == 0)
    {
        if (!big)
        {
            drawChar(x, y, 0);
        }
        else
        {
            drawInitialsChar(x, y, 0);
        }
        return;
    }
    
    for (unsigned int i = 10000; i > 0; i /= 10)
    {
        if (startnum < i) continue; // comment out if you want leading zeroes
               
        b = a / i;
        a = a - (b * i);
        
        if (!big)
        {
            drawChar(x + offset, y, b);  
        }
        else
        {          
            drawInitialsChar(x + offset * 2, y, b);
        }
           
        offset += 4;
    }   
}


void drawBigPixel(char x, char y)
{
    display.drawPixel(x, y, WHITE);
    display.drawPixel(x, y + 1, WHITE);
    display.drawPixel(x + 1, y, WHITE);
    display.drawPixel(x + 1, y + 1, WHITE);
}


void drawInitialsChar(char x, char y, char which)
{
    for (register byte i = 0; i < 3; i++)
    {
        for (register byte j = 0 ; j < 4; j++)
        {
            uint8_t nextByte = pgm_read_byte(font + (which * 3) + i);
            
            if (nextByte & (128 >> j))
            {
                drawBigPixel(x + (j * 2), y + (i * 4));
            }
            
            if (nextByte & (8 >> j))
            {
                drawBigPixel(x + (j * 2), y + (i * 4) + 2);
            }
        }
    }
}



static uint8_t splash[1024] PROGMEM = 
{ 
    0x02, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x03, 0x00,
    0x25, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15, 0x00, 0x0E, 0x00,
    0x32, 0x8C, 0x00, 0x00, 0x03, 0x2C, 0xE6, 0xC4, 0xD8, 0x09, 0x80, 0x00, 0x07, 0x01, 0x1C, 0x00,
    0x3C, 0x44, 0x00, 0x00, 0x03, 0xEF, 0xBE, 0xFC, 0xF8, 0x1F, 0xE0, 0x04, 0x03, 0x82, 0x30, 0x00,
    0x3E, 0xE2, 0x00, 0x00, 0x03, 0xEF, 0xBE, 0xFC, 0xD8, 0x19, 0x38, 0x22, 0x03, 0x82, 0x20, 0x00,
    0x3F, 0x11, 0x00, 0x00, 0x03, 0xEF, 0xBE, 0xFC, 0xF8, 0x18, 0x28, 0x12, 0x06, 0x03, 0xC0, 0x00,
    0x3F, 0xC8, 0x40, 0x00, 0x03, 0xEF, 0xBE, 0xFC, 0xE8, 0x1F, 0xF8, 0x2A, 0x1C, 0x00, 0x80, 0x00,
    0x37, 0xC6, 0x00, 0x00, 0x03, 0xEF, 0xBE, 0xEC, 0xF8, 0x1F, 0xF8, 0x0E, 0x00, 0x00, 0x80, 0x00,
    0x3F, 0xC2, 0x20, 0x00, 0x01, 0xC7, 0x3C, 0x38, 0x30, 0x07, 0xB8, 0x07, 0x00, 0x00, 0x80, 0x03,
    0x3F, 0xD5, 0xA0, 0x00, 0x00, 0xC7, 0x18, 0x30, 0x30, 0x06, 0x18, 0x07, 0x00, 0xFF, 0x80, 0x33,
    0x3B, 0xD0, 0xA0, 0x00, 0x00, 0xC7, 0x18, 0x30, 0x30, 0x06, 0x18, 0x0C, 0x01, 0x00, 0x80, 0xCC,
    0x3F, 0xD4, 0x40, 0x00, 0x00, 0xC7, 0x18, 0x30, 0x30, 0x06, 0x18, 0x38, 0x00, 0xFF, 0x8F, 0xC3,
    0x3F, 0x56, 0xA0, 0x00, 0x00, 0xC7, 0x18, 0x30, 0x30, 0x06, 0x18, 0x00, 0x05, 0x1F, 0xB3, 0x0C,
    0x2F, 0xD5, 0x20, 0x00, 0x00, 0xC7, 0x38, 0x30, 0x30, 0x06, 0x18, 0x00, 0x0A, 0x0F, 0xF4, 0x30,
    0x3F, 0xD7, 0x3F, 0xC0, 0x00, 0xC7, 0x30, 0x30, 0x30, 0x06, 0x18, 0x00, 0x04, 0x06, 0xC3, 0x00,
    0x3E, 0xD3, 0x3F, 0xC0, 0x00, 0xEF, 0xB8, 0x38, 0x70, 0x0F, 0x38, 0x00, 0x28, 0x0F, 0x0C, 0x62,
    0x7F, 0xD7, 0x28, 0x00, 0x00, 0xEF, 0xB0, 0x38, 0x30, 0x07, 0x3C, 0x00, 0x15, 0x01, 0xC8, 0x00,
    0x87, 0xD7, 0x3E, 0x08, 0x00, 0x6D, 0xB0, 0x30, 0x30, 0x07, 0x18, 0x00, 0xA5, 0x06, 0x00, 0x88,
    0x18, 0x75, 0x3F, 0xC8, 0x00, 0x6D, 0xB0, 0x30, 0x30, 0x06, 0x18, 0x00, 0xD5, 0x06, 0x20, 0x00,
    0x1E, 0x1F, 0x00, 0x08, 0x00, 0x6D, 0xB0, 0x30, 0x30, 0xC6, 0x18, 0x03, 0xB5, 0x00, 0x02, 0x22,
    0x17, 0xE1, 0xC0, 0xCE, 0x00, 0x6D, 0xB0, 0x30, 0x30, 0xC6, 0x18, 0x00, 0xD5, 0x08, 0x80, 0x00,
    0x5E, 0xD6, 0x0C, 0x56, 0x00, 0x6D, 0xA0, 0x30, 0x30, 0xC6, 0x18, 0x08, 0xD5, 0x00, 0x08, 0x88,
    0x5F, 0xD1, 0xC3, 0x97, 0x80, 0x3C, 0xE0, 0x30, 0x30, 0xC6, 0x18, 0x05, 0xD4, 0x22, 0x20, 0x00,
    0x17, 0xD5, 0xBF, 0x16, 0x80, 0x38, 0xE0, 0x38, 0x71, 0xC7, 0x38, 0x21, 0xD0, 0xC0, 0x02, 0x22,
    0x16, 0x95, 0xBF, 0x16, 0x20, 0x38, 0xE0, 0xFC, 0xFF, 0xDF, 0xF8, 0x11, 0xAC, 0x08, 0x80, 0x00,
    0x16, 0x91, 0x9B, 0x16, 0x20, 0x38, 0xE0, 0xFC, 0xFF, 0xDF, 0xF8, 0xA5, 0xE0, 0x00, 0x08, 0x88,
    0x16, 0x90, 0x1B, 0x17, 0x20, 0x38, 0xE0, 0xFC, 0xFF, 0xDF, 0xF8, 0x65, 0x80, 0x22, 0x00, 0x01,
    0xDD, 0xF5, 0x9B, 0x04, 0xE0, 0x38, 0xE0, 0x7C, 0xFF, 0xDF, 0xF8, 0x35, 0x03, 0xFF, 0xFF, 0xFF,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x38, 0xE0, 0x7C, 0xFF, 0xDF, 0xF0, 0x35, 0x00, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0x06, 0x20, 0x28, 0xA0, 0x64, 0xDE, 0xD9, 0xE0, 0x21, 0x00, 0x00, 0x00, 0x00,
    0xFA, 0x48, 0xDD, 0x1E, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3D, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0x3E, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3D, 0x80, 0x66, 0xEC, 0xDD,
    0xFF, 0xFF, 0xFF, 0x3E, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3D, 0xB8, 0x7F, 0xFF, 0xFF,
    0xFF, 0xFF, 0xC2, 0x3E, 0xFB, 0x6D, 0xBE, 0x6D, 0x63, 0xA3, 0xFF, 0x1D, 0xAA, 0x7B, 0xFF, 0xFF,
    0xEF, 0xBF, 0xC2, 0x3E, 0xC3, 0xEF, 0xBE, 0x61, 0xE7, 0xE3, 0xFF, 0x1D, 0xAA, 0x3F, 0xFF, 0xFF,
    0xEF, 0xAF, 0x42, 0x3E, 0x93, 0xEF, 0xBE, 0x40, 0x07, 0xE3, 0xEB, 0xCD, 0xAA, 0x20, 0x9F, 0x5F,
    0xEF, 0xB7, 0x42, 0x3F, 0x23, 0xEF, 0xBE, 0x40, 0x67, 0xE3, 0x2F, 0x63, 0xAA, 0x20, 0x80, 0xDF,
    0xEF, 0xA7, 0x42, 0x3E, 0x23, 0xEF, 0xBE, 0x7F, 0xE7, 0xE3, 0xFF, 0x21, 0xAA, 0x20, 0x80, 0x5F,
    0xEF, 0xA7, 0x42, 0x3F, 0xC3, 0xEF, 0xBE, 0x7F, 0xEF, 0xE3, 0xFD, 0x18, 0xAA, 0x21, 0x80, 0x5F,
    0xEF, 0xA7, 0x52, 0x38, 0x80, 0xC7, 0x1C, 0x18, 0xE6, 0x63, 0x33, 0x0F, 0xEA, 0x20, 0x80, 0x5F,
    0x6F, 0xA7, 0x6A, 0x31, 0x80, 0xC7, 0x18, 0x18, 0x64, 0x22, 0x23, 0x04, 0x1A, 0x21, 0x80, 0x5F,
    0x6F, 0xE7, 0x6E, 0x23, 0x00, 0xC7, 0x18, 0x18, 0x66, 0x22, 0x33, 0x06, 0x1A, 0x2A, 0x80, 0x5F,
    0x6F, 0xA7, 0x5A, 0x46, 0x00, 0xC7, 0x18, 0x18, 0x66, 0x22, 0x33, 0x03, 0x06, 0x36, 0x80, 0x7F,
    0x69, 0x27, 0x7E, 0x0C, 0x00, 0xC7, 0x18, 0x1A, 0x47, 0x02, 0x32, 0x01, 0x82, 0x29, 0x80, 0x5F,
    0x69, 0x26, 0x7E, 0x18, 0x00, 0xC7, 0x10, 0x1A, 0x03, 0x00, 0x30, 0x00, 0xC1, 0x3B, 0x80, 0x44,
    0x69, 0x24, 0x7E, 0xF0, 0x00, 0xEF, 0xB0, 0x1F, 0x03, 0x80, 0x30, 0x10, 0x61, 0x25, 0xA0, 0x44,
    0x69, 0x20, 0x00, 0x23, 0x00, 0xEF, 0xF8, 0x3F, 0x03, 0x80, 0x78, 0x00, 0x30, 0x3B, 0xAC, 0x44,
    0x69, 0x21, 0x00, 0x44, 0x00, 0xEF, 0xB0, 0x1E, 0x03, 0xC0, 0x70, 0x50, 0x1F, 0x3F, 0x9E, 0x44,
    0x69, 0x22, 0x00, 0x8C, 0x00, 0x6D, 0xB0, 0x1A, 0x00, 0xE0, 0x20, 0x00, 0x0C, 0xC0, 0x0E, 0x44,
    0x69, 0x60, 0x00, 0x08, 0x00, 0x6D, 0xB0, 0x1A, 0x4C, 0x60, 0x20, 0x00, 0x06, 0x00, 0x37, 0x44,
    0x69, 0xB0, 0x03, 0x08, 0x00, 0x6D, 0xB0, 0x18, 0x6C, 0x60, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x69, 0xA0, 0x02, 0x3C, 0x00, 0x6D, 0xA0, 0x18, 0x6C, 0x20, 0x20, 0x0E, 0xCE, 0xEE, 0x3A, 0xCE,
    0x6B, 0xC0, 0x04, 0x6C, 0x00, 0x7C, 0xE0, 0x18, 0x6C, 0x30, 0x20, 0x0A, 0xA8, 0x88, 0x22, 0xA8,
    0x6F, 0x80, 0x08, 0x4E, 0x00, 0x3C, 0xE0, 0x18, 0xEC, 0x70, 0x70, 0x0E, 0xEC, 0xEE, 0x32, 0xEC,
    0x6F, 0x00, 0x13, 0xFF, 0x00, 0x38, 0xE0, 0x7F, 0xCF, 0xF0, 0xFC, 0x08, 0xC8, 0x22, 0x22, 0xC8,
    0x6E, 0x00, 0x1F, 0xFF, 0x00, 0x38, 0xE0, 0x7F, 0xEF, 0xF0, 0xFC, 0x08, 0xAE, 0xEE, 0x22, 0xAE,
    0x6C, 0x00, 0x3F, 0xFF, 0x80, 0x38, 0xE0, 0x5F, 0xCF, 0xF0, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x6A, 0x01, 0xD7, 0xF3, 0xC0, 0x38, 0xE0, 0x7F, 0xEF, 0xE0, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x6E, 0x03, 0x37, 0xF2, 0xE2, 0x38, 0xE0, 0x7F, 0xCF, 0xA0, 0xFC, 0x00, 0xEE, 0x1D, 0x09, 0x40,
    0xE8, 0x03, 0x03, 0xFE, 0x7E, 0x18, 0xE0, 0x6F, 0xED, 0xC0, 0xFC, 0x00, 0x4A, 0x15, 0x15, 0xC0,
    0xE8, 0x06, 0x0E, 0x0C, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4A, 0x1D, 0x1C, 0x80,
    0xEA, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4A, 0x11, 0x14, 0x80,
    0xEF, 0x98, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4E, 0x11, 0xD4, 0x80,
    0xEF, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


void showSplash()
{
    display.clearDisplay();
  
    for (byte y = 0; y < 64; y++)
    {
        for (byte x = 0; x < 16; x ++)
        {
            uint8_t nextByte = pgm_read_byte(splash + (y << 4) + x);
              
            for (register int j = 0 ; j < 8; j++)
            {          
                if (nextByte & (128 >> j))
                {
                    display.drawPixel((x << 3) + j, y, WHITE);
                }
            }
        }
    }
    
    display.display();
}

