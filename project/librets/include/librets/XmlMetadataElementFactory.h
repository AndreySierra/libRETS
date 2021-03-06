/*
 * Copyright (C) 2005 National Association of REALTORS(R)
 *
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, and/or sell copies of the
 * Software, and to permit persons to whom the Software is furnished
 * to do so, provided that the above copyright notice(s) and this
 * permission notice appear in all copies of the Software and that
 * both the above copyright notice(s) and this permission notice
 * appear in supporting documentation.
 */
#ifndef LIBRETS_XML_METADATA_ELEMENT_FACTORY_H
#define LIBRETS_XML_METADATA_ELEMENT_FACTORY_H
/** 
 * @file XmlMetadataElementFactory.h
 * (Internal) Contains the XML Parser Metadata Element Factory interface class for use with libexpatL.
 */
/// @cond MAINTAINER

#include "librets/xml_forward.h"
#include "librets/error_forward.h"
#include "librets/metadata_forward.h"
#include "librets/RetsObject.h"

namespace librets {
/**
 * (Internal) Contains the prototype for the XmlMetadataElementFactory from which other classes
 * inherit.
 */
class XmlMetadataElementFactory : public RetsObject
{
  public:
    virtual ~XmlMetadataElementFactory();
    
    virtual void SetErrorHandler(RetsErrorHandler * errorHandler) = 0;

    virtual MetadataElementPtr CreateMetadataElement(
        RetsXmlStartElementEventPtr startElementEvent) = 0;
};

};
/// @endcond
#endif

/* Local Variables: */
/* mode: c++ */
/* End: */
