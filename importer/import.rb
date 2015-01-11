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


def importTokens(sp, max_tokens)
  File.open("backup.json", "r") do |f|
    tokens = JSON.parse(f.read)
    tokens.each.with_index do |token, libraryId|
      break if libraryId >= max_tokens
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

def importNames(sp, max_tokens)
  File.open("backup.json", "r") do |f|
    tokens = JSON.parse(f.read)
    puts 'writing N command'
    sp.putc('N')

    count = [tokens.count, max_tokens].min
    puts "write #{count} names"
    sp.write(count)

    tokens.each.with_index do |token, libraryId|
      break if libraryId >= max_tokens
      token = OpenStruct.new(token)
      name = token.name.slice(0, MAX_NAME_LEN)
      eat = elementAndType(token)
      pad_name = name.bytes.fill(0, name.length..15)
      bin = (pad_name + [eat]).pack('c*')
      puts "writing: #{name} #{eat}"
      p bin
      sp.write(bin)
      sleep 1
    end
  end
end

#This needs to be kept in sync with the real code
def elementAndType(token)
  types = ["none", "trapmaster", "trap", "item", "location", "mini", "regular"]
  elements = ["none", "magic", "earth", "water", "fire", "tech", "undead", "life", "air", "dark", "light"]

  eat = (types.find_index(token.type).to_i * 0x10) + (elements.find_index(token.element).to_i)
  puts "#{token.type} + #{token.element} = #{eat}"
  eat
end

def main
  #params for serial port
  port_str = "/dev/tty.usbmodem1411"  #may be different for you
  baud_rate = 115200
  data_bits = 8
  stop_bits = 1
  parity = SerialPort::NONE

  sp = SerialPort.new(port_str, baud_rate, data_bits, stop_bits, parity)

  max_tokens = 256
  max_tokens = ARGV[0].to_i unless ARGV.empty?

  importTokens(sp, max_tokens)
  importNames(sp, max_tokens)

  sp.close
end

main
