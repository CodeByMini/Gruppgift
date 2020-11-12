#include <string.h>
#include <Wire.h>
#include <U8g2lib.h>

#define ODDEVEN 2
#define ODD 1
#define EVEN 2

#define DAYNIGHT 1
#define DAYTIME 0
#define NIGHTTIME 1

#define NOEFFECT 0
#define SCROLL 1
#define BLINK 2


U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display


int allTokens = 0;
char printout[37];
int effect = 0;
int lastCustomer = 0;
int prindex = 0;

bool WaitingFor20Secs = true;
long int EndTime;
long int TimeNow = 0;
#define WAIT20SECS 5000

//---STRUCT---

typedef struct {
  char body[37];
  int effect;
  int times;
  int starttime;
} AD;

typedef struct {
  int tokens;
  AD ad[3];
} CUSTOMER;

CUSTOMER loc[6];

CUSTOMER c1;
CUSTOMER c2;
CUSTOMER c3;
CUSTOMER c4;
CUSTOMER c5;

AD ad10;
AD ad11;
AD ad12;
AD ad20;
AD ad21;
AD ad30;
AD ad31;
AD ad40;
AD ad41;
AD ad50;

//---STRUCT END

int GetTokens() {
  int temp;
  for (int i = 0; i < 5; i++) {
    temp += loc[i].tokens;
  }
  return temp;
}

int GetRelevantAd(int Event, int customer, int i)
{
  if (Event == NIGHTTIME)
  {
  // && Adtime == 2 && Adtimestart == 1) // check if customer has Night time ad.
    if (loc[customer].ad[i].times == 2 && loc[customer].ad[i].starttime == NIGHTTIME)
    {
      strcpy(printout, loc[customer].ad[0].body);
      return 0;
    }
  }
  if (Event == DAYTIME)
  {
  // && Adtime == 2 && Adtimestart == 0) // check if customer has Day time ad.
    if (loc[customer].ad[i].times == 2 && loc[customer].ad[i].starttime == DAYTIME)
      {
        strcpy(printout, loc[customer].ad[1].body);
        return 0;
      }
  }

  // check if customer has odd or even minute setting.
  if (loc[customer].ad[i].times == 1 && loc[customer].ad[i].starttime == ODD)
      {
        strcpy(printout, loc[customer].ad[0].body);
        return 0;
      }
  if (loc[customer].ad[i].times == 1 && loc[customer].ad[i].starttime == EVEN)
      {
        strcpy(printout, loc[customer].ad[1].body);
        return 0;
      }
 }

int GetMessage(int customer, char * printout) {
  int numberOfAds = sizeof(loc[customer].ad) / sizeof(loc[customer].ad[0]);
  Serial.print("\nnumberOfAds");
  Serial.print(numberOfAds);
  int randomInt = random(numberOfAds);
  for (int i = 0; i < numberOfAds; i++)
    if (loc[customer].ad[i].times == 0) {
      while(strlen(loc[customer].ad[randomInt].body)>3 == 0){
        randomInt--;
      }
      strcpy(printout, loc[customer].ad[randomInt].body);
      return randomInt;
    }
    else
    {
      /* check time and use a proper
        function to choose correct ad */
      return 0;
    }
}

int GetEffect(int customer, int adindex) {
  int temp;
  temp = loc[customer].ad[adindex].effect;
  return temp;
}

int GetCustomer(int TotalTokens)
{
  int customerToSend = 0;
  do
  {
    int randTokens = random(1, TotalTokens);

    if (randTokens > 0 && randTokens <= 5000)
    {customerToSend = 0;}
    if (randTokens > 5000 && randTokens <= 8000)
    {customerToSend = 1;}
    if (randTokens > 8000 && randTokens <= 9500)
    {customerToSend = 2;}
    if (randTokens > 9500 && randTokens <= 13500)
    {customerToSend = 3;}
    if (randTokens > 13500 && randTokens <= 14500)
    {customerToSend = 4;}

  } while (lastCustomer == customerToSend);
  lastCustomer = customerToSend;
  return customerToSend;
}

char GetChar(char * printout) {
  if ((unsigned char)printout[prindex] == 195) {
    prindex++;
    if ((unsigned char)printout[prindex] == 164) {
      return ('\xe4');//ä
    } else if ((unsigned char)printout[prindex] == 165) {
      return ('\xe5');//å
    } else if ((unsigned char)printout[prindex] == 182) {
      return ('\xf6');//ö
    }
  }
  else {
    return (printout[prindex]);
  }
}

int CheckWaitingFor20Secs() {
  if (TimeNow > EndTime){
    WaitingFor20Secs = false;
    return 0;
  }else{
    return 1;
  }
}

void setup() {
  Serial.begin(9600);
  u8g2.begin();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFontMode(0);

  //ADS BEGIN
  //Hederliga Harrys
  c1.tokens = 5000;
  strcpy(ad10.body, "Köp bil hos Harry");
  ad10.effect = NOEFFECT;
  ad10.times = 0;
  ad10.starttime = 0;
  c1.ad[0] = ad10;

  strcpy(ad11.body, "En god bilaffär (för Harry!)");
  ad11.effect = SCROLL;
  ad11.times = 0;
  ad11.starttime = 0;
  c1.ad[1] = ad11;

  strcpy(ad12.body, "Hederlige Harrys Bilar");
  ad12.effect = SCROLL;
  ad12.times = 0;
  ad12.starttime = 0;
  c1.ad[2] = ad12;

  //Farmor Ankas Pajer AB
  c2.tokens = 3000;

  strcpy(ad20.body, "Köp paj hos Farmor Anka");
  ad20.effect = SCROLL;
  ad20.times = 0;
  ad20.starttime = 0;
  c2.ad[0] = ad20;

  strcpy(ad21.body, "Skynda innan Mårten ätit alla pajer");
  ad21.effect = SCROLL;
  ad21.times = 0;
  ad21.starttime = 0;
  c2.ad[1] = ad21;

  //Svarte Petters Svartbyggen
  c3.tokens = 1500;

  strcpy(ad30.body, "Låt Petter bygga åt dig");
  ad30.effect = SCROLL;
  ad30.times = 0;
  ad30.starttime = 0;
  c3.ad[0] = ad30;

  strcpy(ad31.body, "Bygga svart? Ring Petter");
  ad31.effect = SCROLL;
  ad31.times = 0;
  ad31.starttime = 0;
  c3.ad[1] = ad31;

  //Långbens detektivbyrå
  c4.tokens = 4000;

  strcpy(ad40.body, "Mysterier? Ring Långben");
  ad40.effect = SCROLL;
  ad40.times = 0;
  ad40.starttime = 0;
  c4.ad[0] = ad40;

  strcpy(ad41.body, "Långben fixar biffen");
  ad41.effect = SCROLL;
  ad41.times = 0;
  ad41.starttime = 0;
  c4.ad[1] = ad41;

  //IoT's Reklambyrå
  c5.tokens = 1000;

  strcpy(ad50.body, "Synas här? IOT:s Reklambyrå");
  ad50.effect = SCROLL;
  ad50.times = 0;
  ad50.starttime = 0;
  c5.ad[0] = ad50;
  //ADS END

  loc[0] = c1;
  loc[1] = c2;
  loc[2] = c3;
  loc[3] = c4;
  loc[4] = c5;

  allTokens = GetTokens();

}

void loop() {
  EndTime = TimeNow + WAIT20SECS;
  int customer = GetCustomer(allTokens);
  int adIndex = GetMessage(customer, printout);
  int effect = GetEffect(customer, adIndex);
  delay(20);
  WaitingFor20Secs = true;

while(WaitingFor20Secs){

  switch (effect) {
    case 1: {
        int cursor = 0;
        while (CheckWaitingFor20Secs()) {
          u8g2.firstPage();
          do {
            u8g2.setCursor(cursor, 20);
            for (prindex = 0; prindex < strlen(printout); prindex++) {
              u8g2.write(GetChar(printout));
            }
            delay(20);
          } while ( u8g2.nextPage() );
          cursor -= 10;
          TimeNow = millis();
        }
      }

    default: {
      while(CheckWaitingFor20Secs()){
        do {
          u8g2.firstPage();
            u8g2.setCursor(0, 20);
            for (prindex = 0; prindex < strlen(printout); prindex++) {
              u8g2.write(GetChar(printout));
            }
            delay(20);
          } while ( u8g2.nextPage() );
          TimeNow = millis();
      }
  }
}
}

}