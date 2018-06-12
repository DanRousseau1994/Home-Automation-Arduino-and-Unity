
//DOOR CODE
void SetupServo()
{
  myServo.attach(9);
}

void ServoOpen(String command)
{
  if (hasTarget != true) {
    pcSerial.println("Not Close Enough To Sensor");
    return;
  }

  myServo.write(10);
  pcSerial.println(command);
}

void ServoClose(String command)
{
  myServo.write(150);
  pcSerial.println(command);
}
//END DOOR CODE
