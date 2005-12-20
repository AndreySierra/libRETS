#!/usr/bin/env python

import sys
import librets

try:
  session = librets.RetsSession("http://demo.crt.realtors.org:6103/rets/login")
  if (not session.Login("Joe", "Schmoe")):
      print "Invalid login"
      sys.exit(1)

  print "Action: ", session.GetAction()
  version = "1.0"
  if session.GetDetectedRetsVersion() == librets.RETS_1_5:
    version = "1.5"
  print "RETS Version: " + version

  request = session.CreateSearchRequest("Property", "ResidentialProperty",
    "(ListPrice=300000-)")
    
  request.SetSelect("ListingID,ListPrice,Beds,City")
  request.SetLimit(librets.SearchRequest.LIMIT_DEFAULT)
  request.SetOffset(librets.SearchRequest.OFFSET_NONE)
  request.SetCountType(librets.SearchRequest.RECORD_COUNT_AND_RESULTS)
  results = session.Search(request)
  
  print "Record count: " + `results.GetCount()`
  print
  columns = results.GetColumns()
  while results.HasNext():
    for column in columns:
      print column + ": " + results.GetString(column)
    print
  
  logout = session.Logout();
  print "Billing info: " + logout.GetBillingInfo()
  print "Logout message: " + logout.GetLogoutMessage()
  print "Connect time: " + str(logout.GetConnectTime())
except librets.RetsException,e:
  print "Caught: " + e.GetMessage()