import serial # 导入serial模块

ser = serial.Serial("/dev/ttyUSB0", 9600)#打开串口，存放到ser中，/dev/ttyUSB0是端口名，9600是波特率

while True:
    line = ser.readline().decode() # readline()是用于读取整行
    # print(line)

    if line.startswith('$GNRMC'):
        line = line.split(',')
        print(line)
        lat= float(line[3][:2]) + float(line[3][2:]) / 60
        lng = float(line[5][:3]) + float(line[5][3:]) / 60
        print("Latitude:\t", lat)
        print("Longitude:\t", lng)
        print(f"-x {int(lng * 100000)} -y {int(lat * 100000)}")
        break
