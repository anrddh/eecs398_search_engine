#pragma once

#include<vector>
#include<deque>
#include<string>
#include <iostream>
#include <map>
#include "GetSSL.hpp"
#include "lib/stddef.hpp"

namespace fb
{
std::string extractURL( const std::string & line);
struct View {
	 const char *p = nullptr;
	 fb::SizeT s = 0;
};

std::ostream& operator<<(std::ostream &os, View &v) {
	 for (fb::SizeT i = 0; i < v.s; ++i) {
			os << v.p[i];
	 }

	 return os;
}

class ParsedUrl
	{
	public:
		const std::string url;

		View Service, Host, Port, Path;
		 bool success = false;

		ParsedUrl( std::string url_ ) : url(std::move(url_))
			{
			Service.p = url.c_str();
			const char Colon = ':', Slash = '/';

			fb::SizeT p = url.find(':');
			if (p == std::string::npos || !p) {
				Path.p = url.c_str();
				Path.s = url.size();
				success = true;
				return;
			}

			Service.s = p++;

			/*
				TODO: If Service does not belong to allowed_services, we exit.
			 */

			if ( url[p] == Slash )
				 p++;
			if ( url[p] == Slash )
				 p++;

			Host.p = url.c_str() + p;

			// check for stl algorithms
			for ( ; p < url.size() && url[p] != Slash && url[p] != Colon; ++p )
				 ;

			Host.s = url.c_str() + p - Host.p;

			if (p < url.size() && url[p] == Colon) {
				// Port specified.  Skip over the colon and
				// the port number.
				 ++p;
				 Port.p = url.c_str() + p;
				 for (; p < url.size() && url[p] != Slash; ++p)
						;
				 Port.s = url.c_str() + p - Port.p;
			}

			Path.p = url.c_str() + p;
			Path.s = url.size() - p;
			success = true;
		}

		~ParsedUrl( )
			{
			delete[ ] pathBuffer;
			}

	private:
		char *pathBuffer;
		constexpr static std::array<const char *, 2> allowed_services = {"https", "http"};
	};

	std::ostream& operator << ( std::ostream& os, ParsedUrl &testguy )
		{
		os << "complete: " << testguy.url << std::endl
			<< "service: " << testguy.Service << std::endl
			<< "host: " << testguy.Host << std::endl
			<< "port: " << testguy.Port << std::endl
			<< "path: " << testguy.Path << std::endl;
		return os;
		}

// Simple exception class for reporting String errors
struct ParserException {
	ParserException(const char* msg_) : msg(msg_)
		{}
	const char* msg;
};


class Parser
{
public:
	enum TagType 
	{
		script,
		a,
		head,
		title,
		body,
		comment,
		strong,
		others,
		DEFAULT
	};

	const std::string domain;
	const static std::map<std::string, TagType> stringToTag;
	const std::string & content;
	// std::vector<TagType> tagStack;
	std::vector<std::string> tagStack;
	std::map<std::string, std::vector<std::string>> urlAnchorText;

	const TagType getTag(std::string tagName)
	{
		auto findResult = stringToTag.find(tagName);
		if( findResult == stringToTag.end())
			return others;
		else
			return findResult->second;
	}

	bool contentEqual( const fb::SizeT start, const std::string & rhs )
		{
		if ( start + rhs.length( ) >= content.length( ) )
			throw ParserException("Comparison out of range");

		for ( size_t i = 0;  i < rhs.length( );  ++i )
			{
			if ( content[ start + i ] != rhs[ i ])
				return false;
			}
		return true;
		}

	Parser( const std::string & content_in, const std::string & domain_in )
	: content( content_in ), domain( domain_in )
	{
		tagStack.push_back( "DEFAULT" );
	}

	bool isSpace( char c )
	{
		return ( c == ' ' ) || ( c == '\t' ) || ( c == '\n' ) || ( c == '\v' ) || ( c == '\f' ) || ( c == '\r' );
	}

	std::string parse( )
		{
		std::string returnString;
		fb::SizeT index = 0;
		char lastChar = '0';
		while ( index < content.length( ) )
			{
			if ( content[ index ] == '<' )
				{
				index = handleTag( index );
				if ( !isSpace( lastChar ) )
					{
					returnString += " ";
					lastChar = ' ';
					}
				}
			else
				{
				if( !( isSpace( content[ index ] ) && isSpace( lastChar ) ) )
					{
					returnString += content[ index ];
					lastChar = content[ index ];
					}
				}
			++index;
			}
		return returnString;
		}

	// return the next index that is not a space
	fb::SizeT skipSpacesForward( fb::SizeT index )
		{
		while ( content[ index ] == ' ' )
			++index;
		return index;
		}

	// return the previous index that is not a space
	fb::SizeT skipSpacesBackward( fb::SizeT index )
		{
		while ( content[ index ] == ' ' )
			--index;
		return index;
		}

	// change tagStack appropriately 
	// opening tag or closing tag
	void setTag( std::string tagName )
	{
		if ( tagName[ 0 ] == '/' )
		{
			// TagType tagType = getTag(tagName.substr(1));
			std::string tagType = tagName.substr( 1 );
			if ( tagStack.back( ) != tagType )
			{
				std::cerr << "Tag do not match, something is wrong. Current tag: " << tagStack.back() << " read: " << tagType <<  std::endl;
			}
			else
			 {
				if ( tagStack.empty() )
					std::cerr << "Possible tag error" << std::endl;
				else
					tagStack.pop_back( );
			 }
		}
		else
			// tagStack.push_back(getTag(tagName));
			tagStack.push_back( tagName );
	}

	// look for occurence of str in content
	// and return the index of end of the occurence
	fb::SizeT seekSubstr( fb::SizeT index, const std::string& str )
	{
		while ( !contentEqual( index, str ) )
			++index;
		return index + str.length() - 1;
	}

	// handle tag and return the end index, which is index of ">".
	// start_index is the index of "<"
	fb::SizeT handleTag( fb::SizeT start_index )
		{
			fb::SizeT i = start_index + 1;

			i = skipSpacesForward( i );

			if ( contentEqual( i, "!-" ) )
				return seekSubstr( i , "-->" );

			std::string tagName;
			for ( ;  content[ i ] != ' ' && content[ i ] != '>' && i < content.length( );  ++i )
				tagName += content[ i ];

			i = seekSubstr( i, ">" );

			size_t last_index = skipSpacesBackward( i - 1 );
			
			// not self closing tag
			if ( content[ last_index ] != '/')
				{
				if ( tagName == "script" )
					i = handleScript( i + 1 );
				else if ( tagName == "a" )
					i = handleAnchor( start_index, i + 1 );
				else
					setTag( tagName );
				}

			return i;
		}

		// Given the start and end indices open anchor tag
		// figure out the link in the tag
		// and get the anchor text, and add those to urlAnchorText
		// return index of ">" of closing anchor tag
		fb::SizeT handleAnchor( fb::SizeT tagStartIndex, fb::SizeT tagEndIndex )
		{
			fb::SizeT index = tagEndIndex;

			std::string aTag = content.substr( tagStartIndex, tagEndIndex - tagStartIndex );

			index = seekSubstr( index, "<" );

			std::string anchorText = content.substr( tagEndIndex, index - tagEndIndex );
			std::string url = extractURL( aTag );

			index = seekSubstr( index, "</a" );

			if( url[0] != '#' )
			{
				if( url[0] == '/' )
					url = domain + url;
				urlAnchorText[ url ].push_back( anchorText );
			}

			index = seekSubstr( index, ">" );

			return index;
		}

		// skip comment in java script and return index after the comments
		fb::SizeT skipJSComment( fb::SizeT index )
			{
			if ( content[ index + 1 ] == '/' )
				index = seekSubstr( index, "\n" );
			if ( content[ index + 1 ] == '*' )
				index = seekSubstr( index, "*/" ) + 1;
			return index + 1;
			}

		// return the index of ">" of end of the closing tag of a script
		fb::SizeT handleScript( fb::SizeT index )
			{
			bool inSingleQuote = false;
			bool inDoubleQuote = false;
			while( index < content.length( ) )
				{
				// handle escape character
				if( content[ index - 1 ] != '\\' )
					{
					char c = content[ index ];
					// exit single quote
					if( c == '\'' && inSingleQuote )
							inSingleQuote = false;
					// exit double quote
					else if( c == '\"' && inDoubleQuote )
							inDoubleQuote = false;
					else
						{
						// not in quote
						if( !inSingleQuote && !inDoubleQuote )
							{
							// skip comment
							if( c == '/' && ( content[ index + 1 ] == '/' || content[ index + 1 ] == '*' ) )
								{
								index = skipJSComment( index );
								continue;
								}
							// enter single quote
							if( c == '\'' )
								inSingleQuote = true;
							// enter double quote
							else if( c == '\"' )
								inDoubleQuote = true;
							}
						}
					}
		
					// only look for end tag if not in quote and not in comment
					if( !inSingleQuote && !inDoubleQuote )
						{
						if( contentEqual( index, "</script") )
							{
							index = seekSubstr( index, ">" );
							break;
							}
						}
					++index;
					}
				return index;
			}

		void printUrls( )
			{
			for(auto i : urlAnchorText)
				{
				std::cout << "URL is: " << i.first << std::endl;
				std::cout << "Anchor text:" << std::endl;
				for(auto j : i.second)
					std::cout << "\t" << j << std::endl;
				}
			}

};

std::string withoutTag( const std::string &line )
	{
	std::string returnString;
	bool inTag = false;
	for ( auto i : line )
		{
		if ( i == '<' )
			inTag = true;
		else if ( i == '>' )
			inTag = false;
		else if ( inTag )
			continue;
		else
			returnString += i;
		}
	return returnString;
	}


const std::map<std::string, Parser::TagType> Parser::stringToTag
{
	{"script", Parser::script},
	{"a", Parser::a},
	{"head", Parser::head},
	{"title", Parser::title},
	{"body", Parser::body},
	{"<--", Parser::comment},
	{"strong", Parser::strong},
};

std::string extractURL( const std::string & line)
	 {
	 fb::SizeT found = line.find( "href" );
		// There might be space between href and =
		found = line.find( "\"", found );
		if ( found == std::string::npos )
			return "";
		// found + 1 to find the next closing quote
		// if no closing quote is found, skip.
		fb::SizeT endURL = line.find( "\"", found + 1 );
		if( endURL == std::string::npos )
			return "";

		std::string url = line.substr( found + 1, endURL - found - 1);

		// just in case a closing quote is not really closing the url, one heuristic is
		// that there would be a space somewhere.
		// For example, we might have
		// href="https://www.nytimes.com/es/ href =    "https://www.nytimes.com/es/
		// it mighbe be possible to just add the substring until the space, but we will see.
		if( url.find( " " ) == std::string::npos )
			return url;
		else
			return "";

	 }

// take in string and a container, vector for now, and append newly found urls
// to the given container.
void extractURL( const std::string & line, std::deque< std::string > & urls , std::string domain)
	 {
	 fb::SizeT found = line.find( "href" );
	 while( found != std::string::npos )
			{
			// There might be space between href and =
			found = line.find( "\"", found );
			if ( found == std::string::npos )
				return;
			// found + 1 to find the next closing quote
			// if no closing quote is found, skip.
			fb::SizeT endURL = line.find( "\"", found + 1 );
			if( endURL == std::string::npos )
				return;

			std::string url = line.substr( found + 1, endURL - found - 1);

			// just in case a closing quote is not really closing the url, one heuristic is
			// that there would be a space somewhere.
			// For example, we might have
			// href="https://www.nytimes.com/es/ href =    "https://www.nytimes.com/es/
			// it mighbe be possible to just add the substring until the space, but we will see.
			if( url.find( " " ) == std::string::npos )
				{
				// ParsedUrl parsedUrl( url.c_str( ) );
				if( url.length() )
					if(url[0] == '/')
					{
					 url = domain + url;
					}
				urls.push_back( url );
				}
			found = line.find( "href", endURL );
			}
	 }
};
