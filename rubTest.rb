#!/usr/bin/ruby
require 'open3'

filename = ARGV.first
file = File.open(filename)
x = file.readlines

x.each { |line|
	l = line.split(',')
	host = l[0]
	port = l[1]
	data = l[2]
	name = l[3]
	name = name.sub("\n", '')

	cmd = "#{data} | ./p1 #{host} #{port}"
	stdout, stderr = Open3.capture3(cmd)
	File.open("#{name}.out", "w"){ |f|
		f.write(stdout)
	}
	File.open("#{name}.err", "w"){ |f2|
		f2.write(stderr)
	}
}
