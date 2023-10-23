# ble_scan_connect.py
from bluepy.btle import Peripheral, UUID
from bluepy.btle import Scanner, DefaultDelegate
from bluepy import btle
import struct

class MyDelegate(btle.DefaultDelegate):
    def __init__(self):
        btle.DefaultDelegate.__init__(self)

    def handleNotification(self, cHandle, data):
        #m16=lambda data:struct.unpack('h',struct.pack('h',data))[0]
        #m16=lambda data:data & 0xffff
        print((data))
        data0,data1,data2=struct.unpack('<3h',data)
        print ("Notification received: handle =", cHandle, "; Raw data =", data0,' ',data1,' ',data2,' ')
      
    #  print(type(data))


class ScanDelegate(DefaultDelegate):
    def __init__(self):
        DefaultDelegate.__init__(self)

    def handleDiscovery(self, dev, isNewDev, isNewData):
        if isNewDev:
            print("Discovered device", dev.addr)
        elif isNewData:
            print("Received new data from", dev.addr)

scanner = Scanner().withDelegate(ScanDelegate())
devices = scanner.scan(4.0)
n = 0
addr = []
num=-1
for dev in devices:
   # print("%d: Device %s (%s), RSSI=%d dB" % (n, dev.addr, dev.addrType, dev.rssi))
    addr.append(dev.addr)
    n += 1

    for (adtype, desc, value) in dev.getScanData():
    #    print(" %s = %s" % (desc, value))
        if(value=='r11921009' and desc=='Complete Local Name'):
            num=n-1
            break
    if (num!=-1):
        break
assert(num!=-1)
    
# number = input('Enter your device number: ')
# print('Device', number)
# num = int(number)
print(addr[num])
print("Connecting...")
dev = Peripheral(addr[num], 'random')
setup_data = b"\x01\x00"
notify = dev.getCharacteristics(uuid=0x2a37)[0]
notify_handle = notify.getHandle() + 1
dev.writeCharacteristic(notify_handle, setup_data, withResponse=True)





print("Services...")

# for i in dev.getDescriptors(startHnd=1, endHnd=0x2909):

#     print(i.uuid)
#     if(i.uuid==0x2902):
#         w=2
#         print(i.read())
        
#         i.write(w.to_bytes(1,'little'))
#         print(i.read())
#         print('write!!!!')
        

#     # print(dir(i))


# print(dev)
# for svc in dev.services:
#     print(str(svc))
# i=0
# try:
#     testService = dev.getServiceByUUID(UUID(0xfff0))
#     for ch in testService.getCharacteristics():
#         print(str(ch))
#         i+=1
#         print("data"+str(i))
#         # if ch.supportsRead():
#         #     print(ch.read())
#         # # if ch.supportsWrite():
#         # else:
#         ch.write("fuck!!!!!!!!!!".encode("utf-8"))
# try:
#     i=0
    # for ch in dev.getCharacteristics(uuid=UUID(0xfff4)):
    #     i+=1
    #     print("data"+str(i))
    #     # if ch.supportsRead():
    #     #     print(ch.read())
    #     # # if ch.supportsWrite():
    #     # else:
    #     ch.write("666666666666666".encode("utf-8"))
i=0


while True:
    #for ch in dev.getCharacteristics():
        # i+=1
        # print("data"+str(i))
     #   if ch.supportsRead():
    #        print(ch.uuid)
   #         print(ch.read())
    #     # # if ch.supportsWrite():
    #     # else:
    #     ch.write("666666666666666".encode("utf-8"))
    dev.setDelegate( MyDelegate() )
    if dev.waitForNotifications(1.0):
        
        # handleNotification() was called
        print('notify!!')
        # continue

        print ("Waiting...")
# finally:
#     dev.disconnect()
