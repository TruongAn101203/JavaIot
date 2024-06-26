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
                        String packet = Convert4Binary((byte)bright)+Convert4Binary((byte)bleft);
                        int decimalValue = Integer.parseInt(packet, 2);
                        sendData(packet);
                        Thread.sleep(80);
                    } catch (InterruptedException e) {
                        // Handle interruption if needed
                        Thread.currentThread().interrupt();
                    }
                }
            }
        });
    }

    public String Convert4Binary(byte value) {
        return String.format("%4s", Integer.toBinaryString(value & 0xFF)).replace(' ', '0').substring(4);
    }
    public char ConvertChar(String binaryString) {
        int charCode = Integer.parseInt(binaryString, 2);
        return (char) charCode;
    }

    private void sendData(final String data) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    byte[] sendData = data.getBytes("UTF-8");
//                    byte sendData = (byte) data;
//                    DatagramPacket packet = new DatagramPacket(new byte[]{sendData}, 1, serverAddress, serverPort);
                    DatagramPacket packet = new DatagramPacket(sendData, sendData.length, serverAddress, serverPort);
                    socket.send(packet);
                } catch (IOException e) {
                }
            }
        }).start();
        mSeekSpeed.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                mViewSpeed.setText("Speed: " + String.valueOf(i));
                maxSpeed = i;
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
        mSeekServo.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                mViewServo.setText("Servo: " + String.valueOf(i));
                angleChange = i;
                bright = i;
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        connectButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String ipAddress = ipAddressEditText.getText().toString();
                String portStr = portEditText.getText().toString();

                if (ipAddress.isEmpty() || portStr.isEmpty()) {
                    connectButton.setText("NULL");
                } else {
                    serverPort = Integer.parseInt(portStr);
                    try {
                        serverAddress = InetAddress.getByName(ipAddress);
                        socket = new DatagramSocket();
                        backgroundThread.start();
                        connectButton.setText("Connected");
                    } catch (Exception e) {
                        connectButton.setText("ERR");
                        e.printStackTrace();
                    }
                }
            }
        });


        JoystickView joystickLeft = findViewById(R.id.joystickView_left);
        joystickLeft.setOnMoveListener(new JoystickView.OnMoveListener() {
            @Override
            public void onMove(int angle, int strength) {
                mTextViewAngleLeft.setText(angle + "°");
                mTextViewStrengthLeft.setText(strength + "%");
                if (strength > 98) strength = 98;
                if (angle <= 180 && angle >= 0) {
                    bleft = map(strength, 0, 98, 100, maxSpeed + 100);
                    mViewSpeed.setText("Speed: " + String.valueOf(bleft));
                } else if (angle <= 360 && angle >= 180) {
                    bleft = map(strength, 0, 98, 100, 100-maxSpeed);
                    mViewSpeed.setText("Speed: " + String.valueOf(bleft));
                }
                Log.d("ADebugBinary", "Value: " + Byte.toString((byte) bleft) + Byte.toString((byte) bright));
            }
        });

        // Bên trong joystickRight.setOnMoveListener
        final JoystickView joystickRight = findViewById(R.id.joystickView_right);
        joystickRight.setOnMoveListener(new JoystickView.OnMoveListener() {
            @SuppressLint("DefaultLocale")
            @Override
            public void onMove(int angle, int strength) {
                mTextViewAngleRight.setText(angle + "°");
                mTextViewStrengthRight.setText(strength + "%");
                if (strength > 98) strength = 98;
                if (angle <= 270 && angle >= 90) {
                    bright = map(strength, 0, 98, angleChange, 0);
                    mViewServo.setText("Servo: " + String.valueOf(bright));
                } else if (((angle <= 90 && angle >= 0) || (angle <= 360 && angle >= 270))) {
                    bright = map(strength, 0, 98, angleChange, 100);
                    mViewServo.setText("Servo: " + String.valueOf(bright));
                }
            }
        });

    }


}

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (socket != null) {
            socket.close();
        }
    }

    public int map(int x, int in_min, int in_max, int out_min, int out_max) {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }


}
