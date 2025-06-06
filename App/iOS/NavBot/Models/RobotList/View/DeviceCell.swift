
import UIKit

class DeviceCell: UICollectionViewCell {
    override func awakeFromNib() {
        super.awakeFromNib()
    }
    @IBOutlet weak var deviceNameLabel: UILabel!
    @IBOutlet weak var powerButton: UIButton!
    
    var cellDict = [String: Any]()
    var clickPowerButtonBlock: (()->())?
    
    func initCell(){
        deviceNameLabel.text = cellDict["name"] as? String ?? ""
        if (cellDict["isConnected"] as? Bool ?? false) == true{
            powerButton.setImage(UIImage(named: "devic_open"), for: .normal)
        }else{
            powerButton.setImage(UIImage(named: "device_off"), for: .normal)
        }
    }
    
    @IBAction func clickPowewButton(_ sender: Any) {
        clickPowerButtonBlock?()
    }
    
}
