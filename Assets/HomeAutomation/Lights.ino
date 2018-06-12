void LightsOn()
{
  digitalWrite(LED_BUILTIN, HIGH);
  pcSerial.println("LIGHTS ARE ON");
}

void LightsOff()
{
  digitalWrite(LED_BUILTIN, LOW);
  pcSerial.println("LIGHTS ARE OFF");
}
