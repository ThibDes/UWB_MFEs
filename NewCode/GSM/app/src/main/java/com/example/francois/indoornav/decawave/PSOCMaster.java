package com.example.francois.indoornav.decawave;

import android.os.Environment;
import android.os.SystemClock;
import android.util.Log;
import com.example.francois.indoornav.location.ILocationProvider;
import com.example.francois.indoornav.spi.USBConnection;
import com.example.francois.indoornav.util.PointD;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Arrays;

import static com.example.francois.indoornav.util.BytesUtils.byteArray5ToLong;


public class PSOCMaster extends PSOC implements ILocationProvider {

    private final int numberSlaves = 3;
    //private static final double correctivePol[] = { -0.0081, 0.0928, 0.6569, -0.0612};
    //private static final double correctivePol[] = {0.004396537699051796, 0.9195024228226539,
    //        0.23848199262062902}; // 4.30m calib, distance
    private static final double correctivePol[] = {-8.18957391e-07, -2.34689642e-04, -2.48275823e-02, -1.15859275e+00,
            -2.04203118e+01}; // 7.94m calib, Pr
    private static final double __estimated_power = -14.3 + 20 * Math.log10(LIGHT_SPEED) -
            20 * Math.log10(4 * Math.PI * 6489e6);
    private double distancemm[] = new double[numberSlaves];
    private byte slave_connection[] = new byte[numberSlaves];
    private PointD coordinates = new PointD();
    private static int BEACONPOS1X, BEACONPOS1Y, BEACONPOS1Z, BEACONPOS2X, BEACONPOS2Y, BEACONPOS2Z, BEACONPOS3X,
            BEACONPOS3Y, BEACONPOS3Z, BEACONPOS4X, BEACONPOS4Y, BEACONPOS4Z;

    private static final int TAGZ = 115;

    private int[] deltah = {BEACONPOS1Z - TAGZ, BEACONPOS2Z - TAGZ, BEACONPOS3Z - TAGZ};

    private File f;
    private  File test;
    private FileOutputStream output;

    public PSOCMaster(USBConnection USB) {
        super(USB);

        String folder_main = "testDir";

        f = new File(Environment.getExternalStorageDirectory(), folder_main);
        if (!f.exists()) {
            f.mkdirs();
        }
        test = new File(f , "testFile.txt");
        try {
            output = new FileOutputStream(test);
        }
        catch (IOException e) {
        }
        int[] defaultPos = {74,353,220,1273,42,224,1297,770,145,845,943,195};
        changeBeaconsPos(defaultPos);
    }

    enum State {
        POLL_INIT,
        GET_TIMES,
        END,
    }


    public static int getDefault(String key) {
        if (key.equals("BEACONPOS1X")) {
            return 74;
        } else if (key.equals("BEACONPOS1Y")) {
            return 353;
        } else if (key.equals("BEACONPOS1Z")) {
            return 220;
        } else if (key.equals("BEACONPOS2X")) {
            return 1273;
        } else if (key.equals("BEACONPOS2Y")) {
            return 42;
        } else if (key.equals("BEACONPOS2Z")) {
            return 224;
        } else if (key.equals("BEACONPOS3X")) {
            return 1297;
        } else if (key.equals("BEACONPOS3Y")) {
            return 770;
        } else if (key.equals("BEACONPOS3Z")) {
            return 145;
        } else if (key.equals("BEACONPOS4X")) {
            return 845;
        } else if (key.equals("BEACONPOS4Y")) {
            return 943;
        } else if (key.equals("BEACONPOS4Z")) {
            return 195;
        }
        return 0;
    }

    public static void changeBeaconsPos(int[] saves){
        BEACONPOS1X = saves[0];
        BEACONPOS1Y = saves[1];
        BEACONPOS1Z = saves[2];
        BEACONPOS2X = saves[3];
        BEACONPOS2Y = saves[4];
        BEACONPOS2Z = saves[5];
        BEACONPOS3X = saves[6];
        BEACONPOS3Y = saves[7];
        BEACONPOS3Z = saves[8];
        BEACONPOS4X = saves[9];
        BEACONPOS4Y = saves[10];
        BEACONPOS4Z = saves[11];

    }

    public double[] getDistances() {
        long[] allClockTime = ranging();
        if (allClockTime == null) {
            return null;
        }
        return compute_distances(allClockTime);
    }

    private long[] ranging() {
        long[] clockTime = new long[12];
        State state = State.POLL_INIT;
        long startTime = SystemClock.elapsedRealtime();
        boolean timeOut = false;
        while (!(state == State.END ||
                (timeOut = (SystemClock.elapsedRealtime() - startTime > 300)))) {
            switch (state) {
                case POLL_INIT:
                    sendRequest();
                    state = State.GET_TIMES;
                    break;
                case GET_TIMES:
                    byte[] data=new byte[63];
                    receiveFrameFromSlave(data);
                    allClockTime(data, clockTime);
                    slave_connection[0]=data[60];
                    slave_connection[1]=data[61];
                    slave_connection[2]=data[62];
                    state = State.END;
                    break;
            }
        }
        if (timeOut) {
            clockTime = null;
        }
        Log.d("TimeSpend", String.valueOf(SystemClock.elapsedRealtime() - startTime));
        return clockTime;
    }

    private void allClockTime(byte[] data, long[] clockTime) {
        for (int i = 0; i < 12; ++i) {
            clockTime[i] = byteArray5ToLong(Arrays.copyOfRange(data, i*5, 5+i*5));
        }
    }

    private double[] compute_distances(long[] allClockTime) {
        double tRoundMaster, tRoundSlave, tReplyMaster, tReplySlave;
        double tof;
        double distance;
        for (int i = 0; i < numberSlaves; ++i) {
            tRoundMaster = allClockTime[4*i];
            tRoundSlave = allClockTime[1+4*i];
            tReplyMaster = allClockTime[2+4*i];
            tReplySlave = allClockTime[3+4*i];
            tof = (tRoundMaster * tRoundSlave - tReplyMaster * tReplySlave) * TIME_UNIT /
                    (tRoundMaster + tRoundSlave + tReplyMaster + tReplySlave);
            double distanceMeasured = tof * LIGHT_SPEED;

            Log.d("Distance", String.valueOf(distanceMeasured));
            double estimated_power = __estimated_power - 20 * Math.log10(distanceMeasured);
            if (estimated_power >= -50 || distanceMeasured <= 0) {
                estimated_power = -50;
            } else if (estimated_power <= -92) {
                estimated_power = -92;
            }
            distance = distanceMeasured - (Math.pow(estimated_power, 4) * correctivePol[0] +
                    Math.pow(estimated_power, 3) * correctivePol[1] +
                    Math.pow(estimated_power, 2) * correctivePol[2] +
                    estimated_power * correctivePol[3] +
                    correctivePol[4]);
            distance *= 100;
            for(int k=0;k<numberSlaves;k++) {
                if(slave_connection[k]==0x01)deltah[k]=BEACONPOS1Z - TAGZ;
                if(slave_connection[k]==0x02)deltah[k]=BEACONPOS2Z - TAGZ;
                if(slave_connection[k]==0x03)deltah[k]=BEACONPOS3Z - TAGZ;
                if(slave_connection[k]==0x04)deltah[k]=BEACONPOS4Z - TAGZ;
            }
            this.distancemm[i] = Math.sqrt(distance * distance - deltah[i] * deltah[i]);
        }

        try {
            output.write(String.valueOf(distancemm[0]).getBytes());
            output.write("\n".getBytes());
            output.write(String.valueOf(distancemm[1]).getBytes());
            output.write("\n".getBytes());
            output.write(String.valueOf(distancemm[2]).getBytes());
            output.write("\n".getBytes());
        } catch (IOException e) {
            e.printStackTrace();
        }
        return distancemm;
    }

    @LocationUpdated
    private int computeCoordinates(double[] distances) {

        if (distances == null) {
            return FAILED;
        }
        int p[][] = new int[2][3];
        for(int k=0;k<numberSlaves;k++) {
            if(slave_connection[k]==0x01) {
                p[0][k] = BEACONPOS1X;
                p[1][k] = BEACONPOS1Y;
            }
            if(slave_connection[k]==0x02) {
                p[0][k] = BEACONPOS2X;
                p[1][k] = BEACONPOS2Y;
            }
            if(slave_connection[k]==0x03) {
                p[0][k] = BEACONPOS3X;
                p[1][k] = BEACONPOS3Y;
            }
            if(slave_connection[k]==0x04) {
                p[0][k] = BEACONPOS4X;
                p[1][k] = BEACONPOS4Y;
            }
        }

        double sumDistanceSquared = 0;
        int[][] ppT = {{0, 0}, {0, 0}};
        double[] c = {0, 0};

        for (int i = 0; i < numberSlaves; ++i) {
            sumDistanceSquared += distances[i] * distances[i];
            ppT[0][0] += p[0][i] * p[0][i];
            ppT[1][0] += p[1][i] * p[0][i];
            ppT[1][1] += p[1][i] * p[1][i];
            c[0] += p[0][i];
            c[1] += p[1][i];
        }

        c[0] /= numberSlaves;
        c[1] /= numberSlaves;

        ppT[0][1] = ppT[1][0];
        double[][] ccT = {{c[0] * c[0], c[0] * c[1]}, {c[0] * c[1], c[1] * c[1]}};

        int pTp;
        double temp;
        double[] a = {0, 0};
        double[][] B = {{sumDistanceSquared - 2 * ppT[0][0], -2 * ppT[0][1]},
                {-2 * ppT[1][0], sumDistanceSquared - 2 * ppT[1][1]}};

        for (int i = 0; i < numberSlaves; ++i) {
            pTp = p[0][i] * p[0][i] + p[1][i] * p[1][i];
            temp = pTp - distances[i] * distances[i];

            a[0] += temp * p[0][i];
            a[1] += temp * p[1][i];

            B[0][0] -= pTp;
            B[1][1] -= pTp;

        }

        a[0] /= numberSlaves;
        a[1] /= numberSlaves;
        B[0][0] /= numberSlaves;
        B[1][0] /= numberSlaves;
        B[0][1] /= numberSlaves;
        B[1][1] /= numberSlaves;

        double[] f = new double[2];

        f[0] = a[0] + B[0][0] * c[0] + B[0][1] * c[1] + 2 * (ccT[0][0] * c[0] + ccT[0][1] * c[1]);
        f[1] = a[1] + B[1][0] * c[0] + B[1][1] * c[1] + 2 * (ccT[1][0] * c[0] + ccT[1][1] * c[1]);

        double[][] H = {{2 * (-ppT[0][0] / (double) numberSlaves + ccT[0][0]),
                2 * (-ppT[0][1] / (double) numberSlaves + ccT[0][1])},
                {2 * (-ppT[1][0] / (double) numberSlaves + ccT[1][0]),
                        2 * (-ppT[1][1] / (double) numberSlaves + ccT[1][1])}};

        double detH = H[0][0] * H[1][1] - H[0][1] * H[1][0];
        double invH[][] = {{H[1][1] / detH, -H[0][1] / detH}, {-H[1][0] / detH, H[0][0] / detH}};
        double q[] = {-invH[0][0] * f[0] - invH[0][1] * f[1], -invH[1][0] * f[0] - invH[1][1] * f[1]};

        coordinates.set(c[0] + q[0], c[1] + q[1]);
        return SUCCESS;
    }

    @Override
    @LocationUpdated
    public int updateLocation() {
        return computeCoordinates(getDistances());
    }

    @Override
    public PointD getLastLocation() {
        return coordinates;
    }

}
