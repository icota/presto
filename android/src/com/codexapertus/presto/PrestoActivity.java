package com.codexapertus.presto;

import java.lang.String;
import java.io.UnsupportedEncodingException;

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
