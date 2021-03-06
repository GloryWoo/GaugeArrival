//
// MySQLstatementImpl.h
//
// $Id: //poco/1.3/Data/MySQL/include/Poco/Data/MySQL/MySQLStatementImpl.h#3 $
//
// Library: Data/MySQL
// Package: MySQL
// Module:  MySQLstatementImpl
//
// Definition of the MySQLStatementImpl class.
//
// Copyright (c) 2008, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#ifndef Data_MySQL_MySQLStatementImpl_INCLUDED
#define Data_MySQL_MySQLStatementImpl_INCLUDED


#include "Poco/Data/MySQL/MySQL.h"
#include "Poco/Data/MySQL/SessionImpl.h"
#include "Poco/Data/MySQL/Binder.h"
#include "Poco/Data/MySQL/Extractor.h"
#include "Poco/Data/MySQL/StatementExecutor.h"
#include "Poco/Data/MySQL/ResultMetadata.h"
#include "Poco/Data/StatementImpl.h"
#include "Poco/SharedPtr.h"
#include "Poco/Format.h"


namespace Poco {
namespace Data {
namespace MySQL {


class MySQL_API MySQLStatementImpl: public Poco::Data::StatementImpl
	/// Implements statement functionality needed for MySQL
{
public:
	MySQLStatementImpl(SessionHandle& h);
		/// Creates the MySQLStatementImpl.
		
	~MySQLStatementImpl();
		/// Destroys the MySQLStatementImpl.
		
protected:
	virtual Poco::UInt32 columnsReturned() const;
		/// Returns number of columns returned by query.		
	
	virtual const MetaColumn& metaColumn(Poco::UInt32 pos) const;
		/// Returns column meta data.
		
	virtual bool hasNext();
		/// Returns true if a call to next() will return data.
		
	virtual void next();
		/// Retrieves the next row from the resultset.
		/// Will throw, if the resultset is empty.
	
	virtual bool canBind() const;
		/// Returns true if a valid statement is set and we can bind.
		
	virtual void compileImpl();
		/// Compiles the statement, doesn't bind yet
		
	virtual void bindImpl();
		/// Binds parameters
		
	virtual AbstractExtractor& extractor();
		/// Returns the concrete extractor used by the statement.
		
	virtual AbstractBinder& binder();
		/// Returns the concrete binder used by the statement.

private:
	enum
	{
		NEXT_DONTKNOW,
		NEXT_TRUE,
		NEXT_FALSE
	};

	StatementExecutor _stmt;
	ResultMetadata    _metadata;
	Binder            _binder;
	Extractor         _extractor;
	int               _hasNext;
};


} } } // namespace Poco::Data::MySQL


#endif // Data_MySQL_MySQLStatementImpl_INCLUDED
