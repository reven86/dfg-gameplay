
package com.dreamfarmgames.util;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.content.Intent;
import android.content.IntentFilter;

import com.dreamfarmgames.util.IabBroadcastReceiver;
import com.dreamfarmgames.util.IabBroadcastReceiver.IabBroadcastListener;
import com.dreamfarmgames.util.IabHelper;
import com.dreamfarmgames.util.IabHelper.IabAsyncInProgressException;
import com.dreamfarmgames.util.IabResult;
import com.dreamfarmgames.util.Inventory;
import com.dreamfarmgames.util.Purchase;
import com.dreamfarmgames.util.SkuDetails;

import org.gameplay3d.GamePlayNativeActivity;

import java.util.List;
import java.util.ArrayList;

import org.json.JSONException;
import org.json.JSONStringer;



public class DFGActivity extends GamePlayNativeActivity implements IabBroadcastListener {

    // Debug tag, for logging
    static final String TAG = "DFGGame";

    // (arbitrary) request code for the purchase flow
    static final int RC_REQUEST = 10001;

    // The helper object
    IabHelper mHelper;

    // Provides purchase notification while this app is running
    IabBroadcastReceiver mBroadcastReceiver;

    List<String> mSKUDetailsQueue;
    GamePlayNativeActivity _IABCallbacks;


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        mSKUDetailsQueue = new ArrayList<String>();
        _IABCallbacks = this;
    }


    public void setupIABHelper(String base64EncodedPublicKey)
    {
        /* base64EncodedPublicKey should be YOUR APPLICATION'S PUBLIC KEY
         * (that you got from the Google Play developer console). This is not your
         * developer public key, it's the *app-specific* public key.
         *
         * Instead of just storing the entire literal string here embedded in the
         * program,  construct the key at runtime from pieces or
         * use bit manipulation (for example, XOR with some other string) to hide
         * the actual key.  The key itself is not secret information, but we don't
         * want to make it easy for an attacker to replace the public key with one
         * of their own and then fake messages from the server.
         */

        // Some sanity checks to see if the developer (that's you!) really followed the
        // instructions to run this sample (don't put these checks on your app!)
        if (base64EncodedPublicKey.contains("CONSTRUCT_YOUR")) {
            throw new RuntimeException("Please put your app's public key in MainActivity.java. See README.");
        }
        if (getPackageName().startsWith("com.example")) {
            throw new RuntimeException("Please change the sample's package name! See README.");
        }

        // Create the helper, passing it our context and the public key to verify signatures with
        Log.d(TAG, "Creating IAB helper.");
        mHelper = new IabHelper(this, base64EncodedPublicKey);

        // enable debug logging (for a production application, you should set this to false).
        mHelper.enableDebugLogging(false);

        // Start setup. This is asynchronous and the specified listener
        // will be called once setup completes.
        Log.d(TAG, "Starting setup.");
        receiptReceived("[]");
        mHelper.startSetup(new IabHelper.OnIabSetupFinishedListener() {
            public void onIabSetupFinished(IabResult result) {
                Log.d(TAG, "Setup finished.");

                if (!result.isSuccess()) {
                    // Oh noes, there was a problem.
                    _IABCallbacks.getProductsFailed(result.getResponse(), result.getMessage());
                    complain("Problem setting up in-app billing: " + result);
                    return;
                }

                _IABCallbacks.setIABEnabled();

                // Have we been disposed of in the meantime? If so, quit.
                if (mHelper == null)
                    return;

                // Important: Dynamically register for broadcast messages about updated purchases.
                // We register the receiver here instead of as a <receiver> in the Manifest
                // because we always call getPurchases() at startup, so therefore we can ignore
                // any broadcasts sent while the app isn't running.
                // Note: registering this listener in an Activity is a bad idea, but is done here
                // because this is a SAMPLE. Regardless, the receiver must be registered after
                // IabHelper is setup, but before first call to getPurchases().
                mBroadcastReceiver = new IabBroadcastReceiver(DFGActivity.this);
                IntentFilter broadcastFilter = new IntentFilter(IabBroadcastReceiver.ACTION);
                registerReceiver(mBroadcastReceiver, broadcastFilter);
            }
        });
    }

    @Override
    public void restorePurchases()
    {
        this.runOnUiThread(new Runnable(){
            public void run() {
            try {
                Log.d(TAG, "restorePurchases");
                mHelper.queryInventoryAsync(mGotInventoryListener);
            } catch (Exception e) {
                Log.e(TAG, "Exception while communicating with IABHelper");
                e.printStackTrace();
            }
            }
        });
    }

    @Override
    public void receivedBroadcast() {
        // Received a broadcast notification that the inventory of items has changed
        Log.d(TAG, "Received broadcast notification. Querying inventory.");
        try {
            mHelper.queryInventoryAsync(mGotInventoryListener);
        } catch (IabAsyncInProgressException e) {
            complain("Error querying inventory. Another async operation in progress.");
        }
    }

    @Override
    public void queueSkuDetailsRequest(String sku)
    {
        mSKUDetailsQueue.add(sku);
    }

    @Override
    public void flushSkuDetailsQueue()
    {
        this.runOnUiThread(new Runnable(){
            public void run() {
            try {
                Log.d(TAG, "flushSkuDetailsQueue");
                mHelper.queryInventoryAsync(true, mSKUDetailsQueue, new ArrayList<String>(), mGotInventoryListener);
            } catch (Exception e) {
                Log.e(TAG, "Exception while communicating with IABHelper");
                e.printStackTrace();
            }
            }
        });
    }

    @Override
    public void purchaseItem(final String sku)
    {
        if (mHelper == null)
            return;

        final Activity activity = this;

        this.runOnUiThread(new Runnable(){
            public void run() {
            try {
                String payload = "";
                mHelper.launchPurchaseFlow(activity, sku, RC_REQUEST,
                        mPurchaseFinishedListener, payload);
            } catch (Exception e) {
                Log.e(TAG, "Exception while communicating with IABHelper");
                e.printStackTrace();
            }
            }
        });
    }

    void updateReceipt(Inventory inventory)
    {
            int i = 0;
            String receipt = "[";
            for (Purchase p : inventory.getAllPurchases())
            {
                if (i > 0)
                    receipt += ", ";
                i++;

                try
                {
                    receipt += new JSONStringer().object()
                                    .key("itemType").value(p.getItemType())
                                    .key("data").value(p.getOriginalJson())
                                    .key("signature").value(p.getSignature())
                                    .endObject().toString();
                }
                catch(JSONException e)
                {
                    Log.w(TAG, "Failed to generate receipt string.");
                }
            }
            receipt += "]";
            //Log.i(TAG, "receipt " + receipt);
            receiptReceived(receipt);
    }

    // Listener that's called when we finish querying the items and subscriptions we own
    IabHelper.QueryInventoryFinishedListener mGotInventoryListener = new IabHelper.QueryInventoryFinishedListener() {
        public void onQueryInventoryFinished(IabResult result, Inventory inventory) {
            Log.d(TAG, "Query inventory finished.");
            mSKUDetailsQueue.clear();

            // Have we been disposed of in the meantime? If so, quit.
            if (mHelper == null)
                return;

            // Is it a failure?
            if (result.isFailure()) {
                _IABCallbacks.getProductsFailed(result.getResponse(), result.getMessage());
                complain("Failed to query inventory: " + result);
                return;
            }

            Log.d(TAG, "Query inventory was successful.");

            /*
             * Check for items we own. Notice that for each purchase, we check
             * the developer payload to see if it's correct! See
             * verifyDeveloperPayload().
             */
            for (SkuDetails d : inventory.getAllSKUs())
            {
                _IABCallbacks.productValidated(d.getSku(), d.getPrice(), d.getTitle(), d.getDescription(), Long.toString(d.getPriceAmountMicros()), d.getPriceCurrencyCode());
            }
            _IABCallbacks.finishProductsValidation();

            for (Purchase p : inventory.getAllPurchases())
            {
                if (verifyDeveloperPayload(p))
                {
                    if (_IABCallbacks.isItemConsumable(p.getSku()) != 0)
                    {
                        try
                        {
                            mHelper.consumeAsync(p, mConsumeFinishedListener);
                        } catch (IabAsyncInProgressException e) {
                            complain("Error consuming item. Another async operation in progress.");
                        }
                    }
                    else
                    {
                        _IABCallbacks.itemRestored(p.getSku(), p.getPurchaseTime(), p.getOrderId());
                    }
                }
            }

            updateReceipt(inventory);

            Log.d(TAG, "Initial inventory query finished; enabling main UI.");
        }
    };

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        Log.d(TAG, "onActivityResult(" + requestCode + "," + resultCode + "," + data);
        if (mHelper == null) return;

        // Pass on the activity result to the helper for handling
        if (!mHelper.handleActivityResult(requestCode, resultCode, data)) {
            // not handled, so handle it ourselves (here's where you'd
            // perform any handling of activity results not related to in-app
            // billing...
            super.onActivityResult(requestCode, resultCode, data);
        }
        else {
            Log.d(TAG, "onActivityResult handled by IABUtil.");
        }
    }

    /** Verifies the developer payload of a purchase. */
    boolean verifyDeveloperPayload(Purchase p) {
        String payload = p.getDeveloperPayload();

        /*
         * TODO: verify that the developer payload of the purchase is correct. It will be
         * the same one that you sent when initiating the purchase.
         *
         * WARNING: Locally generating a random string when starting a purchase and
         * verifying it here might seem like a good approach, but this will fail in the
         * case where the user purchases an item on one device and then uses your app on
         * a different device, because on the other device you will not have access to the
         * random string you originally generated.
         *
         * So a good developer payload has these characteristics:
         *
         * 1. If two different users purchase an item, the payload is different between them,
         *    so that one user's purchase can't be replayed to another user.
         *
         * 2. The payload must be such that you can verify it even when the app wasn't the
         *    one who initiated the purchase flow (so that items purchased by the user on
         *    one device work on other devices owned by the user).
         *
         * Using your own server to store and verify developer payloads across app
         * installations is recommended.
         */

        return true;
    }

    // Callback for when a purchase is finished
    IabHelper.OnIabPurchaseFinishedListener mPurchaseFinishedListener = new IabHelper.OnIabPurchaseFinishedListener() {
        public void onIabPurchaseFinished(IabResult result, Purchase purchase) {
            Log.d(TAG, "Purchase finished: " + result + ", purchase: " + purchase);

            // if we were disposed of in the meantime, quit.
            if (mHelper == null) return;

            if (result.isFailure()) {
                _IABCallbacks.itemPurchaseFailed(purchase == null ? "<error>" : purchase.getSku(), result.getResponse(), result.getMessage());
                complain("Error purchasing: " + result);
                return;
            }
            if (!verifyDeveloperPayload(purchase)) {
                complain("Error purchasing. Authenticity verification failed.");
                return;
            }

            if (_IABCallbacks.isItemConsumable(purchase.getSku()) != 0)
            {
                try {
                    mHelper.consumeAsync(purchase, mConsumeFinishedListener);
                } catch (IabAsyncInProgressException e) {
                    complain("Error consuming item. Another async operation in progress.");
                }
            }
            else
            {
                try
                {
                    updateReceipt(mHelper.queryInventory());    // sync call to update receipt before calling itemPurchased event
                }
                catch(IabException e)
                {
                    complain("Error requesting receipt.");
                }

                _IABCallbacks.itemPurchased(purchase.getSku(), purchase.getPurchaseTime(), purchase.getOrderId());
            }

            Log.d(TAG, "Purchase successful.");
        }
    };

    // Called when consumption is complete
    IabHelper.OnConsumeFinishedListener mConsumeFinishedListener = new IabHelper.OnConsumeFinishedListener() {
        public void onConsumeFinished(Purchase purchase, IabResult result) {
            Log.d(TAG, "Consumption finished. Purchase: " + purchase + ", result: " + result);

            // if we were disposed of in the meantime, quit.
            if (mHelper == null) return;

            // We know this is the "gas" sku because it's the only one we consume,
            // so we don't check which sku was consumed. If you have more than one
            // sku, you probably should check...
            if (result.isSuccess()) {
                // successfully consumed, so we apply the effects of the item in our
                // game world's logic, which in our case means filling the gas tank a bit
                Log.d(TAG, "Consumption successful. Provisioning.");

                try
                {
                    updateReceipt(mHelper.queryInventory());    // sync call to update receipt before calling itemPurchased event
                }
                catch(IabException e)
                {
                    complain("Error requesting receipt.");
                }

                _IABCallbacks.itemPurchased(purchase.getSku(), purchase.getPurchaseTime(), purchase.getOrderId());
            }
            else {
                _IABCallbacks.itemPurchaseFailed(purchase == null ? "<error>" : purchase.getSku(), result.getResponse(), result.getMessage());
                complain("Error while consuming: " + result);
            }
            Log.d(TAG, "End consumption flow.");
        }
    };

    // We're being destroyed. It's important to dispose of the helper here!
    @Override
    public void onDestroy() {
        super.onDestroy();

        // very important:
        if (mBroadcastReceiver != null) {
            unregisterReceiver(mBroadcastReceiver);
        }

        // very important:
        Log.d(TAG, "Destroying helper.");
        if (mHelper != null) {
            try {
                mHelper.dispose();
            } catch (IabAsyncInProgressException e) {
                complain("Another async operation in progress.");
            }
            mHelper = null;
        }
    }

    void complain(String message) {
        Log.e(TAG, "**** Error: " + message);
        //alert("Error: " + message);
    }

    void alert(String message) {
        //AlertDialog.Builder bld = new AlertDialog.Builder(this);
        //bld.setMessage(message);
        //bld.setNeutralButton("OK", null);
        //Log.d(TAG, "Showing alert dialog: " + message);
        //bld.create().show();
    }

    public void onBackButton()
    {
        Intent startMain = new Intent(Intent.ACTION_MAIN);
        startMain.addCategory(Intent.CATEGORY_HOME);
        startMain.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        startActivity(startMain);
    }

    public native void receiptReceived(String text);
}
