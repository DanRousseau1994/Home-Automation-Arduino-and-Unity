void LightsOn()
{
  //Turn On Light LED
  digitalWrite(LED_BUILTIN, HIGH);
  pcSerial.println("LIGHTS ARE ON");
}

void LightsOff()
{
  //Turn Off Light LED
  digitalWrite(LED_BUILTIN, LOW);
  pcSerial.println("LIGHTS ARE OFF");
}
