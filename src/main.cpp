#include <Arduino.h>
#include <math.h>

// Each section of robotic arm in cm
#define a 14.75
#define b 18.5
#define d 11.5
#define h 9.5

// Servo numbers
#define base 0 // theta
#define shoulder 1 // alpha
#define elbow 2 // beta
#define wrist 3 // gamma
#define claw 4 // 500: open, 2500: closed
#define twistClaw 5

#define movingTime 2500
#define resetPosition 1500

// In order of movement
int armParts[] = {base, wrist, elbow, shoulder, claw, twistClaw};
// Letters on the chess board
char letters[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
// Piece names
String pieceNames[] = {"king", "queen", "rook", "bishop", "knight", "pawn"};
// Piece heights (cm)
float pieceHeights[] = {9.5, 7.5, 4.5, 6.5, 5.75, 4.5};

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

  float f = d + T - 3 - h;

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

// Find the I vaule corresponding to the letter
int findCoord(char x_i) {
  for (int i = 0; i < 8; i++) {
    if (letters[i] == x_i) {
      return i + 1; // Coordinate
    }
  }
}
// If I put this inside the setup(): T is not declared in this scope?
float findPieceHeight(String piece) {
  for (int i = 0; i < 6; i++) {
    if (pieceNames[i] == piece) {
      return pieceHeights[i];
    }
  }
}

struct coordinate {
  int x_i;
  int y_j;
};

struct coordinate get_i_j(String position) { // position "first" or "last"
  struct coordinate coordInstance;

  String coord = WaitForInput("Enter " + position + " coordinate: ");
  char i = coord.charAt(0); // First character is a letter
  char j = coord.charAt(1); // Next a number

  // Converting character into integer
  coordInstance.y_j = String(j).toInt();

  // Converting letter into integer
  coordInstance.x_i = findCoord(i);

  Serial.print(position + " coordinate: ");
  Serial.println(coord);

  return coordInstance;


}

void setup() {
  // set the speed for the serial monitor:
  Serial.begin(9600);
  Serial.println("Your Arduino is awake ...");

  struct coordinate firstCoord = get_i_j("first");
  struct coordinate lastCoord = get_i_j("last");

  // The angles depend on how tall the piece is (e.g. a pawn vs a queen)
  String piece = WaitForInput("Enter piece: ");
  float T = findPieceHeight(piece);
  Serial.print("Height of piece: ");
  Serial.println(T);

  struct angles start = calculate(firstCoord.x_i, lastCoord.y_j, T);
  struct angles end = calculate(lastCoord.x_i, lastCoord.y_j, T);

  // Extra space for claw
  int startAngles[4] = {start.theta, start.gamma, start.beta, start.alpha};
  int endAngles[4] = {end.theta, end.gamma, end.beta, end.alpha};

  // Resetting everything
  for(int servos : armParts) {
    move(servos, resetPosition, 5000);
  }

  // picking up
  move(claw, 500, 5000); // Open claw
  for (int i = 0; i < 5; i++) {
    startAngles[4] = 1850; // claw value (not completely closed)
    move(armParts[i], startAngles[i], movingTime);
  }

  // setting up for placement
  move(shoulder, resetPosition, movingTime);

// placement
  for (int i = 0; i < 5; i++) {
    endAngles[4] = 500;
    move(armParts[i], endAngles[i], movingTime);
  }

  for (int i = 3; i >= 0; i--){
    move(armParts[i], resetPosition, 5000);
  }
}

void loop() {
  // leave empty
}
