void DistanceSensor()
{ 
  float volts = analogRead(sensor) * 0.0048828125;
  int distance = 13 * pow(volts, -1);
  delay(1000);

  SetupServo();

  if (distance <= 10) 
    hasTarget = true;  
  else 
    hasTarget = false;
  
}
