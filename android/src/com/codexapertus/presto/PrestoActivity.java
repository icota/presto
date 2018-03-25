package com.codexapertus.presto;

import java.lang.String;
import java.io.UnsupportedEncodingException;

import android.support.v4.content.LocalBroadcastManager;

import android.util.Log;
import android.content.Intent;
import android.os.Bundle;
import android.os.Parcelable;

import android.nfc.NfcAdapter;
import android.nfc.NdefMessage;
import android.nfc.NdefRecord;

import com.codexapertus.presto.LightningApduService;

public class PrestoActivity extends org.qtproject.qt5.android.bindings.QtActivity
{
    public static native void bolt11Received(String bolt11);
    public static PrestoActivity s_activity = null;
    public byte[] m_ourId = null;

    void setId(byte[] id) {
        m_ourId = id;
        Intent intent = new Intent(LightningApduService.ACTION_ID_CHANGED);
        intent.putExtra("id", m_ourId);
        LocalBroadcastManager.getInstance(this).sendBroadcast(intent);
    }

    void forwardSocket(byte[] data) {
        Intent intent = new Intent(LightningApduService.ACTION_FORWARD_SOCKET);
        intent.putExtra("data", data);
        LocalBroadcastManager.getInstance(this).sendBroadcast(intent);
    }

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);

        if (intent != null && NfcAdapter.ACTION_NDEF_DISCOVERED.equals(intent.getAction())) {
            Parcelable[] rawMessages =
                intent.getParcelableArrayExtra(NfcAdapter.EXTRA_NDEF_MESSAGES);
            if (rawMessages != null) {
                NdefMessage[] messages = new NdefMessage[rawMessages.length];
                for (int i = 0; i < rawMessages.length; i++) {
                    messages[i] = (NdefMessage) rawMessages[i];
                }
                String bolt11 = new String();
                try{
                    bolt11 = new String(messages[0].getRecords()[0].getPayload(), "UTF-8");
                }
                    catch(UnsupportedEncodingException exc) {
                }

                bolt11Received(bolt11);
            }
        }

        if (intent != null && LightningApduService.ACTION_BOLT11_RECEIVED.equals(intent.getAction())) {
            String bolt11 = intent.getStringExtra("bolt11");
            if (bolt11 != null) {
                bolt11Received(bolt11);
            }
        }
    }

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        Log.d("presto", "onCreate");
        s_activity = this;
        Intent intent = new Intent(this, LightningApduService.class);
        startService(intent);

        super.onCreate(savedInstanceState);
    }

    @Override
    protected void onDestroy()
    {
        Log.d("presto", "onDestroy");
        super.onDestroy();
        s_activity = null;
    }
}
