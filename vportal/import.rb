require 'rubygems'
require 'serialport'


class Array
  def chr
    self.map { |e| e.chr }
  end
end

#params for serial port
port_str = "/dev/tty.usbmodem1411"  #may be different for you
baud_rate = 115200
data_bits = 8
stop_bits = 1
parity = SerialPort::NONE

sp = SerialPort.new(port_str, baud_rate, data_bits, stop_bits, parity)

libraryId = 1

block_0 = [0xaf, 0xbe, 0xe9, 0xef, 0x17, 0x81, 0x01, 0x0f, 0xc4, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14].chr.join
block_1 = [0xe4, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x30, 0x31, 0xa2].chr.join

puts 'writing I command'
sp.putc('I')

puts "writing library id #{libraryId}"
sp.write(libraryId)

puts "writing block 0: #{block_0}"
sp.write(block_0)

puts "writing block 1: #{block_1}"
sp.write(block_1)


sp.close
