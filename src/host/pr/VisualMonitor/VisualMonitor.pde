import processing.serial.*;

Serial xbPort;
XbRR rr;

void runXbRR() {
  while (true) {
    rr.Run();
  }
}

// ALL lengths in cm
final float stageDimX = 860, stageDimY = 910;
final float lpsBoundMinX = 150, lpsBoundMaxX = 710, lpsBoundMinY = 150, lpsBoundMaxY = 760;
float marginX, marginY;

ArrayList<Basilisk> bs;

void setup() {
  String[] ports = Serial.list();
  printArray(ports);
  xbPort = new Serial(this, "/dev/ttyUSB0", 115200);
  rr = new XbRR(xbPort);
  thread("runXbRR");

  size(1300, 1000);
  frameRate(2);

  marginX = (width - stageDimX) / 2;
  marginY = (height - stageDimY) / 2;

  bs = new ArrayList<Basilisk>();
  for (int suid = 1; suid <= 13; suid++) {
    bs.add(new Basilisk(suid));
  }
}

void draw() {
  background(255);

  pushMatrix();  // Enter stage local xy
  translate(0, height);
  scale(1, -1);
  translate(marginX, marginY);

  // Draw stage boundary
  noFill();
  stroke(0);
  rect(0, 0, stageDimX, stageDimY);

  // Draw LPS boundary
  rect(lpsBoundMinX, lpsBoundMinY, lpsBoundMaxX - lpsBoundMinX, lpsBoundMaxY - lpsBoundMinY);

  // Draw Basilisks
  for (Basilisk b : bs) {
    b.display();
  }

  popMatrix();  // Exit stage local xy
}