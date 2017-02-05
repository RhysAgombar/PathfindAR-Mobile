package com.example.raven.pathfindar;

import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;

import java.util.ArrayList;

/**
 * Created by Raven on 03-Feb-17.
 */

public class tokenAdapterDB extends tokenAdapter{
    private Context context;
    private ArrayList<Token> data;

    public tokenAdapterDB(Context context, ArrayList<Token> data) {
        super(context, data);

        this.context = context;
        this.data = data;
    }

    @Override
    public View getView(final int position, View convertView, ViewGroup parent) {

        final Token tokenToDisplay = data.get(position);

        if (convertView == null){
            LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            convertView = inflater.inflate(R.layout.token_data_db, parent, false);
        }

        TextView tvName = (TextView)convertView.findViewById(R.id.tv_tokenName_DB);
        tvName.setText(tokenToDisplay.name);

        TextView tvMove = (TextView)convertView.findViewById(R.id.tv_movement_DB);
        tvMove.setText(Integer.toString(tokenToDisplay.mRange) + "ft.");

        TextView tvARange1 = (TextView)convertView.findViewById(R.id.tv_range1_DB);
        tvARange1.setText(tokenToDisplay.w1.toString());

        TextView tvARange2 = (TextView)convertView.findViewById(R.id.tv_range2_DB);
        tvARange2.setText(tokenToDisplay.w2.toString());

        TextView tvARange3 = (TextView)convertView.findViewById(R.id.tv_range3_DB);
        tvARange3.setText(tokenToDisplay.w3.toString());

        TextView tvARange4 = (TextView)convertView.findViewById(R.id.tv_range4_DB);
        tvARange4.setText(tokenToDisplay.w4.toString());

        final Button editButton = (Button)convertView.findViewById(R.id.bt_deleteToken_DB);
        editButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                final TokenDBHelper db = new TokenDBHelper(context);
                String test = Integer.toString(data.get(position).id);
                db.deleteElementByID(test);

                data.remove(position);

                //data = (ArrayList<Token>)db.getAllElements();
                //tokenLoadMenu.tokenList = (ArrayList<Token>)db.getAllElements();
                notifyDataSetChanged();

                /*
                tokenLoadMenu.listView.invalidateViews();
                tokenLoadMenu.listView = (ListView)findViewById(R.id.lv_tokenListDB);
                tokenLoadMenu.ad = new tokenAdapterDB(tokenLoadMenu.this, tokenLoadMenu.tokenList);
                tokenLoadMenu.listView.setAdapter(tokenLoadMenu.ad);
                */
                //data = (ArrayList<Token>)db.getAllElements();
                //tokenLoadMenu.tokenList = (ArrayList<Token>)db.getAllElements();
                //tokenLoadMenu.listView.invalidateViews();
            }
        });

        return convertView;
    }



}
