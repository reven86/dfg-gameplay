
package com.dreamfarmgames.util;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.content.Intent;
import android.content.IntentFilter;

import com.android.billingclient.api.BillingClient;
import com.android.billingclient.api.BillingResult;
import com.dreamfarmgames.util.TextViewActivity;

import com.android.billingclient.api.SkuDetails;
import com.android.billingclient.api.Purchase;

import org.gameplay3d.GamePlayNativeActivity;

import java.util.List;
import java.util.ArrayList;

import org.json.JSONException;
import org.json.JSONStringer;



public class DFGActivity extends GamePlayNativeActivity implements BillingDataSource.BillingDataSourceCallbacks {

    // Debug tag, for logging
    static final String TAG = "DFGGame";

    List<String> _skuDetailsQueue;
    GamePlayNativeActivity _IABCallbacks;

    BillingDataSource billingDataSource;

    protected String BASE64_ENCODED_PUBLIC_KEY;


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        _IABCallbacks = this;
        _IABCallbacks.setIABEnabled();

        _skuDetailsQueue = new ArrayList<String>();
    }

    @Override
    public void restorePurchases()
    {
        this.runOnUiThread(new Runnable(){
            public void run() {
            try {
                Log.d(TAG, "restorePurchases");
                if (billingDataSource != null)
                    billingDataSource.refreshPurchasesAsync();
            } catch (Exception e) {
                Log.e(TAG, "Exception while communicating with IABHelper");
                e.printStackTrace();
            }
            }
        });
    }

    @Override
    public void queueSkuDetailsRequest(String sku)
    {
        _skuDetailsQueue.add(sku);
    }

    @Override
    public void flushSkuDetailsQueue()
    {
        DFGActivity activity = this;
        this.runOnUiThread(new Runnable(){
            public void run() {
            try {
                Log.d(TAG, "flushSkuDetailsQueue");

                if (billingDataSource == null) {

                    List<String> regularSKU = new ArrayList<String>();
                    List<String> subscriptionsSKU = new ArrayList<String>();
                    List<String> autoConsumeSKU = new ArrayList<String>();

                    for(String s : _skuDetailsQueue)
                    {
                        if (_IABCallbacks.isSubscription(s) != 0)
                            subscriptionsSKU.add(s);
                        else
                            regularSKU.add(s);
                        if (_IABCallbacks.isItemConsumable(s) != 0)
                            autoConsumeSKU.add(s);
                    };

                    billingDataSource = BillingDataSource.getInstance(
                            getApplication(),
                            regularSKU.toArray(new String[0]),
                            subscriptionsSKU.toArray(new String[0]),
                            autoConsumeSKU.toArray(new String[0]),
                            BASE64_ENCODED_PUBLIC_KEY);
                    billingDataSource.callbacks = activity;
                    billingDataSource.observeNewPurchases().observeForever(activity::onNewPurchase);
                }
                else {
                    billingDataSource.querySkuDetailsAsync();
                }

                _skuDetailsQueue.clear();

            } catch (Exception e) {
                Log.e(TAG, "Exception while communicating with IABHelper");
                e.printStackTrace();
            }
            }
        });
    }

    @Override
    public void purchaseItem(final String sku, final String developerPayload)
    {
        if (billingDataSource == null)
            return;

        final Activity activity = this;

        billingDataSource.launchBillingFlow(this, sku);
    }

    void updateReceipt(List<Purchase> purchases)
    {
        int i = 0;
        String receipt = "[";
        for (Purchase p : purchases)
        {
            if (i > 0)
                receipt += ", ";
            i++;

            try
            {
                receipt += new JSONStringer().object()
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
        Log.d(TAG, "receipt " + receipt);
        receiptReceived(receipt);
    }

    public void onSkuDetailsResponse(BillingResult billingResult, List<SkuDetails> skuDetailsList)
    {
        Log.d(TAG, "Query inventory finished.");

        this.runOnUiThread(new Runnable(){
            public void run() {
                if (billingResult.getResponseCode() != BillingClient.BillingResponseCode.OK) {
                    _IABCallbacks.getProductsFailed(billingResult.getResponseCode(), billingResult.getDebugMessage());
                    return;
                }

                for (SkuDetails d : skuDetailsList) {
                    _IABCallbacks.productValidated(d.getSku(), d.getPrice(), d.getTitle(), d.getDescription(), Long.toString(d.getPriceAmountMicros()), d.getPriceCurrencyCode());
                }
                _IABCallbacks.finishProductsValidation();
            }});
    }

    public void onRefreshPurchases(String skuType, List<Purchase> purchases)
    {
        this.runOnUiThread(new Runnable(){
            public void run() {
                for (Purchase p : purchases) {
                    if (billingDataSource.getUnmaskedPurchaseState(p) == Purchase.PurchaseState.PURCHASED) {
                        for (String sku : p.getSkus()) {
                            if (_IABCallbacks.isItemConsumable(sku) == 0) {
                                _IABCallbacks.itemRestored(sku, p.getPurchaseTime(), p.getOrderId());
                            }
                        }
                    }
                }

                // support only non-consumable INAPPs at the moment for generating receipt data
                if (skuType != BillingClient.SkuType.INAPP)
                    return;

                updateReceipt(purchases);
            }});
    }

    public void onPurchasesUpdated(BillingResult billingResult, List<Purchase> purchases)
    {
        this.runOnUiThread(new Runnable(){
            public void run() {
                if (billingResult.getResponseCode() != BillingClient.BillingResponseCode.OK) {
                    _IABCallbacks.itemPurchaseFailed("<error>", billingResult.getResponseCode(), billingResult.getDebugMessage());
                }
            }});
    }

    void onNewPurchase(Purchase purchase)
    {
        this.runOnUiThread(new Runnable(){
            public void run() {
                for (String sku : purchase.getSkus()) {
                    _IABCallbacks.itemPurchased(sku, purchase.getPurchaseTime(), purchase.getOrderId());
                }
            }});
    }

    public void onBackButton()
    {
        Intent startMain = new Intent(Intent.ACTION_MAIN);
        startMain.addCategory(Intent.CATEGORY_HOME);
        startMain.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        startActivity(startMain);
    }

    public void openTextView(String text)
    {
    	Intent intent = new Intent(this, TextViewActivity.class); 
    	intent.putExtra("text", text);
    	startActivity(intent);
    }

    public native void receiptReceived(String text);
}
