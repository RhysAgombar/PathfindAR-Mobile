package com.example.raven.pathfindar;

import android.content.Intent;
import android.os.Bundle;
import android.support.v4.app.FragmentActivity;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;

import java.util.ArrayList;

/**
 * Created by Raven on 31-Dec-16.
 */

public class tokenMenu extends MainActivity {

    public static ArrayList<Token> tokenList = new ArrayList<>();

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.token_menu);

        Intent callingIntent = getIntent();

        ListView listView = (ListView)findViewById(R.id.lv_tokenList);
        listView.setAdapter(new tokenAdapter(this, tokenList));

    }

}
