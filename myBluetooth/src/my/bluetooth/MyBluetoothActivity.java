package my.bluetooth;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Set;
import java.util.UUID;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.res.AssetFileDescriptor;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.ToneGenerator;
import android.os.Bundle;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.widget.ArrayAdapter;
import android.widget.Toast;


public class MyBluetoothActivity extends Activity {
    private static final int REQUEST_ENABLE_BT = 2;
	/** Called when the activity is first created. */
	
	
	BluetoothAdapter mBluetooth = null; //adaptador local de bluetooth	
	BluetoothDevice mDevice = null; //dispositivo de bluetooth remoto
	BluetoothSocket socket = null;
	ArrayAdapter mArrayAdapter = null;
	UUID MY_UUID;
	BroadcastReceiver mReceiver;
	IntentFilter iFilter= null;
	Toast toast;
    
	
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        

        
        
        mBluetooth = BluetoothAdapter.getDefaultAdapter();
        
        if (mBluetooth == null) {
            // Sorry but you don't have bluetooth.
        }
        
        //
        if (!mBluetooth.isEnabled()) {
            
        	//Solicita ao usuario a ativacao do bluetooth(Fazer forçadamente)
        	Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);            
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        }
        
        
        
        mArrayAdapter = new ArrayAdapter(this, 0);
        
        Set<BluetoothDevice> pairedDevices = mBluetooth.getBondedDevices();
        // If there are paired devices
        if (pairedDevices.size() > 0) {
         // Loop through paired devices
         for (BluetoothDevice device : pairedDevices) {
             // Add the name and address to an array adapter to show in a ListView
             mArrayAdapter.add(device.getName() + "\n" + device.getAddress());
             Log.d("Device:", ""+device.getName()+ " Address: "+device.getAddress() );
             
             //obtem device com nome 'mario-desktop'
             if(device.getName().startsWith("mario-")) mDevice = device;
         }
        }
        
        //Realiza descoberta.        
        // Cria um BroadcastReceiver para a acao ACTION_FOUND(dispositivo encontrado)
        mReceiver = new BroadcastReceiver() {        	
            @Override
        	public void onReceive(Context context, Intent intent) {
                String action = intent.getAction();
                
                
                // Quando a descoberta encontra um dispositivo
                if (BluetoothDevice.ACTION_FOUND.equals(action)) {
                    //Obtem, da Intent, o objeto BluetoothDevice correspondente ao dispositivo encontrado 
                   mDevice = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                    //Adiciona o nome e  endereco desse dispositivo no vetor de dispositivos
                    mArrayAdapter.add(mDevice.getName() + "\n" + mDevice.getAddress());
                    Log.d("Device:", ""+mDevice.getName() );
                   
                }
            }			
        };               
        
        //Cria o filtro de Intent para a intent a ser capturada pelo broadcast receiver
        iFilter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
        
        toast = Toast.makeText(this, "Abrindo Cancela...", Toast.LENGTH_SHORT);
        
    }
    
    @Override
    public void onStart(){
    	super.onStart();
    	
    	
    	
    }
    
    @Override
    public void onResume(){
    	super.onResume();
    	
    	
        registerReceiver(mReceiver, iFilter); // Don't forget to unregister during onDestroy
        
        toast.show();
        
       
        //Cria comunicação rfcomm com o device(dispositivo remoto- mDevice)
        //Get a BluetoothSocket to connect with the given BluetoothDevice
        
            //Cria socket com mDevice(remoto) sobre rfcomm. MY_UUID eh a UUID do app a ser enviada no estabelecimento da comunicação via rfcomm.        	
        	//MY_UUID = UUID.fromString("00000000-0000-0000-0000-000033320000");
        	//MY_UUID = UUID.fromString("00000000-0000-0000-0000-000000000000");
            if (mDevice != null) { 
            	
            	//socket = mDevice.createRfcommSocketToServiceRecord(MY_UUID); Log.d("Device status: ", "found");
            	
            	//workaround sugerido em http://stackoverflow.com/questions/3397071/service-discovery-failed-exception-using-bluetooth-on-android
            	Method m;
				try {
					m = mDevice.getClass().getMethod("createRfcommSocket", new Class[] {int.class});
					socket = (BluetoothSocket) m.invoke(mDevice, 10); 
					
				} catch (SecurityException e) {
					
				} catch (NoSuchMethodException e) {
					
				} catch (IllegalArgumentException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (IllegalAccessException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (InvocationTargetException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
                
            	
            }
            else Log.d("Device status:", " not found");
               
       
        //cancela descoberta assim que socket rfcomm eh criado.
        mBluetooth.cancelDiscovery();        
        
        //Com o socket já inicializado para seu destino (mDevice), realiza a conexão entre o adapter e dispositivo
        try {
            // Connect the device through the socket. This will block
            // until it succeeds or throws an exception
            socket.connect();
        } catch (IOException connectException) {
            // Unable to connect; close the socket and get out
            try {
                socket.close();
            } catch (IOException closeException) { }
            return;
        }
        
        
        //Realiza a transferencia dos dados.
        try {
			InputStream tempIn = socket.getInputStream();
			OutputStream tempOut = socket.getOutputStream();
			
			String message = "Mario151480";
			tempOut.write(message.getBytes()); //envia mensagem	
			Toast.makeText(this, "Cancela Aberta", 1000).show();
			
			
		} catch (IOException e){			
			
		}
        
		
        
    }
    
    @Override    
    public void onPause(){
    	super.onPause();
    	
    	if (mReceiver != null) unregisterReceiver(mReceiver);
    	
    	
    }
    
    @Override
    public void onDestroy(){
    	super.onDestroy();
    	
    	try {
			socket.close();
		} catch (IOException e) {
			System.out.println("Socket closing error");
			
		}
    	
    	
    }
    
    
}