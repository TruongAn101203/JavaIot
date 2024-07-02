package io.github.controlwear.joystickdemo;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.util.Log;
import android.view.InputDevice;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.SeekBar;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

import io.github.controlwear.virtual.joystick.android.JoystickView;

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

    byte bleft = 7, bright = 7;
    int maxSpeed = 50, angleChange = 50;

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

        // Anh xa den cac thanh phan trong UI
        // ipAddressEditText: EditText cho lúc nhập IP address trong app.
        ipAddressEditText = findViewById(R.id.ipAddressEditText);
        // portEditText: EditTextcho lúc nhập port number trong app.
        portEditText = findViewById(R.id.portEditText);
        // connectButton: Nút kết nối.
        connectButton = findViewById(R.id.connectButton);

        // Create a new thread
        Thread backgroundThread = new Thread(new Runnable() {
            @Override
            public void run() {
                // Chạy đến khi cái thread bị dừng
                while (!Thread.currentThread().isInterrupted()) {
                    try {
                        // Gộp 2 values right left thành 1 byte
                        byte packet = combineByte((byte) bleft, (byte) bright);
//                        sendData(String.valueOf(packet));
                        sendData(packet);
                        // ngủ 80 mili giây giữa những lần gửi để tránh overflow network.
                        Thread.sleep(80);
                    } catch (InterruptedException e) {
                        // Handle interruption if needed
                        Thread.currentThread().interrupt();
                        e.printStackTrace();
                    }
                }
            }
        });

        connectButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String ipAddress = ipAddressEditText.getText().toString();
                String portStr = portEditText.getText().toString();
                // Kiểm tra IP address hoặc port is empty
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
        
        // Tìm left joystick view.
        JoystickView joystickLeft = findViewById(R.id.joystickView_left);
        // Gán move listener for the left joystick.
        joystickLeft.setOnMoveListener(new JoystickView.OnMoveListener() {
            @Override
            public void onMove(int angle, int strength) {
                // Update left joystick dựa trên góc và độ mạnh.
                updateLeftJoystick(angle, strength);
            }
        });


        JoystickView joystickRight = findViewById(R.id.joystickView_right);
        joystickRight.setOnMoveListener(new JoystickView.OnMoveListener() {
            @Override
            public void onMove(int angle, int strength) {
                updateRightJoystick(angle, strength);
            }
        });
    }

    private void updateLeftJoystick(int angle, int strength) {
        mTextViewAngleLeft.setText(angle + "°");
        mTextViewStrengthLeft.setText(strength + "%");
        
        // nếu strength > 98 gán strength value = 100.
        try {if (strength > 98) strength = 100;
            if (angle < 180 && angle > 0) {
                // Map angle and strength to the bleft value based on joystick movement.
                bleft = map(strength, 0, 100, 8, 14);
            } else if (angle < 360 && angle > 180) {
                bleft = map(strength, 0, 100, 6, 0);
            }
            else bleft = 7;
            // Updates the speed text view.
            mViewSpeed.setText("Speed: " + String.valueOf(bleft));

            printBinary(bleft, bright);
        }catch (Exception e){
            e.printStackTrace();
        }
    }

    private void updateRightJoystick(int angle, int strength) {
        mTextViewAngleRight.setText(angle + "°");
        mTextViewStrengthRight.setText(strength + "%");

        try {
            if (strength >= 98)
                strength = 100;
            if (angle < 270 && angle > 90) {
                bright = map(strength, 0, 100, 6, 0);
            } else if (((angle < 90 && angle >= 0) || (angle > 270 && angle < 360))) {
                bright = map(strength, 0, 100, 8, 14);
            }
            else bright = 7;

            mViewServo.setText("Servo: " + String.valueOf(bright));

            printBinary(bleft, bright);
        }catch (Exception e){
            e.printStackTrace();
        }
    }

    @Override
    public boolean dispatchGenericMotionEvent(MotionEvent event) {
        // Checks if the event is from a joystick (Controller) and is a move action.
        if ((event.getSource() & InputDevice.SOURCE_JOYSTICK) == InputDevice.SOURCE_JOYSTICK &&
                event.getAction() == MotionEvent.ACTION_MOVE) {
            processJoystickInput(event);
            return true;
        }
        return super.dispatchGenericMotionEvent(event);
    }

    private void processJoystickInput(MotionEvent event) {
        InputDevice device = event.getDevice();
        if (device != null) {
            // Gets the centered axis value for the left joystick's X axis.
            float lx = getCenteredAxis(event, device, MotionEvent.AXIS_X);
            // Same but with Y (Dấu trừ trước do config bị ngược nên nhân - thì bth lại)
            float ly = -getCenteredAxis(event, device, MotionEvent.AXIS_Y);
            // Gets the centered axis value for the right joystick's X axis.
            float rx = getCenteredAxis(event, device, MotionEvent.AXIS_Z);
            // Right joystick Y
            float ry = getCenteredAxis(event, device, MotionEvent.AXIS_RZ);

            // Map these values to the virtual joystick updates
            int leftAngle = calculateAngle(lx, ly);
            int leftStrength = calculateStrength(lx, ly);
            // Updates the left joystick based on the angle and strength values.
            updateLeftJoystick(leftAngle, leftStrength);

            int rightAngle = calculateAngle(rx, ry);
            int rightStrength = calculateStrength(rx, ry);
            updateRightJoystick(rightAngle, rightStrength);
        }
    }

    private float getCenteredAxis(MotionEvent event, InputDevice device, int axis) {
        InputDevice.MotionRange range = device.getMotionRange(axis, event.getSource());
        if (range != null) {
            // Gets the axis value.
            float value = event.getAxisValue(axis);
            // Gets the flat value (dead zone) for the axis.
            float flat = range.getFlat();
            // Returns the value if it is outside the dead zone.
            if (Math.abs(value) > flat) {
                return value;
            }
        }
        // Value is within the dead zone
        return 0;
    }

    private int calculateAngle(float x, float y) {
        // Adjusts the angle to be in the range 0-360 degrees.
        double angle = Math.toDegrees(Math.atan2(y, x));
        if (angle < 0) {
            angle += 360;
        }
        return (int) angle;
    }
    // Calculates the strength as a percentage based on the X and Y values.
    private int calculateStrength(float x, float y) {
        return (int) (Math.sqrt(x * x + y * y) * 100);
    }

    public void printBinary(byte bleft, byte bright){
        int packet = combineByte((byte) bleft, (byte) bright);

        Log.d("ADebugBinary", "Value: " + packet + " " );
        Log.d("ADebugTag", "b Left =: " + bleft + "; b Right =:" + bright);
        // Converts the bleft, bright byte to a binary string and formats it to 4 bits.
        String bleftBinary = String.format("%4s", Integer.toBinaryString(bleft & 0xFF)).replace(' ', '0');
        String brightBinary = String.format("%4s", Integer.toBinaryString(bright & 0xFF)).replace(' ', '0');
        String rsBinary = String.format("%4s", Integer.toBinaryString(packet & 0xFF)).replace(' ', '0');

        System.out.println("bleft (nhị phân): " + bleftBinary);
        System.out.println("bright (nhị phân): " + brightBinary);

        // Dịch trái bright 4 lần
        int shiftedBright = (bright & 0xFF) << 4;

        // In giá trị nhị phân của bright sau khi dịch trái
        System.out.println("bright sau khi dịch trái 4 lần (nhị phân): " + Integer.toBinaryString(shiftedBright));

        // Kết hợp bleft và shiftedBright bằng phép toán OR
        int result = (bleft & 0xFF) | shiftedBright;

        // Trả về kết quả
        System.out.println("bright sau khi dịch trái 4 lần (nhị phân): " + result);
        // Trả về kết quả
        System.out.println("Ket qua cuoi sau khi cong byte: " + rsBinary);

        try {
            String data = String.valueOf(packet);
            byte[] sendData = data.getBytes("UTF-8");
            System.out.println("Data packet: " + sendData + " Length: " + sendData.length);
        }catch (Exception e){
            e.printStackTrace();
        }
    }

    public byte combineByte(byte bleft, byte bright) {
        // Dịch trái bright 4 lần rồi kết hợp với bleft bằng phép OR
        return (byte) ((bright << 4) | (bleft & 0x0F));
    }

    private void sendData(final byte data) {
        // Creates and starts a new thread to send the data.
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    byte sendData = data;
                    // Creates a DatagramPacket with the data, length, server address, and server port.
                    DatagramPacket packet = new DatagramPacket(new byte[]{sendData}, 1, serverAddress, serverPort);
                    socket.send(packet);
                } catch (IOException e) {
                }
            }
        }).start();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (socket != null) {
            socket.close();
        }
    }

    public byte map(int x, int in_min, int in_max, int out_min, int out_max) {
        return (byte)((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
    }

//    private void sendData(final String data) {
//        new Thread(new Runnable() {
//            @Override
//            public void run() {
//                try {
//                    byte[] sendData = data.getBytes("UTF-8");
//                    DatagramPacket packet = new DatagramPacket(sendData, sendData.length, serverAddress, serverPort);
//                    socket.send(packet);
//                } catch (IOException e) {
//                }
//            }
//        }).start();
//    }
}
