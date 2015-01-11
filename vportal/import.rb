require 'rubygems'
require 'serialport'
require 'json'
require 'digest/crc16_ccitt'
require 'securerandom'

class Array
  def chr
    self.map { |e| e.chr }
  end
end

ID_LEN = 8
CRC_LEN = 4
HEADER_LEN = 64
MAX_NAME_LEN = 14 # + null character + elementAndType byte go into a single 16 byte block
BLOCK_SIZE = 16


def importTokens(sp)
  File.open("backup.json", "r") do |f|
    tokens = JSON.parse(f.read)
    tokens.each.with_index do |token, libraryId|
      token = OpenStruct.new(token)
      data = token.data.slice(ID_LEN, HEADER_LEN - ID_LEN - CRC_LEN)

      #Replace ID
      id = SecureRandom.hex.slice(0, ID_LEN)
      data = id + data

      #Write CRC
      bin = [data.slice(0, HEADER_LEN - CRC_LEN)].pack('H*')
      crc = Digest::CRC16CCITT.new
      crc.update(bin)
      crc.finish

      bin << crc.hexdigest[2..3].to_i(16)
      bin << crc.hexdigest[0..1].to_i(16)

      puts 'writing I command'
      sp.putc('I')

      puts "writing library id #{libraryId}"
      sp.write(libraryId)

      block_0 = bin.slice(0, BLOCK_SIZE)
      puts "writing block 0: #{block_0.unpack('H*')}"
      sp.write(block_0)

      block_1 = bin.slice(BLOCK_SIZE, BLOCK_SIZE)
      puts "writing block 1: #{block_1.unpack('H*')}"
      sp.write(block_1)

      sleep 1
    end
  end
end

def importNames(sp)
  File.open("backup.json", "r") do |f|
    tokens = JSON.parse(f.read)
    puts 'writing N command'
    sp.putc('N')

    puts "write #{tokens.count} names"
    sp.write(tokens.count)

    tokens.each.with_index do |token, libraryId|
      token = OpenStruct.new(token)
      name = token.name.slice(0, 15)
      puts "writing: #{name}"
      sp.write(name)

      sleep 1
    end
  end
end

def main
  #params for serial port
  port_str = "/dev/tty.usbmodem1411"  #may be different for you
  baud_rate = 115200
  data_bits = 8
  stop_bits = 1
  parity = SerialPort::NONE

  sp = SerialPort.new(port_str, baud_rate, data_bits, stop_bits, parity)

  importTokens(sp)
  importNames(sp)

  sp.close
end

main
