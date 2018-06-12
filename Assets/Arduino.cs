using System;
using System.Collections;
using System.IO.Ports;
using UnityEngine;

namespace Project.Scripts
{
    public class Arduino : MonoBehaviour
    {

        public ArduinoEvents EventSystm;
        
        [Tooltip("The serial port where the Arduino is connected")]
        public string Port = "COM3";

        [Tooltip("The baudrate of the serial port")]
        public int Baudrate = 9600;

        private SerialPort _stream;

        private void Start()
        {
            Open();
            StartCoroutine(AsynchronousReadFromArduino(SerialCallback, SerialFailed));
        }

        private void OnApplicationQuit()
        {
            Close();
        }

        private void SerialCallback(string _output)
        {
            Debug.Log(_output);
            EventSystm.UpdateEvent(_output);
        }

        private void SerialFailed()
        {
            Debug.LogWarning("Failed to fetch serial");
            StartCoroutine(AsynchronousReadFromArduino(SerialCallback, SerialFailed));
        }

        //END

        private void Open()
        {
            // Opens the serial port
            _stream = new SerialPort(Port, Baudrate);
            _stream.ReadTimeout = 50;
            _stream.Open();
        }

        public void WriteToArduino(string message)
        {
            // Send the request
            _stream.WriteLine(message);
            _stream.BaseStream.Flush();
        }

        public string ReadFromArduino(int timeout = 0)
        {
            _stream.ReadTimeout = timeout;
            try
            {
                return _stream.ReadLine();
            }
            catch (TimeoutException)
            {
                return null;
            }
        }

        private IEnumerator AsynchronousReadFromArduino(Action<string> callback, Action fail = null,
            float timeout = float.PositiveInfinity)
        {
            var initialTime = DateTime.Now;
            DateTime nowTime;
            var diff = default(TimeSpan);

            string dataString = null;

            do
            {
                // A single read attempt
                try
                {
                    dataString = _stream.ReadLine();
                }
                catch (TimeoutException)
                {
                    dataString = null;
                }

                if (dataString != null)
                {
                    callback(dataString);
                    yield return null;
                }
                else
                    yield return new WaitForSeconds(0.05f);

                nowTime = DateTime.Now;
                diff = nowTime - initialTime;
            } while (diff.Milliseconds < timeout);

            if (fail != null)
                fail();
            yield return null;
        }

        private void Close()
        {
            _stream.Close();
        }
    }
}