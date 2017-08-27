
package com.dreamfarmgames.util;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.Display;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.view.WindowManager;
import android.content.Intent;
import android.widget.TextView;
import android.widget.EditText;
import android.view.inputmethod.InputMethodManager;
import android.content.Context;
import android.view.KeyEvent;
import android.view.inputmethod.EditorInfo;

public class TextViewActivity extends Activity
{
  public static TextViewActivity activity;
  private EditText _view;
  private String _text;

  class CustomEditText extends EditText
  {
    public CustomEditText(Context context) {
        super(context);
    }
    @Override
    public boolean onKeyPreIme(int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK) {
            textEntered(TextViewActivity.activity.getIntent().getStringExtra("text"));
            TextViewActivity.activity.finish();
            return true;
        }
        return false;
    }
  }

  public TextViewActivity()
  {
    super();
	activity = this;
  }

  public native void textEntered(String text);

  @Override
  public void onCreate(Bundle savedInstanceState) 
  {
    super.onCreate(savedInstanceState);
	Intent intent = getIntent(); 

	if(intent != null)
	{
		_text = intent.getStringExtra("text");

		_view = new CustomEditText(this);
        _view.setText(_text);

		setContentView(_view);
        _view.setSingleLine(true);

        _view.setOnEditorActionListener(new TextView.OnEditorActionListener(){
            public boolean onEditorAction(TextView view, int actionId, KeyEvent event) {
                if (actionId == EditorInfo.IME_NULL || actionId == EditorInfo.IME_ACTION_DONE)
                {
                    textEntered(view.getText().toString());
                    TextViewActivity.activity.finish();
                }
                return true;
            }});

        _view.setOnKeyListener(new View.OnKeyListener()
        {
            public boolean onKey(View v, int keyCode, KeyEvent event)
            {
                if (event.getAction() == KeyEvent.ACTION_DOWN)
                {
                    //check if the right key was pressed
                    if (keyCode == KeyEvent.KEYCODE_BACK)
                    {
                        TextViewActivity.activity.finish();
                        return true;
                    }
                }
                return false;
            }
        });
	}
  }

    @Override
    public void onResume()
    {
        super.onResume();
        _view.requestFocus();

		View view = getWindow().getDecorView();
        InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
        imm.showSoftInput(_view,InputMethodManager.SHOW_FORCED);
        imm.toggleSoftInput(InputMethodManager.SHOW_FORCED,InputMethodManager.SHOW_IMPLICIT);
    }

	@Override
	public void onAttachedToWindow() {
		super.onAttachedToWindow();
        
		Display display = getWindowManager().getDefaultDisplay();

		View view = getWindow().getDecorView();
		WindowManager.LayoutParams lp = (WindowManager.LayoutParams) view.getLayoutParams();
		lp.gravity = Gravity.BOTTOM;
		lp.width = (int)(display.getWidth());
		//lp.height = (int)(display.getHeight() * 0.79166);
        lp.flags |= WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL;
		getWindowManager().updateViewLayout(view, lp);
	}
}

