using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;

public class ArduinoEvents : MonoBehaviour
{
    public UnityEvent LockDoor;
    public UnityEvent UnlockDoor;

    public UnityEvent OpenGate;
    public UnityEvent CloseGate;

    public UnityEvent TurnOnLights;
    public UnityEvent TurnOffLights;

    public UnityEvent BoilKettle;

    
    public void UpdateEvent(string EventName)
    {
        if (EventName == "LIGHTS ARE ON")
        {
            TurnOnLights.Invoke();
        }
        if (EventName == "LIGHTS ARE OFF")
        {
            TurnOffLights.Invoke();
        }
        if (EventName == "OpenGate")
        {
            OpenGate.Invoke();
        }
        if (EventName == "CloseGate")
        {
            CloseGate.Invoke();
        }
        if (EventName == "UnlockDoor")
        {
            UnlockDoor.Invoke();
        }
        if (EventName == "CloseDoor")
        {
            LockDoor.Invoke();
        }
    }
}