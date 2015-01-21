#!/usr/bin/env ruby

require 'rubygems'
require 'json'
require 'digest/crc16_ccitt'
require 'securerandom'
require 'active_support/core_ext/string/inflections'

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
  tokens.each do |token|
    token['name'] = token['name'].titleize
    token['hexId'] = token['data'][34..35] + token['data'][32..33]
    token['id'] = 255 * token['data'][34..35].to_i(16) + token['data'][32..33].to_i(16)
  end

  tokens.collect{|token| OpenStruct.new(token) }
end

def main
  tokens = parseJson
  tokens.sort_by!(&:id)
  tokens.each do |token|
    puts "#{token.id}(#{token.hexId})".ljust(16) + token.element.to_s.ljust(15) + token.type.to_s.ljust(15) + token.name
  end
end

main
