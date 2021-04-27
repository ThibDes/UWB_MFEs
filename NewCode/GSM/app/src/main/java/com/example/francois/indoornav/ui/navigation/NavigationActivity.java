package com.example.francois.indoornav.ui.navigation;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.pm.ActivityInfo;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.TextView;

import com.example.francois.indoornav.R;
import com.example.francois.indoornav.decawave.PSOCMaster;
import com.example.francois.indoornav.location.ILocationProvider;
import com.example.francois.indoornav.location.LocationProvider;
import com.example.francois.indoornav.spi.SpiMasterListener;
import com.example.francois.indoornav.spi.USBConnection;
import com.example.francois.indoornav.util.PointD;
import com.example.francois.indoornav.util.SensorFusion;


public class NavigationActivity extends AppCompatActivity implements Handler.Callback,
        SpiMasterListener, SensorEventListener {

    private String opt = "opt";
    private SharedPreferences sharedpreferences;
    private NavigationView navigationView;
    private USBConnection mUSB;
    private PSOCMaster mPSOC;
    private LocationProvider location;
    private Handler handler;
    private SensorFusion sensorFusion;
    private SensorManager sensorManager;
    private TextView displayCoordinates;
    private TextView displayData;
    private String[] beacons = {"BEACONPOS1X", "BEACONPOS1Y", "BEACONPOS1Z",
                                "BEACONPOS2X", "BEACONPOS2Y", "BEACONPOS2Z",
                                "BEACONPOS3X", "BEACONPOS3Y", "BEACONPOS3Z",
                                "BEACONPOS4X", "BEACONPOS4Y", "BEACONPOS4Z"};


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_navigation);
        navigationView = findViewById(R.id.navigationView);
        displayCoordinates = findViewById(R.id.displayCoordinates);
        displayData = findViewById(R.id.displayData);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        sensorManager = (SensorManager) this.getSystemService(SENSOR_SERVICE);
        registerSensorManagerListeners();
        sensorFusion = new SensorFusion();
        sensorFusion.setMode(SensorFusion.Mode.FUSION);
        mUSB = new USBConnection(this, this);

        handler = new Handler(this);
        updateBeacons();
    }

    private void updateBeacons(){
        /*
        Recovers saved beacons positions and sends them to PSOCMaster to be updated when the localization starts
        */
        sharedpreferences = getSharedPreferences(opt, Context.MODE_PRIVATE);
        int[] newPos = {0,0,0,0,0,0,0,0,0,0,0,0};
        for(int i = 0; i<beacons.length; i++){
            newPos[i] = Integer.valueOf(sharedpreferences.getString(beacons[i], "0"));
        }
        PSOCMaster.changeBeaconsPos(newPos);
    }

    private void registerSensorManagerListeners() {
        sensorManager.registerListener(this,
                sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER),
                SensorManager.SENSOR_DELAY_UI);

        sensorManager.registerListener(this,
                sensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE),
                SensorManager.SENSOR_DELAY_UI);

        sensorManager.registerListener(this,
                sensorManager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD),
                SensorManager.SENSOR_DELAY_UI);
    }

    public void updateOrientationDisplay() {
        double azimuthValue = sensorFusion.getAzimuth();
        navigationView.setOrientation(azimuthValue);
    }

    @Override
    protected void onResume() {
        super.onResume();
        navigationView.setVisibility(View.VISIBLE);
        registerSensorManagerListeners();
        mUSB.open();
        if(location != null) {
            location.onResume();
        }
    }

    @Override
    protected void onPause() {
        navigationView.setVisibility(View.INVISIBLE);
        sensorManager.unregisterListener(this);
        if (location != null) {
            location.onPause();
        }
        super.onPause();
    }

    @Override
    protected void onDestroy() {
        mUSB.unregisterListener(this);
        handler.removeCallbacksAndMessages(null);
        if (location != null) {
            location.quit();
            location.interrupt();
            location = null;
        }
        mUSB.close();
        super.onDestroy();
    }

    @Override
    public boolean handleMessage(Message msg) {
        if (msg.what == ILocationProvider.SUCCESS) {
            navigationView.setPositions((PointD) msg.obj);
            displayCoordinates.setText(getString(R.string.coor_cm,
                    ((PointD) msg.obj).x, ((PointD) msg.obj).y));
            displayData.setText(getString(R.string.coor_cm,
                    ((PointD) msg.obj).x, ((PointD) msg.obj).y));
        }
        return true;
    }

    @Override
    public void onDeviceConnected(){
        if(mPSOC == null) {
            mPSOC = new PSOCMaster(mUSB);
        }
        if (location == null){
            location = new LocationProvider(handler, mPSOC);
        }
    }

    @Override
    public void onDeviceDisconnected() {
        if(location != null) {
            location.onPause();
            location.quit();
            location.interrupt();
            location = null;
        }
        mPSOC = null;
    }


    @Override
    public void onDataFailure(int status) {
        Log.d("PSOC IOError", String.valueOf(status));
        if(location != null) {
            location.onPause();
            location.quit();
            location.interrupt();
            location = null;
        }
        mPSOC = null;
        mUSB.open();
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        switch (event.sensor.getType()) {
            case Sensor.TYPE_ACCELEROMETER:
                sensorFusion.setAccel(event.values);
                sensorFusion.calculateAccMagOrientation();
                //myText.setText(String.valueOf(event.values[0]));
                break;

            case Sensor.TYPE_GYROSCOPE:
                sensorFusion.gyroFunction(event);
                //myText.setText(String.valueOf(event.values[0]));
                break;

            case Sensor.TYPE_MAGNETIC_FIELD:
                sensorFusion.setMagnet(event.values);
                //myText.setText(String.valueOf(event.values[0]));
                break;
        }
        updateOrientationDisplay();
    }

}
