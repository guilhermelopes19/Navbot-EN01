import UIKit
import CoreBluetooth

class RootMenuListVC: UIViewController,UICollectionViewDataSource, UICollectionViewDelegateFlowLayout, ZYBluetoothHandlerDelegate{
    
    var myCollection: UICollectionView!
    var deviceListModels = [[String: Any]]()
    var uuid_this_v = ""
    
    var current_ScanedDevice = [[String: Any]]()
    
    var currentHUDMessage: MBProgressHUD!
    
    override func viewDidLoad() {
        super.viewDidLoad()
    }
    var isFirst = true
    override func viewDidLayoutSubviews() {
        super.viewDidLayoutSubviews()
        if isFirst{
            initUI()
            initBluetooth()
            requestDeviceListData()
            isFirst = false
        }
    }
    func initUI(){
        
        view.backgroundColor = UIColor(red: 237/255, green: 243/255, blue: 228/255, alpha: 1.0)
        
        let safe_insets = view.safeAreaInsets
        print("Top: \(safe_insets.top), Left: \(safe_insets.left), Bottom: \(safe_insets.bottom), Right: \(safe_insets.right)")
        
        let titileLabel = UILabel(frame: CGRect(x: safe_insets.left+10, y: safe_insets.top+22, width: 200, height: 25))
        titileLabel.textAlignment = .left
        titileLabel.textColor = .black
        titileLabel.font = UIFont.systemFont(ofSize: 24, weight: .semibold)
        titileLabel.text = "My Robot"
        view.addSubview(titileLabel)
        
        let layout = UICollectionViewFlowLayout()
        layout.scrollDirection = .horizontal
        layout.minimumInteritemSpacing = 8
        layout.minimumLineSpacing = 8
        let item_width_height = kScreen_HEIGHT-(safe_insets.top+54)-90
        layout.itemSize = CGSize(width: item_width_height, height: item_width_height)
        
        let collection_frame = CGRect(x: safe_insets.left, y: safe_insets.top+54, width: kScreen_WIDTH-safe_insets.left, height: kScreen_HEIGHT-(safe_insets.top+54)-90)
        myCollection = UICollectionView(frame: collection_frame, collectionViewLayout: layout)
        myCollection.translatesAutoresizingMaskIntoConstraints = false
        myCollection.backgroundColor = .clear
        myCollection.register(UINib(nibName: "DeviceCell", bundle: Bundle.main), forCellWithReuseIdentifier: "DeviceCellID")
        myCollection.delegate = self
        myCollection.dataSource = self
        myCollection.showsHorizontalScrollIndicator = false
        myCollection.showsVerticalScrollIndicator = false
        view.addSubview(myCollection)
        
        let addButton = UIButton(type: .custom)
        addButton.frame = CGRect(x: kScreen_WIDTH-24-56, y: kScreen_HEIGHT-24-56, width: 56, height: 56)
        addButton.setImage(UIImage(named: "addDeviceButton"), for: .normal)
        addButton.addTarget(self, action: #selector(clickAddButton), for: .touchUpInside)
        view.addSubview(addButton)
        
        NotificationCenter.default.addObserver(self, selector: #selector(refreshDeviceLlist), name: NSNotification.Name(rawValue: "UpdateLocalDeviceList"), object: nil)
    }
    func initBluetooth(){
        NotificationCenter.default.addObserver(self, selector: #selector(alreadyConnectedDevice), name: NSNotification.Name(rawValue: "setNotifyValueSuccess"), object: nil)
        uuid_this_v = getRandomDigitsString()
        BluetoothManager.shared.delegate = self
        BluetoothManager.shared.startScanBluetoothDevice(type: uuid_this_v)
    }
    func requestDeviceListData(){
        let allScanedDevices = UserDefaults.standard.value(forKey: "allScanedDevices") as? [[String: Any]] ?? [[String: Any]]()
        deviceListModels = allScanedDevices
        myCollection.reloadData()
    }
    @objc func clickAddButton(){
        let vc = ScanDeviceListVC()
        self.present(vc, animated: true)
    }
    @objc func refreshDeviceLlist(){
        let allScanedDevices = UserDefaults.standard.value(forKey: "allScanedDevices") as? [[String: Any]] ?? [[String: Any]]()
        deviceListModels = allScanedDevices
        myCollection.reloadData()
    }
   //MARK: UICollectionViewDataSource, UICollectionViewDelegateFlowLayout
    func numberOfSections(in collectionView: UICollectionView) -> Int {
        return 1
    }
    func collectionView(_ collectionView: UICollectionView, numberOfItemsInSection section: Int) -> Int {
        return deviceListModels.count
    }
    func collectionView(_ collectionView: UICollectionView, cellForItemAt indexPath: IndexPath) -> UICollectionViewCell {
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: "DeviceCellID", for: indexPath) as! DeviceCell
        cell.cellDict = deviceListModels[indexPath.row]
        cell.initCell()
        cell.clickPowerButtonBlock = {
            let cell_data = self.deviceListModels[indexPath.row]
            if (cell_data["isConnected"] as? Bool ?? false) == true{
                if BluetoothManager.shared.current_connecting_CBPeripheral?.identifier.uuidString ?? "" == cell_data["identifier"] as? String ?? "",
                   BluetoothManager.shared.device_connected_status == .connected{
                    print("Actively disconnect the device")
                    BluetoothManager.shared.fromVCType = self.uuid_this_v
                    BluetoothManager.shared.disconnectDevice()
                    //The callback function was not triggered：disconnectDevice
                    var new_allDevices = [[String: Any]]()
                    for item in self.deviceListModels{
                        var new_item = item
                        if item["name"] as? String == cell_data["name"] as? String,
                           item["identifier"] as? String == cell_data["identifier"] as? String{
                            new_item["isConnected"] = false
                        }
                        new_allDevices.append(new_item)
                    }
                    UserDefaults.standard.setValue(new_allDevices, forKey: "allScanedDevices")
                    UserDefaults.standard.synchronize()
                    self.deviceListModels = new_allDevices
                    self.myCollection.reloadData()
                }else{
                    print("The device is not currently connected; modifying the data directly.")
                    var new_allDevices = [[String: Any]]()
                    for item in self.deviceListModels{
                        var new_item = item
                        if item["name"] as? String == cell_data["name"] as? String,
                           item["identifier"] as? String == cell_data["identifier"] as? String{
                            new_item["isConnected"] = false
                        }
                        new_allDevices.append(new_item)
                    }
                    UserDefaults.standard.setValue(new_allDevices, forKey: "allScanedDevices")
                    UserDefaults.standard.synchronize()
                    self.deviceListModels = new_allDevices
                    self.myCollection.reloadData()
                }
                BluetoothManager.shared.startScanBluetoothDevice(type: self.uuid_this_v)
                BluetoothManager.shared.delegate = self
                print("Current Data:\(self.current_ScanedDevice)")
            }else{
                //Starting to connect to the device:
                var isHaveThisDevice = false
                for item in self.current_ScanedDevice{
                    if let current_CBPeripheral = item["device"] as? CBPeripheral,
                       current_CBPeripheral.identifier.uuidString == cell_data["identifier"] as? String,
                       current_CBPeripheral.name == cell_data["name"] as? String{
                        isHaveThisDevice = true
                        print("Device found. Connecting now…")
                        self.currentHUDMessage = MBProgressHUD.showJuHuaAndTitle(title: "Connecting...", view: self.view)
                        BluetoothManager.shared.fromVCType = self.uuid_this_v
                        BluetoothManager.shared.startConnectionDevice(device: current_CBPeripheral)
                    }
                }
                if !isHaveThisDevice{
                    print("Device not found. Unable to connect.")
                    let AlertVC = UIAlertController(title: "Device not found.", message: "Could you delete this device.", preferredStyle: .alert)
                    let confirmBtn = UIAlertAction(title: "Yes", style: .default) { alert in
                        var new_allDevices = [[String: Any]]()
                        for item in self.deviceListModels{
                            let new_item = item
                            if item["name"] as? String == cell_data["name"] as? String,
                               item["identifier"] as? String == cell_data["identifier"] as? String{
                            }else{
                                new_allDevices.append(new_item)
                            }
                        }
                        UserDefaults.standard.setValue(new_allDevices, forKey: "allScanedDevices")
                        UserDefaults.standard.synchronize()
                        self.deviceListModels = new_allDevices
                        self.myCollection.reloadData()
                    }
                    AlertVC.addAction(confirmBtn)
                    let cancelBtn = UIAlertAction(title: "No", style: .cancel)
                    AlertVC.addAction(cancelBtn)
                    self.present(AlertVC, animated: true)
                }
            }
        }
        return cell
    }
    func collectionView(_ collectionView: UICollectionView, didSelectItemAt indexPath: IndexPath) {
        let cell_data = self.deviceListModels[indexPath.row]
        if (cell_data["isConnected"] as? Bool ?? false) == true{
            let vc = DeviceRemoteControlVC()
            vc.backVCBlock = {
                BluetoothManager.shared.delegate = self
                BluetoothManager.shared.startScanBluetoothDevice(type: self.uuid_this_v)
            }
            present(vc, animated: true)
        }else{
            MBProgressHUD.showErrorText(text: "Please connect the device first.", view: view)
        }
    }
    //MARK: ZYBluetoothHandlerDelegate
    //2.1.Discovered a new device
    func scanNewDevice(device: CBPeripheral) {
        if BluetoothManager.shared.fromVCType != uuid_this_v{
            return
        }
        // Only display one type devices
        let all_Device_data = BluetoothManager.shared.allScanedDevices
        var origin_allModels = [[String: Any]]()
        for value in all_Device_data{
            // Only display specific devices
            /*
            if let device = value["device"] as? CBPeripheral,
               (device.name ?? "").contains("nov"){
                var newDeviceValue = value
                newDeviceValue["isConnected"] = false
                origin_allModels.append(value)
            }*/
            //Display all devices
            origin_allModels.append(value)
        }
        //Only update the view when the count changes, otherwise frequent refreshing may cause tap events on cells to be unresponsive
        if current_ScanedDevice.count != origin_allModels.count{
            current_ScanedDevice = origin_allModels
        }
    }
    //2.2.Successfully connected to the device -- not set notify
    func connectDeviceSuccess(device: CBPeripheral) {
        if BluetoothManager.shared.fromVCType != uuid_this_v{
            return
        }
    }
    //2.3.Failed to connect to the device
    func connectDeviceFailtrue(device: CBPeripheral) {
        if BluetoothManager.shared.fromVCType != uuid_this_v{
            return
        }
        MBProgressHUD.showText(text: "Failed to connect to navbot.", view: view)
    }
    //2.4.Device disconnected
    func disconnectDevice(device: CBPeripheral) {
        if BluetoothManager.shared.fromVCType != uuid_this_v{
            return
        }
        MBProgressHUD.nowHiddenMBProgressHUD(self.currentHUDMessage)
        var new_allDevices = [[String: Any]]()
        for item in self.deviceListModels{
            var new_item = item
            if item["name"] as? String == BluetoothManager.shared.current_connecting_CBPeripheral?.name,
               item["identifier"] as? String == BluetoothManager.shared.current_connecting_CBPeripheral?.identifier.uuidString{
                new_item["isConnected"] = false
            }
            new_allDevices.append(new_item)
        }
        UserDefaults.standard.setValue(new_allDevices, forKey: "allScanedDevices")
        UserDefaults.standard.synchronize()
        self.deviceListModels = new_allDevices
        self.myCollection.reloadData()
    }
    //MARK: Connected Device Success
    @objc func alreadyConnectedDevice(){
        if BluetoothManager.shared.fromVCType != uuid_this_v{
            return
        }
        MBProgressHUD.nowHiddenMBProgressHUD(self.currentHUDMessage)
        var new_allDevices = [[String: Any]]()
        for item in self.deviceListModels{
            var new_item = item
            if item["name"] as? String == BluetoothManager.shared.current_connecting_CBPeripheral?.name,
               item["identifier"] as? String == BluetoothManager.shared.current_connecting_CBPeripheral?.identifier.uuidString{
                new_item["isConnected"] = true
            }
            new_allDevices.append(new_item)
        }
        UserDefaults.standard.setValue(new_allDevices, forKey: "allScanedDevices")
        UserDefaults.standard.synchronize()
        self.deviceListModels = new_allDevices
        self.myCollection.reloadData()
    }
    deinit{
        NotificationCenter.default.removeObserver(self)
    }
}
