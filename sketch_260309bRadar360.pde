/*   Arduino Radar Project
 *
 *   Updated version. Fits any screen resolution!
 *   Just change the values in the size() function,
 *   with your screen resolution.
 *      
 *  By Dejan Nedelkovski, Modified By Joao Matos
 *  Dejan Nedelkovski's Website: www.HowToMechatronics.com
 *  
 */
import processing.serial.*; // imports library for serial communication
import java.awt.event.KeyEvent; // imports library for reading the data from the serial port
import java.io.IOException;

Serial myPort; // defines Object Serial
// variables
String angle="";
String distance="";
String data="";
String noObject;
float pixsDistance;
int iAngle, iAngle1, iAngle2, iDistance;
int index1=0;
int index2=0;
PFont orcFont;
boolean James = false;
int Data1,Data2,Data3;
int LoopNumber = 200;
boolean Data1Bol = true;
boolean Data2Bol = false;
boolean Data3Bol = false;

void setup() {
  
 size (1480, 1100); // ***CHANGE THIS TO YOUR SCREEN RESOLUTION***
 //smooth();
 myPort = new Serial(this, Serial.list()[0], 115200); // starts the serial communication
 ////////////////////////////////////////↑///////Change This Number To Correct COM PORT FOR UART
}

void draw() {
  
  while (myPort.available() > 0) {
    
    int inByte = myPort.read();
    
    if (Data1Bol == true){
    iAngle2 = inByte;
    Data1Bol = false;
    Data2Bol = true;
    }
    else if(Data2Bol == true){
    iAngle1 = inByte;
    Data2Bol = false;
    Data3Bol = true;
    }
    else if(Data3Bol == true){
    iDistance = inByte;
    Data3Bol = false;
    Data1Bol = true;
    }
  }
 println("Distance: " + iDistance);
 println("Angle One: " + iAngle1);
 println("Angle Two: " + iAngle2);

  if (iAngle1 == 255)
    iAngle1 = 0;
  else if(iAngle2 == 255)
    iAngle2 = 0;
 
 iAngle = iAngle1 + iAngle2;
 
  fill(98,245,31);
  // textFont(orcFont);
  // simulating motion blur and slow fade of the moving line
  noStroke();
  fill(0,4); 
  rect(0, 0, width, height); 
  
  fill(220,245,31); // green color
  // calls the functions for drawing the radar
  drawRadar(); 
  drawLine();
  drawObject();
  drawText();
}

void drawRadar() {
  pushMatrix();
  translate(width/2,height/2); // moves the starting coordinats to new location
  noFill();
  strokeWeight(2);
  stroke(#14D4D4);
  // draws the arc lines
  arc(0,0,(width-width*0.0625)*0.75,(width-width*0.0625)*0.75,0,TWO_PI);
  arc(0,0,(width-width*0.27)*0.75,(width-width*0.27)*0.75,0,TWO_PI);
  arc(0,0,(width-width*0.479)*0.75,(width-width*0.479)*0.75,0,TWO_PI);
  arc(0,0,(width-width*0.687)*0.75,(width-width*0.687)*0.75,0,TWO_PI);
  // draws the angle lines
  line((-width/2)*0.7,0,(width/2)*0.7,0);
  line(0,0,(-width/2)*cos(radians(30))*0.7,(-width/2)*sin(radians(30))*0.7);
  line(0,0,(-width/2)*cos(radians(60))*0.7,(-width/2)*sin(radians(60))*0.7);
  line(0,0,(-width/2)*cos(radians(90))*0.7,(-width/2)*sin(radians(90))*0.7);
  line(0,0,(-width/2)*cos(radians(120))*0.7,(-width/2)*sin(radians(120))*0.7);
  line(0,0,(-width/2)*cos(radians(150))*0.7,(-width/2)*sin(radians(150))*0.7);
  //line(0,0,(-width/2)*cos(radians(150)),(-width/2)*sin(radians(180)));
  line(0,0,(-width/2)*cos(radians(210))*0.7,(-width/2)*sin(radians(210))*0.7);
  line(0,0,(-width/2)*cos(radians(240))*0.7,(-width/2)*sin(radians(240))*0.7);
  line(0,0,(-width/2)*cos(radians(270))*0.7,(-width/2)*sin(radians(270))*0.7);
  line(0,0,(-width/2)*cos(radians(300))*0.7,(-width/2)*sin(radians(300))*0.7);
  line(0,0,(-width/2)*cos(radians(330))*0.7,(-width/2)*sin(radians(330))*0.7);
  //line((-width/2)*cos(radians(30)),0,width/2,0);
  popMatrix();
}
void drawObject() {
  pushMatrix();
  translate(width/2,height/2); // moves the starting coordinats to new location
  strokeWeight(9);
  stroke(#E0107B); // red color
  pixsDistance = iDistance*((height-height*0.1666)*0.025)*0.7; // covers the distance from the sensor from cm to pixels
  // limiting the range to 40 cms
  if(iDistance<40){
    // draws the object according to the angle and the distance
  line(pixsDistance*cos(radians(iAngle)),-pixsDistance*sin(radians(iAngle)),(width-width*0.505)*cos(radians(iAngle))*0.7,-(width-width*0.505)*sin(radians(iAngle))*0.7);
  }
  popMatrix();
}
void drawLine() {
  pushMatrix();
  strokeWeight(9);
  stroke(#10E075);
  translate(width/2,height/2); // moves the starting coordinats to new location
  line(0,0,(width-width*0.505)*cos(radians(iAngle))*0.7,-(width-width*0.505)*sin(radians(iAngle))*0.7); // draws the line according to the angle
  popMatrix();
}
void drawText() { // draws the texts on the screen
  
  pushMatrix();
  if(iDistance>40) {
  noObject = "Out of Range";
  }
  else {
  noObject = "In Range";
  }
  fill(#1F1F1F); 
  noStroke();
  rect(0, 0, width-width*0.8, height-height*0.8);
  fill(#10E075);
  textSize(25);
  
  text("10cm",width-width*0.4554,height-height*0.505);
  text("20cm",width-width*0.36,height-height*0.505);
  text("30cm",width-width*0.281,height-height*0.505);
  text("40cm",width-width*0.2029,height-height*0.505);
  
  
  textSize(40);
  //text("Object: " + noObject, width-width*0.98, height-height*0.85);
  text("Angle: " + iAngle +" °", width-width*0.98, height-height*0.90);
  text("Distance: ", width-width*0.98, height-height*0.9477);
  if(iDistance<40) {
  text("               " + iDistance +" cm", width-width*0.95, height-height*0.9477);
  }
  
  
  textSize(25);
  fill(#10E075);
  translate((width-width*0.19),(height-height*0.7507));
  rotate(-radians(-60));
  text("30°",0,0);
  resetMatrix();
  translate((width-width*0.32),(height-height*0.92));
  rotate(-radians(-30));
  text("60°",0,0);
  resetMatrix();
  translate((width-width*0.51),(height-height*0.98));
  rotate(radians(0));
  text("90°",0,0);
  resetMatrix();
  translate((width-width*0.69),(height-height*0.914));
  rotate(radians(-30));
  text("120°",0,0);
  resetMatrix();
  translate((width-width*0.82),(height-height*0.73));
  rotate(radians(-60));
  text("150°",0,0);
  resetMatrix();
  translate((width-width*0.89),(height-height*0.49));
  text("180°",0,0);
  resetMatrix();
  translate((width-width*0.83),(height-height*0.264));
  rotate(radians(60));
  text("210°",0,0);
  resetMatrix();
  translate((width-width*0.70),(height-height*0.078));
  rotate(radians(30));
  text("240°",0,0);
  resetMatrix();
  translate((width-width*0.54),(height-height*0.04));
  text("270°",0,0);
  resetMatrix();
  translate((width-width*0.325),(height-height*0.058));
  rotate(radians(-30));
  text("300°",0,0);
  resetMatrix();
  translate((width-width*0.185),(height-height*0.234));
  rotate(radians(-60));
  text("330°",0,0);
  resetMatrix();
  popMatrix(); 
}
