extern"C" {
#include <delay.h>
#include <FillPat.h>
#include <I2CEEPROM.h>
#include <LaunchPad.h>
#include <OrbitBoosterPackDefs.h>
#include <OrbitOled.h>
#include <OrbitOledChar.h>
#include <OrbitOledGrph.h>
}


/* ------------------------------------------------------------ */
/*   			 Local Type Definitions   	 */
/* ------------------------------------------------------------ */
#define DEMO_0   	 0
#define DEMO_1   	 2
#define DEMO_2   	 1
#define DEMO_3   	 3
#define RED_LED   GPIO_PIN_1
#define BLUE_LED  GPIO_PIN_2
#define GREEN_LED GPIO_PIN_3

char I2CGenTransmit(char * pbData, int cSize, bool fRW, char bAddr);
bool I2CGenIsNotIdle();


/* ------------------------------------------------------------ */
/*   			 Global Variables   	 */
/* ------------------------------------------------------------ */
extern int xchOledMax; // defined in OrbitOled.c
extern int ychOledMax; // defined in OrbitOled.c




/* ------------------------------------------------------------ */
/*   			 Local Variables   		 */
//* ------------------------------------------------------------ */
char    chSwtCur;
char    chSwtPrev;
bool    fClearOled;
long unsigned int poten;
short accel;
char  chX0Addr = 0x32;
char  chY0Addr = 0x34;
char  chZ0Addr = 0x36;
int y, yMin, yMax;
int x, xMin, xMax;
long lBtn1;

typedef struct {
	char *name;
	char *icon;
	int currentValue;
	int highScore;
	int (*game)(void);
}stat;

//images

//poop
char poop[] = 	{0xFF, 0x01, 0x01, 0x01, 0x49, 0x31, 0x01, 0x81, 0x01, 0x49, 0x31, 0x01, 0xFF,
				0xFF, 0x80, 0x80, 0x98, 0x9C, 0x9E, 0x9F, 0x9F, 0x9E, 0x9C, 0x98, 0x80, 0xFF};

char poop_inv[] = 	{~0xFF, ~0x01, ~0x01, ~0x01, ~0x49, ~0x31, ~0x01, ~0x81, ~0x01, ~0x49, ~0x31, ~0x01, ~0xFF,
					~0xFF, ~0x80, ~0x80, ~0x98, ~0x9C, ~0x9E, ~0x9F, ~0x9F, ~0x9E, ~0x9C, ~0x98, ~0x80, ~0xFF};
			
//food
char food[] = 	{0xFF, 0x01, 0x09, 0x1D, 0x9D, 0x49, 0xC9, 0x49, 0x89, 0x09, 0x01, 0x01, 0xFF,
				0xFF, 0x80, 0x80, 0x87, 0x99, 0x91, 0x91, 0x91, 0x99, 0x87, 0x80, 0x80, 0xFF};

char food_inv[] = 	{~0xFF, ~0x01, ~0x09, ~0x1D,~0x9D,~0x49, ~0xC9, ~0x49,~0x89, ~0x09, ~0x01, ~0x01, ~0xFF,
					~0xFF, ~0x80, ~0x80, ~0x87, ~0x99, ~0x91, ~0x91, ~0x91, ~0x99, ~0x87, ~0x80, ~0x80, ~0xFF};
			
//Sleep
char Sleep[] = 	{0xFF, 0x01, 0x41, 0x41, 0x41, 0xC1, 0x01, 0x11, 0x91, 0x51, 0x31, 0x01, 0xFF,
            	0xFF, 0x80, 0x84, 0x86, 0x85, 0x84, 0x80, 0x81, 0x81, 0x81, 0x81, 0x80, 0xFF};
				
char Sleep_inv[] = 	{~0xFF, ~0x01, ~0x41, ~0x41, ~0x41, ~0xC1, ~0x01, ~0x11, ~0x91, ~0x51, ~0x31, ~0x01, ~0xFF,
					~0xFF, ~0x80, ~0x84, ~0x86, ~0x85, ~0x84, ~0x80, ~0x81, ~0x81, ~0x81, ~0x81, ~0x80, ~0xFF};

//heart
char heart[] = 	{0xFF, 0x01, 0xC1, 0x21, 0x21, 0x21, 0x41, 0x21, 0x21, 0x21, 0xC1, 0x01, 0xFF,
            	0xFF, 0x80, 0x81, 0x82, 0x84, 0x88, 0x90, 0x88, 0x84, 0x82, 0x81, 0x80, 0xFF};

char heart_inv[] = 		{~0xFF, ~0x01, ~0xC1, ~0x21, ~0x21, ~0x21, ~0x41, ~0x21, ~0x21, ~0x21, ~0xC1, ~0x01, ~0xFF,
						~0xFF, ~0x80, ~0x81, ~0x82, ~0x84, ~0x88, ~0x90, ~0x88, ~0x84, ~0x82, ~0x81, ~0x80, ~0xFF};				

//faceHole
char faceHole[] = {0xFF, 0x01, 0x01, 0x01, 0xE1, 0xC1, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xE1, 0xC1, 0x01, 0x01, 0x01, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,0x0F, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x0F};

// functions pre-declaration .
void DeviceInit();
long getPoten();
short getAccel(int Axis);
int Runner_Game(void);
void OrbitOledPutNumber(int num);
void LightLED(int n);
int _min(int a, int b){return a < b ? a : b;}
int _max(int a, int b){return a > b ? a : b;}

stat hygiene;// = { “hygiene”, poop, 50, 0, //insert function pointer name here};
stat hunger;// = {“hunger”,  food, 50, 0, //insert function pointer name here};
stat sleepiness;// = { “sleep”,  Sleep, 50, 0, //insert function pointer name here};
stat love;// = {“love”,  heart, 50, 0, //insert function pointer name here};

void setup()
{
	DeviceInit();
	
	y = 16;
	yMin = 0;
	yMax = 31;
    
	x= 64;
	xMin = 0;
	xMax = 127;

	hunger.name = "Hunger";
    hunger.icon = food;
    hunger.currentValue = 50;
    hunger.highScore = 0;
    hunger.game = &Runner_Game;
      
    hygiene.name = "hygiene";
    hygiene.icon = poop;
    hygiene.currentValue = 50;
    hygiene.highScore = 0;
    hygiene.game = &Runner_Game;
        
    sleepiness.name = "Sleepiness";
    sleepiness.icon = Sleep;
    sleepiness.currentValue = 50;
    sleepiness.highScore = 0;
    sleepiness.game = &Runner_Game;
        
    love.name = "love";
    love.icon = heart;
    love.currentValue = 50;
    love.highScore = 0;
    love.game = &Runner_Game;    
        
}

    
void loop()
{
	// put your main code here, to run repeatedly
	
	accel = getAccel(chX0Addr);
	//x = (int)(((double)poten / 4095)*128);
	x += (int)(((double)accel));
    
	accel = getAccel(chY0Addr);
	y += (int)(((double)accel / 255)*16/7.5);
    
	
    main_menu();
}

/*----------------------------------------------------------*/
/* Main menu function separated from the loop */
/* use the mini_games() for the mini game coding */
/*----------------------------------------------------------*/


void main_menu(){
 	long lBtn1;
	int main_xMax = xMax-33; // 13 + 20
	int main_xMin = 13;
	int main_yMax = yMax-20;
	int main_yMin = 0;
	int velocity_x = 1;
	int velocity_y = 1;
	int face_x = xMax/2; 
	int face_y = (main_yMax - main_yMin)/2;
	int frame_num = 0;
	int score = 0;

	
	while(1){
  
	// Poten operations with menu selecting
	poten = getPoten();
	lBtn1 = GPIOPinRead(BTN1Port, BTN1);
	
	frame_num++;
	

	
	if(!(frame_num % 800)){
                //nice flash when it decays, remove?
                LightLED(100);
                delay(100);
                LightLED(0);
                delay(100);
		hygiene.currentValue -= 5;
		hunger.currentValue -= 5;
		love.currentValue -= 5;
		sleepiness.currentValue -= 5;
	}
	
	
	
	if(GPIOPinRead(BTN2Port, BTN2)){
		num_stat();
	}
	
	// Menu Selection Startv -----------------------------
	if(poten <= 819){
		hygiene.icon = poop;
		hunger.icon = food_inv;
		sleepiness.icon = Sleep;
		love.icon = heart;
		
		LightLED(hunger.currentValue);

		if (lBtn1){
			LightLED(0);
			score = Runner_Game();
			hunger.currentValue += _min(score, 100 - hunger.currentValue);
			hunger.highScore = _max(score, hunger.highScore);
		}
	}
	else if(poten <= 1638){
		hygiene.icon = poop_inv;
		hunger.icon = food;
		sleepiness.icon = Sleep;
		love.icon = heart;
		
		LightLED(hygiene.currentValue);
		
		if (lBtn1){
			LightLED(0);
			score = Shit_Storm()
			hygiene.currentValue += _min(score, 100 - hygiene.currentValue);
			hygiene.highScore = _max(score, hygiene.highScore);
		}
		
	}
	else if(poten <= 2457){
		hygiene.icon = poop;
		hunger.icon = food;
		sleepiness.icon = Sleep;
		love.icon = heart;
		
		LightLED((hunger.currentValue + hygiene.currentValue + sleepiness.currentValue + love.currentValue)/4);
	}
	else if(poten <= 3276){
		hygiene.icon = poop;
		hunger.icon = food;
		sleepiness.icon = Sleep_inv;
		love.icon = heart;
		
		LightLED(sleepiness.currentValue);
	}
	else{
		hygiene.icon = poop;
		hunger.icon = food;
		sleepiness.icon = Sleep;
		love.icon = heart_inv;
		
		LightLED(love.currentValue);
	}
// modified this part but still needs testing 

	if (face_y > main_yMax) {
		face_y = main_yMax;
		velocity_y *= -1;
	}
    
	if (face_y < main_yMin) {
		face_y = main_yMin;
		velocity_y *= -1;
	}
    
	if (face_x > main_xMax) {
		face_x = main_xMax;
		velocity_x *= -1;
	}
    
	if (face_x < main_xMin) {
		face_x = main_xMin;
		velocity_x *= -1;
	}
	
	face_x += velocity_x;
	face_y += velocity_y;
	
    
	OrbitOledClear();
    
	OrbitOledMoveTo(0, 0);
	OrbitOledPutBmp(13, 16, hygiene.icon);
    
	OrbitOledMoveTo(0, yMax - 16);
	OrbitOledPutBmp(13, 16, hunger.icon);
    
	OrbitOledMoveTo(xMax - 13, 0);
	OrbitOledPutBmp(13, 16, sleepiness.icon);
    
	OrbitOledMoveTo(xMax - 13, yMax - 16);
	OrbitOledPutBmp(13, 16, love.icon);
    
	OrbitOledMoveTo(face_x, face_y);
	OrbitOledPutBmp(20, 20, faceHole);
        
	OrbitOledUpdate();
	delay(30);
  }
}





/* --------------------------------------*/
/* displays all the stats in numerical form*/
/* -------------------------------------------*/
void num_stat(){
	
	OrbitOledClear();
	
        while(GPIOPinRead(BTN2Port, BTN2)){}

	while(!GPIOPinRead(BTN2Port, BTN2)){
		if(GPIOPinRead(BTN1Port, BTN1)){
			
			OrbitOledSetCursor(0,0);
			OrbitOledPutString("Fod H Scr: ");
			OrbitOledPutNumber(hunger.highScore);
			OrbitOledPutString("  ");
		
			OrbitOledSetCursor(0, 1);
			OrbitOledPutString("Hyg H Scr: ");
			OrbitOledPutNumber(hygiene.highScore);
			OrbitOledPutString("  ");
		
		
			OrbitOledSetCursor(0 ,2);
			OrbitOledPutString("Slp H Scr: ");
			OrbitOledPutNumber(sleepiness.highScore);
			OrbitOledPutString("  ");
		
			OrbitOledSetCursor(0, 3);
			OrbitOledPutString("Lov H Scr: ");
			OrbitOledPutNumber(love.highScore);
			OrbitOledPutString("  ");
		}else{
		
			OrbitOledSetCursor(0,0);
			OrbitOledPutString("Fod Value: ");
			OrbitOledPutNumber(hunger.currentValue);
			OrbitOledPutString("  ");
			
			OrbitOledSetCursor(0, 1);
			OrbitOledPutString("Hyg Value: ");
			OrbitOledPutNumber(hygiene.currentValue);
			OrbitOledPutString("  ");
		
			OrbitOledSetCursor(0 ,2);
			OrbitOledPutString("Slp Value: ");
			OrbitOledPutNumber(sleepiness.currentValue);
			OrbitOledPutString("  ");
		
			OrbitOledSetCursor(0, 3);
			OrbitOledPutString("Lov Value: ");
			OrbitOledPutNumber(love.currentValue);
			OrbitOledPutString("  ");
		}
		OrbitOledUpdate();
	}
	
        while(GPIOPinRead(BTN2Port, BTN2)){}
}


/* --------------------------------------*/
/* Hygiene stat */
/* clean poop */
/* -------------------------------------------*/

void mini_game1(){

}
/* --------------------------------------*/
/* hunger stat */
/* collect food from dropping foods while avoiding stuff */
/* -------------------------------------------*/
int Runner_Game(void){
  //-------images-----------
  char obs[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  //char obs[] = {0x00, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x00};
  //char point[] = {0x00, 0x7E, 0x42, 0x42, 0x42, 0x42, 0x7E, 0x00}; //empty box
  char point[] = {0x3C, 0xCA, 0x8D, 0x8B, 0x8D, 0x8B, 0xCE, 0x3C};  //large food
  //char point[] = {0x00, 0x3C, 0x4E, 0x4A, 0x4A, 0x4E, 0x3C, 0x00};//small food
  char runner[] = {0xFF, 0x81, 0x89, 0xA1, 0xA1, 0xA1, 0x81, 0xFF};
  
  //------location-----------
  int obsX[5], obsY[5];
  int ptX[5], ptY[5];
  int plX = 8, plY = 0;
  
  //gameplay values
  int size = 8;
  int spacing = 10*size;
  
  //Time-keeping values
  int spd = 10;
  int framerate = 1000/60;
  int count = 0;
  
  //jump-stuff
  int jumpspd = 13;
  int jumptime = 0;
  int jumpheight = 0;
  //int jump = 0;
  
  int lose = 0;
  int score = 0;
  int pt_val = 1;
  
  //INTRODUCTION#############################################  
  OrbitOledClear();
  OrbitOledSetCursor(0, 0);
  OrbitOledPutString("Runner Game");
  OrbitOledSetCursor(0, 1);
  OrbitOledPutString("Avoid: ");
  OrbitOledSetCursor(0, 2);
  OrbitOledPutString("Collect:");
  OrbitOledSetCursor(0, 3);
  OrbitOledPutString("Jump w\\:");
  OrbitOledSetCursor(13, 3);
  OrbitOledPutString("BTN");
  
  OrbitOledMoveTo(xMax-size+1, size);
  OrbitOledPutBmp(size,size,obs);
  
  OrbitOledMoveTo(xMax-2*size+1, size*2);
  OrbitOledPutBmp(size,size,point);
  
  OrbitOledUpdate();
  
  
  do{
        lBtn1 = GPIOPinRead(BTN1Port, BTN1);
  }while(lBtn1 == BTN1 && BTN2 != GPIOPinRead(BTN2Port, BTN2));//while button is pressed
  do{
        lBtn1 = GPIOPinRead(BTN1Port, BTN1);
  }while(lBtn1 != BTN1 && BTN2 != GPIOPinRead(BTN2Port, BTN2));//while button is not pressed
  
  
  //MAIN GAME################################################
  //initializing obstacles
  for (int i = 0; i < 5; i++){
    obsX[i] = plX + size + size + spacing + (spacing+size)*i;
    obsY[i] = random((yMax/size))*size;
  }

  for (int i = 0; i < 5; i++){
    ptX[i] = plX + size + size + spacing/2 + (spacing+size)*i;
    ptY[i] = random((yMax/size))*size;
  }

  
  while (!lose){//main game loop
    if(!(count % framerate)){
      //draw everything
      OrbitOledClear();
      
      for(int i = 0; i < 5; i++){//drawing the obstacles
        OrbitOledMoveTo(obsX[i], yMax - obsY[i] - size);
        OrbitOledPutBmp(size,size,obs);
      }
      
      for(int i = 0; i < 5; i++){//drawing the pellets? do we still need these?
        OrbitOledMoveTo(ptX[i], yMax - ptY[i] - size);
        OrbitOledPutBmp(size,size,point);
      }
      
      OrbitOledMoveTo(plX, yMax-plY-size);
      OrbitOledPutBmp(size,size,runner);
      //putting the score
      OrbitOledSetCursor(6, 0);
      OrbitOledPutString("Score: ");
      OrbitOledPutNumber(score); 
      
      
      OrbitOledUpdate();
    }


    //collision checks here, because why not?
    for(int i = 0; i < 5; i++){
       //'<' used instead of '<=' used in some cases to give wiggle room; scritcly speaking should all be '<='
      if(((plY >= obsY[i] && plY < obsY[i]+size-1)||(plY+size-1 > obsY[i] && plY+size-1 <= obsY[i]+size-1))&&
          ((plX >= obsX[i] && plX < obsX[i]+size-1)||(plX+size-1 > obsX[i] && plX+size-1 <= obsX[i]+size-1))){
            lose = 1;
            break;
          }
    }

    for(int i = 0; i < 5; i++){
      // '<' used instead of '<=' used in some cases to give wiggle room; scritcly speaking should all be '<='
      if(((plY >= ptY[i] && plY < ptY[i]+size-1)||(plY+size-1 > ptY[i] && plY+size-1 <= ptY[i]+size-1))&&
          ((plX >= ptX[i] && plX < ptX[i]+size-1)||(plX+size-1 > ptX[i] && plX+size-1 <= ptX[i]+size-1))){
            score+= pt_val;
            ptX[i] = -1;
            break; 
          }
    }


    if(!(count % spd)){//movement
      for(int i = 0; i < 5; i++){//moving the obstacles
        obsX[i] -= 1;
        if (obsX[i] < 1){//resseting the obstacle to off the screen
          obsX[i] = (i == 0) ? obsX[4]+size+spacing-1 : obsX[i-1]+size+spacing;
          obsY[i] = random((yMax/size))*size;
        }
      }
      
      for(int i = 0; i < 5; i++){//moving the points
        ptX[i] -= 1;
        if (ptX[i] < 1){//resseting the obstacle to off the screen
          ptX[i] = (i == 0) ? ptX[4]+size+spacing-1 : ptX[i-1]+size+spacing;
          ptY[i] = random((yMax/size))*size;
        }
      }
      
    }
    
    //Jumping stuff
    if (plY == jumpheight){
      lBtn1 = GPIOPinRead(BTN1Port, BTN1);
      if(lBtn1 == BTN1) {//button is pressed
        if(jumpheight < (yMax/size)*size-size){
          jumpheight = plY + size;
        }else{
          jumpheight = plY - size;
        }
      }
      else{
        if(jumpheight > 0){
          jumpheight = plY - size;
        }
      }
        
    }else if(!((count - jumptime)%jumpspd)){//the player isn't at an acceptable height, and it's time to increment!!!!
      if (plY < jumpheight){
         plY++;
      }
      else{ //(plY >= jumpheight)
        plY--;
      }
    }
    
    /*
    //Jumping stuff Mk 1 (aka flying)
    lBtn1 = GPIOPinRead(BTN1Port, BTN1);
    if(lBtn1 == BTN1) {//button is pressed
      if (!jump){
        jump = 1;
        jumptime = count;
      }
    }else if(jump){
      jump = 0;
    }
    
    if(!((count - jumptime)%jumpspd)){
      if(jump){
        plY++;
        if (plY > yMax - size){
          plY = yMax - size;
        }
      }
      else if (plY > 0){
        plY --;
      }
    }
    */
    
    lBtn1 = GPIOPinRead(BTN2Port, BTN2);
    if(lBtn1 == BTN2){
      lose = 1;
    }
    count ++;
    delay(1);//should probably use a clock and measure time between frames, this is probably good enough
  }
  if (lBtn1 != BTN2){
    delay(500);
  }else{
    while(GPIOPinRead(BTN2Port, BTN2)){}
  }
  return score;
}

/* --------------------------------------*/
/* sleepiness stat */
/* simon says? */
/* -------------------------------------------*/
void mini_game3(){


}
/* --------------------------------------*/
/* lovestat */
/* petting. */
/* -------------------------------------------*/
void mini_game4(){

}

/*---------------------------------------------------------------*/
void LightLED(int n){
	
	if(n >= 25){
		GPIOPinWrite(LED4Port, LED4, LED4);
	}else{
		GPIOPinWrite(LED4Port, LED4, LOW);
	}
	
	if (n >= 50){
		GPIOPinWrite(LED3Port, LED3, LED3);
	}else{
		GPIOPinWrite(LED3Port, LED3, LOW);
	}
	
	if (n >= 75){
		GPIOPinWrite(LED2Port, LED2, LED2);
	}else{
		GPIOPinWrite(LED2Port, LED2, LOW);
	}
	
	if (n >= 100){
		GPIOPinWrite(LED1Port, LED1, LED1);
	}else{
		GPIOPinWrite(LED1Port, LED1, LOW);
	}
}



/* ------------------------------------------------------------ */

void OrbitOledPutNumber(int num){
  int length = 1;
  int tempnum = num/10;
  int i;
  
  char *strnum;
  
  while(tempnum > 0){
    tempnum = tempnum/10;
    length++;
  }
  
  strnum = (char*)malloc(sizeof(char)*length+1);
  
  i = 1;
  tempnum = num;
  strnum[length] = '\0';
  do{
    strnum[length-i] = '0' + (tempnum%10);
    i++;
    tempnum = tempnum / 10;
  }while(tempnum > 0);
  
  OrbitOledPutString(strnum);
  free(strnum);
  
}

/***    CheckSwitches()
 **
 **    Parameters:
 **   	 none
 **
 **    Return Value:
 **   	 none
 **
 **    Errors:
 **   	 none
 **
 **    Description:
 **   	 Return the state of the Switches
 */
char CheckSwitches() {

  long     lSwt1;
  long     lSwt2;

  chSwtPrev = chSwtCur;

  lSwt1 = GPIOPinRead(SWT1Port, SWT1);
  lSwt2 = GPIOPinRead(SWT2Port, SWT2);

  chSwtCur = (lSwt1 | lSwt2) >> 6;

  if(chSwtCur != chSwtPrev) {
	fClearOled = true;
  }

  return chSwtCur;

}



long getPoten(){
  //uint32_t ulAIN0;
  long unsigned int ulAIN0;
 
  ADCProcessorTrigger(ADC0_BASE, 0);
  while(!ADCIntStatus(ADC0_BASE, 0, false));
  ADCSequenceDataGet(ADC0_BASE, 0, &ulAIN0);
 
  return ulAIN0;
}

short getAccel(int Addr) {

  short    dataX;
 
  char printVal[10];
 
  char     chPwrCtlReg = 0x2D;
  char     chX0Addr = 0x32;
  char  chY0Addr = 0x34;
  char  chZ0Addr = 0x36;
 
  char     rgchReadAccl[] = {
	0, 0, 0        	};
  char     rgchWriteAccl[] = {
	0, 0        	};
 
	/*
 	* Enable I2C Peripheral
 	*/
	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
	SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);

	/*
 	* Set I2C GPIO pins
 	*/
	GPIOPinTypeI2C(I2CSDAPort, I2CSDA_PIN);
	GPIOPinTypeI2CSCL(I2CSCLPort, I2CSCL_PIN);
	GPIOPinConfigure(I2CSCL);
	GPIOPinConfigure(I2CSDA);

	/*
 	* Setup I2C
 	*/
	I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);

	/* Initialize the Accelerometer
 	*
 	*/
	GPIOPinTypeGPIOInput(ACCL_INT2Port, ACCL_INT2);

	rgchWriteAccl[0] = chPwrCtlReg;
	rgchWriteAccl[1] = 1 << 3;   	 // sets Accl in measurement mode
	I2CGenTransmit(rgchWriteAccl, 1, WRITE, ACCLADDR);
 
 
  /*
   * Loop and check for movement until switches
   * change
   */

	/*
 	* Read the data register
 	*/
	rgchReadAccl[0] = Addr;
    
	I2CGenTransmit(rgchReadAccl, 2, READ, ACCLADDR);
    
	dataX = (rgchReadAccl[2] << 8) | rgchReadAccl[1];
    
	return dataX;
    
}

/* ------------------------------------------------------------ */
/***    DeviceInit
 **
 **    Parameters:
 **   	 none
 **
 **    Return Value:
 **   	 none
 **
 **    Errors:
 **   	 none
 **
 **    Description:
 **   	 Initialize I2C Communication, and GPIO
 */
 
void DeviceInit()
{
  /*
   * First, Set Up the Clock.
   * Main OSC   	   -> SYSCTL_OSC_MAIN
   * Runs off 16MHz clock -> SYSCTL_XTAL_16MHZ
   * Use PLL   	   -> SYSCTL_USE_PLL
   * Divide by 4      -> SYSCTL_SYSDIV_4
   */
  SysCtlClockSet(SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ | SYSCTL_USE_PLL | SYSCTL_SYSDIV_4);

  /*
   * Enable and Power On All GPIO Ports
   */
  //SysCtlPeripheralEnable(    SYSCTL_PERIPH_GPIOA | SYSCTL_PERIPH_GPIOB | SYSCTL_PERIPH_GPIOC |
  //   					 SYSCTL_PERIPH_GPIOD | SYSCTL_PERIPH_GPIOE | SYSCTL_PERIPH_GPIOF);

  SysCtlPeripheralEnable(    SYSCTL_PERIPH_GPIOA );
  SysCtlPeripheralEnable(    SYSCTL_PERIPH_GPIOB );
  SysCtlPeripheralEnable(    SYSCTL_PERIPH_GPIOC );
  SysCtlPeripheralEnable(    SYSCTL_PERIPH_GPIOD );
  SysCtlPeripheralEnable(    SYSCTL_PERIPH_GPIOE );
  SysCtlPeripheralEnable(    SYSCTL_PERIPH_GPIOF );
  /*
   * Pad Configure.. Setting as per the Button Pullups on
   * the Launch pad (active low).. changing to pulldowns for Orbit
   */
  GPIOPadConfigSet(SWTPort, SWT1 | SWT2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);

  GPIOPadConfigSet(BTN1Port, BTN1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
  GPIOPadConfigSet(BTN2Port, BTN2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);

  GPIOPadConfigSet(LED1Port, LED1, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);
  GPIOPadConfigSet(LED2Port, LED2, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);
  GPIOPadConfigSet(LED3Port, LED3, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);
  GPIOPadConfigSet(LED4Port, LED4, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);

  /*
   * Initialize Switches as Input
   */
  GPIOPinTypeGPIOInput(SWTPort, SWT1 | SWT2);

  /*
   * Initialize Buttons as Input
   */
  GPIOPinTypeGPIOInput(BTN1Port, BTN1);
  GPIOPinTypeGPIOInput(BTN2Port, BTN2);

  /*
   * Initialize LEDs as Output
   */
  GPIOPinTypeGPIOOutput(LED1Port, LED1);
  GPIOPinTypeGPIOOutput(LED2Port, LED2);
  GPIOPinTypeGPIOOutput(LED3Port, LED3);
  GPIOPinTypeGPIOOutput(LED4Port, LED4);

  /*
   * Enable ADC Periph
   */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

  GPIOPinTypeADC(AINPort, AIN);

  /*
   * Enable ADC with this Sequence
   * 1. ADCSequenceConfigure()
   * 2. ADCSequenceStepConfigure()
   * 3. ADCSequenceEnable()
   * 4. ADCProcessorTrigger();
   * 5. Wait for sample sequence ADCIntStatus();
   * 6. Read From ADC
   */
  ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
  ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH0);
  ADCSequenceEnable(ADC0_BASE, 0);

  /*
   * Initialize the OLED
   */
  OrbitOledInit();

  /*
   * Reset flags
   */
  chSwtCur = 0;
  chSwtPrev = 0;
  fClearOled = true;

}

char I2CGenTransmit(char * pbData, int cSize, bool fRW, char bAddr) {

  int    	 i;
  char *    	 pbTemp;

  pbTemp = pbData;

  /*Start*/

  /*Send Address High Byte*/
  /* Send Write Block Cmd*/
  I2CMasterSlaveAddrSet(I2C0_BASE, bAddr, WRITE);
  I2CMasterDataPut(I2C0_BASE, *pbTemp);

  I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);

  DelayMs(1);

  /* Idle wait*/
  while(I2CGenIsNotIdle());

  /* Increment data pointer*/
  pbTemp++;

  /*Execute Read or Write*/

  if(fRW == READ) {

	/* Resend Start condition
    ** Then send new control byte
    ** then begin reading
    */
	I2CMasterSlaveAddrSet(I2C0_BASE, bAddr, READ);

	while(I2CMasterBusy(I2C0_BASE));

	/* Begin Reading*/
	for(i = 0; i < cSize; i++) {

  	if(cSize == i + 1 && cSize == 1) {
    	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

    	DelayMs(1);

    	while(I2CMasterBusy(I2C0_BASE));
  	}
  	else if(cSize == i + 1 && cSize > 1) {
    	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

    	DelayMs(1);

    	while(I2CMasterBusy(I2C0_BASE));
  	}
  	else if(i == 0) {
    	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);

    	DelayMs(1);

    	while(I2CMasterBusy(I2C0_BASE));

    	/* Idle wait*/
    	while(I2CGenIsNotIdle());
  	}
  	else {
    	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);

    	DelayMs(1);

    	while(I2CMasterBusy(I2C0_BASE));

    	/* Idle wait */
    	while(I2CGenIsNotIdle());
  	}

  	while(I2CMasterBusy(I2C0_BASE));

  	/* Read Data */
  	*pbTemp = (char)I2CMasterDataGet(I2C0_BASE);

  	pbTemp++;

	}

  }
  else if(fRW == WRITE) {

	/*Loop data bytes */
	for(i = 0; i < cSize; i++) {
  	/* Send Data */
  	I2CMasterDataPut(I2C0_BASE, *pbTemp);

  	while(I2CMasterBusy(I2C0_BASE));

  	if(i == cSize - 1) {
    	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);

    	DelayMs(1);

    	while(I2CMasterBusy(I2C0_BASE));
  	}
  	else {
    	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);

    	DelayMs(1);

    	while(I2CMasterBusy(I2C0_BASE));

    	/* Idle wait */
    	while(I2CGenIsNotIdle());
  	}

  	pbTemp++;
	}

  }

  /*Stop*/

  return 0x00;

}



bool I2CGenIsNotIdle() {

  return !I2CMasterBusBusy(I2C0_BASE);

}

int Shit_Storm(void){
  //----------constants-----
  const int mxp = 10;
  long poten;
  
  //-------images-----------
  //char obs[] = {0x00, 0x7E, 0x42, 0x42, 0x42, 0x42, 0x7E, 0x00}; //empty box
  char point[] = {0xC0, 0xE5, 0xF2, 0xF8, 0xFC, 0xF0, 0xE5, 0xC2};  //large food
  //char point[] = {0x00, 0x3C, 0x4E, 0x4A, 0x4A, 0x4E, 0x3C, 0x00};//small food
  char runner[] = {0xFF, 0x81, 0x89, 0xA1, 0xA1, 0xA1, 0x81, 0xFF};
  
  //------location-----------
  //int obsX[5], obsY[5];
  int ptX[mxp], ptY[mxp];
  int plX = 8, plY = 0;
  
  //gameplay values
  int size = 8;
  int spacing = size;
  int maxpoop = 3;
  
  //Time-keeping values
  int spd = 16;
  int framerate = 1000/60;
  int count = 0;
  
  int lose = 0;
  int score = 0;
  int pt_val = 1;
  
  //INTRODUCTION#############################################  
  OrbitOledClear();
  OrbitOledSetCursor(0, 0);
  OrbitOledPutString("Catching Game");
  OrbitOledSetCursor(0, 1);
  OrbitOledPutString("Collect:");
  OrbitOledSetCursor(0, 3);
  OrbitOledPutString("Move w\\:");
  OrbitOledSetCursor(12, 3);
  OrbitOledPutString("DIAL");
  
  OrbitOledMoveTo(xMax-2*size+1, size*2);
  OrbitOledPutBmp(size,size,point);
  
  OrbitOledUpdate();
  
  
  do{
        lBtn1 = GPIOPinRead(BTN1Port, BTN1);
  }while(lBtn1 == BTN1 && BTN2 != GPIOPinRead(BTN2Port, BTN2));//while button is pressed
  do{
        lBtn1 = GPIOPinRead(BTN1Port, BTN1);
  }while(lBtn1 != BTN1 && BTN2 != GPIOPinRead(BTN2Port, BTN2));//while button is not pressed
  
  
  //MAIN GAME################################################
  //initializing obstacles

  for (int i = 0; i < mxp; i++){
    ptY[i] = plY + size + size + (spacing+size)*i;
    ptX[i] = random((xMax/size))*size;
  }

  
  while (!lose){//main game loop
    if(!(count % framerate)){
      //draw everything
      OrbitOledClear();
      
      for(int i = 0; i < mxp; i++){//drawing the poop? do we still need these?
        OrbitOledMoveTo(ptX[i], yMax - ptY[i] - size + 1);
        OrbitOledPutBmp(size,size,point);
      }
      
      OrbitOledMoveTo(plX, yMax-plY-size + 1);
      OrbitOledPutBmp(size,size,runner);
      //putting the score
      OrbitOledSetCursor(6, 0);
      OrbitOledPutString("Score: ");
      OrbitOledPutNumber(score); 
      
      
      OrbitOledUpdate();
    }


    //collision checks here, because why not
    for(int i = 0; i < mxp; i++){
      // '<' used instead of '<=' used in some cases to give wiggle room; scritcly speaking should all be '<='
      //can only collect when they are on the ground, adds some challenge
            if(((plY >= ptY[i] && plY < ptY[i]+size-1)||(plY+size-1 > ptY[i] && plY+size-1 <= ptY[i]+size-1))&&
          ((plX >= ptX[i] && plX < ptX[i]+size-1)||(plX+size-1 > ptX[i] && plX+size-1 <= ptX[i]+size-1))&&
          ptY[i] < size/2){
            score+= pt_val;
            ptY[i] = -size;
          }
    }

    //poop falling movement
    if(!(count % spd)){ 
      for(int i = 0; i < mxp; i++){//moving the points
        if (ptY[i] > 0){
          ptY[i] -= 1;
        }else if (ptY[i] < 0){
          ptX[i] = random((xMax/size))*size;
          //find the highest poop, and place above it
          int highest = yMax;
          for (int j= 0; j < mxp; j++){
            if (ptY[j] > highest){
              highest = ptY[j];
            }
          }
          ptY[i] = highest + size + spacing; 
        }
      }
    }
    
    //moving the player
    poten = getPoten();
    plX = (int) (((double)poten / 4095)*(xMax-size));
    
    lBtn1 = GPIOPinRead(BTN2Port, BTN2);
    if(lBtn1 == BTN2){
      lose = 1;
    }
    count ++;
    delay(1);//should probably use a clock and measure time between frames, this is probably good enough
  }
  if (lBtn1 != BTN2){
    delay(500);
  }else{
    while(GPIOPinRead(BTN2Port, BTN2)){}
  }
  return score;
}
