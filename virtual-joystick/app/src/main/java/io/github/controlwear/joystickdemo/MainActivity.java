package io.github.controlwear.joystickdemo;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
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
