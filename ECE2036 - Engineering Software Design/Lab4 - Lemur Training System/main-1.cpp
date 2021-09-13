#include "mbed.h"
#include "uLCD_4DGL.h"
#include "Speaker.h"

uLCD_4DGL uLCD(p9, p10, p11); //create a global lcd object
DigitalIn pbLeft(p21);      //create a global left button object
DigitalIn pbRight(p22);     //create a global right button object
DigitalIn pbExit(p23);

Speaker mySpeaker(p24);     //instantiate speaker object for startup tune
//populate left bin with random objects
int populateLeft(int count) //argument here counts amount of objects that were created
{
    int trueFalse;      //create or don't create
    
    int randRadius;     //random radius
    int color;          //random color
    
    int x;
    int y;
    int j = 0;
    
    //loop that runs through entire bin and decides whether or not random circles appear
    for (int i = 0; i < 18; i++)
    {
        trueFalse = rand()%2;
        x = ((i%3)+1)*20-10;            
        y = (j+1)*20-10;
        
        
        randRadius = rand()%10+1;
        
        color = rand()%4;
        switch (color)
        {
            case 0:
            color = 0XFFFFFF;
            break;
            case 1:
            color = 0XFF0000;
            break;
            case 2:
            color = 0X00FF00;
            break;
            case 3:
            color = 0X0000FF;
            break;
        }
        
        if (trueFalse == 1)
        {
            uLCD.filled_circle(x, y, randRadius, color);
            count++;    //if circle is made, count increment
        }
        
        if (i%3 == 2)
        {
            j++;            //semi-array downward
        }   
    }
    return count;
}

//functionality mirrors that of populateLeft
int populateRight(int count)
{
    int trueFalse;
    
    int randRadius;
    int color;
    
    int x;
    int y;
    int j = 0;
    
    for (int i = 0; i < 18; i++)
    {
        x = ((i%3)+1)*20+50;
        y = (j+1)*20-10;
        
        trueFalse = rand()%2;
        randRadius = rand()%10+1;
        
        color = rand()%4;
        switch (color)
        {
            case 0:
            color = 0XFFFFFF;
            break;
            case 1:
            color = 0XFF0000;
            break;
            case 2:
            color = 0X00FF00;
            break;
            case 3:
            color = 0X0000FF;
            break;
        }
        
        if (trueFalse == 1)
        {
            uLCD.filled_circle(x, y, randRadius, color);
            count++;
        }
        
        if (i%3 == 2)
        {
            j++;
        }   
    }
    return count;
}

//function for displaying the correct answer when the corresponding pin is pushed
void displayCorrect(int inTrial, int inNumCorrect, float inTime)
{
    uLCD.printf("Correct!\n");
    uLCD.printf("%s%d%s", "Trial Number: ", inTrial, "\n");
    uLCD.printf("%s%d%s%d%s", "Fraction: ", inNumCorrect, "/", inTrial, "\n");
    uLCD.printf("%s%f%s", "Time(s): ", inTime, "\n");
    uLCD.printf("\n\n\n\n\nHold bottom button to exit.\n"); //display all relevant data and give user instructions on how to end program
}

//identical with displayCorrect function aside from the first line
void displayIncorrect(int inTrial, int inNumCorrect, float inTime)
{
    uLCD.printf("Incorrect.\n");
    uLCD.printf("%s%d%s", "Trial Number: ", inTrial, "\n");
    uLCD.printf("%s%d%s%d%s", "Fraction: ", inNumCorrect, "/", inTrial, "\n");
    uLCD.printf("%s%f%s", "Time(s): ", inTime, "\n");
    uLCD.printf("\n\n\n\n\nHold bottom \nbutton to exit.\n");
}

int main() {
    
    uLCD.printf("\nWelcome Lemurs!\n");         //prompt user
    mySpeaker.PlayNote(200, 0.25, 0.05);
    mySpeaker.PlayNote(250, 0.25, 0.05);
    mySpeaker.PlayNote(300, 0.25, 0.05);
    mySpeaker.PlayNote(350, 0.25, 0.05);
    mySpeaker.PlayNote(300, 0.25, 0.05);
    mySpeaker.PlayNote(250, 0.25, 0.05);
    mySpeaker.PlayNote(200, 0.25, 0.05);        //tunes
    wait(2);
    uLCD.cls();                 //clear screen before white bin outlines are created
    wait(1);
    uLCD.rectangle(0, 0, 59, 119, 0XFFFFFF);
    uLCD.rectangle(60, 0, 119, 119, 0XFFFFFF);
    
    srand(time(0));
    
    pbExit.mode(PullUp);
    int trial = 1;
    int numCorrect = 0;
    
    bool cont = true;
    
    Timer t;
    
    while (cont)
    {
    uLCD.rectangle(0, 0, 59, 119, 0XFFFFFF);
    uLCD.rectangle(60, 0, 119, 119, 0XFFFFFF);
    int countLeft = 0;
    int countRight = 0;
    
    countLeft = populateLeft(countLeft);        //check left and right counts in order to reset if the amount of objects 
    countRight = populateRight(countRight);     //are equal
    
    while (countLeft == countRight)
    {
        countLeft = 0;
        countRight = 0;
        uLCD.cls();
        uLCD.rectangle(0, 0, 59, 119, 0XFFFFFF);
        uLCD.rectangle(60, 0, 119, 119, 0XFFFFFF);
        countLeft = populateLeft(countLeft);
        countRight = populateRight(countRight);
    }
    
    t.start();
    
    int answer;
    
    if (countLeft < countRight)
    {
        answer = 0;
    }
    else if (countRight < countLeft)
    {
        answer = 1;
    }
    
    pbLeft.mode(PullUp);            //pre-set DigitalIn buttons to pull up mode
    pbRight.mode(PullUp);
    wait(0.1);
    
    bool next = false;
    
    while (!next)
    {
        if (!pbLeft)
        {
            t.stop();
            uLCD.cls();
            next = true;
            if (answer == 0)
            {
                numCorrect++;
                displayCorrect(trial, numCorrect, t.read());            //four possibilities for button input and which was correct
                wait(3);
                if (!pbExit)
                {
                    cont = false;  
                }
                uLCD.cls();
            }
            else
            {
                displayIncorrect(trial, numCorrect, t.read());
                wait(3);
                if (!pbExit)
                {
                    cont = false;  
                }
                uLCD.cls();
            }
        }   
        if (!pbRight)
        {
            t.stop();
            uLCD.cls();
            next = true;
            if (answer == 1)
            {
                numCorrect++;
                displayCorrect(trial, numCorrect, t.read());
                wait(3);
                if (!pbExit)
                {
                    cont = false;   
                }
                uLCD.cls();
            }
            else
            {
                displayIncorrect(trial, numCorrect, t.read());
                wait(3);
                if (!pbExit)
                {
                    cont = false;  
                }
                uLCD.cls();
            }
        }
    }
    trial++;        //trial increment
    t.reset();      //reset timer
    }
    uLCD.printf("Goodbye.\n");  //exit program
}
