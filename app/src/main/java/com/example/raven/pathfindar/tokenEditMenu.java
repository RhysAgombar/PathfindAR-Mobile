package com.example.raven.pathfindar;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by Raven on 02-Jan-17.
 */

public class tokenEditMenu extends MainActivity{
    public static ArrayList<Token> tokenList = new ArrayList<>();
    int position;
    Token selectedToken;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.edit_token_menu);

        Intent callingIntent = getIntent();
        position = callingIntent.getIntExtra("pos", 0);

        selectedToken = tokenList.get(position);

        EditText tokenName = (EditText)findViewById(R.id.et_editTokenName);
        tokenName.setText(selectedToken.name);

        EditText w1Name = (EditText)findViewById(R.id.et_editW1Name);
        w1Name.setText(selectedToken.w1.name);

        EditText w2Name = (EditText)findViewById(R.id.et_editW2Name);
        w2Name.setText(selectedToken.w2.name);

        EditText w3Name = (EditText)findViewById(R.id.et_editW3Name);
        w3Name.setText(selectedToken.w3.name);

        EditText w4Name = (EditText)findViewById(R.id.et_editW4Name);
        w4Name.setText(selectedToken.w4.name);

        TextView movement = (TextView)findViewById(R.id.tv_numMR);
        movement.setText(Integer.toString(selectedToken.mRange));

        TextView w1Range = (TextView)findViewById(R.id.tv_numW1);
        w1Range.setText(Integer.toString(selectedToken.w1.range));

        TextView w2Range = (TextView)findViewById(R.id.tv_numW2);
        w2Range.setText(Integer.toString(selectedToken.w2.range));

        TextView w3Range = (TextView)findViewById(R.id.tv_numW3);
        w3Range.setText(Integer.toString(selectedToken.w3.range));

        TextView w4Range = (TextView)findViewById(R.id.tv_numW4);
        w4Range.setText(Integer.toString(selectedToken.w4.range));


        CheckBox w1Reach = (CheckBox)findViewById(R.id.cb_reach1);
        w1Reach.setChecked(selectedToken.w1.reach);

        CheckBox w2Reach = (CheckBox)findViewById(R.id.cb_reach2);
        w2Reach.setChecked(selectedToken.w2.reach);

        CheckBox w3Reach = (CheckBox)findViewById(R.id.cb_reach3);
        w3Reach.setChecked(selectedToken.w3.reach);

        CheckBox w4Reach = (CheckBox)findViewById(R.id.cb_reach4);
        w4Reach.setChecked(selectedToken.w4.reach);


        CheckBox w1Ranged = (CheckBox)findViewById(R.id.cb_ranged1);
        w1Ranged.setChecked(selectedToken.w1.ranged);

        CheckBox w2Ranged = (CheckBox)findViewById(R.id.cb_ranged2);
        w2Ranged.setChecked(selectedToken.w2.ranged);

        CheckBox w3Ranged = (CheckBox)findViewById(R.id.cb_ranged3);
        w3Ranged.setChecked(selectedToken.w3.ranged);

        CheckBox w4Ranged = (CheckBox)findViewById(R.id.cb_ranged4);
        w4Ranged.setChecked(selectedToken.w4.ranged);


        w1Reach.setOnCheckedChangeListener(new CheckBox.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked){
                    CheckBox ranged1 = (CheckBox)findViewById(R.id.cb_ranged1);
                    ranged1.setChecked(false);
                }
            }
        });

        w1Ranged.setOnCheckedChangeListener(new CheckBox.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked){
                    CheckBox reach1 = (CheckBox)findViewById(R.id.cb_reach1);
                    reach1.setChecked(false);
                }
            }
        });

        w2Reach.setOnCheckedChangeListener(new CheckBox.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked){
                    CheckBox ranged2 = (CheckBox)findViewById(R.id.cb_ranged2);
                    ranged2.setChecked(false);
                }
            }
        });

        w2Ranged.setOnCheckedChangeListener(new CheckBox.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked){
                    CheckBox reach2 = (CheckBox)findViewById(R.id.cb_reach2);
                    reach2.setChecked(false);
                }
            }
        });

        w3Reach.setOnCheckedChangeListener(new CheckBox.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked){
                    CheckBox ranged3 = (CheckBox)findViewById(R.id.cb_ranged3);
                    ranged3.setChecked(false);
                }
            }
        });

        w3Ranged.setOnCheckedChangeListener(new CheckBox.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked){
                    CheckBox reach3 = (CheckBox)findViewById(R.id.cb_reach3);
                    reach3.setChecked(false);
                }
            }
        });

        w4Reach.setOnCheckedChangeListener(new CheckBox.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked){
                    CheckBox ranged4 = (CheckBox)findViewById(R.id.cb_ranged4);
                    ranged4.setChecked(false);
                }
            }
        });

        w4Ranged.setOnCheckedChangeListener(new CheckBox.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked){
                    CheckBox reach4 = (CheckBox)findViewById(R.id.cb_reach4);
                    reach4.setChecked(false);
                }
            }
        });

    }


    public void decrementRange1(View view) {
        TextView w1Range = (TextView)findViewById(R.id.tv_numW1);
        if (selectedToken.w1.range > 0){
            selectedToken.w1.range -= 5;
        }
        w1Range.setText(Integer.toString(selectedToken.w1.range));
    }

    public void incrementRange1(View view) {
        TextView w1Range = (TextView)findViewById(R.id.tv_numW1);
        selectedToken.w1.range += 5;
        w1Range.setText(Integer.toString(selectedToken.w1.range));
    }

    public void decrementRange2(View view) {
        TextView w2Range = (TextView)findViewById(R.id.tv_numW2);
        if (selectedToken.w2.range > 0){
            selectedToken.w2.range -= 5;
        }
        w2Range.setText(Integer.toString(selectedToken.w2.range));
    }

    public void incrementRange2(View view) {
        TextView w2Range = (TextView)findViewById(R.id.tv_numW2);
        selectedToken.w2.range += 5;
        w2Range.setText(Integer.toString(selectedToken.w2.range));
    }

    public void decrementRange3(View view) {
        TextView w3Range = (TextView)findViewById(R.id.tv_numW3);
        if (selectedToken.w3.range > 0){
            selectedToken.w3.range -= 5;
        }
        w3Range.setText(Integer.toString(selectedToken.w3.range));
    }

    public void incrementRange3(View view) {
        TextView w3Range = (TextView)findViewById(R.id.tv_numW3);
        selectedToken.w3.range += 5;
        w3Range.setText(Integer.toString(selectedToken.w3.range));
    }

    public void decrementRange4(View view) {
        TextView w4Range = (TextView)findViewById(R.id.tv_numW4);
        if (selectedToken.w4.range > 0){
            selectedToken.w4.range -= 5;
        }
        w4Range.setText(Integer.toString(selectedToken.w4.range));
    }

    public void incrementRange4(View view) {
        TextView w4Range = (TextView)findViewById(R.id.tv_numW4);
        selectedToken.w4.range += 5;
        w4Range.setText(Integer.toString(selectedToken.w4.range));
    }

    public void decrementMovement(View view) {
        TextView moveRange = (TextView)findViewById(R.id.tv_numMR);
        if (selectedToken.mRange > 0){
            selectedToken.mRange -= 5;
        }
        moveRange.setText(Integer.toString(selectedToken.mRange));
    }

    public void incrementMovement(View view) {
        TextView moveRange = (TextView)findViewById(R.id.tv_numMR);
        selectedToken.mRange += 5;
        moveRange.setText(Integer.toString(selectedToken.mRange));
    }

    public void closeTokenEdit(View view) {
        setContentView(R.layout.token_menu);

        tokenMenu.listView.invalidateViews();

        finish();
    }

    public void updateToken(View view) {
        selectedToken.name = ((EditText)findViewById(R.id.et_editTokenName)).getText().toString();

        selectedToken.w1.name = ((EditText)findViewById(R.id.et_editW1Name)).getText().toString();
        selectedToken.w2.name = ((EditText)findViewById(R.id.et_editW2Name)).getText().toString();
        selectedToken.w3.name = ((EditText)findViewById(R.id.et_editW3Name)).getText().toString();
        selectedToken.w4.name = ((EditText)findViewById(R.id.et_editW4Name)).getText().toString();

        selectedToken.w1.ranged = ((CheckBox)findViewById(R.id.cb_ranged1)).isChecked();
        selectedToken.w2.ranged = ((CheckBox)findViewById(R.id.cb_ranged2)).isChecked();
        selectedToken.w3.ranged = ((CheckBox)findViewById(R.id.cb_ranged3)).isChecked();
        selectedToken.w4.ranged = ((CheckBox)findViewById(R.id.cb_ranged4)).isChecked();

        selectedToken.w1.reach = ((CheckBox)findViewById(R.id.cb_reach1)).isChecked();
        selectedToken.w2.reach = ((CheckBox)findViewById(R.id.cb_reach2)).isChecked();
        selectedToken.w3.reach = ((CheckBox)findViewById(R.id.cb_reach3)).isChecked();
        selectedToken.w4.reach = ((CheckBox)findViewById(R.id.cb_reach4)).isChecked();

        tokenList.set(position, selectedToken);

        Toast toast = Toast.makeText(this, "Token Updated", Toast.LENGTH_SHORT);
        toast.show();
    }

    public void saveTokenToDB(View view) {

    }

    public void loadTokenFromDB(View view) {

    }

    /*


    public void decrementRange1(View view) {
        TextView w1Range = (TextView)findViewById(R.id.tv_numW1);
        int holder = selectedToken.w1.range - 5;
        w1Range.setText(Integer.toString(holder));
    }

    public void incrementRange1(View view) {
        TextView w1Range = (TextView)findViewById(R.id.tv_numW1);
        int holder = selectedToken.w1.range + 5;
        w1Range.setText(Integer.toString(holder));
    }

    public void decrementRange2(View view) {
        TextView w2Range = (TextView)findViewById(R.id.tv_numW2);
        int holder = selectedToken.w2.range - 5;
        w2Range.setText(Integer.toString(holder));
    }

    public void incrementRange2(View view) {
        TextView w2Range = (TextView)findViewById(R.id.tv_numW2);
        int holder = selectedToken.w2.range + 5;
        w2Range.setText(Integer.toString(holder));
    }

    public void decrementRange3(View view) {
        TextView w3Range = (TextView)findViewById(R.id.tv_numW3);
        int holder = selectedToken.w3.range - 5;
        w3Range.setText(Integer.toString(holder));
    }

    public void incrementRange3(View view) {
        TextView w3Range = (TextView)findViewById(R.id.tv_numW3);
        int holder = selectedToken.w3.range + 5;
        w3Range.setText(Integer.toString(holder));
    }

    public void decrementRange4(View view) {
        TextView w4Range = (TextView)findViewById(R.id.tv_numW4);
        int holder = selectedToken.w4.range - 5;
        w4Range.setText(Integer.toString(holder));
    }

    public void incrementRange4(View view) {
        TextView w4Range = (TextView)findViewById(R.id.tv_numW4);
        int holder = selectedToken.w4.range + 5;
        w4Range.setText(Integer.toString(holder));
    }

     */

}
