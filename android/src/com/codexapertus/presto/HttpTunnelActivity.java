//package com.codexapertus.presto;

//import android.content.BroadcastReceiver;
//import android.content.Context;
//import android.content.DialogInterface;
//import android.content.Intent;
//import android.content.IntentFilter;
//import android.nfc.NfcAdapter;
//import android.os.Bundle;
//import android.provider.Settings;
//import android.support.annotation.Nullable;
//import android.support.v4.content.LocalBroadcastManager;
//import android.support.v7.app.AlertDialog;
//import android.support.v7.app.AppCompatActivity;
//import android.support.v7.widget.Toolbar;
//import android.util.Log;
//import android.view.Menu;
//import android.view.MenuItem;
//import android.view.View;
//import android.webkit.WebView;
//import android.widget.ProgressBar;
//import android.widget.TextView;

//import java.io.UnsupportedEncodingException;
//import java.net.MalformedURLException;
//import java.net.URL;

///**
// * @author Alex Suzuki, Classy Code GmbH, 2017
// */
//public class HttpTunnelActivity extends AppCompatActivity implements SetUrlDialogFragment.SetUrlDialogListener {

//    private static final String TAG = Constants.LOG_TAG;

//    private TextView statusLabel;
//    private WebView webView;
//    private ProgressBar progressBar;
//    private LocalBroadcastManager lbm;
//    private long requestStartTimeMillis;
//    private boolean dataReceived;

//    @Override
//    protected void onCreate(@Nullable Bundle savedInstanceState) {
//        super.onCreate(savedInstanceState);

//        setContentView(R.layout.activity_http_tunnel);

//        webView = (WebView) findViewById(R.id.web_view);
//        statusLabel = (TextView) findViewById(R.id.status_label);
//        progressBar = (ProgressBar) findViewById(R.id.progress_bar);
//        setSupportActionBar((Toolbar) findViewById(R.id.toolbar));
//        progressBar.setVisibility(View.INVISIBLE);
//        lbm = LocalBroadcastManager.getInstance(this);
//    }

//    @Override
//    protected void onResume() {
//        super.onResume();

//        checkForNFC();

//        lbm.registerReceiver(linkEstablishedReceiver, new IntentFilter(LightningApduService.BROADCAST_INTENT_LINK_ESTABLISHED));
//        lbm.registerReceiver(requestSentReceiver, new IntentFilter(LightningApduService.BROADCAST_INTENT_REQUEST_SENT));
//        lbm.registerReceiver(progressUpdateReceiver, new IntentFilter(LightningApduService.BROADCAST_INTENT_PROGRESS_UPDATED));
//        lbm.registerReceiver(dataReceivedReceiver, new IntentFilter(LightningApduService.BROADCAST_INTENT_DATA_RECEIVED));
//        lbm.registerReceiver(linkDeactivatedReceiver, new IntentFilter(LightningApduService.BROADCAST_INTENT_LINK_DEACTIVATED));

//        statusLabel.setText(R.string.tunnel_status_ready);
//    }

//    @Override
//    protected void onPause() {
//        lbm.unregisterReceiver(linkEstablishedReceiver);
//        lbm.unregisterReceiver(requestSentReceiver);
//        lbm.unregisterReceiver(progressUpdateReceiver);
//        lbm.unregisterReceiver(dataReceivedReceiver);
//        lbm.unregisterReceiver(linkDeactivatedReceiver);

//        super.onPause();
//    }

//    @Override
//    public boolean onCreateOptionsMenu(Menu menu) {
//        getMenuInflater().inflate(R.menu.main_menu, menu);
//        return true;
//    }

//    @Override
//    public boolean onOptionsItemSelected(MenuItem item) {
//        if (item.getItemId() == R.id.action_set_url) {
//            final SetUrlDialogFragment setUrlDialog = new SetUrlDialogFragment();
//            setUrlDialog.show(getSupportFragmentManager(), null);
//            return true;

//        } else {
//            return super.onOptionsItemSelected(item);
//        }
//    }

//    @Override
//    public void onSetUrlDialogCancelled() {
//    }

//    @Override
//    public void onSetUrlDialogSucceeded(String urlSpec) {
//        TunnelSettings.setUrl(this, urlSpec);
//    }

//    private void checkForNFC() {
//        NfcAdapter nfcAdapter = NfcAdapter.getDefaultAdapter(getApplicationContext());
//        if (!nfcAdapter.isEnabled()) {
//            final AlertDialog.Builder builder = new AlertDialog.Builder(this);
//            builder.setTitle(R.string.error_nfc_disabled_title);
//            builder.setMessage(R.string.error_nfc_disabled_message);
//            builder.setPositiveButton(android.R.string.ok, new DialogInterface.OnClickListener() {
//                @Override
//                public void onClick(DialogInterface dialog, int which) {
//                    startActivity(new Intent(Settings.ACTION_NFC_SETTINGS));
//                }
//            });
//            builder.setNegativeButton(android.R.string.cancel, new DialogInterface.OnClickListener() {
//                @Override
//                public void onClick(DialogInterface dialog, int which) {
//                }
//            });
//            builder.show();
//        }
//    }

//    private BroadcastReceiver linkEstablishedReceiver = new BroadcastReceiver() {
//        @Override
//        public void onReceive(Context context, Intent intent) {
//            statusLabel.setText(R.string.tunnel_status_link_established);
//            webView.setVisibility(View.INVISIBLE);
//            progressBar.setVisibility(View.VISIBLE);
//            dataReceived = false;
//        }
//    };

//    private BroadcastReceiver requestSentReceiver = new BroadcastReceiver() {
//        @Override
//        public void onReceive(Context context, Intent intent) {
//            statusLabel.setText(R.string.tunnel_status_request_sent);

//            // record start time for rate calculation
//            requestStartTimeMillis = System.currentTimeMillis();
//        }
//    };

//    private BroadcastReceiver progressUpdateReceiver = new BroadcastReceiver() {
//        @Override
//        public void onReceive(Context context, Intent intent) {
//            final int packetsReceived = intent.getIntExtra("numReceived", -1);
//            final int totalPackets = intent.getIntExtra("numTotal", -1);
//            statusLabel.setText(getString(R.string.tunnel_status_progress, packetsReceived, totalPackets));
//        }
//    };

//    private BroadcastReceiver dataReceivedReceiver = new BroadcastReceiver() {
//        @Override
//        public void onReceive(Context context, Intent intent) {
//            dataReceived = true;
//            final long elapsedTimeMillis = System.currentTimeMillis() - requestStartTimeMillis;
//            final byte[] data = intent.getByteArrayExtra("data");
//            try {
//                // we assume it's HTML, encoded in UTF-8
//                webView.loadData(new String(data, "UTF-8"), "text/html", "UTF-8");
//            } catch (UnsupportedEncodingException e) {
//                throw new IllegalStateException(e); // should never happen
//            }
//            webView.setVisibility(View.VISIBLE);

//            // calculate transmission rate
//            final double kbPerSec = (data.length / (elapsedTimeMillis / 1000.0)) / 1000.0;
//            statusLabel.setText(getString(R.string.tunnel_status_data_received, data.length / 1000.0, kbPerSec));
//        }
//    };

//    private BroadcastReceiver linkDeactivatedReceiver = new BroadcastReceiver() {
//        @Override
//        public void onReceive(Context context, Intent intent) {
//            if (!dataReceived) { // link terminated before data was received
//                statusLabel.setText(R.string.tunnel_status_link_deactivated);
//            }

//            progressBar.setVisibility(View.INVISIBLE);
//        }
//    };

//}
