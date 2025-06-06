package com.hyun.robot.ui

import android.annotation.SuppressLint
import android.bluetooth.BluetoothDevice
import android.content.Context
import android.content.Intent
import android.os.Bundle
import android.view.View
import androidx.activity.compose.setContent
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.IntrinsicSize
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.aspectRatio
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.lazy.grid.GridCells
import androidx.compose.foundation.lazy.grid.LazyVerticalGrid
import androidx.compose.foundation.lazy.grid.items
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.text.KeyboardActions
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.ListItem
import androidx.compose.material3.LocalTextStyle
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.material3.TextField
import androidx.compose.material3.TextFieldDefaults
import androidx.compose.material3.TopAppBar
import androidx.compose.material3.TopAppBarDefaults
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.focus.FocusRequester
import androidx.compose.ui.focus.focusRequester
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalFocusManager
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.input.ImeAction
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat.startActivity
import com.hyun.robot.MyApplication
import com.hyun.robot.MyApplication.Companion.bleManager
import com.hyun.robot.R
import com.hyun.robot.ui.theme.RobotTheme
import com.hyun.robot.utils.BleManagerDevice

class MainActivity : BaseActivity() {
    private lateinit var context: Context

    companion object{
        private const val REQUEST_CODE: Int = 0x01
    }

    private fun requestBLEPermission() {
        ActivityCompat.requestPermissions(this, arrayOf(android.Manifest.permission.BLUETOOTH_ADMIN,
            android.Manifest.permission.ACCESS_COARSE_LOCATION), REQUEST_CODE
        )
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        window.decorView.systemUiVisibility = View.SYSTEM_UI_FLAG_FULLSCREEN
        actionBar?.hide()
        context = this
        MyApplication.isFirstDeviceDetail = true
        setContent {
            var showSplash by remember { mutableStateOf(true) }
            if (showSplash) {
                LoadingScreen(onTimeout = { showSplash = false })
            } else {
                MainScreen()
                showSplash = false
            }
        }
        requestBLEPermission()
    }

    override fun onDestroy() {
        super.onDestroy()
        bleManager.stopDiscovery()
    }
}

@Composable
fun AddDevice() {
    val intent = Intent(LocalContext.current, AddDeviceActivity::class.java)
    var bundle = Bundle()
    val context = LocalContext.current
    LaunchedEffect(Unit) {
        startActivity(context, intent, bundle)
    }
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun MainScreen() {
    val image = painterResource(R.drawable.bg_robot_list)
    var isAddDeviceScreenVisible by remember { mutableStateOf(false) }
    Box(modifier = Modifier.fillMaxSize().fillMaxHeight().fillMaxWidth()) {
        Image(
            painter = image,
            contentDescription = null,
            modifier = Modifier.fillMaxSize(),
            contentScale = ContentScale.Crop
        )
        Scaffold(
            modifier = Modifier.fillMaxSize(),
            containerColor = Color.Transparent,
            topBar = {
                TopAppBar(
                    colors = TopAppBarDefaults.centerAlignedTopAppBarColors(
                        containerColor = Color.Transparent,
                        scrolledContainerColor = Color.Transparent
                    ),
                    title = {
                        Text(
                            "My Robot", fontWeight = FontWeight.SemiBold,
                            textAlign = TextAlign.Left,
                            fontSize = 24.sp,
                        )
                    }
                )
            },
            floatingActionButton = {
                IconButton(
                    onClick = {
                        isAddDeviceScreenVisible = true
                    },
                    modifier = Modifier
                        .size(56.dp)
                        .clip(CircleShape)
                        .background(MaterialTheme.colorScheme.primary)
                ) {
                    Image(
                        painter = painterResource(id = R.drawable.ic_add),
                        contentDescription = "Add",
                        modifier = Modifier.size(60.dp)
                    )
                }
            }
        ) { paddingValues ->
            Column(
                modifier = Modifier
                    .padding(paddingValues)
                    .fillMaxSize()
            ) {
                Spacer(modifier = Modifier.height(8.dp))
                LazyVerticalGrid(
                    columns = GridCells.Fixed(3),
                    contentPadding = PaddingValues(8.dp),
                    verticalArrangement = Arrangement.spacedBy(8.dp),
                    horizontalArrangement = Arrangement.spacedBy(8.dp)
                ) {
                    items(bleManager.pairedDevices) { device ->
                        GridItem(
                            device = device,
                            modifier = Modifier
                                .aspectRatio(1f)
                                .padding(4.dp)
                        )
                    }
                }
            }
            if (isAddDeviceScreenVisible) {
                AddDevice()
            }
        }
    }
}

@SuppressLint("MissingPermission")
@Composable
fun showDetail(device: BleManagerDevice.BluetoothManagerDevice,deviceName:String) {
    val intent = Intent(LocalContext.current, DeviceDetailActivity::class.java).apply {
        putExtra("DEVICE_ADDRESS", device.address)
        putExtra("DEVICE_NAME", deviceName)
    }
    var bundle = Bundle()
    val context = LocalContext.current
    LaunchedEffect(Unit) {
        startActivity(context, intent, bundle)
    }
}

fun getFirst8CharsOfMac(macAddress: String): String {
    val cleanMac = macAddress.replace(":", "")
    return cleanMac.substring(0, 4)
}

@OptIn(ExperimentalMaterial3Api::class)
@SuppressLint("MissingPermission")
@Composable
private fun GridItem(
    device: BleManagerDevice.BluetoothManagerDevice,
    modifier: Modifier = Modifier
) {
    var isShowDetail by remember { mutableStateOf(false) }
    val backgroundColor = Color.White
    var context = LocalContext.current
    var deviceName by remember { mutableStateOf("") }

    Box(
        modifier = modifier
            .clip(RoundedCornerShape(16.dp))
            .background(backgroundColor)
            .clickable {
//                if(!isChildClicked) {
                    isShowDetail = true
//                }
            },
        contentAlignment = Alignment.Center
    ) {

        Column(
            horizontalAlignment = Alignment.CenterHorizontally,
            verticalArrangement = Arrangement.Center
        ) {
            deviceName = (device.name + device.address?.let { getFirst8CharsOfMac(it) })?: "Unknown"
            if(device.robotName!=null ){
                deviceName = device.robotName!!
            }
            EditableText(
                initialText = deviceName,
                device = device,
                onTextChanged = { newName ->
                    deviceName = newName
                    device.robotName = newName
                    bleManager.updateDeviceName(device,newName)
                }
            )

            Spacer(Modifier.height(16.dp))

            Image(
                painter = painterResource(R.drawable.ic_robot),
                contentDescription = "Device Image",
                modifier = Modifier.size(120.dp)
                    .clickable {
                        isShowDetail = true
                },
                contentScale = ContentScale.Fit
            )

            if (isShowDetail) {
               showDetail(device,deviceName)
            }
        }
    }
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun EditableText(
    initialText: String,
    device :BleManagerDevice.BluetoothManagerDevice,
    onTextChanged: (String) -> Unit
) {
    var isEditing by remember { mutableStateOf(false) }
    var text by remember { mutableStateOf(initialText) }
    val focusManager = LocalFocusManager.current

    Row(
        verticalAlignment = Alignment.CenterVertically,
        horizontalArrangement = Arrangement.SpaceBetween,
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp)
    ) {
        Box(modifier = Modifier.weight(1f).clickable { isEditing = true }) {
            if (isEditing) {
                TextField(
                    value = text,
                    onValueChange = { text = it },
                    singleLine = false,
                    maxLines = 2,
                    textStyle = LocalTextStyle.current.copy(
                        color = Color.Black,
                        fontSize = 16.sp
                    ),
                    colors = TextFieldDefaults.textFieldColors(
                        containerColor =  Color(0xFFCAD7E4),
                        focusedIndicatorColor = Color.Transparent,
                        unfocusedIndicatorColor = Color.Transparent,
                        cursorColor = Color.Black
                    ),
                    modifier = Modifier.fillMaxWidth()
                        .border(1.dp, Color.Black, RoundedCornerShape(4.dp))
                        .height(IntrinsicSize.Min)
                        .padding(0.dp)
                        .clickable {
                            isEditing  = true
                          },
                    keyboardOptions = KeyboardOptions(imeAction = ImeAction.Done),
                    keyboardActions = KeyboardActions(
                        onDone = {
                            focusManager.clearFocus()
                            onTextChanged(text)
                        }
                    )
                )
            } else {
                Text(
                    modifier = Modifier.padding(start = 16.dp)
                        .clickable{ isEditing  = true},
                    text = text,
                    color = Color.Black,
                    fontSize = 16.sp,
                    maxLines = 2,
                )
            }
        }

        IconButton(
            onClick = {
                isEditing = !isEditing
                if(!isEditing) {
                    bleManager.updateDeviceName(device, text)
                }
            },
            modifier = Modifier.size(40.dp)
        ) {
            Icon(
                painter = painterResource(id = R.drawable.ic_edit),
                contentDescription = "Edit",
                tint = Color.Unspecified,
                modifier = Modifier.size(30.dp)
            )
        }
    }
}

@Preview(showBackground = true)
@Composable
fun DefaultPreview() {
    RobotTheme {
        MainScreen()
    }
}

