package com.classycode.nfchttptun;

import android.content.Context;
import android.content.Intent;
import android.nfc.cardemulation.HostApduService;
import android.os.Bundle;
import android.os.Vibrator;
import android.support.v4.content.LocalBroadcastManager;
import android.util.Log;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.Arrays;

/**
 * @author Alex Suzuki, Classy Code GmbH, 2017
 */
public class TunnelApduService extends HostApduService {

    public static final String BROADCAST_INTENT_LINK_ESTABLISHED = "LINK_ESTABLISHED";
    public static final String BROADCAST_INTENT_PROGRESS_UPDATED = "PROGRESS_UPDATED";
    public static final String BROADCAST_INTENT_DATA_RECEIVED = "DATA_RECEIVED";
    public static final String BROADCAST_INTENT_REQUEST_SENT = "REQUEST_SENT";
    public static final String BROADCAST_INTENT_LINK_DEACTIVATED = "LINK_DEACTIVATED";

    private static final String TAG = Constants.LOG_TAG;

    // the response sent from the phone if it does not understand an APDU
    private static final byte[] UNKNOWN_COMMAND_RESPONSE = {(byte) 0xff};

    // the SELECT AID APDU issued by the terminal
    // our AID is 0xF0ABCDEF0000
    private static final byte[] SELECT_AID_COMMAND = {
            (byte) 0x00, // Class
            (byte) 0xA4, // Instruction
            (byte) 0x04, // Parameter 1
            (byte) 0x00, // Parameter 2
            (byte) 0x06, // length
            (byte) 0xF0,
            (byte) 0xAB,
            (byte) 0xCD,
            (byte) 0xEF,
            (byte) 0x00,
            (byte) 0x00
    };

    // OK status sent in response to SELECT AID command (0x9000)
    private static final byte[] SELECT_RESPONSE_OK = {(byte) 0x90, (byte) 0x00};

    // Custom protocol commands issued by terminal
    private static final byte READ_URL_COMMAND = (byte) 0x01;
    private static final byte DATA_COMMAND = (byte) 0x02;

    // Custom protocol responses by phone
    private static final byte READ_URL_RESPONSE = (byte) 0x00;
    private static final byte DATA_RESPONSE_OK = (byte) 0x00;
    private static final byte DATA_RESPONSE_NOK = (byte) 0x01;

    private boolean isProcessing;

    private int nextSeqNo;
    private ByteArrayOutputStream buffer;
    private String url;
    
    @Override
    public void onCreate() {
        super.onCreate();

        isProcessing = false;
    }

    private byte[] getUrlBytes() {
        try {
            return TunnelSettings.getUrl(this).getBytes("ASCII");
        } catch (UnsupportedEncodingException e) {
            throw new IllegalStateException(e); // never happens
        }
    }

    @Override
    public byte[] processCommandApdu(byte[] commandApdu, Bundle extras) {

        if (!isProcessing) {
            isProcessing = true;
            buffer = new ByteArrayOutputStream(262144); // 256 KB buffer
            nextSeqNo = 0;
        }

        if (Arrays.equals(SELECT_AID_COMMAND, commandApdu)) {
            Log.i(TAG, "Link established");
            notifyLinkEstablished();
            return SELECT_RESPONSE_OK;
        } else if (commandApdu[0] == READ_URL_COMMAND) {
            byte[] urlToDownload = getUrlBytes();
            byte[] responseApdu = new byte[urlToDownload.length + 1];
            responseApdu[0] = READ_URL_RESPONSE;
            System.arraycopy(urlToDownload, 0, responseApdu, 1, urlToDownload.length);
            notifyRequestSent();
            return responseApdu;
        } else if (commandApdu[0] == DATA_COMMAND) {
            try {
                final DataInputStream in = new DataInputStream(new ByteArrayInputStream(commandApdu, 1, commandApdu.length - 1));
                final int seqNo = in.readUnsignedShort();
                final int totalPackets = in.readUnsignedShort();
                if (seqNo != nextSeqNo) {
                    Log.e(TAG, "Received seq no " + seqNo + " does not match expected seq no: " + nextSeqNo);
                    return new byte[]{DATA_RESPONSE_NOK};
                }
                buffer.write(commandApdu, 5, commandApdu.length - 5);
                nextSeqNo++;

                Log.d(TAG, "Received packet " + seqNo + ": " + (commandApdu.length - 5) + " bytes");
                notifyProgressUpdate(nextSeqNo, totalPackets);

                if (nextSeqNo == totalPackets) {
                    notifyDataReceived(buffer.toByteArray());
                }
                return new byte[]{DATA_RESPONSE_OK};
            } catch (IOException ex) {
                Log.e(TAG, "Failed to parse data command", ex);
                return new byte[]{DATA_RESPONSE_NOK};
            }
        } else {
            Log.e(TAG, "Terminal sent unknown command: " + HexEncoder.convertByteArrayToHexString(commandApdu));
            return UNKNOWN_COMMAND_RESPONSE;
        }
    }

    @Override
    public void onDeactivated(int reason) {
        Log.d(TAG, "Link deactivated: " + reason);

        isProcessing = false;
        notifyLinkDeactivated(reason);
    }

    private void notifyLinkEstablished() {
        Vibrator v = (Vibrator) getApplicationContext().getSystemService(Context.VIBRATOR_SERVICE);
        v.vibrate(200);

        LocalBroadcastManager.getInstance(getApplicationContext()).sendBroadcast(new Intent(BROADCAST_INTENT_LINK_ESTABLISHED));
    }

    private void notifyRequestSent() {
        Intent intent = new Intent(BROADCAST_INTENT_REQUEST_SENT);
        LocalBroadcastManager.getInstance(getApplicationContext()).sendBroadcast(intent);
    }

    private void notifyProgressUpdate(int numReceived, int numTotal) {
        Intent intent = new Intent(BROADCAST_INTENT_PROGRESS_UPDATED);
        intent.putExtra("numReceived", numReceived);
        intent.putExtra("numTotal", numTotal);
        LocalBroadcastManager.getInstance(getApplicationContext()).sendBroadcast(intent);
    }

    private void notifyDataReceived(byte[] data) {
        Intent intent = new Intent(BROADCAST_INTENT_DATA_RECEIVED);
        intent.putExtra("data", data);
        LocalBroadcastManager.getInstance(getApplicationContext()).sendBroadcast(intent);
    }

    private void notifyLinkDeactivated(int reason) {
        Intent intent = new Intent(BROADCAST_INTENT_LINK_DEACTIVATED);
        intent.putExtra("reason", reason);
        LocalBroadcastManager.getInstance(getApplicationContext()).sendBroadcast(intent);
    }

}
