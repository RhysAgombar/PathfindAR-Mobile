package com.example.raven.pathfindar;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.TextView;

import java.util.ArrayList;

/**
 * Created by Raven on 03-Feb-17.
 */

public class tokenLoadMenu extends MainActivity {

    public ListView listView;
    public ArrayList<Token> tokenList = new ArrayList<>();

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.token_db_load);

        Intent callingIntent = getIntent();

        final TokenDBHelper db = new TokenDBHelper(this);
        tokenList = (ArrayList<Token>) db.getAllElements();

        listView = (ListView)findViewById(R.id.lv_tokenListDB);
        listView.setAdapter(new tokenAdapterDB(this, tokenList));

        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) { // When you click an item in the list:

                //Token selTok = tokenList.get(position);

                finishLoad(tokenList.get(position).id); // might not work?

            }
        });

    }


    public void closeTokenMenu(View view) {
        finishLoad(-1);
    }

    public void finishLoad(int selTok){
        Intent resultIntent = new Intent();
        resultIntent.putExtra("tokenID", selTok);
        setResult(RESULT_OK, resultIntent);
        finish();
    }
}
