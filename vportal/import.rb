require 'rubygems'
require 'serialport'
require 'json'
require 'digest/crc16_ccitt'


class Array
  def chr
    self.map { |e| e.chr }
  end
end

#Send tokens in a fixed order, then send name/type/element afterwards

def send_token(libraryId, blocks)

  #Assign random tag id to first 4 bytes of block [0]
  #Calculate crc of blocks and write to last 2 bytes of block[1]

  puts 'writing I command'
  sp.putc('I')

  puts "writing library id #{libraryId}"
  sp.write(libraryId)

  puts "writing block 0: #{blocks[0]}"
  sp.write(blocks[0].chr.join)

  puts "writing block 1: #{blocks[1]}"
  sp.write(blocks[1].chr.join)
end

def message(message)
  ar = Array.new()
  message.each do |val|
    ar.push(swapbits(val))
  end
  nval = ar.pack("c*")

  crc = Digest::CRC16CCITT.new
  crc.update(nval)
  crc.finish

  a = crc.hexdigest[2..3].to_i(16)
  b = crc.hexdigest[0..1].to_i(16)

  message << a
  message << b

  # send the message to the serial port
  @port.write(message.flatten.pack('C*'))

  return message
end

File.open("backup.json", "r") do |f|
  tokens = JSON.parse(f.read)
  tokens.each do |token|
    token = OpenStruct.new(token)
    data = [token.data.slice(0, 60)].pack('H*')
    checksum = token.data.slice(60, 4)

    crc = Digest::CRC16CCITT.new
    crc.update(data)
    crc.finish

    a = crc.hexdigest[2..3].to_i(16)
    b = crc.hexdigest[0..1].to_i(16)

    a1 = checksum[0..1].to_i(16)
    b1 = checksum[2..3].to_i(16)

    if (a == a1 && b == b1)
      puts token.name
    else
      data << a
      data << b
    end

  end
end

exit

#params for serial port
port_str = "/dev/tty.usbmodem1411"  #may be different for you
baud_rate = 115200
data_bits = 8
stop_bits = 1
parity = SerialPort::NONE


sp = SerialPort.new(port_str, baud_rate, data_bits, stop_bits, parity)

#serial stuff

sp.close



libraryId = 1

#Knight M
block_0 = [0xaf, 0xbe, 0xe9, 0xef, 0x17, 0x81, 0x01, 0x0f, 0xc4, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14]
block_1 = [0xe4, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x30, 0x31, 0xa2]

#NKK
#block_0 = [0xdf ,0x0a ,0x11 ,0x1a ,0xde ,0x81 ,0x01 ,0x0f ,0xc4 ,0x09 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x14]
#block_1 = [0xde ,0x01 ,0x00 ,0x00 ,0x41 ,0xfc ,0xf2 ,0xaf ,0x4b ,0x2f ,0x00 ,0x00 ,0x02 ,0x34 ,0xdd ,0x80]



