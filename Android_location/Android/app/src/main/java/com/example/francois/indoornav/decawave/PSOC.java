package com.example.francois.indoornav.decawave;

import com.example.francois.indoornav.spi.USBConnection;


public abstract class PSOC {

    static final double TIME_UNIT = 1 / (128 * 499.2 * 1000000);
    static final int LIGHT_SPEED = 299792458;

    private final USBConnection mUSB;

    // Constructor
    PSOC(USBConnection USB){
        mUSB = USB;
    }

    void sendRequest() {
        byte[] writeBuffer = {0x01};
        int sizeToTransfer = 1;
        mUSB.singleWrite(writeBuffer, sizeToTransfer);
    }

    void receiveFrameFromSlave(byte[] data) {
        mUSB.singleRead(data,63);
    }

}



