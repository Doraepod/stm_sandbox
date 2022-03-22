clear port
close all
port = serialport('COM4', 115200);

figure();
hold on
set(gcf, 'CurrentCharacter', '~')

while 1
    if get(gcf, 'CurrentCharacter') ~= '~'
        break;
    end
    
    buf = single(read(port, 3, 'single'));
    checkval = typecast(buf(1), 'uint32');
    if checkval ~= 2538843507
        flush(port);
        continue;
    end
    plot(buf(3), buf(2), 'xr')
    
    drawnow();
    
end

clear port