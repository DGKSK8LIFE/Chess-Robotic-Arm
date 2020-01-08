#include <Arduino.h>
#include <math.h>

// Each section of robotic arm
#define a 14.75
#define b 18.5
#define d 11.5
#define h 9.5

#define base 0 // theta
#define shoulder 1 // alpha
#define elbow 2 // beta
#define wrist 3 // gamma
#define claw 4 // 500: open, 2500: closed
#define twistClaw 5

// Regular angles converted to "Servo Degrees" (500-2500)
int convertServo(float angle) {
  return (((angle * 2000) / 180) + 500);
}

// From arduino write to the SSC-32U servo controller
void move(int servo, int position, int time) {
   Serial.print("#");
   Serial.print(servo); // servo pin in SSC-32
   Serial.print(" P");
   Serial.print(position); // position of servo, 500 is left and 2500 is right
   Serial.print(" T");
   Serial.println(time); // time in milliseconds
   delay(time);
}

// Wait for user input
String WaitForInput(String Question) {
  Serial.println(Question);

  while(!Serial.available()) {
    // wait for input
  }
  // press enter
  return Serial.readStringUntil(10);
}

// Cannot return more than one angle (eg. return alpha, beta, gamma, theta;)
// need to make this struct
struct angles {
  int alpha;
  int beta;
  int gamma;
  int theta;
};

/* trigonometry done by hand first. Required some ammount of calculation because
the robotic arm doesn't lie on a 2D plane. The base needs to move in order to
reach different parts of the chess board.
*/
struct angles calculate(int i, int j, float T) {
  struct angles angleInstance;

  angleInstance.theta = convertServo(90 - degrees(atan((4.5 - i) / (0.5 + j))));

  float l = sqrt(sq(4.5 - i) + sq(0.5 + j)) * 5.75;

  float w = T - 3;

  float f = d + w - h;

  float c = sqrt(sq(f) + sq(l));

  float gamma_2 = degrees(atan(l / f));

  float alpha_1 = degrees(atan(f / l));

  float gamma_3 = degrees(acos((sq(c) + sq(b) - sq(a)) / (2 * b * c)));

  float alpha_2 = degrees(acos((sq(a) + sq(c) - sq(b)) / (2 * a * c)));

  angleInstance.gamma = convertServo(gamma_2 + gamma_3);

  angleInstance.beta = convertServo(gamma_3 + alpha_2);

  angleInstance.alpha = convertServo(alpha_1 + alpha_2);

  return angleInstance;
}

// Find the i vaule corresponding to the letter
// TODO: make it cleaner
int findCoord(char x_i){
  int i;
  switch(x_i){
    case 'a':
      return i = 1;
    case 'b':
      return i = 2;
    case 'c':
      return i = 3;
    case 'd':
      return i = 4;
    case 'e':
      return i = 5;
    case 'f':
      return i = 6;
    case 'g':
      return i = 7;
    case 'h':
      return i = 8;
  }
}

void setup() {
  // set the speed for the serial monitor:
  Serial.begin(9600);
  Serial.println("Your Arduino is awake ...");

  String firstCoord = WaitForInput("Enter first coordinate: ");
  char start_x_i = firstCoord.charAt(0); // First character is a letter
  char start_y_j = firstCoord.charAt(1); // Next a number

  // Converting character into integer
  String _start_j = String(start_y_j);
  int start_j = _start_j.toInt();

  // Converting letter into integer
  int start_i = findCoord(start_x_i);

  Serial.print("First coordinate: ");
  Serial.println(firstCoord);

  // Same process with second coordinate
  String lastCoord = WaitForInput("Enter last coordinate: ");
  char end_x_i = lastCoord.charAt(0);
  char end_y_j = lastCoord.charAt(1);

  String _end_j = String(end_y_j);
  int end_j = _end_j.toInt();

  int end_i = findCoord(end_x_i);

  Serial.print("Last coordinate: ");
  Serial.println(lastCoord);

  // The angles depend on how tall the piece is (e.g. a pawn vs a queen)
  String _T = WaitForInput("Enter piece height: ");
  float T = _T.toFloat();

  Serial.print("Height: ");
  Serial.println(T);

  struct angles start = calculate(start_i, start_j, T);
  struct angles end = calculate(end_i, end_j, T);

  Serial.print("start theta/base: ");
  Serial.println(start.theta);
  Serial.print("start gamma/wrist: ");
  Serial.println(start.gamma);
  Serial.print("start beta/elbow: ");
  Serial.println(start.beta);
  Serial.print("start alpha/shoulder: ");
  Serial.println(start.alpha);

  Serial.print("end theta/base: ");
  Serial.println(end.theta);
  Serial.print("end gamma/wrist: ");
  Serial.println(end.gamma);
  Serial.print("end beta/elbow: ");
  Serial.println(end.beta);
  Serial.print("end alpha/shoulder: ");
  Serial.println(end.alpha);

  // Resetting
  for(int servos = 0; servos <= 5; servos++) {
    move(servos, 1500, 5000);
  }

  // picking up
  move(claw, 500, 5000);
  move(base, start.theta, 2500);
  move(wrist, start.gamma, 2500);
  move(elbow, start.beta, 2500);
  move(shoulder, start.alpha, 2500);
  move(claw, 1850, 2500);

  // setting up for placement
  move(shoulder, 1500, 2500);
  move(twistClaw, 500, 500);
  move(twistClaw, 2500, 500);
  move(twistClaw, 1500, 1500);

  // placement
  move(base, end.theta, 2500);
  move(wrist, end.gamma, 2500);
  move(elbow, end.beta, 2500);
  move(shoulder, end.alpha, 2500);
  move(claw, 500, 2500);

  for (int servos = 3; servos >= 0; servos--){
    move(servos, 1500, 5000);
  }
}


void loop() {
  // leave empty
}
