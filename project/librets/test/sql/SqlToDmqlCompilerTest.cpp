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

#include <sstream>
#include <cppunit/extensions/HelperMacros.h>
#include "testUtil.h"
#include "TestSqlMetadata.h"
#include "librets/SqlToDmqlCompiler.h"
#include "librets/DmqlQuery.h"
#include "librets/GetObjectQuery.h"
#include "librets/DmqlCriterion.h"
#include "librets/DmqlExpression.h"
#include "librets/RetsSqlException.h"
#include "librets/SearchRequest.h"

using namespace librets;
using namespace librets::DmqlExpression;
using namespace std;

#define CLASS SqlToDmqlCompilerTest

class CLASS : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(CLASS);
    CPPUNIT_TEST(testGreaterThan);
    CPPUNIT_TEST(testSelectColumns);
    CPPUNIT_TEST(testFullyQualifiedColumnNames);
    CPPUNIT_TEST(testInvalidColumnFQColumnNames);
    CPPUNIT_TEST(testOrderByIgnored);
    CPPUNIT_TEST(testLessThan);
    CPPUNIT_TEST(testEquals);
    CPPUNIT_TEST(testNotEquals);
    CPPUNIT_TEST(testNotEqualsAlt);
    CPPUNIT_TEST(testStringEquals);
    CPPUNIT_TEST(testStringEqualsWithSpace);
    CPPUNIT_TEST(testLessThanOrEquals);
    CPPUNIT_TEST(testGreaterThanOrEquals);
    CPPUNIT_TEST(testLookupEquals);
    CPPUNIT_TEST(testLookupNotEquals);
    CPPUNIT_TEST(testInvalidTableNames);
    CPPUNIT_TEST(testComments);
    CPPUNIT_TEST(testOr);
    CPPUNIT_TEST(testIn);
    CPPUNIT_TEST(testNotIn);
    CPPUNIT_TEST(testInWithSingleValue);
    CPPUNIT_TEST(testLookupIn);
    CPPUNIT_TEST(testAnd);
    CPPUNIT_TEST(testNot);
    CPPUNIT_TEST(testEmptyWhere);
    CPPUNIT_TEST(testQuotedLiterals);
    CPPUNIT_TEST(testTableAlias);
    CPPUNIT_TEST(testTableAliasWithoutAs);
    CPPUNIT_TEST(testInvalidTableAliases);
    CPPUNIT_TEST(testLimit);
    CPPUNIT_TEST(testInvalidLimit);
    CPPUNIT_TEST(testOffset);
    CPPUNIT_TEST(testInvalidOffset);
    CPPUNIT_TEST(testCount);
    CPPUNIT_TEST(testInvalidCount);
    CPPUNIT_TEST(testQuestionMark);
    CPPUNIT_TEST_SUITE_END();

  public:
    CLASS();
    
  protected:
    void testGreaterThan();
    void testSelectColumns();
    void testFullyQualifiedColumnNames();
    void testInvalidColumnFQColumnNames();
    void testOrderByIgnored();
    void testLessThan();
    void testEquals();
    void testNotEquals();
    void testNotEqualsAlt();
    void testStringEquals();
    void testStringEqualsWithSpace();
    void testLessThanOrEquals();
    void testGreaterThanOrEquals();
    void testLookupEquals();
    void testLookupNotEquals();
    void testInvalidTableNames();
    void testComments();
    void testOr();
    void testIn();
    void testNotIn();
    void testInWithSingleValue();
    void testLookupIn();
    void testAnd();
    void testNot();
    void testEmptyWhere();
    void testQuotedLiterals();
    void testTableAlias();
    void testTableAliasWithoutAs();
    void testInvalidTableAliases();
    void testLimit();
    void testInvalidLimit();
    void testOffset();
    void testInvalidOffset();
    void testCount();
    void testInvalidCount();
    void testQuestionMark();

    DmqlQueryPtr sqlToDmql(string sql);
    void assertInvalidSql(string sql,
                          const CPPUNIT_NS::SourceLine & sourceLine);

    SqlToDmqlCompilerPtr mCompiler;
};

CPPUNIT_TEST_SUITE_REGISTRATION(CLASS);

CLASS::CLASS()
{
    TestSqlMetadataPtr metadata(new TestSqlMetadata());
    mCompiler.reset(new SqlToDmqlCompiler(metadata));
}

DmqlQueryPtr CLASS::sqlToDmql(string sql)
{
    if (mCompiler->sqlToDmql(sql) != SqlToDmqlCompiler::DMQL_QUERY)
    {
        CPPUNIT_FAIL("Not a DMQL query");
    }
    return mCompiler->GetDmqlQuery();
}

#define ASSERT_INVALID_SQL(_SQL_) assertInvalidSql(_SQL_, CPPUNIT_SOURCELINE())

void CLASS::assertInvalidSql(string sql,
                             const CPPUNIT_NS::SourceLine & sourceLine)
{
    try
    {
        istringstream inputStream(sql);
        mCompiler->sqlToDmql(inputStream);
        CPPUNIT_NS::Asserter::fail(
            CPPUNIT_NS::Message("forced failure",
                                "should have thrown exception"),
            sourceLine);
    }
    catch (RetsSqlException &)
    {
        // Expected
    }
}

void CLASS::testGreaterThanOrEquals()
{
    DmqlQueryPtr query =
    sqlToDmql("select * "
              " from data:Property:RES "
              " where ListPrice >= 300000;");
    ASSERT_STRING_EQUAL("Property", query->GetResource());
    ASSERT_STRING_EQUAL("RES", query->GetClass());
    ASSERT_EQUAL(SearchRequest::RECORD_COUNT_AND_RESULTS,
                 query->GetCountType());
    
    StringVector columns;
    ASSERT_VECTOR_EQUAL(columns, *query->GetFields());
    
    DmqlCriterionPtr criterion = gt("ListPrice", literal("300000"));
    ASSERT_EQUAL(*criterion, *query->GetCriterion());
    int expectedLimit = SearchRequest::LIMIT_DEFAULT;
    int expectedOffset = SearchRequest::OFFSET_NONE;
    ASSERT_EQUAL(expectedLimit, query->GetLimit());
    ASSERT_EQUAL(expectedOffset, query->GetOffset());
}

void CLASS::testGreaterThan()
{
    DmqlQueryPtr query =
        sqlToDmql("select * from data:Property:RES where ListPrice > 300000;");
    ASSERT_STRING_EQUAL("Property", query->GetResource());
    ASSERT_STRING_EQUAL("RES", query->GetClass());

    StringVector columns;
    ASSERT_VECTOR_EQUAL(columns, *query->GetFields());

    DmqlCriterionPtr criterion =
        logicAnd(gt("ListPrice", literal(300000)),
                 logicNot(eq("ListPrice", literal(300000))));;
    ASSERT_EQUAL(*criterion, *query->GetCriterion());
}

void CLASS::testLookupEquals()
{
    DmqlQueryPtr query =
    sqlToDmql("select * "
              " from data:Property:RES "
              " where Status = 'Active';");
    ASSERT_STRING_EQUAL("Property", query->GetResource());
    ASSERT_STRING_EQUAL("RES", query->GetClass());
    
    StringVector columns;
    ASSERT_VECTOR_EQUAL(columns, *query->GetFields());
    
    DmqlCriterionPtr criterion = lookupOr("Status", literal("Active"));
    ASSERT_EQUAL(*criterion, *query->GetCriterion());
}

void CLASS::testLookupNotEquals()
{
    DmqlQueryPtr query =
    sqlToDmql("select * "
              " from data:Property:RES "
              " where Status <> 'Active';");
    ASSERT_STRING_EQUAL("Property", query->GetResource());
    ASSERT_STRING_EQUAL("RES", query->GetClass());
    
    StringVector columns;
    ASSERT_VECTOR_EQUAL(columns, *query->GetFields());
    
    DmqlCriterionPtr criterion =
        logicNot(lookupOr("Status", literal("Active")));
    
    ASSERT_EQUAL(*criterion, *query->GetCriterion());
}

void CLASS::testSelectColumns()
{
    DmqlQueryPtr query =
        sqlToDmql("select ListingID, ListPrice "
                  "  from data:Property:RES "
                  " where ListPrice >= 300000;");
    ASSERT_STRING_EQUAL("Property", query->GetResource());
    ASSERT_STRING_EQUAL("RES", query->GetClass());

    StringVector columns;
    columns.push_back("ListingID");
    columns.push_back("ListPrice");
    ASSERT_VECTOR_EQUAL(columns, *query->GetFields());

    DmqlCriterionPtr criterion = gt("ListPrice", literal("300000"));
    ASSERT_EQUAL(*criterion, *query->GetCriterion());
}

void CLASS::testFullyQualifiedColumnNames()
{
    DmqlQueryPtr query =
    sqlToDmql("select data:Property:RES.ListingID, data:Property:RES.ListPrice "
              "  from data:Property:RES "
              " where data:Property:RES.ListPrice >= 300000;");
    ASSERT_STRING_EQUAL("Property", query->GetResource());
    ASSERT_STRING_EQUAL("RES", query->GetClass());
    
    StringVector columns;
    columns.push_back("ListingID");
    columns.push_back("ListPrice");
    ASSERT_VECTOR_EQUAL(columns, *query->GetFields());
    
    DmqlCriterionPtr criterion = gt("ListPrice", literal("300000"));
    ASSERT_EQUAL(*criterion, *query->GetCriterion());
}

void CLASS::testInvalidColumnFQColumnNames()
{
    ASSERT_INVALID_SQL(
        "select invalid.ListingID, data:Property:RES.ListPrice "
        "  from data:Property:RES "
        " where data:Property:RES.ListPrice >= 300000;");
    ASSERT_INVALID_SQL(
        "select data:Property:RES.ListingID, data:Property:RES.ListPrice "
        "  from data:Property:RES "
        " where invalid.ListPrice >= 300000;");
}

void CLASS::testOrderByIgnored()
{
    DmqlQueryPtr query =
    sqlToDmql("select * "
              " from data:Property:RES "
              " where ListPrice >= 300000 "
              "order by ListPrice, data:Property:RES.ListingID;");
    ASSERT_STRING_EQUAL("Property", query->GetResource());
    ASSERT_STRING_EQUAL("RES", query->GetClass());
    
    StringVector columns;
    ASSERT_VECTOR_EQUAL(columns, *query->GetFields());
    
    DmqlCriterionPtr criterion = gt("ListPrice", literal("300000"));
    ASSERT_EQUAL(*criterion, *query->GetCriterion());
}


void CLASS::testLessThanOrEquals()
{
    DmqlQueryPtr query =
    sqlToDmql("select * "
              "  from data:Property:RES "
              " where ListPrice <= 300000;");
    ASSERT_STRING_EQUAL("Property", query->GetResource());
    ASSERT_STRING_EQUAL("RES", query->GetClass());
    
    StringVector columns;
    ASSERT_VECTOR_EQUAL(columns, *query->GetFields());
    
    DmqlCriterionPtr criterion = lt("ListPrice", literal("300000"));
    ASSERT_EQUAL(*criterion, *query->GetCriterion());
}

void CLASS::testLessThan()
{
    DmqlQueryPtr query =
        sqlToDmql("select * "
                  "  from data:Property:RES "
                  " where ListPrice < 300000;");
    ASSERT_STRING_EQUAL("Property", query->GetResource());
    ASSERT_STRING_EQUAL("RES", query->GetClass());

    StringVector columns;
    ASSERT_VECTOR_EQUAL(columns, *query->GetFields());

    DmqlCriterionPtr criterion =
        logicAnd(lt("ListPrice", literal("300000")),
                 logicNot(eq("ListPrice", literal("300000"))));
    ASSERT_EQUAL(*criterion, *query->GetCriterion());
}

void CLASS::testEquals()
{
    DmqlQueryPtr query =
        sqlToDmql("select * "
                  "  from data:Property:RES "
                  " where ListPrice = 300000;");
    ASSERT_STRING_EQUAL("Property", query->GetResource());
    ASSERT_STRING_EQUAL("RES", query->GetClass());

    StringVector columns;
    ASSERT_VECTOR_EQUAL(columns, *query->GetFields());

    DmqlCriterionPtr criterion = eq("ListPrice", literal("300000"));
    ASSERT_EQUAL(*criterion, *query->GetCriterion());
}

void CLASS::testNotEquals()
{
    DmqlQueryPtr query =
        sqlToDmql("select * "
                  "  from data:Property:RES "
                  " where ListPrice != 300000;");
    ASSERT_STRING_EQUAL("Property", query->GetResource());
    ASSERT_STRING_EQUAL("RES", query->GetClass());

    StringVector columns;
    ASSERT_VECTOR_EQUAL(columns, *query->GetFields());

    DmqlCriterionPtr criterion = logicNot(eq("ListPrice", literal("300000")));
    ASSERT_EQUAL(*criterion, *query->GetCriterion());
}


void CLASS::testNotEqualsAlt()
{
    DmqlQueryPtr query =
        sqlToDmql("select * "
                  "  from data:Property:RES "
                  " where ListPrice <> 300000;");
    ASSERT_STRING_EQUAL("Property", query->GetResource());
    ASSERT_STRING_EQUAL("RES", query->GetClass());

    StringVector columns;
    ASSERT_VECTOR_EQUAL(columns, *query->GetFields());

    DmqlCriterionPtr criterion = logicNot(eq("ListPrice", literal("300000")));
    ASSERT_EQUAL(*criterion, *query->GetCriterion());
}

void CLASS::testStringEquals()
{
    DmqlQueryPtr query =
        sqlToDmql("select * "
                  "  from data:Property:RES "
                  " where ListingID = 'LN000002';");
    ASSERT_STRING_EQUAL("Property", query->GetResource());
    ASSERT_STRING_EQUAL("RES", query->GetClass());

    StringVector columns;
    ASSERT_VECTOR_EQUAL(columns, *query->GetFields());
    DmqlCriterionPtr criterion = eq("ListingID", literal("LN000002"));
    ASSERT_EQUAL(*criterion, *query->GetCriterion());
}

void CLASS::testStringEqualsWithSpace()
{
    DmqlQueryPtr query =
    sqlToDmql("select * "
              "  from data:Property:RES "
              " where ListingID = 'LN 2';");
    ASSERT_STRING_EQUAL("Property", query->GetResource());
    ASSERT_STRING_EQUAL("RES", query->GetClass());
    
    StringVector columns;
    ASSERT_VECTOR_EQUAL(columns, *query->GetFields());
    DmqlCriterionPtr criterion = eq("ListingID", literal("\"LN 2\""));
    ASSERT_EQUAL(*criterion, *query->GetCriterion());
}

void CLASS::testInvalidTableNames()
{
    // Table names must have 3 sections, separated by ":"
    ASSERT_INVALID_SQL("select * from rets_Property where a > 0");
    ASSERT_INVALID_SQL("select * from data:Property:RES:foo where a > 0");
    // Table names must begin with "data:"
    ASSERT_INVALID_SQL("select * from foo:Property:RES where a > 0");
}

void CLASS::testComments()
{
    DmqlQueryPtr query =
        sqlToDmql("select * -- Put fields to select here\n"
                  "from data:Property:RES \n"
                  "\n -- ignore this! \n"
                  "where ListPrice >= 300000;");
    ASSERT_STRING_EQUAL("Property", query->GetResource());
    ASSERT_STRING_EQUAL("RES", query->GetClass());

    StringVector columns;
    ASSERT_VECTOR_EQUAL(columns, *query->GetFields());

    DmqlCriterionPtr criterion = gt("ListPrice", literal("300000"));
    ASSERT_EQUAL(*criterion, *query->GetCriterion());
}

void CLASS::testOr()
{
    DmqlQueryPtr query =
        sqlToDmql("select * "
                  "  from data:Property:RES "
                  " where ListingID = 'LN1' OR ListingID = 'LN2';");
    ASSERT_STRING_EQUAL("Property", query->GetResource());
    ASSERT_STRING_EQUAL("RES", query->GetClass());

    StringVector columns;
    ASSERT_VECTOR_EQUAL(columns, *query->GetFields());

    DmqlCriterionPtr criterion = logicOr(eq("ListingID", literal("LN1")),
                                         eq("ListingID", literal("LN2")));
    ASSERT_EQUAL(*criterion, *query->GetCriterion());
}

void CLASS::testIn()
{
    DmqlQueryPtr query =
    sqlToDmql("select * "
              "  from data:Property:RES "
              " where ListingID IN ('LN1', 'LN2', 'LN3');");
    ASSERT_STRING_EQUAL("Property", query->GetResource());
    ASSERT_STRING_EQUAL("RES", query->GetClass());
    
    StringVector columns;
    ASSERT_VECTOR_EQUAL(columns, *query->GetFields());
    
    DmqlCriterionPtr criterion =
        logicOr(eq("ListingID", literal("LN1")),
                logicOr(eq("ListingID", literal("LN2")),
                        eq("ListingID", literal("LN3"))));
    ASSERT_EQUAL(*criterion, *query->GetCriterion());
}

void CLASS::testNotIn()
{
    DmqlQueryPtr query =
    sqlToDmql("select * "
              "  from data:Property:RES "
              " where ListingID NOT IN ('LN1', 'LN2', 'LN3');");
    ASSERT_STRING_EQUAL("Property", query->GetResource());
    ASSERT_STRING_EQUAL("RES", query->GetClass());
    
    StringVector columns;
    ASSERT_VECTOR_EQUAL(columns, *query->GetFields());

    DmqlCriterionPtr criterion =
        logicNot(logicOr(eq("ListingID", literal("LN1")),
                logicOr(eq("ListingID", literal("LN2")),
                        eq("ListingID", literal("LN3")))));
    
    ASSERT_EQUAL(*criterion, *query->GetCriterion());
}

void CLASS::testInWithSingleValue()
{
    DmqlQueryPtr query =
        sqlToDmql("select * "
                  "  from data:Property:RES "
                  " where ListingID IN ('LN1');");
    ASSERT_STRING_EQUAL("Property", query->GetResource());
    ASSERT_STRING_EQUAL("RES", query->GetClass());
    
    StringVector columns;
    ASSERT_VECTOR_EQUAL(columns, *query->GetFields());
    
    DmqlCriterionPtr criterion = eq("ListingID", literal("LN1"));
    ASSERT_EQUAL(*criterion, *query->GetCriterion());
}

void CLASS::testLookupIn()
{
    DmqlQueryPtr query =
    sqlToDmql("select * "
              "  from data:Property:RES "
              " where Status IN ('Active', 'Closed');");
    ASSERT_STRING_EQUAL("Property", query->GetResource());
    ASSERT_STRING_EQUAL("RES", query->GetClass());
    
    StringVector columns;
    ASSERT_VECTOR_EQUAL(columns, *query->GetFields());
    
    DmqlCriterionPtr criterion = logicOr(lookupOr("Status", literal("Active")),
                                         lookupOr("Status", literal("Closed")));
    ASSERT_EQUAL(*criterion, *query->GetCriterion());
}

void CLASS::testAnd()
{
    DmqlQueryPtr query =
        sqlToDmql("select * "
                  "  from data:Property:RES "
                  " where ListPrice >= 300000 AND ListPrice <= 400000;");
    ASSERT_STRING_EQUAL("Property", query->GetResource());
    ASSERT_STRING_EQUAL("RES", query->GetClass());

    StringVector columns;
    ASSERT_VECTOR_EQUAL(columns, *query->GetFields());

    DmqlCriterionPtr criterion = logicAnd(gt("ListPrice", literal(300000)),
                                          lt("ListPrice", literal(400000)));
    ASSERT_EQUAL(*criterion, *query->GetCriterion());
}

void CLASS::testNot()
{
    DmqlQueryPtr query =
        sqlToDmql("select * "
                  "  from data:Property:RES "
                  " where not ListPrice >= 300000;");
    ASSERT_STRING_EQUAL("Property", query->GetResource());
    ASSERT_STRING_EQUAL("RES", query->GetClass());

    StringVector columns;
    ASSERT_VECTOR_EQUAL(columns, *query->GetFields());

    DmqlCriterionPtr criterion = logicNot(gt("ListPrice", literal(300000)));
    ASSERT_EQUAL(*criterion, *query->GetCriterion());
}

void CLASS::testEmptyWhere()
{
    DmqlQueryPtr query =
    sqlToDmql("select * "
              " from data:Property:RES;");
    ASSERT_STRING_EQUAL("Property", query->GetResource());
    ASSERT_STRING_EQUAL("RES", query->GetClass());
    
    StringVector columns;
    ASSERT_VECTOR_EQUAL(columns, *query->GetFields());

    DmqlCriterionPtr criterion = query->GetCriterion();
    CPPUNIT_ASSERT(!criterion);
}

void CLASS::testQuotedLiterals()
{
    DmqlQueryPtr query =
        sqlToDmql("select ListingID, \"data:Property:RES\".ListPrice, \"Order\" "
                  " from \"data:Property:RES\" "
                  " where \"data:Property:RES\".ListPrice >= 300000;");
    ASSERT_STRING_EQUAL("Property", query->GetResource());
    ASSERT_STRING_EQUAL("RES", query->GetClass());
    
    StringVector columns;
    columns.push_back("ListingID");
    columns.push_back("ListPrice");
    columns.push_back("Order");
    ASSERT_VECTOR_EQUAL(columns, *query->GetFields());
    
    DmqlCriterionPtr criterion = gt("ListPrice", literal("300000"));
    ASSERT_EQUAL(*criterion, *query->GetCriterion());
}

void CLASS::testTableAlias()
{
    DmqlQueryPtr query =
        sqlToDmql("select res.ListingID, res.ListPrice "
                  " from \"data:Property:RES\" as res"
                  " where res.ListPrice >= 300000;");
    ASSERT_STRING_EQUAL("Property", query->GetResource());
    ASSERT_STRING_EQUAL("RES", query->GetClass());
    
    StringVector columns;
    columns.push_back("ListingID");
    columns.push_back("ListPrice");
    ASSERT_VECTOR_EQUAL(columns, *query->GetFields());
    
    DmqlCriterionPtr criterion = gt("ListPrice", literal("300000"));
    ASSERT_EQUAL(*criterion, *query->GetCriterion());
}

void CLASS::testTableAliasWithoutAs()
{
    DmqlQueryPtr query =
        sqlToDmql("select res.ListingID, res.ListPrice "
                  " from \"data:Property:RES\" res"
                  " where res.ListPrice >= 300000;");
    ASSERT_STRING_EQUAL("Property", query->GetResource());
    ASSERT_STRING_EQUAL("RES", query->GetClass());
    
    StringVector columns;
    columns.push_back("ListingID");
    columns.push_back("ListPrice");
    ASSERT_VECTOR_EQUAL(columns, *query->GetFields());
    
    DmqlCriterionPtr criterion = gt("ListPrice", literal("300000"));
    ASSERT_EQUAL(*criterion, *query->GetCriterion());
}

void CLASS::testInvalidTableAliases()
{
    // If you use an alias, you can no longer use the original table name
    ASSERT_INVALID_SQL("select * "
                       "from data:Property:RES r "
                       "where data:Property:RES.ListPrice > 0");
    
    ASSERT_INVALID_SQL("select  "
                       "from data:Property:RES r "
                       "where res.ListPrice > 0");
    
    ASSERT_INVALID_SQL("select res.ListPrice "
                       "from data:Property:RES r "
                       "where r.ListPrice > 0");
}


void CLASS::testLimit()
{
    DmqlQueryPtr query =
    sqlToDmql("select * "
              " from data:Property:RES"
              " where ListPrice = 300000"
              " limit 5;");
    ASSERT_STRING_EQUAL("Property", query->GetResource());
    ASSERT_STRING_EQUAL("RES", query->GetClass());
    
    ASSERT_STRING_EQUAL("Property", query->GetResource());
    ASSERT_STRING_EQUAL("RES", query->GetClass());

    StringVector columns;
    ASSERT_VECTOR_EQUAL(columns, *query->GetFields());

    DmqlCriterionPtr criterion = eq("ListPrice", literal("300000"));
    ASSERT_EQUAL(*criterion, *query->GetCriterion());

    int limit = query->GetLimit();
    ASSERT_EQUAL(5, limit);
}

void CLASS::testInvalidLimit()
{
    ASSERT_INVALID_SQL("select * "
                       " from data:Property:RES"
                       " where ListPrice = 300000"
                       " limit foo;");
}

void CLASS::testOffset()
{
    DmqlQueryPtr query =
    sqlToDmql("select * "
              " from data:Property:RES"
              " where ListPrice = 300000"
              " offset 5;");
    ASSERT_STRING_EQUAL("Property", query->GetResource());
    ASSERT_STRING_EQUAL("RES", query->GetClass());

    StringVector columns;
    ASSERT_VECTOR_EQUAL(columns, *query->GetFields());

    DmqlCriterionPtr criterion = eq("ListPrice", literal("300000"));
    ASSERT_EQUAL(*criterion, *query->GetCriterion());

    int offset = query->GetOffset();
    ASSERT_EQUAL(5, offset);
}

void CLASS::testInvalidOffset()
{
    ASSERT_INVALID_SQL("select * "
                       " from data:Property:RES"
                       " where ListPrice = 300000"
                       " offset foo;");
}

void CLASS::testCount()
{
    DmqlQueryPtr query =
        sqlToDmql("select count(*) "
                  "  from data:Property:RES"
                  " WHERE ListPrice = 300000");

    ASSERT_STRING_EQUAL("Property", query->GetResource());
    ASSERT_STRING_EQUAL("RES", query->GetClass());

    StringVector columns;
    ASSERT_VECTOR_EQUAL(columns, *query->GetFields());

    DmqlCriterionPtr criterion = eq("ListPrice", literal("300000"));
    ASSERT_EQUAL(*criterion, *query->GetCriterion());

    ASSERT_EQUAL(SearchRequest::RECORD_COUNT_ONLY, query->GetCountType());
}

void CLASS::testInvalidCount()
{
    ASSERT_INVALID_SQL("select count(*),foo "
                       " from data:Property:RES"
                       " where ListPrice = 300000");

    ASSERT_INVALID_SQL("select foo,count(*) "
                       " from data:Property:RES"
                       " where ListPrice = 300000");

    ASSERT_INVALID_SQL("select count"
                       " from data:Property:RES"
                       " where ListPrice = 300000");

    ASSERT_INVALID_SQL("select count(foo) "
                       " from data:Property:RES"
                       " where ListPrice = 300000");
}

void CLASS::testQuestionMark()
{
    DmqlQueryPtr query =
        sqlToDmql("select *"
                  "  from data:Property:RES"
                  " WHERE ListPrice = ?");

    ASSERT_STRING_EQUAL("Property", query->GetResource());
    ASSERT_STRING_EQUAL("RES", query->GetClass());

    StringVector columns;
    ASSERT_VECTOR_EQUAL(columns, *query->GetFields());

    DmqlCriterionPtr criterion = eq("ListPrice", literal("?"));
    ASSERT_EQUAL(*criterion, *query->GetCriterion());

//     std::cout << criterion << std::endl;
//     std::cout << criterion->ToDmqlString() << std::endl;

    query =
        sqlToDmql("select *"
                  "  from data:Property:RES"
                  " WHERE ListPrice >= ? AND ListPrice <= ?");

    ASSERT_STRING_EQUAL("Property", query->GetResource());
    ASSERT_STRING_EQUAL("RES", query->GetClass());

    columns.clear();
    ASSERT_VECTOR_EQUAL(columns, *query->GetFields());

    criterion = logicAnd(gt("ListPrice", literal("?")),
                         lt("ListPrice", literal("?")));
    ASSERT_EQUAL(*criterion, *query->GetCriterion());

//     std::cout << criterion << std::endl;
//     std::cout << criterion->ToDmqlString() << std::endl;

    query =
        sqlToDmql("select * "
                  "  from data:Property:RES "
                  " where ListingID IN (?, ?, ?);");
    ASSERT_STRING_EQUAL("Property", query->GetResource());
    ASSERT_STRING_EQUAL("RES", query->GetClass());
    
    columns.clear();
    ASSERT_VECTOR_EQUAL(columns, *query->GetFields());
    
    criterion =
        logicOr(eq("ListingID", literal("?")),
                logicOr(eq("ListingID", literal("?")),
                        eq("ListingID", literal("?"))));
    ASSERT_EQUAL(*criterion, *query->GetCriterion());

//     std::cout << criterion << std::endl;
//     std::cout << criterion->ToDmqlString() << std::endl;
}
