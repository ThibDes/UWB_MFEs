package com.example.francois.indoornav.spi;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.usb.UsbManager;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbInterface;
import android.hardware.usb.UsbEndpoint;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbConstants;

import java.util.HashMap;


public class USBConnection {
    private Context mContext;
    private SpiMasterListener mListener;
    private UsbManager manager;
    private boolean listening = false;
    private final int VendorId = 1204;
    private final int ProductId = 57361;

    public USBConnection(Context context) {
        this(context, null);
    }

    public USBConnection(Context context, SpiMasterListener listener) {
        mContext = context.getApplicationContext();
        mListener = listener;
    }

    public void open() {
        if (!listening) {
            IntentFilter filter = new IntentFilter(UsbManager.ACTION_USB_DEVICE_DETACHED);
            mContext.registerReceiver(mUsbReceiver, filter);
            listening = true;
            if (mListener != null) {
                mListener.onDeviceConnected();
            }
        }
    }

    public void close() {
        if (listening) {
            mContext.unregisterReceiver(mUsbReceiver);
            listening = false;
        }
        if (mListener != null) {
            mListener.onDeviceDisconnected();
        }
    }

    private final BroadcastReceiver mUsbReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (UsbManager.ACTION_USB_DEVICE_DETACHED.equals(action)) {
                synchronized (this) {
                    close();
                }
            }
        }
    };

    public void registerListener(SpiMasterListener listener) {
        mListener = listener;
    }

    public void unregisterListener(SpiMasterListener listener) {
        if (mListener == listener) {
            mListener = null;
        }
    }


    public synchronized int singleWrite(byte[] writeBuffer, int sizeToTransfer) {
        int TIMEOUT = 0;
        int nbr_end_point;
        int i;
        int result = 0;
        UsbEndpoint endpoint;
        manager = (UsbManager) mContext.getSystemService(Context.USB_SERVICE);
        HashMap<String, UsbDevice> deviceList = manager.getDeviceList();
        if (!deviceList.isEmpty()) {
            UsbDevice device = deviceList.values().iterator().next();
            if (device != null) {
                if (device.getVendorId() == VendorId && device.getProductId() == ProductId) {
                    UsbInterface intf = device.getInterface(0);
                    nbr_end_point = intf.getEndpointCount();
                    for (i = 0; i < nbr_end_point; i++) {
                        endpoint = intf.getEndpoint(i);
                        if (endpoint.getDirection() == UsbConstants.USB_DIR_OUT &&
                                endpoint.getType() == UsbConstants.USB_ENDPOINT_XFER_BULK) {
                            UsbDeviceConnection connection = manager.openDevice(device);
                            if (connection.claimInterface(intf, true)) {
                                result = connection.bulkTransfer(endpoint, writeBuffer, sizeToTransfer, TIMEOUT);
                            }
                            connection.close();
                        }
                    }
                }
            }
        }
        return result;
    }

    public synchronized int singleRead(byte[] readBuffer, int sizeToTransfer) {
        int TIMEOUT = 0;
        int nbr_end_point;
        int i;
        int result = 0;
        UsbEndpoint endpoint;
        manager = (UsbManager) mContext.getSystemService(Context.USB_SERVICE);
        HashMap<String, UsbDevice> deviceList = manager.getDeviceList();
        if (!deviceList.isEmpty()) {
            UsbDevice device = deviceList.values().iterator().next();
            if (device != null) {
                if (device.getVendorId() == VendorId && device.getProductId() == ProductId) {
                    UsbInterface intf = device.getInterface(0);
                    nbr_end_point = intf.getEndpointCount();
                    for (i = 0; i < nbr_end_point; i++) {
                        endpoint = intf.getEndpoint(i);
                        if (endpoint.getDirection() == UsbConstants.USB_DIR_IN &&
                                endpoint.getType() == UsbConstants.USB_ENDPOINT_XFER_BULK) {
                            UsbDeviceConnection connection = manager.openDevice(device);
                            if (connection.claimInterface(intf, true)) {
                                result = connection.bulkTransfer(endpoint, readBuffer, sizeToTransfer, TIMEOUT);
                            }
                            connection.close();
                        }
                    }
                }
            }
        }
        return result;
    }

}
