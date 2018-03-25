package com.codexapertus.presto;

import android.content.BroadcastReceiver;
import android.support.v4.content.LocalBroadcastManager;
import android.content.IntentFilter;

import android.content.Context;
import android.content.Intent;
import android.nfc.cardemulation.HostApduService;
import android.os.Bundle;
import android.util.Log;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.util.Arrays;

/**
 * @author Igor Cota, Codex Apertus d.o.o., 2018
 */
public class LightningApduService extends HostApduService {

    public static final String ACTION_BOLT11_RECEIVED = "lightning.action.BOLT11_RECEIVED";
    public static final String ACTION_ID_CHANGED = "lightning.action.ID_CHANGED";
    public static final String ACTION_FORWARD_SOCKET = "lightning.action.FORWARD_SOCKET";

    public static final String BROADCAST_INTENT_PROGRESS_UPDATED = "PROGRESS_UPDATED";
    public static final String BROADCAST_INTENT_DATA_RECEIVED = "DATA_RECEIVED";
    public static final String BROADCAST_INTENT_REQUEST_SENT = "REQUEST_SENT";
    public static final String BROADCAST_INTENT_LINK_DEACTIVATED = "LINK_DEACTIVATED";

    private final BroadcastReceiver lightningIdChangedReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {
            if (intent != null && ACTION_ID_CHANGED.equals(intent.getAction())) {
                Bundle bundle = intent.getExtras();
                lightningPeerId = bundle.getByteArray("id");
            }
        }
    };

    private static final String TAG = "presto-apdu";

    // the response sent from the phone if it does not understand an APDU
    private static final byte[] UNKNOWN_COMMAND_RESPONSE = {(byte) 0xff};

    // the SELECT AID APDU issued by the terminal
    // our AID is 0xF04C494748544E494E47
    private static final byte[] SELECT_AID_COMMAND = {
            (byte) 0x00, // Class
            (byte) 0xA4, // Instruction
            (byte) 0x04, // Parameter 1
            (byte) 0x00, // Parameter 2
            (byte) 0x0A, // Length
            (byte) 0xF0, // Custom AID
            (byte) 0x4C, // L
            (byte) 0x49, // I
            (byte) 0x47, // G
            (byte) 0x48, // H
            (byte) 0x54, // T
            (byte) 0x4E, // N
            (byte) 0x49, // I
            (byte) 0x4E, // N
            (byte) 0x47  // G
    };

    // OK status sent in response to SELECT AID command (0x9000)
    private static final byte[] SELECT_RESPONSE_OK = {(byte) 0x90, (byte) 0x00};

    // Protocol commands issued by terminal
    private static final byte BOLT11_COMMAND = (byte) 0x01;

    // Protocol commands issued by phone
    private static final byte NFC_SOCKET_COMMAND = (byte) 0x02;

    // Custom protocol responses by phone
    private static final byte[] BOLT11_RECEIVED_NO_SOCKET = {(byte) 0x03};

    private static final byte NFC_SOCKET_STREAM = (byte) 0x04;
    private static final byte[] NFC_SOCKET_STREAM_NO_DATA = {(byte) 0x05};
    private static final byte[] DATA_RESPONSE_OK = {(byte) 0x00};
    private static final byte DATA_RESPONSE_NOK = (byte) 0x01;

    private byte[] lightningPeerId;

    private boolean lightningOnline;
    private boolean isProcessing;
    private boolean isReceivingBolt11;

    private ByteArrayOutputStream socketSendBuffer;
    private ByteArrayOutputStream bolt11receiveBuffer;
    
    @Override
    public void onCreate() {
        Log.i(TAG, "Created");
        super.onCreate();

        LocalBroadcastManager.getInstance(this).registerReceiver(lightningIdChangedReceiver,
                new IntentFilter(ACTION_ID_CHANGED));

        socketSendBuffer = new ByteArrayOutputStream();
        bolt11receiveBuffer = new ByteArrayOutputStream();
        isProcessing = false;
        isReceivingBolt11 = false;
        lightningOnline = false;
        //lightningPeerId = "testtest";
    }

    @Override
    public byte[] processCommandApdu(byte[] commandApdu, Bundle extras) {
        Log.i(TAG, "processCommandApdu");

        if (Arrays.equals(SELECT_AID_COMMAND, commandApdu)) {
            Log.i(TAG, "Link established");
            return SELECT_RESPONSE_OK;
        }
        else if (commandApdu[0] == BOLT11_COMMAND) {
            final int seqNo = commandApdu[1];
            final int totalPackets = commandApdu[2];

            if (seqNo == 0 && totalPackets > 1) {
                bolt11receiveBuffer = new ByteArrayOutputStream();
            }

            bolt11receiveBuffer.write(commandApdu, 3, commandApdu.length - 3);

            if (totalPackets > 1) {
                isReceivingBolt11 = true;
                if (seqNo == totalPackets - 1) { // Last packet
                    return checkIfWeAreOnlineAndReply();
                }
                return DATA_RESPONSE_OK;
            }
            else {
                return checkIfWeAreOnlineAndReply();
            }
        }
        else if (commandApdu[0] == NFC_SOCKET_COMMAND) {
            return UNKNOWN_COMMAND_RESPONSE;
        }
        else if (commandApdu[0] == NFC_SOCKET_STREAM) {
            // forward incoming data to socket
            // empty our buffer
            if (socketSendBuffer.size() <= 0) {
                // Nothing to send
                return NFC_SOCKET_STREAM_NO_DATA;
            }
            else {
                ByteArrayOutputStream commandOutputStream = new ByteArrayOutputStream();
                commandOutputStream.write(NFC_SOCKET_STREAM);
                try {
                    commandOutputStream.write(socketSendBuffer.toByteArray());
                } catch (IOException e) {
                    e.printStackTrace();
                }

                return commandOutputStream.toByteArray();
            }
        }
        else {
            return UNKNOWN_COMMAND_RESPONSE;
        }
    }

    @Override
    public void onDeactivated(int reason) {
        Log.d(TAG, "Link deactivated: " + reason);

        isProcessing = false;
        isReceivingBolt11 = false;
        notifyLinkDeactivated(reason);
    }

    private byte[] checkIfWeAreOnlineAndReply() {
        Intent intent = new Intent(ACTION_BOLT11_RECEIVED);
        intent.putExtra("bolt11", bolt11receiveBuffer.toString());
        startActivity(intent);

        if (!lightningOnline) {
            ByteArrayOutputStream commandOutputStream = new ByteArrayOutputStream();
            commandOutputStream.write(NFC_SOCKET_COMMAND);
            try {
                commandOutputStream.write(lightningPeerId);
            } catch (IOException e) {
                e.printStackTrace();
            }

            return commandOutputStream.toByteArray();
        }
        else {
            return BOLT11_RECEIVED_NO_SOCKET;
        }

    }

    private void notifyRequestSent() {
        Log.d(TAG, "notifyRequestSent");
        Intent intent = new Intent(BROADCAST_INTENT_REQUEST_SENT);
        //LocalBroadcastManager.getInstance(getApplicationContext()).sendBroadcast(intent);
    }

    private void notifyProgressUpdate(int numReceived, int numTotal) {
        Log.d(TAG, "notifyProgressUpdate: ");
        Intent intent = new Intent(BROADCAST_INTENT_PROGRESS_UPDATED);
        intent.putExtra("numReceived", numReceived);
        intent.putExtra("numTotal", numTotal);
        //LocalBroadcastManager.getInstance(getApplicationContext()).sendBroadcast(intent);
    }

    private void notifyDataReceived(byte[] data) {
        Log.d(TAG, "notifyDataReceived: ");
        Intent intent = new Intent(BROADCAST_INTENT_DATA_RECEIVED);
        intent.putExtra("data", data);
        //LocalBroadcastManager.getInstance(getApplicationContext()).sendBroadcast(intent);
    }

    private void notifyLinkDeactivated(int reason) {
        Log.d(TAG, "notifyLinkDeactivated: " + reason);
        Intent intent = new Intent(BROADCAST_INTENT_LINK_DEACTIVATED);
        intent.putExtra("reason", reason);
        //LocalBroadcastManager.getInstance(getApplicationContext()).sendBroadcast(intent);
    }

}
