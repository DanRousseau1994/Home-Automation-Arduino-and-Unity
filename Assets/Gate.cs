using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Gate : MonoBehaviour
{
    public Animator GateAnim;
    public Animator DoorAnim;

    public void Open()
    {
        GateAnim.SetBool("Open Gate", true);
    }

    public void Close()
    {
        GateAnim.SetBool("Open Gate", false);
    }
    
    public void OpenDoor()
    {
        DoorAnim.SetBool("Open", true);
    }

    public void CloseDoor()
    {
        DoorAnim.SetBool("Open", false);
    }
}