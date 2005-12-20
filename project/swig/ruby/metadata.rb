#!/usr/bin/env ruby

require 'librets'
include Librets

def dump_system(metadata)
  system = metadata.GetSystem
  puts "System ID: " + system.GetSystemID
  puts "Description: " + system.GetSystemDescription
  puts "Comment: " + system.GetComments
end

def dump_all_resources(metadata)
  puts
  metadata.GetAllResources.each do |resource|
    dump_all_classes(metadata, resource)
  end
end

def dump_all_classes(metadata, resource)
  resource_name = resource.GetResourceID();
  metadata.GetAllClasses(resource_name).each do |aClass|
    puts "Resource name: " +  resource_name + " [" + 
      resource.GetStandardName + "]"
    puts "Class name: " + aClass.GetClassName + " [" +
      aClass.GetStandardName + "]"
    dump_all_tables(metadata, aClass)
    puts
  end
end

def dump_all_tables(metadata, aClass)
  metadata.GetAllTables(aClass).each do |table|
    puts "Table name: " + table.GetSystemName + " [" + table.GetStandardName +
      "]"
  end
end

session = RetsSession.new("http://demo.crt.realtors.org:6103/rets/login")
if (ARGV.length == 1) && (ARGV[0] == "full")
  session.SetIncrementalMetadata(false)
end

if !session.Login("Joe", "Schmoe")
  puts "Invalid login"
  exit 1
end

metadata = session.GetMetadata
dump_system(metadata)
dump_all_resources(metadata)

session.Logout()