/*
 * 
 * Demo code for BerlinClock replica
 * 
 * code For Ardruino Nano hocked up to costum air wired LED Matrix
 * 
 * more information here:
 * 
 * https://hackaday.io/project/171360-berlinclock
 * 
 * License: CC-BY-NC-SA 4.0
 * 
 * Use on own risk!
 */



volatile uint8_t row_index = 0;
const uint8_t row_index_MAX = 4;
const uint8_t rows[row_index_MAX + 1] = {8, 9, 10, 11, 12};
volatile uint8_t col_index = 0;
const uint8_t col_index_MAX = 7;
const uint8_t cols[col_index_MAX + 8] = {2, 3, 4, 5, 6, 7, 17, 16};

uint8_t test_hours = 0;
uint8_t test_mins = 0;

static volatile uint8_t colMasks[5] = {0x8F, 0xCF, 0xAA, 0xAA, 0xff};

void setup()
{
  int i;

  // setup Timer
  TCCR2A = (1 << WGM21); //Set the CTC mode
  OCR2A = 0xFF; //Value for ORC0A
  TIMSK2 |= (1 << OCIE2A); //Set the interrupt request
  TCCR2B = (1 << CS22);

  //setup IO
  for (i = 0; i <= row_index_MAX; i++)
    pinMode(rows[i], OUTPUT);
  for (i = 0; i <= col_index_MAX; i++)
  {
    pinMode(cols[i], OUTPUT);
    digitalWrite(cols[i], HIGH);
  }
  pinMode(13, OUTPUT);

  sei(); //Enable interrupt
}

void loop()
{
  showHours(test_hours);
  showMinutes(test_mins++);
  digitalWrite(13, test_mins & 1);
  if (test_mins >= 60)
  {
    test_mins = 0;
    test_hours++;
    if (test_hours > 24) test_hours = 0;
  }
  delay(500);
}

static void displaySetCols(uint8_t  colMask)
{
  PORTD |= (0xFC);    // PD2...PD7  -> High
  PORTC |= (0x0C);    // PC3/PC2    -> High
  if (colMask & 0x01) PORTD &= ~(1 << 2); //digitalWrite(cols[0], LOW);   // sitch col on
  if (colMask & 0x02) PORTD &= ~(1 << 3); //digitalWrite(cols[1], LOW);   // sitch col on
  if (colMask & 0x04) PORTD &= ~(1 << 4); //digitalWrite(cols[2], LOW);   // sitch col on
  if (colMask & 0x08) PORTD &= ~(1 << 5); //digitalWrite(cols[3], LOW);   // sitch col on
  if (colMask & 0x10) PORTD &= ~(1 << 6); //digitalWrite(cols[4], LOW);   // sitch col on
  if (colMask & 0x20) PORTD &= ~(1 << 7); //digitalWrite(cols[5], LOW);   // sitch col on
  if (colMask & 0x40) PORTC &= ~(1 << 3); //digitalWrite(cols[6], LOW);   // sitch col on
  if (colMask & 0x80) PORTC &= ~(1 << 2); //digitalWrite(cols[7], LOW);   // sitch col on
}


ISR(TIMER2_COMPA_vect)   //This is the interrupt request
{
  //digitalWrite(rows[row_index], LOW);
  PORTB &= ~(0x1F);   // all rows off;

  displaySetCols(colMasks[row_index]);
  digitalWrite(rows[row_index], HIGH);

  row_index++;
  if (row_index > row_index_MAX)
    row_index = 0;
}

static void showHours(uint8_t hours)
{
  uint8_t colMask0Shadow = 0;
  uint8_t colMask1Shadow = 0;

  // first row with 5h digits
  if (hours >= 5)    colMask0Shadow |= 0xC0; // show first 5h digit
  if (hours >= 10)   colMask0Shadow |= 0x30; // show second 5h digit
  if (hours >= 15)   colMask0Shadow |= 0x0C; // show third 5h digit
  if (hours >= 20)   colMask0Shadow |= 0x03; // show fourth 5h digit
  hours %= 5;

  // second row with 1h digits
  if (hours >= 1)    colMask1Shadow |= 0xC0; // show first 1h digit
  if (hours >= 2)    colMask1Shadow |= 0x30; // show second 1h digit
  if (hours >= 3)    colMask1Shadow |= 0x0C; // show third 1h digit
  if (hours >= 4)    colMask1Shadow |= 0x03; // show fourth 1h digit

  colMasks[0] = colMask0Shadow;
  colMasks[1] = colMask1Shadow;
}


static void showMinutes(uint8_t minutes)
{
  uint8_t colMask2Shadow = 0;
  uint8_t colMask3Shadow = 0;
  uint8_t colMask4Shadow = 0;

  // first row with 5min digits
  if (minutes >= 5)   colMask3Shadow |= 0x04;
  if (minutes >= 10)  colMask3Shadow |= 0x08;
  if (minutes >= 15)  colMask3Shadow |= 0x80;
  if (minutes >= 20)  colMask3Shadow |= 0x40;
  if (minutes >= 25)  colMask3Shadow |= 0x20;
  if (minutes >= 30)  colMask3Shadow |= 0x10;
  if (minutes >= 35)  colMask2Shadow |= 0x10;
  if (minutes >= 40)  colMask2Shadow |= 0x08;
  if (minutes >= 45)  colMask2Shadow |= 0x04;
  if (minutes >= 50)  colMask2Shadow |= 0x02;
  if (minutes >= 55)  colMask2Shadow |= 0x01;

  minutes %= 5;
  if (minutes >= 1)   colMask4Shadow |= 0xC0;
  if (minutes >= 2)   colMask4Shadow |= 0x30;
  if (minutes >= 3)   colMask4Shadow |= 0x0C;
  if (minutes >= 4)   colMask4Shadow |= 0x03;


  colMasks[2] = colMask2Shadow;
  colMasks[3] = colMask3Shadow;
  colMasks[4] = colMask4Shadow;
}
