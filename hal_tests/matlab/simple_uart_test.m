clear sd
sd = serialport('COM4', 115200);
i = 0;
while 1
    write(sd, i, 'UINT16');
    i = i + 1;
    pause(0.2);
    if i > 100
        break;
    end
end
clear sd