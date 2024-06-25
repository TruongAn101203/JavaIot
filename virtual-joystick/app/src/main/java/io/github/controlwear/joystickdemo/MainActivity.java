package io.github.controlwear.joystickdemo;

import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.widget.EditText;
import android.widget.SeekBar;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

public class MainActivity extends AppCompatActivity {
    private EditText ipAddressEditText, portEditText, dataEditText;
    private Button connectButton;
    private DatagramSocket socket;
    private InetAddress serverAddress;
    private int serverPort;

    //
    private TextView mTextViewAngleLeft;
    private TextView mViewSpeed;
    private TextView mViewServo;

    private SeekBar mSeekSpeed;

    private SeekBar mSeekServo;
    private TextView mTextViewStrengthLeft;
    TextView txtBtn3, txtBtn4;
    private TextView mTextViewAngleRight;
    private TextView mTextViewStrengthRight;
//    private TextView mTextViewCoordinateRight;

    int bleft = 100, bright = 50, maxSpeed = 50, angleChange = 50;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mTextViewAngleRight = findViewById(R.id.textView_angle_right);
        mTextViewStrengthRight = findViewById(R.id.textView_strength_right);
        mTextViewAngleLeft = findViewById(R.id.textView_angle_left);
        mTextViewStrengthLeft = findViewById(R.id.textView_strength_left);
        mViewSpeed = findViewById(R.id.textViewSpeed);
        mViewServo = findViewById(R.id.textViewSevor);
        mSeekSpeed = findViewById(R.id.seekBar);
        mSeekServo = findViewById(R.id.seekBar2);

        // Ănh xáº¡ cĂ¡c thĂ nh pháº§n giao diá»‡n
        ipAddressEditText = findViewById(R.id.ipAddressEditText);
        portEditText = findViewById(R.id.portEditText);
        connectButton = findViewById(R.id.connectButton);

        // Create a new thread
        Thread backgroundThread = new Thread(new Runnable() {
            @Override
            public void run() {
                while (!Thread.currentThread().isInterrupted()) {
                    try {
                        Log.d("ADebugBinary", "Value: " + packet + " " + decimalValue ; Log.d("ADebugTag", "Value: " + ConvertChar(packet) + " " + ConvertChar(decimalValue));
                        packet = Convert4Binary(bright)+Convert4Binary(bleft)); int decimalValue = Integer.parseInt(binaryString, 2); sendData(packet); (edited)

                                // Sleep for 1 millisecond
                                Thread.sleep(80);
                    } catch (InterruptedException e) {
                        // Handle interruption if needed
                        Thread.currentThread().interrupt();
                    }
                }
            }
        });


    }

        private void sendData(final String data) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
//                    byte[] sendData = data.getBytes("UTF-8");
                    byte sendData = (byte) data;
//                    DatagramPacket packet = new DatagramPacket(new byte[]{sendData}, 1, serverAddress, serverPort);
                    DatagramPacket packet = new DatagramPacket(sendData, sendData.length, serverAddress, serverPort);
                    socket.send(packet);
                } catch (IOException e) {
                }
            }
        }).start();
    }
}
