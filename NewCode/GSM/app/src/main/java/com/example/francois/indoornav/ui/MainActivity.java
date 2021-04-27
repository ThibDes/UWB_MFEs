package com.example.francois.indoornav.ui;

import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import com.example.francois.indoornav.R;
import com.example.francois.indoornav.ui.navigation.NavigationActivity;
import com.example.francois.indoornav.ui.options.OptionsActivity;
import com.example.francois.indoornav.ui.test.CalibrationTest;
import com.example.francois.indoornav.ui.test.TestOrientationActivity;
import com.example.francois.indoornav.ui.test.TestUSBConnectionActivity;


public class MainActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
    }

    public void startNavigation(View view) {
        startActivity(new Intent(this, NavigationActivity.class));
    }

    public void startTestUSBConnection(View view){
        startActivity(new Intent(this, TestUSBConnectionActivity.class));
    }

    public void startTestOrientation(View view) {
        startActivity(new Intent(this, TestOrientationActivity.class));
    }

    public void startCalibration(View view) { startActivity(new Intent(this, CalibrationTest.class));}

    public void startOptions(View view) { startActivity(new Intent(this, OptionsActivity.class));}

}
