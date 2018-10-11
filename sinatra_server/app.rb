# myapp.rb
require 'sinatra'
require 'serialport'

set :port, 5000
ser = SerialPort.new("/dev/ttyACM0", 9600)

get '/' do
  'Hello world!'
end

get '/:device/:mode/:dim_level' do
  "Device: #{params[:device]}, Mode: #{params[:mode]}, Dim level #{params[:dim_level]}"
  ["#{params[:device]}", "/", "#{params[:mode]}", "/", "#{params[:dim_level]}", "%"].each do |m|
    ser.write m
    sleep 0.01  
  end
  #ser.write "#{params[:device]}"
  #ser.write "/"
  #ser.write "#{params[:mode]}"
  #ser.write "/"
  #ser.write "#{params[:dim_level]}"
  #ser.write "%"
end
