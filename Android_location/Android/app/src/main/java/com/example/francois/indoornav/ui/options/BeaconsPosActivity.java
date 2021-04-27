package com.example.francois.indoornav.ui.options;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import com.example.francois.indoornav.R;
import com.example.francois.indoornav.decawave.PSOCMaster;


public class BeaconsPosActivity extends AppCompatActivity {

    private String opt = "opt";
    private SharedPreferences sharedpreferences;
    private Button nextButton;
    private Button skipButton;
    private int[] coords = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
    private TextView beacon;
    private int number;
    private EditText x;
    private EditText y;
    private EditText z;

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        number = 0;
        setContentView(R.layout.activity_beacons_positions);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
        nextButton = findViewById(R.id.button1);
        skipButton = findViewById(R.id.button2);
        beacon = findViewById(R.id.textView4);
        beacon.setText("Beacon " + (number+1));
        x = findViewById(R.id.X1);
        y = findViewById(R.id.Y1);
        z = findViewById(R.id.Z1);
        x.setHint(getPreferenceValue("BEACONPOS1X"));
        y.setHint(getPreferenceValue("BEACONPOS1Y"));
        z.setHint(getPreferenceValue("BEACONPOS1Z"));
    }


    public void nextBeacon(View view){
        /*
        next button : read values, wipe and increment beacon counter
         */
        String xTemp = x.getText().toString();
        String yTemp = y.getText().toString();
        String zTemp = z.getText().toString();
        if(!xTemp.isEmpty()){
            coords[number*3] = Integer.parseInt(xTemp);
        }
        if(!yTemp.isEmpty()) {
            coords[number*3+1] = Integer.parseInt(yTemp);
        }
        if(!zTemp.isEmpty()){
            coords[number*3+2] = Integer.parseInt(zTemp);
        }
        x.getText().clear();
        y.getText().clear();
        z.getText().clear();
        number += 1;
        if(number == 3){
            nextButton.setVisibility(View.GONE);
            skipButton.setVisibility(View.GONE);
        }
        beacon.setText("Beacon " + (number+1));
        update();
    }

    public void accept(View view){
        /*
        accept button : send data to PSoCMaster
        */
        String xTemp = x.getText().toString();
        String yTemp = y.getText().toString();
        String zTemp = z.getText().toString();
        if(!xTemp.isEmpty()){
            coords[number*3] = Integer.parseInt(xTemp);
        }
        if(!yTemp.isEmpty()) {
            coords[number*3+1] = Integer.parseInt(yTemp);
        }
        if(!zTemp.isEmpty()){
            coords[number*3+2] = Integer.parseInt(zTemp);
        }

        writeToPreference();
        PSOCMaster.changeBeaconsPos(coords);
        this.finish();
    }

    public void skip(View view){
        x.getText().clear();
        y.getText().clear();
        z.getText().clear();
        if(number == 2){
            nextButton.setVisibility(View.GONE);
            skipButton.setVisibility(View.GONE);
        }
        number += 1;
        beacon.setText("Beacon " + (number+1));
        update();
    }

    private void update() {
        if(number == 1) {
            x.setHint(getPreferenceValue("BEACONPOS2X"));
            y.setHint(getPreferenceValue("BEACONPOS2Y"));
            z.setHint(getPreferenceValue("BEACONPOS2Z"));
        }else if(number == 2){
            x.setHint(getPreferenceValue("BEACONPOS3X"));
            y.setHint(getPreferenceValue("BEACONPOS3Y"));
            z.setHint(getPreferenceValue("BEACONPOS3Z"));
        }else if(number == 3){
            x.setHint(getPreferenceValue("BEACONPOS4X"));
            y.setHint(getPreferenceValue("BEACONPOS4Y"));
            z.setHint(getPreferenceValue("BEACONPOS4Z"));
        }
    }

    public void init(View view){
        coords[0] = PSOCMaster.getDefault("BEACONPOS1X");
        coords[1] = PSOCMaster.getDefault("BEACONPOS1Y");
        coords[2] = PSOCMaster.getDefault("BEACONPOS1Z");
        coords[3] = PSOCMaster.getDefault("BEACONPOS2X");
        coords[4] = PSOCMaster.getDefault("BEACONPOS2Y");
        coords[5] = PSOCMaster.getDefault("BEACONPOS2Z");
        coords[6] = PSOCMaster.getDefault("BEACONPOS3X");
        coords[7] = PSOCMaster.getDefault("BEACONPOS3Y");
        coords[8] = PSOCMaster.getDefault("BEACONPOS3Z");
        coords[9] = PSOCMaster.getDefault("BEACONPOS4X");
        coords[10] = PSOCMaster.getDefault("BEACONPOS4Y");
        coords[11] = PSOCMaster.getDefault("BEACONPOS4Z");
        writeToPreference();
        PSOCMaster.changeBeaconsPos(coords);
        this.finish();
    }

    public String getPreferenceValue(String value)
    {
        sharedpreferences = getSharedPreferences(opt, Context.MODE_PRIVATE);
        String val = sharedpreferences.getString(value, "0");
        return val;
    }

    public void writeToPreference()
    {
        sharedpreferences = getSharedPreferences(opt, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedpreferences.edit();
        if(coords[0] >= 0){
            editor.putString("BEACONPOS1X", Integer.toString(coords[0]));
        }
        if(coords[1] >= 0){
            editor.putString("BEACONPOS1Y", Integer.toString(coords[1]));
        }
        if(coords[2] >= 0){
            editor.putString("BEACONPOS1Z", Integer.toString(coords[2]));
        }
        if(coords[3] >= 0){
            editor.putString("BEACONPOS2X", Integer.toString(coords[3]));
        }
        if(coords[4] >= 0){
            editor.putString("BEACONPOS2Y", Integer.toString(coords[4]));
        }
        if(coords[5] >= 0){
            editor.putString("BEACONPOS2Z", Integer.toString(coords[5]));
        }
        if(coords[6] >= 0){
            editor.putString("BEACONPOS3X", Integer.toString(coords[6]));
        }
        if(coords[7] >= 0){
            editor.putString("BEACONPOS3Y", Integer.toString(coords[7]));
        }
        if(coords[8] >= 0){
            editor.putString("BEACONPOS3Z", Integer.toString(coords[8]));
        }
        if(coords[9] >= 0){
            editor.putString("BEACONPOS4X", Integer.toString(coords[9]));
        }
        if(coords[10] >= 0){
            editor.putString("BEACONPOS4Y", Integer.toString(coords[10]));
        }
        if(coords[11] >= 0){
            editor.putString("BEACONPOS4Z", Integer.toString(coords[11]));
        }
        editor.apply();
        coords = new int[] {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
    }


}
