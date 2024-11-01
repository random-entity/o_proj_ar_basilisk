import processing.serial.*;

Serial xbPort;
XbRR rr;

void runXbRR() {
  while (xbPort.available() > 0) {
    xbPort.read();
  }
  while (true) {
    rr.run();
  }
}

// ALL lengths in cm
final float stageDimX = 880, stageDimY = 550;
final float lpsBoundMinX = 50, lpsBoundMaxX = 1010, lpsBoundMinY = -110, lpsBoundMaxY = 430;
float marginX, marginY;

ArrayList<Basilisk> bs;

void setup() {
  String[] ports = Serial.list();
  printArray(ports);
  xbPort = new Serial(this, "/dev/ttyUSB1", 115200);
  rr = new XbRR(xbPort);
  thread("runXbRR");

  size(1300, 1000);
  frameRate(10);

  marginX = (width - stageDimX) / 2;
  marginY = (height - stageDimY) / 2;

  bs = new ArrayList<Basilisk>();
  for (int suid = 1; suid <= 13; suid++) {
    bs.add(new Basilisk(suid));
  }
}

void draw() {
  for (int i = 0; i < 10; i++) {
    rr.run();
  }
  
  background(255);

  pushMatrix();  // Enter stage local xy
  translate(0, height);
  scale(1, -1);
  translate(marginX, marginY);

  // Draw stage boundary
  noFill();
  stroke(0);
  rect(0, 0, stageDimX, stageDimY);

  for (int x = 0; x <= stageDimX + 150; x += 50) {
    for (int y = -150; y <= stageDimY; y+= 50) {
      pushMatrix();
      translate(x, y);
      strokeWeight(4);
      point(x, y);
      strokeWeight(1);
      scale(1, -1);
      textSize(12);
      text((x / 10) + ", " + (y / 10), 0, 0);
      popMatrix();
    }
  }

  // Draw LPS boundary
  rect(lpsBoundMinX, lpsBoundMinY, lpsBoundMaxX - lpsBoundMinX, lpsBoundMaxY - lpsBoundMinY);

  // Draw Basilisks
  for (Basilisk b : bs) {
    b.display();
  }

  popMatrix();  // Exit stage local xy
}
