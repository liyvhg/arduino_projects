#!/usr/bin/env ruby

require 'rubygems'
require 'serialport'
require 'json'
require 'digest/crc16_ccitt'
require 'securerandom'
require 'active_support/core_ext/string/inflections'

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

def parseJson
  tokens = []
  File.open("backup.json", "r") do |f|
    tokens = JSON.parse(f.read)
  end
  tokens.each {|token| token['name'] = token['name'].titleize}
  tokens.collect{|token| OpenStruct.new(token) }
end

def importTokens(tokens, sp)
  tokens.each.with_index do |token, libraryId|
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

    #puts 'writing I command'
    sp.putc('I')

    puts "writing #{libraryId}. #{token.name}"
    sp.write(libraryId)
    sleep 1

    block_0 = bin.slice(0, BLOCK_SIZE)
    #puts "writing block 0: #{block_0.unpack('H*')}"
    sp.write(block_0)

    block_1 = bin.slice(BLOCK_SIZE, BLOCK_SIZE)
    #puts "writing block 1: #{block_1.unpack('H*')}"
    sp.write(block_1)

  end
end

def importNames(tokens, sp)
  puts 'writing N command'
  sp.putc('N')

  puts "write #{tokens.count} names"
  sp.write(tokens.count)

  tokens.each.with_index do |token, libraryId|
    eat = elementAndType(token)
    #Trim if overlong
    name = token.name.titleize.slice(0, MAX_NAME_LEN)
    #pad if undersized
    pad_name = name.bytes.fill(0, name.length..14)
    bin = (pad_name + [eat]).pack('c16')
    puts "writing: #{name} #{eat.to_s(16)}"
    sp.write(bin)
  end
end

#This needs to be kept in sync with the real code
def elementAndType(token)
  types = ["none", "trapmaster", "trap", "item", "location", "mini", "regular"]
  elements = ["none", "magic", "earth", "water", "fire", "tech", "undead", "life", "air", "dark", "light"]

  type_enum = types.find_index(token.type).to_i * 0x10
  element_enum = elements.find_index(token.element).to_i
  #puts "#{token.type} + #{token.element} = #{type_enum.to_s(16)} + #{element_enum.to_s(16)}"
  type_enum + element_enum
end

def main
  #params for serial port
  port_str = "/dev/tty.usbmodem1411"  #may be different for you
  baud_rate = 115200
  data_bits = 8
  stop_bits = 1
  parity = SerialPort::NONE

  sp = SerialPort.new(port_str, baud_rate, data_bits, stop_bits, parity)
  tokens = parseJson
  tokens.sort_by!(&:name)

  tokens = tokens.take(ARGV[0].to_i) unless ARGV.empty?

  importNames(tokens, sp)
  importTokens(tokens, sp)

  sp.close
end

main
