# Robotic arm that can move chess pieces across a board
Given the chess piece you want to move (due to the size of different chess pieces) and the starting square of the chess piece in a chess board, the arm can lift the chess piece to another desired chess square.
Used an Arduino Uno to transmit TTL serial data to the servo controller board, a SSC-32U Lynxmotion.
Model of robotic arm: Lynxmotion AL5D robotic arm

Challenging project. Needed to learn a bit of Arduino cpp. Lots of precise diagrams and angles and using trigonomtery to find the exact angles the arm needed to move. Problems arose, like the arm would move too low and not pick up the piece. Another problem was the fact that the trigonometry was wrong, or too many numbers to compute; values were rounded off prematurely and the final angles were off. The code itself is a bit rough but it works.

Next step: Attach a camera, make a deep learning chess bot, beat myself at chess. 
