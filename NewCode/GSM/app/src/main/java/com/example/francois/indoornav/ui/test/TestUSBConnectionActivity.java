package com.example.francois.indoornav.ui.test;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import com.example.francois.indoornav.R;
import com.example.francois.indoornav.spi.USBConnection;


public class TestUSBConnectionActivity extends AppCompatActivity {

    private USBConnection mUSB;
    private TextView textTestBox;
    private TextView textTestBox1;
    private TextView textTestBox2;
    private TextView textTestBox3;
    private TextView textTestBox4;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_test_usbconnection);

        mUSB = new USBConnection(this);

        textTestBox    = findViewById(R.id.textView);
        textTestBox1    = findViewById(R.id.textView10);
        textTestBox2    = findViewById(R.id.textView11);
        textTestBox3    = findViewById(R.id.textView12);
        textTestBox4    = findViewById(R.id.textView13);
    }

    public void test_send(View view){
        byte[] writeBuffer = {0x04, 0x06};
        int sizeToTransfer = 2;
        byte[] readBuffer=new byte[4];
        int sizeToReceive = 4;
        int Tx;
        int Rx;
        Tx=mUSB.singleWrite(writeBuffer, sizeToTransfer);
        textTestBox1.setText("Bytes send:");
        textTestBox2.setText(Integer.toString(Tx));
        Rx= mUSB.singleRead(readBuffer,sizeToReceive);
        textTestBox3.setText("Bytes receive:");
        textTestBox4.setText(Integer.toString(Rx));

        if(readBuffer[0]==0x02 && readBuffer[1]==0x03 && readBuffer[2]==0x46
                && readBuffer[3]==0x37 )textTestBox.setText("USB communication Ok");
        else textTestBox.setText("USB Communication not Ok");
    }

    @Override
    protected void onResume() {
        super.onResume();
        mUSB.open();
    }

    @Override
    protected void onDestroy() {
        mUSB.close();
        super.onDestroy();
    }
}
