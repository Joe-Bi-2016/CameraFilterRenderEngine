package com.bisw.camera;

import androidx.appcompat.app.AppCompatActivity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.SimpleAdapter;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = MainActivity.class.getSimpleName();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);

        ListView listview = (ListView)findViewById(R.id.listview);

        List<Map<String, String>> data = new ArrayList<Map<String, String>>();

        Map<String, String> i0 = new HashMap<String, String>();
        i0.put("test", "Camera - TextureView");

        Map<String, String> i1 = new HashMap<String, String>();
        i1.put("test", "Camera - SurfaceView");

        Map<String, String> i2 = new HashMap<String, String>();
        i2.put("test", "Camera2 - TextureView");

        Map<String, String> i3 = new HashMap<String, String>();
        i3.put("test", "Camera2 - SurfaceView");

        Map<String, String> i4 = new HashMap<String, String>();
        i4.put("test", "Camera - GLESTextureView");

        Map<String, String> i5 = new HashMap<String, String>();
        i5.put("test", "Camera - GLESSurfaceView");

        Map<String, String> i6 = new HashMap<String, String>();
        i6.put("test", "Camera2 - GLESTextureView");

        Map<String, String> i7 = new HashMap<String, String>();
        i7.put("test", "Camera2 - GLESSurfaceView");

        data.add(i0);
        data.add(i1);
        data.add(i2);
        data.add(i3);
        data.add(i4);
        data.add(i5);
        data.add(i6);
        data.add(i7);

        listview.setAdapter(new SimpleAdapter(this, data, R.layout.item, new String[] {"test"}, new int[]{R.id.tv_name}));

        listview.setOnItemClickListener(new AdapterView.OnItemClickListener(){
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                Intent intent = new Intent();
                intent.setClass(MainActivity.this, ActualView.class);
                if(id == 0)
                    intent.putExtra("xmlName", R.layout.activity_texture_camera);
                else if(id == 1)
                    intent.putExtra("xmlName", R.layout.activity_surface_camera);
                else if(id == 2)
                    intent.putExtra("xmlName", R.layout.activity_texture_camera2);
                else if(id == 3)
                    intent.putExtra("xmlName", R.layout.activity_surface_camera2);
                else if(id == 4)
                    intent.putExtra("xmlName", R.layout.activity_oestexture_camera);
                else if(id == 5)
                    intent.putExtra("xmlName", R.layout.activity_oessurface_camera);
                else if(id == 6)
                    intent.putExtra("xmlName", R.layout.activity_oestexture_camera2);
                else if(id == 7)
                    intent.putExtra("xmlName", R.layout.activity_oessurface_camera2);
                startActivity(intent);
            }
        });


    }

    @Override public void onResume() {
        super.onResume();
    }

    @Override public void onPause() {
        super.onPause();
    }

    @Override public void onDestroy() {
        super.onDestroy();
    }

}


